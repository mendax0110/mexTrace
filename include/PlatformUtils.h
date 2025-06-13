#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <sys/types.h>
#include <signal.h>
#include <sys/ptrace.h>

/// @brief PlatformUtils is a utility class providing platform-specific functions for process management and symbol resolution. \class PlatformUtils
class PlatformUtils
{
public:
    /**
     * @brief Checks if a process with the given PID is currently running.
     * @param pid The process ID to check.
     * @return A boolean indicating whether the process is running.
     */
    static bool isProcessRunning(pid_t pid);

    /**
     * @brief Retrieves a list of all currently running processes.
     * @return A vector of process IDs (PIDs) for all running processes.
     */
    static std::vector<pid_t> getAllProcesses();

    /**
     * @brief Gets the name of a process by its PID.
     * @param pid The process ID for which to retrieve the name.
     * @return A string containing the name of the process, or an empty string if not found.
     */
    static std::string getProcessName(pid_t pid);

    /**
     * @brief Attaches to a process with the given PID using ptrace.
     * @param pid The process ID to attach to.
     * @return A boolean indicating whether the attachment was successful.
     */
    static bool attachToProcess(pid_t pid);

    /**
     * @brief Detaches from a process that was previously attached using ptrace.
     * @param pid The process ID to detach from.
     */
    static void detachFromProcess(pid_t pid);

    /**
     * @brief Retrieves the executable path of a process by its PID.
     * @param pid The process ID for which to retrieve the executable path.
     * @return A string containing the path to the executable, or an empty string if not found.
     */
    static std::string getExecutablePath(pid_t pid);

    /**
     * @brief Resolves a symbolic link to its target path.
     * @param path The path to the symbolic link.
     * @return A string containing the resolved path, or an empty string if the resolution fails.
     */
    static std::string resolveSymbolicLink(const std::string& path);

private:

    /**
     * @brief Private constructor to prevent instantiation of this utility class.
     */
    PlatformUtils() = delete;

    /**
     * @brief Private destructor to prevent deletion of this utility class.
     */
    ~PlatformUtils() = delete;
};