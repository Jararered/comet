#pragma once

#include <comet.pch>

namespace Comet
{
    class Callbacks
    {
    public:
        inline static auto &Get()
        {
            static Callbacks instance;
            return instance;
        }

        static double GetScrollOffset() { return Get().m_ScrollOffset; }

    private:
        Callbacks()
        {
            glfwSetScrollCallback(glfwGetCurrentContext(), [](GLFWwindow *window, double xoffset, double yoffset) {
                Get().m_ScrollOffset += yoffset;
                std::cout << Get().m_ScrollOffset << "\n";
            });

            glfwSetKeyCallback(
                glfwGetCurrentContext(), [](GLFWwindow *window, int key, int scancode, int action, int mods) {
                    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                    {
                        glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                        glfwSetCursorPos(glfwGetCurrentContext(), 0.0, 0.0);
                    }
                });
        }
        Callbacks(Callbacks const &);
        void operator=(Callbacks const &);

        double m_ScrollOffset;
    };
} // namespace Comet
