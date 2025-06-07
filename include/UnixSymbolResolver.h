#ifndef MEXTRACE_UNIXSYMBOLRESOLVER_H
#define MEXTRACE_UNIXSYMBOLRESOLVER_H

#include "SymbolResolver.h"
#include <vector>
#include <sys/types.h>

/// @brief The mexTrace namespace contains classes and functions for capturing and printing stack traces. \namespace mex
namespace mex
{
    /// @brief UnixSymbolResolver is a concrete implementation of SymbolResolver for Unix-like systems, providing methods to resolve symbols and capture stack traces. \class UnixSymbolResolver
    class UnixSymbolResolver : public SymbolResolver
    {
    public:

        /**
         * @brief Default constructor for UnixSymbolResolver, initializes for the current process.
         */
        UnixSymbolResolver();

        /**
         * @brief Constructs a UnixSymbolResolver for a specific process ID.
         * @param pid The process ID to attach to.
         */
        explicit UnixSymbolResolver(pid_t pid);

        /**
         * @brief Destructor for UnixSymbolResolver, cleans up resources.
         */
        ~UnixSymbolResolver() override;

        /**
         * @brief Resolves the symbol information for a given stack frame.
         * @param frame The stack frame to resolve.
         * @return True if the resolution was successful, false otherwise.
         */
        bool resolve(StackFrame& frame) override;

        /**
         * @brief Captures the current stack trace up to a specified maximum depth.
         * @param maxDepth The maximum depth of the stack trace to capture.
         * @return A vector of shared pointers to StackFrame objects representing the captured stack trace.
         */
        std::vector<StackFramePtr> captureStackTrace(size_t maxDepth) override;

        /**
         * @brief Attaches the resolver to a specific process by its PID.
         * @param pid The process ID to attach to.
         * @return True if the attachment was successful, false otherwise.
         */
        bool attachToProcess(int pid) override;

        /**
         * @brief Detaches the resolver from the currently attached process.
         * @return True if the detachment was successful, false otherwise.
         */
        bool detachFromProcess() override;

        /**
         * @brief Checks if the resolver is currently attached to a process.
         * @return True if attached, false otherwise.
         */
        bool isAttached() const override { return m_attached; }

    private:
        pid_t m_pid;
        bool m_attached;
#ifdef __APPLE__
        mach_port_t m_task;
#endif

        /**
         * @brief Demangles a C++ symbol name.
         * @param name The mangled symbol name.
         * @return The demangled symbol name.
         */
        std::string demangle(const char* name);

        /**
         * @brief Gets the function name for a given address.
         * @param address The address to resolve.
         * @return The function name as a string.
         */
        std::string getFunctionName(void* address);

        /**
         * @brief Gets the source file name and line number for a given address.
         * @param address The address to resolve.
         * @return A pair containing the source file name and line number.
         */
        std::pair<std::string, uint32_t> getSourceInfo(void* address);

        /**
         * @brief Captures the stack trace for the current process.
         * @param maxDepth The maximum depth of the stack trace to capture.
         * @param frames The vector to fill with captured stack frames.
         * @return True if the capture was successful, false otherwise.
         */
        bool captureCurrentProcessStackTrace(size_t maxDepth, std::vector<StackFramePtr>& frames);
#ifdef __APPLE__

        /**
         * @brief Captures the stack trace for a Mach task.
         * @param maxDepth The maximum depth of the stack trace to capture
         * @param frames The vector to fill with captured stack frames.
         * @return True if the capture was successful, false otherwise.
         */
        bool captureMachTaskStackTrace(size_t maxDepth, std::vector<StackFramePtr>& frames);
#endif
    };

} // namespace mex

#endif // MEXTRACE_UNIXSYMBOLRESOLVER_H