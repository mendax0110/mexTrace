#pragma once
#include "SymbolReceiver.h"
#include <string>

/// @brief UnixSymbolReceiver is a concrete implementation of SymbolReceiver for Unix-like systems, resolving symbols using addr2line or backtrace. \class UnixSymbolReceiver
class UnixSymbolReceiver : public SymbolReceiver
{
public:

    /**
     * @brief Constructs a UnixSymbolReceiver for a specific process ID.
     * @param pid The process ID to attach to. If 0, it will resolve symbols for the current process.
     */
    UnixSymbolReceiver(pid_t pid = 0);

    /**
     * @brief Destructor for UnixSymbolReceiver.
     */
    ~UnixSymbolReceiver() override;

    /**
     * @brief Resolves a memory address to a StackFrame, providing information about the function, source file, and line number.
     * @param address The memory address to resolve.
     * @return A StackFrame object containing the resolved information.
     */
    StackFrame resolve(uintptr_t address) override;

    /**
     * @brief Checks if the UnixSymbolReceiver is available for use, meaning it can resolve symbols.
     * @return A boolean indicating whether the UnixSymbolReceiver is available.
     */
    bool isAvailable() const override;

private:
    pid_t targetPid;
    std::string executablePath;

    /**
     * @brief Resolves a memory address using the addr2line command.
     * @param address The memory address to resolve.
     * @return A string containing the resolved function name, source file, and line number.
     */
    std::string resolveWithAddr2line(uintptr_t address) const;

    /**
     * @brief Resolves a memory address using backtrace_symbols or similar methods.
     * @param address The memory address to resolve.
     * @return A string containing the resolved function name or an empty string if resolution fails.
     */
    static std::string resolveWithBacktrace(uintptr_t address) ;
};