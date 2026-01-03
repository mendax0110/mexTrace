#include "StackTrace.h"
#include "PlatformUtils.h"
#include <algorithm>
#include <ranges>

StackTrace::StackTrace(const size_t maxDepth) noexcept
    : m_maxDepth(maxDepth)
{

}

void StackTrace::setMaxDepth(const size_t depth) noexcept
{
    m_maxDepth = depth;
}

std::vector<StackFrame> StackTrace::captureCurrentThread() const
{
    const auto st = std::stacktrace::current();
    return convertStackTrace(st);
}

std::expected<std::vector<StackFrame>, StackTrace::Error> StackTrace::captureProcess(const pid_t pid) const
{
    if (!PlatformUtils::isProcessRunning(pid))
    {
        return std::unexpected(Error::ProcessNotRunning);
    }

    if (!PlatformUtils::attachToProcess(pid))
    {
        return std::unexpected(Error::AttachFailed);
    }

    auto frames = PlatformUtils::readProcessStack(pid);
    PlatformUtils::detachFromProcess(pid);

    if (frames.empty())
    {
        return std::unexpected(Error::CaptureFailed);
    }

    if (frames.size() > m_maxDepth)
    {
        frames.resize(m_maxDepth);
    }

    return frames;
}

std::string StackTrace::errorToString(const Error error) noexcept
{
    switch (error)
    {
        case Error::ProcessNotRunning: return "Process is not running";
        case Error::AttachFailed: return "Failed to attach to process";
        case Error::DetachFailed: return "Failed to detach from process";
        case Error::CaptureFailed: return "Failed to capture stack trace";
    }
    return "Unknown error";
}

std::vector<StackFrame> StackTrace::convertStackTrace(const std::stacktrace& st) const
{
    std::vector<StackFrame> frames;
    const auto stSize = static_cast<std::size_t>(st.size());
    const auto limit = std::min(stSize, m_maxDepth);
    frames.reserve(limit);

    for (size_t i = 0; i < limit; ++i)
    {
        const auto& entry = st[i];
        frames.emplace_back(
            reinterpret_cast<uintptr_t>(entry.native_handle()),
            entry.description(),
            entry.source_file(),
            entry.source_line()
        );
    }

    return frames;
}
