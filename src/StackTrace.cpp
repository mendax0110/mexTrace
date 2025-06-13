#include "StackTrace.h"
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/stacktrace.hpp>
#include <boost/stacktrace/frame.hpp>
#include "PlatformUtils.h"
#include <stdexcept>
#include "ErrorHandling.h"
#include <sys/wait.h>

StackTrace::StackTrace(std::unique_ptr<SymbolReceiver> receiver)
    : symbolReceiver(std::move(receiver))
{

}

StackTrace::~StackTrace() = default;

void StackTrace::setSymbolReceiver(std::unique_ptr<SymbolReceiver> receiver)
{
    symbolReceiver = std::move(receiver);
}

std::vector<StackFrame> StackTrace::captureCurrentThread() const
{
    const boost::stacktrace::stacktrace st;
    std::vector<StackFrame> frames;
    frames.reserve(st.size());

    for (const auto& frame : st)
    {
        frames.emplace_back(
            reinterpret_cast<uintptr_t>(frame.address()),
            frame.name(),
            frame.source_file(),
            frame.source_line()
        );
    }

    if (symbolReceiver)
    {
        resolveSymbols(frames);
    }

    return frames;
}

std::vector<StackFrame> StackTrace::captureThread(const pid_t tid) const
{
    if (tid == 0)
    {
        TRT_ERROR("Thread ID cannot be zero");
    }

    sigset_t old_mask;
    if (sigprocmask(SIG_SETMASK, nullptr, &old_mask))
    {
        TRT_ERROR("Failed to get current signal mask");
    }

    sigset_t new_mask;
    sigfillset(&new_mask);
    if (sigprocmask(SIG_SETMASK, &new_mask, nullptr))
    {
        TRT_ERROR("Failed to block signals");
    }

    try
    {
        if (ptrace(PTRACE_ATTACH, tid, nullptr, nullptr) == -1)
        {
            TRT_ERROR("Failed to attach to thread " + std::to_string(tid));
        }

        int status;
        if (waitpid(tid, &status, __WALL) == -1)
        {
            ptrace(PTRACE_DETACH, tid, nullptr, nullptr);
            TRT_ERROR("Failed to wait for thread " + std::to_string(tid));
        }

        auto frames = captureCurrentThread();

        if (ptrace(PTRACE_DETACH, tid, nullptr, nullptr) == -1)
        {
            TRT_ERROR("Failed to detach from thread " + std::to_string(tid));
        }

        if (sigprocmask(SIG_SETMASK, &old_mask, nullptr))
        {
            TRT_ERROR("Failed to restore signal mask");
        }

        return frames;
    }
    CATCH_WC(nullptr, "Failed to capture stack trace for thread " + std::to_string(tid),
    {
        ptrace(PTRACE_DETACH, tid, nullptr, nullptr);
        sigprocmask(SIG_SETMASK, &old_mask, nullptr);
        throw;
    })
}

std::vector<StackFrame> StackTrace::captureProcess(const pid_t pid) const
{
    if (!PlatformUtils::isProcessRunning(pid))
    {
        TRT_ERROR("Process " + std::to_string(pid) + " is not running");
    }

    if (!PlatformUtils::attachToProcess(pid))
    {
        TRT_ERROR("Failed to attach to process " + std::to_string(pid));
    }

    try
    {
        auto frames = captureCurrentThread();
        PlatformUtils::detachFromProcess(pid);
        return frames;
    }
    CATCH_WC(nullptr, "Failed to capture stack trace for process " + std::to_string(pid),
    {
        PlatformUtils::detachFromProcess(pid);
        throw;
    })
}

void StackTrace::resolveSymbols(std::vector<StackFrame>& frames) const
{
    for (auto& frame : frames)
    {
        if (frame.functionName.empty())
        {
            const auto resolved = symbolReceiver->resolve(frame.address);
            frame.functionName = resolved.functionName;
            frame.sourceFile = resolved.sourceFile;
            frame.lineNumber = resolved.lineNumber;
        }
    }
}
