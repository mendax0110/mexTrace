#include "PlatformUtils.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <array>
#include <charconv>
#include <format>
#include <regex>


bool PlatformUtils::isProcessRunning(const pid_t pid) noexcept
{
    return kill(pid, 0) == 0;
}

std::expected<std::vector<pid_t>, std::string> PlatformUtils::getAllProcesses() noexcept
{
    std::vector<pid_t> pids;
    DIR* dir = opendir("/proc");

    if (dir == nullptr)
    {
        return std::unexpected("Failed to open /proc directory");
    }

    const dirent* entry = nullptr;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_DIR)
        {
            pid_t pid = 0;
            auto [ptr, ec] = std::from_chars(entry->d_name, entry->d_name + std::strlen(entry->d_name), pid);
            if (ec == std::errc() && *ptr == '\0')
            {
                pids.push_back(pid);
            }
        }
    }

    closedir(dir);
    return pids;
}

std::optional<std::string> PlatformUtils::getProcessName(const pid_t pid) noexcept
{
    const auto path = std::format("/proc/{}/comm", pid);
    std::ifstream commFile(path);

    if (!commFile)
    {
        return std::nullopt;
    }

    std::string name;
    std::getline(commFile, name);
    return name.empty() ? std::nullopt : std::optional{name};
}


bool PlatformUtils::attachToProcess(const pid_t pid) noexcept
{
    if (ptrace(PTRACE_ATTACH, pid, nullptr, nullptr) == -1)
    {
        return false;
    }

    int status = 0;
    if (waitpid(pid, &status, 0) == -1)
    {
        return false;
    }

    return true;
}

void PlatformUtils::detachFromProcess(const pid_t pid) noexcept
{
    ptrace(PTRACE_DETACH, pid, nullptr, nullptr);
}

std::optional<std::string> PlatformUtils::getExecutablePath(const pid_t pid) noexcept
{
    std::array<char, PATH_MAX> path{};
    const auto procPath = std::format("/proc/{}/exe", pid);
    const auto len = readlink(procPath.c_str(), path.data(), path.size() - 1);

    if (len == -1)
    {
        return std::nullopt;
    }

    path[static_cast<size_t>(len)] = '\0';
    return std::string(path.data());
}

std::vector<StackFrame> PlatformUtils::readProcessStack(const pid_t pid) noexcept
{
    std::vector<StackFrame> frames;

    const auto execPath = getExecutablePath(pid);
    if (!execPath)
    {
        return frames;
    }

    user_regs_struct regs{};
    if (ptrace(PTRACE_GETREGS, pid, nullptr, &regs) == -1)
    {
        return frames;
    }

#if defined(__x86_64__)
    auto ip = regs.rip;
    auto bp = regs.rbp;
#elif defined(__i386__)
    auto ip = regs.eip;
    auto bp = regs.ebp;
#elif defined(__aarch64__)
    auto ip = regs.pc;
    auto bp = regs.regs[29];
#else
    return frames;
#endif

    frames.push_back(resolveAddress(*execPath, ip));

    constexpr std::size_t maxFrames = 64;
    for (std::size_t i = 0; i < maxFrames && bp != 0; ++i)
    {
        std::uintptr_t nextBp = 0;
        std::uintptr_t retAddr = 0;

        errno = 0;
        nextBp = static_cast<std::uintptr_t>(
            ptrace(PTRACE_PEEKDATA, pid, bp, nullptr)
        );
        if (errno != 0)
        {
            break;
        }

        errno = 0;
        retAddr = static_cast<std::uintptr_t>(
            ptrace(PTRACE_PEEKDATA, pid, bp + sizeof(void*), nullptr)
        );
        if (errno != 0)
        {
            break;
        }

        if (retAddr == 0)
        {
            break;
        }

        frames.push_back(resolveAddress(*execPath, retAddr));
        bp = nextBp;

        if (nextBp <= bp && nextBp != 0)
        {
            break;
        }
    }

    return frames;
}

std::optional<std::string> PlatformUtils::resolveSymbolicLink(const std::string_view path) noexcept
{
    std::array<char, PATH_MAX> resolved{};
    if (realpath(std::string(path).c_str(), resolved.data()) == nullptr)
    {
        return std::nullopt;
    }
    return std::string(resolved.data());
}

bool PlatformUtils::isAddr2lineAvailable() noexcept
{
    return access("/usr/bin/addr2line", X_OK) == 0 ||
            access("/usr/local/bin/addr2line", X_OK) == 0;
}

StackFrame PlatformUtils::resolveAddress(std::string_view execPath, uintptr_t address) noexcept
{
    StackFrame frame(address);

    if (!isAddr2lineAvailable())
    {
        return frame;
    }

    const auto cmd = std::format("addr2line - e {} -f -C -p {:x} 2>/dev/null", execPath, address);

    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe == nullptr)
    {
        return frame;
    }

    std::array<char, 512> buffer{};
    std::string result;
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
    {
        result += buffer.data();
    }

    if (result.empty() || result.find("??") != std::string::npos)
    {
        return frame;
    }

    if (!result.empty() && result.back() == '\n')
    {
        result.pop_back();
    }

    const auto atPos = result.find(" at ");
    if (atPos != std::string::npos)
    {
        frame.setFunctionName(result.substr(0, atPos));

        const auto localStr = result.substr(atPos, 4);
        const auto colonPos = localStr.rfind(':');
        if (colonPos != std::string::npos)
        {
            frame.setSourceFile(localStr.substr(0, colonPos));
            size_t lineNum = 0;
            const auto lineStr = localStr.substr(0, colonPos + 1);
            auto [ptr, ec] = std::from_chars(lineStr.data(), lineStr.data() + lineStr.size(), lineNum);

            if (ec == std::errc())
            {
                frame.setLineNumber(lineNum);
            }
        }
    }
    else
    {
        frame.setFunctionName(result);
    }

    return frame;
}
