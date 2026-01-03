#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <ostream>
#include <cstdint>
#include <memory>
#include "StackFrame.h"

/// @brief ConsolePrinter is a utility class for printing formatted messages to the console. \class ConsolePrinter
class ConsolePrinter
{
public:

    /// @brief Enum representing different text colors for console output. \enum Color
    enum class Color : uint8_t
    {
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        Default
    };

    /**
     * @brief Constructs a ConsolePrinter that outputs to the specified stream.
     * @param os The output stream to use. Defaults to std::cout.
     */
    ConsolePrinter(std::ostream& os) noexcept;

    /**
     * @brief Default Ctor.
     */
    ConsolePrinter() noexcept;

    /**
     * @brief Destructor for ConsolePrinter.
     */
    ~ConsolePrinter() = default;

    /**
     * @brief Prints a formatted stack trace to the console.
     * @param frames The vector of StackFrame objects representing the stack trace.
     */
    void printStackTrace(const std::vector<StackFrame>& frames) const;

    /**
     * @brief Prints an error message to the console.
     * @param message The error message to print.
     */
    void printError(std::string_view message) const;

    /**
     * @brief Prints a success message to the console.
     * @param message The success message to print.
     */
    void printSuccess(std::string_view message) const;

    /**
     * @brief Prints an informational message to the console.
     * @param message The informational message to print.
     */
    void printInfo(std::string_view message) const;

    /**
     * @brief Prints a warning message to the console.
     * @param message The warning message to print.
     */
    void printWarning(std::string_view message) const;

    /**
     * @brief Colorizes the given text with the specified color and optional bold formatting.
     * @param text The text to colorize.
     * @param color The color to apply to the text.
     * @param bold The optional flag to make the text bold. Defaults to false.
     * @return A string containing the colorized text.
     */
    [[nodiscard]] std::string colorize(std::string_view text, Color color, bool bold = false) const;

private:
    std::ostream& m_outputStream;
    bool m_useColors;

    /**
     * @brief Returns the ANSI escape code for the specified color and boldness.
     * @param color The color to use.
     * @param bold The flag indicating whether the text should be bold.
     * @return A string_view containing the ANSI escape code for the specified color and boldness.
     */
    [[nodiscard]] static std::string_view getColorCode(Color color, bool bold) noexcept;

    /**
     * @brief Prints a single stack frame to the console.
     * @param frame The StackFrame object to print.
     * @param index The index of the frame in the stack trace.
     */
    void printFrame(const StackFrame& frame, size_t index) const;
};