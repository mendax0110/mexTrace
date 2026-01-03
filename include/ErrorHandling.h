#pragma once

#include <stdexcept>
#include <source_location>
#include <format>
#include <string_view>

/// @brief Throws a runtime error with file and line information. \def TRACE_ERROR
#define TRACE_ERROR(message)                                                        \
    throw std::runtime_error(                                                       \
        std::format("[ERROR] {} ({}:{})",                                           \
        message,                                                                    \
        std::source_location::current().file_name(),                                \
        std::source_location::current().line());
