#include "StackTrace.h"
#include "SymbolResolver.h"
#include "PlatformUtils.h"
#include <iostream>

using namespace mex;

StackTrace::StackTrace(size_t maxDepth)
        : m_maxDepth(maxDepth), m_resolver(SymbolResolver::create()), m_printer(std::cerr)
{
    if (!PlatformUtils::isPlatformSupported())
    {
        throw std::runtime_error("Unsupported platform: " + PlatformUtils::getPlatformName());
    }
}

StackTrace::StackTrace(int pid, size_t maxDepth)
        : m_maxDepth(maxDepth), m_resolver(SymbolResolver::createForProcess(pid)), m_printer(std::cerr)
{

}

void StackTrace::capture()
{
    m_frames = m_resolver->captureStackTrace(m_maxDepth);
    if (m_frames.empty())
    {
        throw std::runtime_error("Failed to capture stack trace");
    }
    for (auto& frame : m_frames)
    {
        if (!m_resolver->resolve(*frame))
        {
            throw std::runtime_error("Failed to resolve stack frame: " + frame->toString());
        }
    }
    m_printer.printStackTrace(m_frames);
}

const std::vector<StackFramePtr>& StackTrace::getFrames() const
{
    return m_frames;
}

void StackTrace::print() const
{
    m_printer.printStackTrace(m_frames);
}