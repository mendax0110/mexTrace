#ifndef MEXTRACE_STACKFRAME_H
#define MEXTRACE_STACKFRAME_H

#include <cstdint>
#include <string>
#include <memory>

/// @brief The mexTrace namespace contains classes and functions for capturing and printing stack frames. \namespace mex
namespace mex
{
    /// @brief StackFrame represents a single frame in a stack trace, containing information about the function, file, and line number. \class StackFrame
    class StackFrame
    {
    public:
        StackFrame(void* address);

        /**
         * @brief Gets the address of the stack frame.
         * @return A pointer to the address of the stack frame.
         */
        void* getAddress() const;

        /**
         * @brief Gets the function name associated with the stack frame.
         * @return A string containing the function name.
         */
        const std::string& getFunctionName() const;

        /**
         * @brief Gets the file name associated with the stack frame.
         * @return A string containing the file name.
         */
        const std::string& getFileName() const;

        /**
         * @brief Gets the line number associated with the stack frame.
         * @return An unsigned integer representing the line number.
         */
        uint32_t getLineNumber() const;

        /**
         * @brief Sets the function name for the stack frame.
         * @param name The function name to set.
         */
        void setFunctionName(const std::string& name);

        /**
         * @brief Sets the file name for the stack frame.
         * @param file The file name to set.
         */
        void setFileName(const std::string& file);

        /**
         * @brief Sets the line number for the stack frame.
         * @param line The line number to set.
         */
        void setLineNumber(uint32_t line);

        /**
         * @brief Converts the stack frame to a string representation.
         * @return A string containing the formatted stack frame information.
         */
        std::string toString() const;

        /**
         * @brief Creates a shared pointer to a StackFrame with the specified address.
         * @param address The address of the stack frame.
         * @return A shared pointer to the created StackFrame.
         */
        static std::shared_ptr<StackFrame> create(void* address);

        /**
         * @brief Creates a shared pointer to a StackFrame with the specified function name, file name, and line number.
         * @param functionName The name of the function.
         * @param fileName The name of the file.
         * @param lineNumber The line number in the file.
         * @return A shared pointer to the created StackFrame.
         */
        static std::shared_ptr<StackFrame> create(const std::string& functionName, const std::string& fileName, uint32_t lineNumber);

    private:
        void* m_address;
        std::string m_functionName;
        std::string m_fileName;
        uint32_t m_lineNumber;
    };

    /// @brief A shared pointer type for StackFrame objects. \typedef StackFramePtr
    using StackFramePtr = std::shared_ptr<StackFrame>;

} // namespace mex

#endif // MEXTRACE_STACKFRAME_H