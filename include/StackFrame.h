#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <compare>

/// @brief StackFrame represents a single frame in a stack trace, containing information about the function, source file, and line number. \class StackFrame
class StackFrame
{
public:

    /**
     * @brief Constructs a StackFrame with the specified address, function name, source file, and line number.
     * @param addr The memory address of the stack frame.
     * @param func The name of the function at this stack frame.
     * @param file The source file where this stack frame is defined.
     * @param line The line number in the source file where this stack frame is defined.
     */
    explicit StackFrame(
        uintptr_t addr,
        std::string  func = {},
        std::string  file = {},
        size_t line = 0) noexcept;

    /**
     * @brief Default constructor.
     */
    StackFrame() noexcept = default;

    /**
     * @brief Gets the memory address of this stack frame.
     * @return A uintptr_t representing the memory address of the stack frame.
     */
    [[nodiscard]] constexpr uintptr_t getAddress() const noexcept
    {
        return m_address;
    }

    /**
     * @brief Gets the name of the function at this stack frame.
     * @return A string_view representing the function name.
     */
    [[nodiscard]] std::string_view getFunctionName() const noexcept
    {
        return m_functionName;
    }

    /**
     * @brief Gets the source file where this stack frame is defined.
     * @return A string_view representing the source file.
     */
    [[nodiscard]] std::string_view getSourceFile() const noexcept
    {
        return m_sourceFile;
    }

    /**
     * @brief Gets the line number in the source file where this stack frame is defined.
     * @return A size_t representing the line number.
     */
    [[nodiscard]] constexpr size_t getLineNumber() const noexcept
    {
        return m_lineNumber;
    }

    /**
     * @brief Checks if this StackFrame has valid symbol information (function name).
     * @return A boolean indicating whether the StackFrame has symbol information.
     */
    [[nodiscard]] bool hasSymbolInfo() const noexcept
    {
        return !m_functionName.empty();
    }

    /**
     * @brief Sets the function name for this StackFrame.
     * @param name The function name to set.
     */
    void setFunctionName(std::string name) noexcept;

    /**
     * @brief Sets the source file for this StackFrame.
     * @param file The source file to set.
     */
    void setSourceFile(std::string file) noexcept;

    /**
     * @brief Sets the line number for this StackFrame.
     * @param line The line number to set.
     */
    void setLineNumber(size_t line) noexcept;

    /**
     * @brief Compares this StackFrame with another for ordering based on address.
     * @param other The other StackFrame to compare with.
     * @return A boolean indicating whether this StackFrame's address is less than the other's.
     */
    [[nodiscard]] auto operator<=>(const StackFrame& other) const noexcept = default;

private:
    uintptr_t m_address{0};
    std::string m_functionName;
    std::string m_sourceFile;
    size_t m_lineNumber{0};
};