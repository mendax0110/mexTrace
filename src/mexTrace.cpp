#include "StackTrace.h"
#include "UnixSymbolReceiver.h"
#include "ConsolePrinter.h"
#include "PlatformUtils.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <csignal>
#include <cstdlib>
#include <getopt.h>

void printHelp()
{
    const ConsolePrinter printer;
    printer.printInfo("Usage: stacktracer [options]");
    printer.printInfo("Options:");
    printer.printInfo("  -p, --pid <pid>        Attach to specified process ID");
    printer.printInfo("  -l, --list             List all running processes");
    printer.printInfo("  -h, --help             Show this help message");
    printer.printInfo("  -v, --verbose          Enable verbose output");
}

void listProcesses()
{
    const ConsolePrinter printer;
    const auto pids = PlatformUtils::getAllProcesses();

    printer.printInfo("Running processes:");
    printer.printInfo("PID\tName");
    printer.printInfo("----------------");

    for (const pid_t pid : pids)
    {
        std::string name = PlatformUtils::getProcessName(pid);
        std::cout << pid << "\t" << name << "\n";
    }
}

void attachToProcess(pid_t pid, const bool verbose)
{
    const ConsolePrinter printer;

    if (!PlatformUtils::isProcessRunning(pid))
    {
        printer.printError("Process " + std::to_string(pid) + " is not running");
        return;
    }

    try
    {
        const std::string procName = PlatformUtils::getProcessName(pid);
        const std::string exePath = PlatformUtils::getExecutablePath(pid);

        if (verbose)
        {
            printer.printInfo("Attaching to process: " + std::to_string(pid));
            printer.printInfo("Process name: " + procName);
            printer.printInfo("Executable path: " + exePath);
        }

        auto receiver = std::make_unique<UnixSymbolReceiver>(pid);
        StackTrace tracer(std::move(receiver));

        const auto frames = tracer.captureProcess(pid);
        printer.printStackTrace(frames);

        printer.printSuccess("Successfully captured stack trace for process " + std::to_string(pid));
    }
    catch (const std::exception& e)
    {
        printer.printError("Failed to capture stack trace: " + std::string(e.what()));
    }
}

int main(const int argc, char* argv[])
{
    int opt;
    pid_t pid = 0;
    bool list = false;
    bool verbose = false;

    static struct option long_options[] = {
        {"pid", required_argument, nullptr, 'p'},
        {"list", no_argument, nullptr, 'l'},
        {"help", no_argument, nullptr, 'h'},
        {"verbose", no_argument, nullptr, 'v'},
        {nullptr, 0, nullptr, 0}
    };

    while ((opt = getopt_long(argc, argv, "p:lhv", long_options, nullptr)) != -1)
    {
        switch (opt)
        {
            case 'p':
                pid = static_cast<pid_t>(std::stoi(optarg));
                break;
            case 'l':
                list = true;
                break;
            case 'h':
                printHelp();
                return EXIT_SUCCESS;
            case 'v':
                verbose = true;
                break;
            default:
                printHelp();
                return EXIT_FAILURE;
        }
    }

    const ConsolePrinter printer;

    try
    {
        if (list)
        {
            listProcesses();
        }
        else if (pid != 0)
        {
            attachToProcess(pid, verbose);
        }
        else
        {
            printer.printWarning("No operation specified. Use -h for help.");
            printHelp();
        }
    }
    catch (const std::exception& e)
    {
        printer.printError("Error: " + std::string(e.what()));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}