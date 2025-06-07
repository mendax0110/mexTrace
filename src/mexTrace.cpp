#include "StackTrace.h"
#include <iostream>
#include <string>
#include <vector>

void printHelp()
{
    std::cout << "mexTrace - Cross-platform stack trace utility\n"
              << "Usage: mexTrace [options] [pid]\n"
              << "Options:\n"
              << "  -h, --help       Show this help message\n"
              << "  -d, --depth NUM   Maximum stack depth (default: 64)\n"
              << "  -n, --no-color    Disable colored output\n"
              << "  -p, --pid PID     Attach to specified process ID\n";
}

int main(int argc, char* argv[])
{
    size_t depth = 64;
    bool useColor = true;
    int pid = -1;

    std::vector<std::string> args(argv + 1, argv + argc);
    for (size_t i = 0; i < args.size(); ++i)
    {
        if (args[i] == "-h" || args[i] == "--help")
        {
            printHelp();
            return 0;
        }
        else if (args[i] == "-d" || args[i] == "--depth")
        {
            if (i + 1 < args.size())
            {
                depth = std::stoul(args[++i]);
            }
        }
        else if (args[i] == "-n" || args[i] == "--no-color")
        {
            useColor = false;
        }
        else if (args[i] == "-p" || args[i] == "--pid")
        {
            if (i + 1 < args.size())
            {
                pid = std::stoi(args[++i]);
            }
        }
        else if (args[i][0] != '-')
        {
            pid = std::stoi(args[i]);
        }
    }

    try
    {
        mex::StackTrace trace(pid == -1 ? depth : pid, depth);
        trace.capture();
        trace.print();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}