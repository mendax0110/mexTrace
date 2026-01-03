#include "StackFrame.h"
#include <utility>

StackFrame::StackFrame(
    const std::uintptr_t addr,
    std::string func,
    std::string file,
    const size_t line
) noexcept
    : m_address(addr)
    , m_functionName(std::move(func))
    , m_sourceFile(std::move(file))
    , m_lineNumber(line)
{

}

void StackFrame::setFunctionName(std::string name) noexcept
{
    m_functionName = std::move(name);
}

void StackFrame::setSourceFile(std::string file) noexcept
{
    m_sourceFile = std::move(file);
}

void StackFrame::setLineNumber(const std::size_t line) noexcept
{
    m_lineNumber = line;
}
