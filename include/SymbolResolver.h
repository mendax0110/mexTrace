#ifndef MEXTRACE_SYMBOLRESOLVER_H
#define MEXTRACE_SYMBOLRESOLVER_H

#include "StackFrame.h"
#include <memory>
#include <vector>

/// @brief The mexTrace namespace contains classes and functions for capturing and printing stack traces. \namespace mex
namespace mex
{
    /// @brief SymbolResolver is an abstract base class for resolving symbols in stack frames and capturing stack traces. \class SymbolResolver
    class SymbolResolver
    {
    public:

        /**
         * @brief Default constructor for SymbolResolver.
         */
        virtual ~SymbolResolver() = default;

        /**
         * @brief Resolves the symbol information for a given stack frame.
         * @param frame The stack frame to resolve.
         * @return True if the resolution was successful, false otherwise.
         */
        virtual bool resolve(StackFrame &frame) = 0;

        /**
         * @brief Captures the current stack trace up to a specified maximum depth.
         * @param maxDepth The maximum depth of the stack trace to capture.
         * @return A vector of shared pointers to StackFrame objects representing the captured stack trace.
         */
        virtual std::vector<StackFramePtr> captureStackTrace(size_t maxDepth) = 0;

        /**
         * @brief Attaches the resolver to a specific process by its PID.
         * @param pid The process ID to attach to.
         * @return True if the attachment was successful, false otherwise.
         */
        virtual bool attachToProcess(int pid) = 0;

        /**
         * @brief Detaches the resolver from the currently attached process.
         * @return True if the detachment was successful, false otherwise.
         */
        virtual bool detachFromProcess() = 0;

        /**
         * @brief Checks if the resolver is currently attached to a process.
         * @return True if attached, false otherwise.
         */
        virtual bool isAttached() const = 0;

        /**
         * @brief Creates a new instance of SymbolResolver.
         * @return A unique pointer to a SymbolResolver instance.
         */
        static std::unique_ptr<SymbolResolver> create();

        /**
         * @brief Creates a SymbolResolver for a specific process by its PID.
         * @param pid The process ID to attach to.
         * @return A unique pointer to a SymbolResolver instance for the specified process.
         */
        static std::unique_ptr<SymbolResolver> createForProcess(int pid);
    };
} // namespace mex

#endif // MEXTRACE_SYMBOLRESOLVER_H