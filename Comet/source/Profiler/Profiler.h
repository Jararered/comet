#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace Comet
{
class Profiler
{
public:
    class Scope
    {
    public:
        Scope(std::string_view name, std::string_view category);
        ~Scope();

        Scope(const Scope&) = delete;
        Scope& operator=(const Scope&) = delete;

    private:
        std::string_view m_Name;
        std::string_view m_Category;
        std::chrono::steady_clock::time_point m_Start;
    };

    static Profiler& Instance();
    void Start();
    void Stop();
    void FlushIfDue();
    void Record(std::string_view name, std::string_view category, double milliseconds, std::uint64_t value = 0);
};
}

#define COMET_PROFILE_SCOPE(name, category) ::Comet::Profiler::Scope cometProfileScope_##__LINE__(name, category)
