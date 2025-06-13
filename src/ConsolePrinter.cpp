#include "ConsolePrinter.h"
#include <iomanip>
#include <sstream>

ConsolePrinter::ConsolePrinter(std::ostream& os) : outputStream(os) {}

void ConsolePrinter::printStackTrace(const std::vector<StackFrame>& frames) const
{
    outputStream << colorize("\nStack Trace:\n", Color::CYAN, true);

    for (size_t i = 0; i < frames.size(); ++i)
    {
        printFrame(frames[i], i);
    }

    outputStream << "\n";
}

void ConsolePrinter::printError(const std::string& message) const
{
    outputStream << colorize("[ERROR] ", Color::RED, true) << message << "\n";
}

void ConsolePrinter::printSuccess(const std::string& message) const
{
    outputStream << colorize("[SUCCESS] ", Color::GREEN, true) << message << "\n";
}

void ConsolePrinter::printInfo(const std::string& message) const
{
    outputStream << colorize("[INFO] ", Color::BLUE, true) << message << "\n";
}

void ConsolePrinter::printWarning(const std::string& message) const
{
    outputStream << colorize("[WARNING] ", Color::YELLOW, true) << message << "\n";
}

std::string ConsolePrinter::colorize(const std::string& text, const Color color, const bool bold) const
{
    if (&outputStream != &std::cout)
    {
        return text;
    }
    return getColorCode(color, bold) + text + "\033[0m";
}

std::string ConsolePrinter::getColorCode(const Color color, const bool bold)
{
    std::string code = "\033[";
    if (bold) code += "1;";
    else code += "0;";

    switch (color)
    {
        case Color::RED: code += "31"; break;
        case Color::GREEN: code += "32"; break;
        case Color::YELLOW: code += "33"; break;
        case Color::BLUE: code += "34"; break;
        case Color::MAGENTA: code += "35"; break;
        case Color::CYAN: code += "36"; break;
        case Color::WHITE: code += "37"; break;
        default: code += "39"; break;
    }

    return code + "m";
}

void ConsolePrinter::printFrame(const StackFrame& frame, const size_t index) const
{
    std::ostringstream oss;
    oss << std::setw(2) << index << "# ";

    if (frame.functionName.empty())
    {
        oss << "0x" << std::hex << frame.address;
    }
    else
    {
        oss << frame.functionName;
        if (!frame.sourceFile.empty())
        {
            oss << " at " << frame.sourceFile;
            if (frame.lineNumber != 0)
            {
                oss << ":" << std::dec << frame.lineNumber;
            }
        }
    }

    const Color frameColor = (index % 2) ? Color::MAGENTA : Color::CYAN;
    outputStream << colorize(oss.str(), frameColor) << "\n";
}