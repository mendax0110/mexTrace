#pragma once
#include <cstdint>
#include <string>

/// @brief StackFrame represents a single frame in a stack trace, containing information about the function, source file, and line number. \class StackFrame
class StackFrame
{
public:
    /**
     * @bnrief Constructs a StackFrame with the given address and optional function name, source file, and line number.
     * @param addr The memory address of the stack frame.
     * @param func The name of the function at this stack frame (optional).
     * @param file The source file where this stack frame is defined (optional).
     * @param line The line number in the source file where this stack frame is defined (optional).
     */
    explicit StackFrame(uintptr_t addr, std::string  func = "",
               std::string  file = "", size_t line = 0);

    /**
     * @brief Gets the memory address of this stack frame.
     * @return A uintptr_t representing the memory address of the stack frame.
     */
    uintptr_t getAddress() const;

    /**
     * @brief Gets the name of the function at this stack frame.
     * @return A const reference to a string containing the function name.
     */
    const std::string& getFunctionName() const;

    /**
     * @brief Gets the source file where this stack frame is defined.
     * @return A const reference to a string containing the source file name.
     */
    const std::string& getSourceFile() const;

    /**
     * @brief Gets the line number in the source file where this stack frame is defined.
     * @return A size_t representing the line number in the source file.
     */
    size_t getLineNumber() const;

    /**
     * @brief Compares this StackFrame with another StackFrame based on the memory address.
     * @param other The other StackFrame to compare against.
     * @return A boolean indicating whether this StackFrame's address is less than the other StackFrame's address.
     */
    bool operator<(const StackFrame& other) const;

    uintptr_t address;
    std::string functionName;
    std::string sourceFile;
    size_t lineNumber;
};