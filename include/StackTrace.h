#ifndef MEXTRACE_STACKTRACE_H
#define MEXTRACE_STACKTRACE_H

#include "StackFrame.h"
#include "SymbolResolver.h"
#include "ConsolePrinter.h"
#include <vector>

/// @brief The mexTrace namespace contains classes and functions for capturing and printing stack traces. \namespace mex
namespace mex
{
    /// @brief StackTrace captures and manages a stack trace, allowing for retrieval and printing of stack frames. \class StackTrace
    class StackTrace
    {
    public:

        /**
         * @brief Constructs a StackTrace object with a specified maximum depth.
         * @param maxDepth
         */
        explicit StackTrace(size_t maxDepth);

        /**
         * @brief Constructs a StackTrace object for a specific process ID with a specified maximum depth.
         * @param pid The process ID to attach to.
         * @param maxDepth The maximum depth of the stack trace.
         */
        explicit StackTrace(int pid, size_t maxDepth = 64);

        /**
         * @brief Captures the current stack trace.
         * @throws std::runtime_error if capturing the stack trace fails.
         */
        void capture();

        /**
         * @brief Gets the captured stack frames.
         * @return A vector of shared pointers to StackFrame objects.
         */
        const std::vector<StackFramePtr>& getFrames() const;

        /**
         * @brief Prints the captured stack trace to the console.
         */
        void print() const;

    private:
        size_t m_maxDepth;
        std::unique_ptr<SymbolResolver> m_resolver;
        ConsolePrinter m_printer;
        std::vector<StackFramePtr> m_frames;
    };
} // namespace mex

#endif // MEXTRACE_STACKTRACE_H