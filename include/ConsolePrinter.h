#pragma once
#include <vector>
#include <memory>
#include "StackFrame.h"
#include <iostream>
#include <string>

/// @brief ConsolePrinter is a utility class for printing formatted messages to the console. \class ConsolePrinter
class ConsolePrinter
{
public:

    /// @brief Enum representing different text colors for console output. \enum Color
    enum class Color
    {
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,
        DEFAULT
    };

    /**
     * @brief Constructs a ConsolePrinter that outputs to the specified stream.
     * @param os The output stream to use. Defaults to std::cout.
     */
    ConsolePrinter(std::ostream& os = std::cout);

    /**
     * @brief Destructor for ConsolePrinter.
     */
    virtual ~ConsolePrinter() = default;

    /**
     * @brief Prints a formatted stack trace to the console.
     * @param frames The vector of StackFrame objects representing the stack trace.
     */
    void printStackTrace(const std::vector<StackFrame>& frames) const;

    /**
     * @brief Prints an error message to the console.
     * @param message The error message to print.
     */
    void printError(const std::string& message) const;

    /**
     * @brief Prints a success message to the console.
     * @param message The success message to print.
     */
    void printSuccess(const std::string& message) const;

    /**
     * @brief Prints an informational message to the console.
     * @param message The informational message to print.
     */
    void printInfo(const std::string& message) const;

    /**
     * @brief Prints a warning message to the console.
     * @param message The warning message to print.
     */
    void printWarning(const std::string& message) const;

    /**
     * @brief Colorizes the given text with the specified color and optional bold formatting.
     * @param text The text to colorize.
     * @param color The color to apply to the text.
     * @param bold The optional flag to make the text bold. Defaults to false.
     * @return A string containing the colorized text.
     */
    std::string colorize(const std::string& text, Color color, bool bold = false) const;

private:
    std::ostream& outputStream;

    /**
     * @brief Returns the ANSI escape code for the specified color and boldness.
     * @param color The color to use.
     * @param bold The flag indicating whether the text should be bold.
     * @return A string containing the ANSI escape code for the specified color and boldness.
     */
    static std::string getColorCode(Color color, bool bold) ;

    /**
     * @brief Prints a single stack frame to the console.
     * @param frame The StackFrame object to print.
     * @param index The index of the frame in the stack trace.
     */
    void printFrame(const StackFrame& frame, size_t index) const;
};