#ifndef MEXTRACE_WINDOWSSYMBOLRESOLVER_H
#define MEXTRACE_WINDOWSSYMBOLRESOLVER_H

#include "SymbolResolver.h"
#include <windows.h>
#include <vector>

/// @brief The mexTrace namespace contains classes and functions for capturing and printing stack traces. \namespace mex
namespace mex
{
    /// @brief WindowsSymbolResolver is a concrete implementation of SymbolResolver for Windows, providing methods to resolve symbols and capture stack traces. \class WindowsSymbolResolver
    class WindowsSymbolResolver : public SymbolResolver
    {
    public:

        /**
         * @brief Default constructor for WindowsSymbolResolver, initializes for the current process.
         */
        WindowsSymbolResolver();

        /**
         * @brief Constructs a WindowsSymbolResolver for a specific process handle.
         * @param process The handle to the process to attach to.
         */
        explicit WindowsSymbolResolver(HANDLE process);

        /**
         * @brief Destructor for WindowsSymbolResolver, cleans up resources.
         */
        ~WindowsSymbolResolver() override;

        /**
         * @brief Resolves the symbol information for a given stack frame.
         * @param frame The stack frame to resolve.
         * @return True if the resolution was successful, false otherwise.
         */
        bool resolve(StackFrame &frame) override;

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
        HANDLE m_process;
        bool m_attached;
        bool m_shouldCleanup;

        /**
         * @brief Gets the function name for a given address.
         * @param address Address of the function to resolve.
         * @return A string containing the function name.
         */
        std::string getFunctionName(void *address);

        /**
         * @brief Gets the source file and line number for a given address.
         * @param address Address of the code to resolve.
         * @param lineNumber Reference to store the line number.
         * @return A string containing the source file name.
         */
        std::string getSourceFile(void *address, DWORD &lineNumber);
    };
} // namespace mex

#endif // MEXTRACE_WINDOWSSYMBOLRESOLVER_H