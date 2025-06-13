#include "StackFrame.h"

#include <utility>

StackFrame::StackFrame(const uintptr_t addr, std::string  func,
                       std::string  file, const size_t line)
    : address(addr)
    , functionName(std::move(func))
    , sourceFile(std::move(file))
    , lineNumber(line)
{

}

uintptr_t StackFrame::getAddress() const
{
    return address;
}

const std::string& StackFrame::getFunctionName() const
{
    return functionName;
}

const std::string& StackFrame::getSourceFile() const
{
    return sourceFile;
}

size_t StackFrame::getLineNumber() const
{
    return lineNumber;
}

bool StackFrame::operator<(const StackFrame& other) const
{
    return address < other.address;
}