#include "WindowsSymbolResolver.h"
#include "StackFrame.h"
#include <vector>
#include <memory>
#include <stdexcept>

using namespace mex;

WindowsSymbolResolver::WindowsSymbolResolver()
        : m_process(GetCurrentProcess()), m_attached(false), m_shouldCleanup(true)
{
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
    if (!SymInitialize(m_process, nullptr, TRUE))
    {
        throw std::runtime_error("Failed to initialize symbol handler");
    }
}

WindowsSymbolResolver::WindowsSymbolResolver(HANDLE process)
        : m_process(process), m_attached(false), m_shouldCleanup(false)
{
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
    if (!SymInitialize(m_process, nullptr, TRUE))
    {
        throw std::runtime_error("Failed to initialize symbol handler");
    }
}

WindowsSymbolResolver::~WindowsSymbolResolver()
{
    if (m_shouldCleanup)
    {
        SymCleanup(m_process);
    }
}

bool WindowsSymbolResolver::attachToProcess(int pid)
{
    m_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!m_process)
    {
        return false;
    }

    if (!SymInitialize(m_process, nullptr, TRUE))
    {
        CloseHandle(m_process);
        return false;
    }

    m_attached = true;
    m_shouldCleanup = true;
    return true;
}

bool WindowsSymbolResolver::detachFromProcess()
{
    if (!m_attached) return true;

    SymCleanup(m_process);
    CloseHandle(m_process);
    m_attached = false;
    return true;
}

bool WindowsSymbolResolver::resolve(StackFrame& frame)
{
    void* address = frame.getAddress();
    frame.setFunctionName(getFunctionName(address));

    DWORD lineNumber = 0;
    std::string sourceFile = getSourceFile(address, lineNumber);
    if (!sourceFile.empty())
    {
        frame.setFileName(sourceFile);
        frame.setLineNumber(lineNumber);
    }
    return true;
}

std::vector<StackFramePtr> WindowsSymbolResolver::captureStackTrace(size_t maxDepth)
{
    std::vector<StackFramePtr> frames;
    void* stack[64];
    unsigned short framesCaptured = CaptureStackBackTrace(0,
                                                          static_cast<DWORD>(std::min(maxDepth, static_cast<size_t>(64))),
                                                          stack, nullptr);

    for (unsigned short i = 0; i < framesCaptured; ++i)
    {
        frames.push_back(std::make_shared<StackFrame>(stack[i]));
    }
    return frames;
}

std::string WindowsSymbolResolver::getFunctionName(void* address)
{
    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    DWORD64 displacement = 0;
    if (SymFromAddr(m_process, (DWORD64)address, &displacement, symbol))
    {
        return symbol->Name;
    }
    return "";
}

std::string WindowsSymbolResolver::getSourceFile(void* address, DWORD& lineNumber)
{
    IMAGEHLP_LINE64 line;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    DWORD displacement = 0;
    if (SymGetLineFromAddr64(m_process, (DWORD64)address, &displacement, &line))
    {
        lineNumber = line.LineNumber;
        return line.FileName;
    }
    return "";
}