#include "PlatformUtils.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <stdexcept>

bool PlatformUtils::isProcessRunning(const pid_t pid)
{
    return kill(pid, 0) == 0;
}

std::vector<pid_t> PlatformUtils::getAllProcesses()
{
    std::vector<pid_t> pids;
    DIR* dir = opendir("/proc");

    if (!dir)
    {
        throw std::runtime_error("Failed to open /proc directory");
    }

    dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_DIR)
        {
            char* end;
            const long pid = strtol(entry->d_name, &end, 10);
            if (*end == '\0')
            {
                pids.push_back(static_cast<pid_t>(pid));
            }
        }
    }

    closedir(dir);
    return pids;
}

std::string PlatformUtils::getProcessName(const pid_t pid)
{
    const std::string path = "/proc/" + std::to_string(pid) + "/comm";
    std::ifstream commFile(path);

    if (!commFile)
    {
        return "";
    }

    std::string name;
    std::getline(commFile, name);
    return name;
}

bool PlatformUtils::attachToProcess(const pid_t pid)
{
    return ptrace(PTRACE_ATTACH, pid, nullptr, nullptr) == 0;
}

void PlatformUtils::detachFromProcess(const pid_t pid)
{
    ptrace(PTRACE_DETACH, pid, nullptr, nullptr);
}

std::string PlatformUtils::getExecutablePath(const pid_t pid)
{
    char path[PATH_MAX];
    const auto procPath = "/proc/" + std::to_string(pid) + "/exe";
    const auto len = readlink(procPath.c_str(), path, sizeof(path) - 1);

    if (len == -1)
    {
        return "";
    }

    path[len] = '\0';
    return path;
}

std::string PlatformUtils::resolveSymbolicLink(const std::string& path)
{
    char resolved[PATH_MAX];
    if (realpath(path.c_str(), resolved) == nullptr)
    {
        return path;
    }
    return resolved;
}