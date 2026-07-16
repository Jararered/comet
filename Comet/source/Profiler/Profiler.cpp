#include "Profiler.h"

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

namespace Comet
{
namespace
{
struct Event { std::uint64_t sequence; std::uint64_t thread; std::string name; std::string category; double ms; std::uint64_t value; };
struct State
{
    std::mutex lock;
    std::condition_variable wakeWriter;
    std::vector<Event> events;
    std::uint64_t sequence = 0;
    std::atomic_bool running = false;
    std::chrono::steady_clock::time_point lastFlush{};
    std::filesystem::path directory;
    std::jthread writer;
};

constexpr auto ProfilerFlushInterval = std::chrono::milliseconds(100);
State& GetState() { static State state; return state; }
thread_local std::uint64_t threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
thread_local std::vector<Event> pending;

void CommitPending(State& state)
{
    if (pending.empty()) return;
    state.events.insert(state.events.end(), std::make_move_iterator(pending.begin()), std::make_move_iterator(pending.end()));
    pending.clear();
}

void WriteEvents(const std::filesystem::path& directory, const std::vector<Event>& events)
{
    if (events.empty()) return;

    const auto stamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::ofstream timeline(directory / ("timeline-" + std::to_string(stamp) + ".jsonl"));
    std::ofstream summary(directory / "summary.tsv", std::ios::app);
    summary << "# flush " << stamp << "\n";
    std::unordered_map<std::string, std::pair<double, std::uint64_t>> totals;
    for (const auto& event : events)
    {
        timeline << "{\"seq\":" << event.sequence << ",\"thread\":" << event.thread << ",\"name\":\"" << event.name
                 << "\",\"category\":\"" << event.category << "\",\"ms\":" << std::fixed << std::setprecision(3) << event.ms << ",\"value\":" << event.value << "}\n";
        auto& total = totals[event.category + "\t" + event.name];
        total.first += event.ms;
        ++total.second;
    }
    for (const auto& [key, total] : totals)
        summary << key << "\t" << std::fixed << std::setprecision(3) << total.first << "\t" << total.second << "\t" << (total.first / total.second) << "\n";
}

void WriterLoop(std::stop_token stopToken)
{
    auto& state = GetState();
    while (true)
    {
        std::vector<Event> events;
        std::filesystem::path directory;
        {
            std::unique_lock lock(state.lock);
            state.wakeWriter.wait_for(lock, std::chrono::seconds(5), [&] { return stopToken.stop_requested() || !state.running; });
            events.swap(state.events);
            directory = state.directory;
        }

        WriteEvents(directory, events);
        if (stopToken.stop_requested())
            return;
    }
}
}

Profiler& Profiler::Instance() { static Profiler profiler; return profiler; }

void Profiler::Start()
{
    auto& state = GetState();
    std::lock_guard lock(state.lock);
    state.directory = std::filesystem::current_path() / "logs" / "profiler";
    std::filesystem::create_directories(state.directory);
    state.events.clear();
    state.sequence = 0;
    state.running = true;
    state.writer = std::jthread(WriterLoop);
}

void Profiler::Stop()
{
    auto& state = GetState();
    {
        std::lock_guard lock(state.lock);
        for (auto& event : pending) event.sequence = state.sequence++;
        CommitPending(state);
        state.running = false;
    }
    state.wakeWriter.notify_one();
    state.writer.request_stop();
    if (state.writer.joinable()) state.writer.join();
}

void Profiler::Record(std::string_view name, std::string_view category, double milliseconds, std::uint64_t value)
{
    auto& state = GetState();
    if (!state.running) return;
    pending.push_back({0, threadId, std::string(name), std::string(category), milliseconds, value});
    if (pending.size() < 64) return;
    std::lock_guard lock(state.lock);
    if (!state.running) { pending.clear(); return; }
    for (auto& event : pending) event.sequence = state.sequence++;
    CommitPending(state);
    state.wakeWriter.notify_one();
}

void Profiler::FlushIfDue()
{
    auto& state = GetState();
    if (!state.running) return;

    const auto now = std::chrono::steady_clock::now();
    {
        std::lock_guard lock(state.lock);
        if (now - state.lastFlush < ProfilerFlushInterval)
            return;
        state.lastFlush = now;
        for (auto& event : pending) event.sequence = state.sequence++;
        CommitPending(state);
    }
    state.wakeWriter.notify_one();
}

Profiler::Scope::Scope(std::string_view name, std::string_view category) : m_Name(name), m_Category(category), m_Start(std::chrono::steady_clock::now()) {}
Profiler::Scope::~Scope()
{
    const double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - m_Start).count();
    Profiler::Instance().Record(m_Name, m_Category, ms);
}
}
