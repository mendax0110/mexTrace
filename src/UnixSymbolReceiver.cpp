#include "UnixSymbolReceiver.h"
#include "PlatformUtils.h"
#include "ErrorHandling.h"
#include <boost/stacktrace.hpp>
#include <boost/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <iostream>


using namespace boost::stacktrace::detail;

UnixSymbolReceiver::UnixSymbolReceiver(const pid_t pid) : targetPid(pid)
{
    if (pid != 0)
    {
        executablePath = PlatformUtils::getExecutablePath(pid);
        if (executablePath.empty())
        {
            TRT_ERROR("Failed to get executable path for PID " + std::to_string(pid));
        }
    }
}

UnixSymbolReceiver::~UnixSymbolReceiver() = default;

StackFrame UnixSymbolReceiver::resolve(const uintptr_t address)
{
    StackFrame frame(address);

    try
    {
        boost::stacktrace::frame st_frame(reinterpret_cast<void*>(address));
        frame.functionName = st_frame.name();
        frame.sourceFile = st_frame.source_file();
        frame.lineNumber = st_frame.source_line();

        if (!frame.functionName.empty() &&
            frame.functionName.find("0x") == std::string::npos)
        {
            return frame;
        }
    }
    CATCH_WC(frame.functionName, "Failed to resolve symbol using backtrace", {});

    if (!executablePath.empty())
    {
        if (std::string resolved = resolveWithAddr2line(address); !resolved.empty())
        {
            if (const size_t paren_pos = resolved.find('('); paren_pos != std::string::npos)
            {
                frame.functionName = resolved.substr(0, paren_pos);

                if (const size_t colon_pos = resolved.find(':', paren_pos); colon_pos != std::string::npos)
                {
                    frame.sourceFile = resolved.substr(paren_pos + 1, colon_pos - paren_pos - 1);
                    try
                    {
                        frame.lineNumber = static_cast<size_t>(std::stoul(resolved.substr(colon_pos + 1)));
                    }
                    CATCH_WC(frame.lineNumber, "Failed to parse line number from addr2line output", {});
                }
            }
        }
    }

    return frame;
}

bool UnixSymbolReceiver::isAvailable() const
{
    return !executablePath.empty() && system("which addr2line > /dev/null 2>&1") == 0;
}

std::string UnixSymbolReceiver::resolveWithAddr2line(const uintptr_t address) const
{
    std::ostringstream cmd;
    cmd << "addr2line -e " << executablePath << " -f -C -p " << std::hex << address;

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) return "";

    char buffer[512];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }
    pclose(pipe);

    if (!result.empty() && result.back() == '\n')
    {
        result.pop_back();
    }

    return result;
}

std::string UnixSymbolReceiver::resolveWithBacktrace(const uintptr_t address)
{
    return boost::stacktrace::frame(reinterpret_cast<void*>(address)).name();
}