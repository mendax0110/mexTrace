#include "StackTrace.h"
#include "ConsolePrinter.h"
#include "PlatformUtils.h"
#include <cstdlib>
#include <print>
#include <string>
#include <string_view>
#include <span>
#include <charconv>
#include <algorithm>

/// @brief Anonymous namespace
namespace
{
    /// @brief Options struct to hold command-line options. \struct Options
    struct Options
    {
        pid_t pid{0};
        bool list{false};
        bool verbose{false};
        bool help{false};
        bool self{false};
    };
}


void printHelp()
{
    const ConsolePrinter printer;
    printer.printInfo("Usage: mexTrace [options]");
    printer.printInfo("Options:");
    printer.printInfo("  -p, --pid <pid>   Attach to specified process ID");
    printer.printInfo("  -l, --list        List all running processes");
    printer.printInfo("  -s, --self        Capture stack trace of this process");
    printer.printInfo("  -h, --help        Show this help message");
    printer.printInfo("  -v, --verbose     Enable verbose output");
}

Options parseArgs(const std::span<char*> args)
{
    Options opts;

    for (size_t i = 1; i < args.size(); ++i)
    {
        const std::string_view arg = args[i];

        if (arg == "-h" || arg == "--help")
        {
            opts.help = true;
        }
        else if (arg == "-l" || arg == "--list")
        {
            opts.list = true;
        }
        else if (arg == "-s" || arg == "--self")
        {
            opts.self = true;
        }
        else if (arg == "-v" || arg == "--verbose")
        {
            opts.verbose = true;
        }
        else if ((arg == "-p" || arg == "--pid") && i + 1 < args.size())
        {
            ++i;
            const std::string_view pidStr = args[i];
            auto [ptr, ec] = std::from_chars(
                pidStr.data(),
                pidStr.data() + pidStr.size(),
                opts.pid
            );

            if (ec != std::errc())
            {
                opts.pid = 0;
            }
        }
    }

    return opts;
}

void listProcesses()
{
    const ConsolePrinter printer;
    auto result = PlatformUtils::getAllProcesses();

    if (!result)
    {
        printer.printError(result.error());
        return;
    }

    printer.printInfo("Running processes:");
    std::println("PID\tName");
    std::println("----------------");

    for (const pid_t pid : *result)
    {
        if (auto name = PlatformUtils::getProcessName(pid))
        {
            std::println("{}\t{}", pid, *name);
        }
    }
}

void attachToProcess(const pid_t pid, const bool verbose)
{
    const ConsolePrinter printer;

    if (!PlatformUtils::isProcessRunning(pid))
    {
        printer.printError(std::format("Process {} is not running", pid));
        return;
    }

    if (verbose)
    {
        printer.printInfo(std::format("Attaching to process: {}", pid));

        if (auto name = PlatformUtils::getProcessName(pid))
        {
            printer.printInfo(std::format("Process name: {}", *name));
        }

        if (auto path = PlatformUtils::getExecutablePath(pid))
        {
            printer.printInfo(std::format("Executable path: {}", *path));
        }
    }

    const StackTrace tracer;
    auto result = tracer.captureProcess(pid);

    if (!result)
    {
        printer.printError(StackTrace::errorToString(result.error()));
        return;
    }

    printer.printStackTrace(*result);
    printer.printSuccess(std::format("Captured stack trace for process {}", pid));
}

void captureOwnStack()
{
    const ConsolePrinter printer;
    const StackTrace tracer;

    const auto frames = tracer.captureCurrentThread();
    printer.printStackTrace(frames);
    printer.printSuccess("Captured current thread stack trace");
}

int main(const int argc, char* argv[])
{
    const auto args = std::span(argv, static_cast<std::size_t>(argc));
    const auto [pid, list, verbose, help, self] = parseArgs(args);
    const ConsolePrinter printer;

    if (help)
    {
        printHelp();
        return EXIT_SUCCESS;
    }

    if (list)
    {
        listProcesses();
        return EXIT_SUCCESS;
    }

    if (self)
    {
        captureOwnStack();
        return EXIT_SUCCESS;
    }

    if (pid != 0)
    {
        attachToProcess(pid, verbose);
        return EXIT_SUCCESS;
    }

    printer.printWarning("No operation specified. Use -h for help.");
    printHelp();
    return EXIT_FAILURE;
}