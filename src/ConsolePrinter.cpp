#include "ConsolePrinter.h"
#include <iomanip>
#include <sstream>

using namespace mex;

ConsolePrinter::ConsolePrinter(std::ostream& os) : m_os(os), m_colorEnabled(true) {}

void ConsolePrinter::printStackTrace(const std::vector<StackFramePtr>& frames) const
{
    for (size_t i = 0; i < frames.size(); ++i)
    {
        printFrame(i, frames[i]);
    }
}

void ConsolePrinter::setColorEnabled(bool enabled)
{
    m_colorEnabled = enabled;
}

void ConsolePrinter::printFrame(size_t index, const StackFramePtr& frame) const
{
    if (!frame) return;

    std::string output = formatFrame(index, frame);
    m_os << output << std::endl;
}

std::string ConsolePrinter::formatFrame(size_t index, const StackFramePtr& frame) const
{
    std::ostringstream oss;
    oss << "[" << std::setw(3) << index << "] "
        << colorize(frame->getFunctionName(), Color::BrightBlue) << " at "
        << colorize(frame->getFileName(), Color::Cyan) << ":"
        << colorize(std::to_string(frame->getLineNumber()), Color::Green);
    return oss.str();
}

std::string ConsolePrinter::colorize(const std::string& text, Color color) const
{
    if (!m_colorEnabled) return text;

    const char* code = "";
    switch (color)
    {
        case Color::Red: code = "\033[31m"; break;
        case Color::Green: code = "\033[32m"; break;
        case Color::Yellow: code = "\033[33m"; break;
        case Color::Blue: code = "\033[34m"; break;
        case Color::Magenta: code = "\033[35m"; break;
        case Color::Cyan: code = "\033[36m"; break;
        case Color::White: code = "\033[37m"; break;
        case Color::BrightRed: code = "\033[91m"; break;
        case Color::BrightGreen: code = "\033[92m"; break;
        case Color::Reset: code = "\033[0m"; break;
        default: break;
    }

    return code + text + "\033[0m";
}