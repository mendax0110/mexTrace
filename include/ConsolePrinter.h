#ifndef MEXTRACE_CONSOLEPRINTER_H
#define MEXTRACE_CONSOLEPRINTER_H

#include "StackFrame.h"
#include <vector>
#include <ostream>
#include <iostream>

/// @brief The mexTrace namespace contains classes and functions for capturing and printing stack traces. \namespace mex
namespace mex
{
    /// @brief Enum class representing different colors for console output. \enum Color
    enum class Color
    {
        Reset,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        BrightRed,
        BrightGreen,
        BrightYellow,
        BrightBlue,
        BrightMagenta,
        BrightCyan,
        BrightWhite
    };

    /// @brief ConsolePrinter is responsible for printing stack traces to the console with optional color support. \class ConsolePrinter
    class ConsolePrinter
    {
    public:
        ConsolePrinter(std::ostream& os = std::cerr);

        /**
         * @brief Prints the stack trace to the console.
         * @param frames The stack frames to print.
         */
        void printStackTrace(const std::vector<StackFramePtr>& frames) const;

        /**
         * @brief Enables or disables colored output.
         * @param enabled The flag to enable or disable color output.
         */
        void setColorEnabled(bool enabled);

        /**
         * @brief Colorizes the given text with the specified color.
         * @param text The text to colorize.
         * @param color The color to apply to the text.
         * @return A string with ANSI escape codes for the specified color.
         */
        std::string colorize(const std::string& text, Color color) const;

    private:

        /**
         * @brief Prints a single stack frame to the console.
         * @param index The index of the frame in the stack trace.
         * @param frame The stack frame to print.
         */
        void printFrame(size_t index, const StackFramePtr& frame) const;

        /**
         * @brief Formats a stack frame for console output.
         * @param index The index of the frame in the stack trace.
         * @param frame The stack frame to format.
         * @return A formatted string representing the stack frame.
         */
        std::string formatFrame(size_t index, const StackFramePtr& frame) const;

        std::ostream& m_os;
        bool m_colorEnabled;
    };
} // namespace mex

#endif // MEXTRACE_CONSOLEPRINTER_H