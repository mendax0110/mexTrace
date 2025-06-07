#ifndef MEXTRACE_PLATFORMUTILS_H
#define MEXTRACE_PLATFORMUTILS_H

#include <string>
#include <stdexcept>

/// @brief The mexTrace namespace contains utility classes and functions for platform-specific operations. \namespace mex
namespace mex
{
    /// @brief PlatformUtils provides methods to determine the current platform and its support status. \class PlatformUtils
    class PlatformUtils
    {
    public:

        /// @brief Enum class representing different platforms. \enum Platform
        enum class Platform
        {
            Windows = 0,
            Linux = 1,
            MacOS = 2,
            Unknown = 3
        };

        /**
         * @brief Gets the current platform.
         * @return A Platform enum value representing the current platform.
         */
        static Platform getPlatform();

        /**
         * @brief Checks if the current platform is supported.
         * @return True if the platform is supported, false otherwise.
         * @throws std::runtime_error if the platform is not supported.
         */
        static bool isPlatformSupported();

        /**
         * @brief Gets the name of the current platform.
         * @return A string representing the name of the current platform.
         */
        static std::string getPlatformName();
    };
} // namespace mex

#endif // MEXTRACE_PLATFORMUTILS_H