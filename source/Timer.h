// #include <comet.pch>
#pragma once

#include <comet.pch>

class Timer
{
public:
    Timer();
    Timer(const std::string &scope);
    ~Timer();

private:
    double m_StartTime;
    std::string m_ScopeName;

public:
    double StartTime() const { return m_StartTime; }
    void SetStartTime(double StartTime) { m_StartTime = StartTime; }

    std::string ScopeName() const { return m_ScopeName; }
    void SetScopeName(const std::string &ScopeName) { m_ScopeName = ScopeName; }
};
