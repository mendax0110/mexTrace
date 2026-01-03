#include "ConsolePrinter.h"
#include <format>
#include <iostream>
#include <print>
#include <unistd.h>

ConsolePrinter::ConsolePrinter(std::ostream& os) noexcept
    : m_outputStream(os)
    , m_useColors(&os == &std::cout && isatty(STDOUT_FILENO))
{

}

ConsolePrinter::ConsolePrinter() noexcept
    : m_outputStream(std::cout)
    , m_useColors(isatty(STDOUT_FILENO))
{

}

void ConsolePrinter::printStackTrace(const std::vector<StackFrame>& frames) const
{
    std::println(m_outputStream, "{}", colorize("\nStack Trace:", Color::Cyan, true));

    for (size_t i = 0; i < frames.size(); ++i)
    {
        printFrame(frames[i], i);
    }

    std::println(m_outputStream, "");
}

void ConsolePrinter::printError(std::string_view message) const
{
    std::println(m_outputStream, "{}{}", colorize("[ERROR] ", Color::Red, true), message);
}

void ConsolePrinter::printSuccess(std::string_view message) const
{
    std::println(m_outputStream, "{}{}", colorize("[SUCCESS] ", Color::Green, true), message);
}

void ConsolePrinter::printInfo(std::string_view message) const
{
    std::println(m_outputStream, "{}{}", colorize("[INFO] ", Color::Blue, true), message);
}

void ConsolePrinter::printWarning(std::string_view message) const
{
    std::println(m_outputStream, "{}{}", colorize("[WARNING] ", Color::Yellow, true), message);
}

std::string ConsolePrinter::colorize(const std::string_view text, const Color color, const bool bold) const
{
    if (!m_useColors)
    {
        return std::string(text);
    }
    return std::format("{}{}\033[0m", getColorCode(color, bold), text);
}

std::string_view ConsolePrinter::getColorCode(const Color color, const bool bold)noexcept
{
    if (bold)
    {
        switch (color)
        {
            case Color::Red:     return "\033[1;31m";
            case Color::Green:   return "\033[1;32m";
            case Color::Yellow:  return "\033[1;33m";
            case Color::Blue:    return "\033[1;34m";
            case Color::Magenta: return "\033[1;35m";
            case Color::Cyan:    return "\033[1;36m";
            case Color::White:   return "\033[1;37m";
            default:             return "\033[1;39m";
        }
    }
    else
    {
        switch (color)
        {
            case Color::Red:     return "\033[0;31m";
            case Color::Green:   return "\033[0;32m";
            case Color::Yellow:  return "\033[0;33m";
            case Color::Blue:    return "\033[0;34m";
            case Color::Magenta: return "\033[0;35m";
            case Color::Cyan:    return "\033[0;36m";
            case Color::White:   return "\033[0;37m";
            default:             return "\033[0;39m";
        }
    }
}

void ConsolePrinter::printFrame(const StackFrame& frame, const size_t index) const
{
    std::string frameStr;

    if (!frame.hasSymbolInfo())
    {
        frameStr = std::format("{:2}# 0x{:x}", index, frame.getAddress());
    }
    else
    {
        frameStr = std::format("{:2}# {}", index, frame.getFunctionName());
        if (!frame.getSourceFile().empty())
        {
            frameStr += std::format(" at {}", frame.getSourceFile());
            if (frame.getLineNumber() != 0)
            {
                frameStr += std::format(":{}", frame.getLineNumber());
            }
        }
    }

    const auto frameColor = (index % 2) != 0 ? Color::Magenta : Color::Cyan;
    std::println(m_outputStream, "{}", colorize(frameStr, frameColor));
}