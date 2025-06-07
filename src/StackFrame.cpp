#include "StackFrame.h"
#include <sstream>
#include <iomanip>

using namespace mex;

StackFrame::StackFrame(void* address) : m_address(address), m_lineNumber(0)
{

}

void* StackFrame::getAddress() const
{
    return m_address;
}

const std::string& StackFrame::getFunctionName() const
{
    return m_functionName;
}

const std::string& StackFrame::getFileName() const
{
    return m_fileName;
}

uint32_t StackFrame::getLineNumber() const
{
    return m_lineNumber;
}

void StackFrame::setFunctionName(const std::string& name)
{
    m_functionName = name;
}

void StackFrame::setFileName(const std::string& file)
{
    m_fileName = file;
}

void StackFrame::setLineNumber(uint32_t line)
{
    m_lineNumber = line;
}

std::string StackFrame::toString() const
{
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setw(16) << std::setfill('0') << reinterpret_cast<uintptr_t>(m_address)
        << " " << m_functionName << " at " << m_fileName << ":" << m_lineNumber;
    return oss.str();
}

StackFramePtr StackFrame::create(void* address)
{
    return std::make_shared<StackFrame>(address);
}

StackFramePtr StackFrame::create(const std::string& functionName, const std::string& fileName, uint32_t lineNumber)
{
    StackFramePtr frame = create(nullptr);
    frame->setFunctionName(functionName);
    frame->setFileName(fileName);
    frame->setLineNumber(lineNumber);
    return frame;
}