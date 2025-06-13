#pragma once

#include <exception>
#include <iostream>
#include <string>
#include <boost/exception/all.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <stdexcept>
#include <sstream>


#define CATCH_WC(output_expr, message_str, code_block)                              \
    catch (const std::exception& e)                                                 \
    {                                                                               \
        code_block;                                                                 \
        std::cerr << (message_str) << " (std::exception): " << e.what() << '\n';    \
        std::cerr << "Output: " << (output_expr) << '\n';                           \
    }                                                                               \
    catch (const boost::exception& e)                                               \
    {                                                                               \
        code_block;                                                                 \
        std::cerr << (message_str) << " (boost::exception): "                       \
        << boost::diagnostic_information(e) << '\n';                                \
        std::cerr << "Output: " << (output_expr) << '\n';                           \
    }

#define TRT_ERROR(message_str)                                                      \
    do {                                                                            \
        std::ostringstream __trt_err_stream__;                                      \
        __trt_err_stream__ << "[ERROR] " << (message_str)                           \
        << " (" << __FILE__ << ":" << __LINE__ << ")";                              \
        throw std::runtime_error(__trt_err_stream__.str());                         \
    } while (0)
