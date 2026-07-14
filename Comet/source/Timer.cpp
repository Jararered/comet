#include "Timer.h"

#include <raylib.h>

#include <iostream>

Timer::Timer() : m_StartTime(GetTime()), m_ScopeName("")
{
}

Timer::Timer(const std::string& scope) : m_StartTime(GetTime()), m_ScopeName(scope)
{
    m_ScopeName.append(" ");
}

Timer::~Timer()
{
    std::cout << m_ScopeName << "took " << GetTime() - m_StartTime << " seconds.\n";
}