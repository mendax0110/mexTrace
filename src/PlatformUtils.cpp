#include "PlatformUtils.h"
#include <iostream>
#include <stdexcept>

using namespace mex;

PlatformUtils::Platform PlatformUtils::getPlatform()
{
#ifdef _WIN32
    return Platform::Windows;
#elif defined(__linux__)
    return Platform::Linux;
#elif defined(__APPLE__) || defined(__MACH__)
    return Platform::MacOS;
#else
    return Platform::Unknown;
#endif
}

bool PlatformUtils::isPlatformSupported()
{
    Platform platform = getPlatform();
    if (platform == Platform::Unknown)
    {
        std::cerr << "Unsupported platform detected." << std::endl;
        return false;
    }
    return true;
}

std::string PlatformUtils::getPlatformName()
{
    switch (getPlatform())
    {
        case Platform::Windows: return "Windows";
        case Platform::Linux: return "Linux";
        case Platform::MacOS: return "MacOS";
        default: return "Unknown";
    }
}