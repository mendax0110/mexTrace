#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <expected>
#include <optional>
#include <sys/types.h>
#include "StackFrame.h"

/// @brief PlatformUtils is a utility class providing platform-specific functions for process management and symbol resolution. \class PlatformUtils
class PlatformUtils
{
public:

    /**
     * @brief Checks if a process with the given PID is currently running.
     * @param pid The process ID to check.
     * @return A boolean indicating whether the process is running.
     */
    [[nodiscard]] static bool isProcessRunning(pid_t pid) noexcept;

    /**
     * @brief Retrieves a list of all currently running process IDs (PIDs) on the system.
     * @return A std::expected containing a vector of PIDs on success, or an error message on failure.
     */
    [[nodiscard]] static std::expected<std::vector<pid_t>, std::string> getAllProcesses() noexcept;

    /**
     * @brief Retrieves the name of a process by its PID.
     * @param pid The process ID for which to retrieve the name.
     * @return An optional string containing the process name, or std::nullopt if not found.
     */
    [[nodiscard]] static std::optional<std::string> getProcessName(pid_t pid) noexcept;

    /**
     * @brief Attaches to a process using ptrace.
     * @param pid The process ID to attach to.
     * @return A boolean indicating whether the attachment was successful.
     */
    [[nodiscard]] static bool attachToProcess(pid_t pid) noexcept;

    /**
     * @brief Detaches from a process using ptrace.
     * @param pid The process ID to detach from.
     */
    static void detachFromProcess(pid_t pid) noexcept;

    /**
     * @brief Gets the executable path of a process by its PID.
     * @param pid The process ID for which to retrieve the executable path.
     * @return A std::optional string containing the executable path, or std::nullopt if not found.
     */
    [[nodiscard]] static std::optional<std::string> getExecutablePath(pid_t pid) noexcept;

    /**
     * @brief Read the process stack.
     * @param pid The process ID for which to retrieve the Stack
     * @return A vector of stackframes
     */
    [[nodiscard]] static std::vector<StackFrame> readProcessStack(pid_t pid) noexcept;

    /**
     * @brief Resolves a symbolic link to its target path.
     * @param path The symbolic link path to resolve.
     * @return A std::optional string containing the resolved path, or std::nullopt if resolution fails.
     */
    [[nodiscard]] static std::optional<std::string> resolveSymbolicLink(std::string_view path) noexcept;

    /**
     * @brief Checks if the 'addr2line' utility is available on the system.
     * @return A boolean indicating whether 'addr2line' is available.
     */
    [[nodiscard]] static bool isAddr2lineAvailable() noexcept;

    /**
     * @brief Resolves a memory address to a StackFrame using 'addr2line'.
     * @param execPath The path to the executable.
     * @param address The memory address to resolve.
     * @return A StackFrame object containing the resolved information.
     */
    [[nodiscard]] static StackFrame resolveAddress(std::string_view execPath, uintptr_t address) noexcept;

private:

    /**
     * @brief Private destructor to prevent deletion of this utility class.
     */
    ~PlatformUtils() = delete;
};