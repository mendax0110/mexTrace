#pragma once
#include <vector>
#include <cstddef>
#include <stacktrace>
#include <expected>
#include <string>
#include <sys/types.h>
#include "StackFrame.h"

/// @brief StackTrace is a utility class for capturing and resolving stack traces in a process or thread. \class StackTrace
class StackTrace
{
public:

    /// @brief Enum representing the error codes for stack trace operations \enum Error
    enum class Error
    {
        ProcessNotRunning,
        AttachFailed,
        DetachFailed,
        CaptureFailed
    };

    /**
     * @brief Ctor for StackTrace.
     * @param maxDepth The maximum depth of the stack trace to capture.
     */
    explicit StackTrace(std::size_t maxDepth = 128) noexcept;

    /**
     * @brief Destructor for StackTrace.
     */
    ~StackTrace() = default;

    /**
     * @brief Sets the maximum depth of the stack trace to capture.
     * @param depth The maximum depth.
     */
    void setMaxDepth(size_t depth) noexcept;

    /**
     * @brief Captures the current thread's stack trace.
     * @return A vector of StackFrame objects.
     */
    [[nodiscard]] std::vector<StackFrame> captureCurrentThread() const;

    /**
     * @brief Captures the stack trace of a specific thread by its thread ID (TID).
     * @param pid The thread ID of the thread to capture the stack trace from.
     * @return A std::expected containing a vector of StackFrame objects on success, or an Error code on failure.
     */
    [[nodiscard]] std::expected<std::vector<StackFrame>, Error> captureProcess(pid_t pid) const;

    /**
     * @brief Converts an Error code to a human-readable string.
     * @param error The Error code to convert.
     * @return A string representation of the error.
     */
    [[nodiscard]] static std::string errorToString(Error error) noexcept;

private:
    size_t m_maxDepth;

    /**
     * @brief Converts a std::stacktrace to a vector of StackFrame objects.
     * @param st The std::stacktrace to convert.
     * @return A vector of StackFrame objects.
     */
    [[nodiscard]] std::vector<StackFrame> convertStackTrace(const std::stacktrace& st) const;
};