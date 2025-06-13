#pragma once
#include "StackFrame.h"
#include <memory>

/// @brief SymbolReceiver is an abstract base class for resolving symbols from memory addresses in stack traces. \class SymbolReceiver
class SymbolReceiver
{
public:

    /**
     * @brief Virtual destructor for SymbolReceiver.
     */
    virtual ~SymbolReceiver() = default;

    /**
     * @brief Resolves a memory address to a StackFrame, providing information about the function, source file, and line number.
     * @param address The memory address to resolve.
     * @return A StackFrame object containing the resolved information.
     */
    virtual StackFrame resolve(uintptr_t address) = 0;

    /**
     * @brief Checks if the SymbolReceiver is available for use, meaning it can resolve symbols.
     * @return A boolean indicating whether the SymbolReceiver is available.
     */
    virtual bool isAvailable() const = 0;
};