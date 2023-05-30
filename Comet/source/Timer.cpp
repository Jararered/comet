#include "Timer.hpp"

#include <glfw/glfw3.h>

#include <iostream>

Timer::Timer() : m_StartTime(glfwGetTime()), m_ScopeName("")
{
}

Timer::Timer(const std::string& scope) : m_StartTime(glfwGetTime()), m_ScopeName(scope)
{
    // weird formatting workaround
    m_ScopeName.append(" ");
}

Timer::~Timer()
{
    std::cout << m_ScopeName << "took " << glfwGetTime() - m_StartTime << " seconds.\n";
}
