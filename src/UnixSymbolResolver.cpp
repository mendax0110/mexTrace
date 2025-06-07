#include "UnixSymbolResolver.h"
#include "PlatformUtils.h"
#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>
#include <sys/wait.h>
#include <unistd.h>
#include <memory>
#include <vector>

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <mach/vm_map.h>
#include <mach/vm_region.h>
#include <mach-o/dyld.h>
#include <pthread.h>
#include <iostream>
#include <sys/sysctl.h>
#include <sys/types.h>
#else
#include <sys/ptrace.h>
#endif

using namespace mex;

UnixSymbolResolver::UnixSymbolResolver()
        : m_pid(getpid()), m_attached(false)
#ifdef __APPLE__
        , m_task(mach_task_self())
#endif
{

}

UnixSymbolResolver::UnixSymbolResolver(pid_t pid)
        : m_pid(pid), m_attached(false)
#ifdef __APPLE__
        , m_task(MACH_PORT_NULL)
#endif
{
    if (pid != getpid() && !attachToProcess(pid))
    {
        throw std::runtime_error("Failed to attach to process " + std::to_string(pid));
    }
}

UnixSymbolResolver::~UnixSymbolResolver()
{
    if (m_attached)
    {
        detachFromProcess();
    }
}

bool UnixSymbolResolver::attachToProcess(int pid)
{
    if (pid == getpid())
    {
        m_pid = pid;
        m_attached = false;
        return true;
    }

#ifdef __APPLE__
        // First try task_for_pid
        kern_return_t kr = task_for_pid(mach_task_self(), pid, &m_task);
        if (kr == KERN_SUCCESS)
        {
            m_pid = pid;
            m_attached = true;
            return true;
        }

        int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, pid };
        struct kinfo_proc info;
        size_t size = sizeof(info);

        if (sysctl(mib, 4, &info, &size, NULL, 0) == 0)
        {
            m_pid = pid;
            m_attached = false;
            return true;
        }

        std::cerr << "Both task_for_pid and sysctl failed for pid " << pid
                  << ": " << mach_error_string(kr) << std::endl;
        return false;
#else
    if (ptrace(PTRACE_ATTACH, pid, nullptr, nullptr) == -1)
    {
        std::cerr << "ptrace attach failed: " << strerror(errno) << std::endl;
        return false;
    }

    int status;
    if (waitpid(pid, &status, 0) == -1)
    {
        std::cerr << "waitpid failed: " << strerror(errno) << std::endl;
        return false;
    }

    m_pid = pid;
    m_attached = true;
    return true;
#endif
}

bool UnixSymbolResolver::detachFromProcess()
{
    if (!m_attached) return true;

#ifdef __APPLE__
    mach_port_deallocate(mach_task_self(), m_task);
    m_task = MACH_PORT_NULL;
#else
    if (ptrace(PTRACE_DETACH, m_pid, nullptr, nullptr) == -1)
    {
        return false;
    }
#endif

    m_attached = false;
    return true;
}

bool UnixSymbolResolver::resolve(StackFrame& frame)
{
    void* address = frame.getAddress();
    frame.setFunctionName(getFunctionName(address));

    auto sourceInfo = getSourceInfo(address);
    if (!sourceInfo.first.empty())
    {
        frame.setFileName(sourceInfo.first);
        frame.setLineNumber(sourceInfo.second);
    }
    return true;
}

std::vector<StackFramePtr> UnixSymbolResolver::captureStackTrace(size_t maxDepth)
{
    std::vector<StackFramePtr> frames;

    if (!m_attached)
    {
        // Current process
        if (!captureCurrentProcessStackTrace(maxDepth, frames))
        {
            throw std::runtime_error("Failed to capture current process stack trace");
        }
    }
#ifdef __APPLE__
    else
    {
        if (!captureMachTaskStackTrace(maxDepth, frames))
        {
            throw std::runtime_error("Failed to capture Mach task stack trace");
        }
    }
#else
    else
    {
        // Attached process on Linux
        if (ptrace(PTRACE_GETREGS, m_pid, nullptr, nullptr) == -1)
        {
            std::cerr << "ptrace PTRACE_GETREGS failed: " << strerror(errno) << std::endl;
            return frames;
        }
        if (!captureCurrentProcessStackTrace(maxDepth, frames))
        {
            throw std::runtime_error("Failed to capture stack trace for attached process");
        }

        // now we can resolve the frames
        for (auto& frame : frames)
        {
            if (!resolve(*frame))
            {
                std::cerr << "Failed to resolve stack frame: " << frame->toString() << std::endl;
            }
        }
    }
#endif

    return frames;
}

bool UnixSymbolResolver::captureCurrentProcessStackTrace(size_t maxDepth, std::vector<StackFramePtr>& frames)
{
    constexpr size_t MAX_STACK_DEPTH = 128;
    void* callstack[MAX_STACK_DEPTH];
    int framesCaptured = backtrace(callstack, static_cast<int>(std::min(maxDepth, MAX_STACK_DEPTH)));

    for (int i = 0; i < framesCaptured; ++i)
    {
        frames.push_back(std::make_shared<StackFrame>(callstack[i]));
    }
    return true;
}

#ifdef __APPLE__
bool UnixSymbolResolver::captureMachTaskStackTrace(size_t maxDepth, std::vector<StackFramePtr>& frames)
{
    thread_act_port_array_t thread_list;
    mach_msg_type_number_t thread_count;

    kern_return_t kr = task_threads(m_task, &thread_list, &thread_count);
    if (kr != KERN_SUCCESS)
    {
        std::cerr << "task_threads failed: " << mach_error_string(kr) << std::endl;
        return false;
    }

    for (mach_msg_type_number_t i = 0; i < thread_count; i++)
    {
#if defined(__x86_64__)
        x86_thread_state64_t state;
        mach_msg_type_number_t state_count = x86_THREAD_STATE64_COUNT;
        kr = thread_get_state(thread_list[i], x86_THREAD_STATE64,
                            (thread_state_t)&state, &state_count);
        if (kr == KERN_SUCCESS)
        {
            frames.push_back(std::make_shared<StackFrame>((void*)state.__rip));
        }
#elif defined(__arm64__)
        arm_thread_state64_t state;
        mach_msg_type_number_t state_count = ARM_THREAD_STATE64_COUNT;
        kr = thread_get_state(thread_list[i], ARM_THREAD_STATE64,
                              (thread_state_t)&state, &state_count);
        if (kr == KERN_SUCCESS)
        {
            frames.push_back(std::make_shared<StackFrame>((void*)state.__pc));
        }
#endif
    }

    // Cleanup
    for (mach_msg_type_number_t i = 0; i < thread_count; i++)
    {
        mach_port_deallocate(mach_task_self(), thread_list[i]);
    }
    vm_deallocate(mach_task_self(), (vm_address_t)thread_list,
                  thread_count * sizeof(thread_act_t));

    return !frames.empty();
}
#endif

std::string UnixSymbolResolver::demangle(const char* name)
{
    int status = 0;
    std::unique_ptr<char, void(*)(void*)> demangled(
            abi::__cxa_demangle(name, nullptr, nullptr, &status),
            std::free
    );
    return (status == 0) ? demangled.get() : name;
}

std::string UnixSymbolResolver::getFunctionName(void* address)
{
    Dl_info info;
    if (dladdr(address, &info) && info.dli_sname)
    {
        return demangle(info.dli_sname);
    }
    return "";
}

std::pair<std::string, uint32_t> UnixSymbolResolver::getSourceInfo(void* address)
{
    Dl_info info;
    if (dladdr(address, &info))
    {
        return {info.dli_fname, 0};
    }
    return {"", 0};
}