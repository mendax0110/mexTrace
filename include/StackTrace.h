#pragma once
#include <vector>
#include <memory>
#include "StackFrame.h"
#include "SymbolReceiver.h"

/// @brief StackTrace is a utility class for capturing and resolving stack traces in a process or thread. \class StackTrace
class StackTrace
{
public:

    /// @brief Enum representing different capture modes for stack traces. \enum CaptureMode
    enum class CaptureMode
    {
        CURRENT_THREAD,
        ALL_THREADS,
        SPECIFIC_THREAD,
        PROCESS
    };

    /**
     * @brief Constructs a StackTrace object with an optional SymbolReceiver.
     * @param receiver The SymbolReceiver to use for resolving symbols. If nullptr, default resolution will be used.
     */
    StackTrace(std::unique_ptr<SymbolReceiver> receiver = nullptr);

    /**
     * @brief Destructor for StackTrace.
     */
    ~StackTrace();

    /**
     * @brief Sets a custom SymbolReceiver for resolving symbols in stack frames.
     * @param receiver The SymbolReceiver to set. If nullptr, default resolution will be used.
     */
    void setSymbolReceiver(std::unique_ptr<SymbolReceiver> receiver);

    /**
     * @brief Captures the current thread's stack trace.
     * @return A vector of StackFrame objects representing the current thread's stack trace.
     */
    std::vector<StackFrame> captureCurrentThread() const;

    /**
     * @brief Captures the stack trace of a specific thread identified by its thread ID.
     * @param tid The thread ID of the thread to capture.
     * @return A vector of StackFrame objects representing the specified thread's stack trace.
     */
    std::vector<StackFrame> captureThread(pid_t tid) const;

    /**
     * @brief Captures the stack trace of a process identified by its process ID.
     * @param pid The process ID of the process to capture.
     * @return A vector of StackFrame objects representing the specified process's stack trace.
     */
    std::vector<StackFrame> captureProcess(pid_t pid) const;

private:
    std::unique_ptr<SymbolReceiver> symbolReceiver;
    size_t maxDepth = 128;
    CaptureMode captureMode = CaptureMode::CURRENT_THREAD;

    /**
     * @brief Resolves symbols in the provided stack frames using the SymbolReceiver.
     * @param frames The vector of StackFrame objects to resolve symbols for.
     */
    void resolveSymbols(std::vector<StackFrame>& frames) const;
};