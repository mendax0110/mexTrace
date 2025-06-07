#include "SymbolResolver.h"
#include "PlatformUtils.h"

#ifdef MEXTRACE_WINDOWS
#include "WindowsSymbolResolver.h"
#else
#include "UnixSymbolResolver.h"
#endif


using namespace mex;

std::unique_ptr<SymbolResolver> SymbolResolver::create()
{
    if (!PlatformUtils::isPlatformSupported())
    {
        throw std::runtime_error("Unsupported platform");
    }

#ifdef _WIN32
    return std::make_unique<WindowsSymbolResolver>();
#else
    return std::make_unique<UnixSymbolResolver>();
#endif
}

std::unique_ptr<SymbolResolver> SymbolResolver::createForProcess(int pid)
{
    if (!PlatformUtils::isPlatformSupported())
    {
        throw std::runtime_error("Unsupported platform");
    }

#ifdef _WIN32
    return std::make_unique<WindowsSymbolResolver>(pid);
#else
    return std::make_unique<UnixSymbolResolver>(pid);
#endif
}