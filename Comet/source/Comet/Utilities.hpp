#pragma once

namespace Comet
{
    namespace Statistics
    {
        static double PhysicsTime = 0.0;
        static double RenderTime = 0.0;
    };

    namespace Clock
    {
        static double Dt = 0.0;
        static void Reset() { Dt = glfwGetTime(); }
        static double Time() { return glfwGetTime() - Dt; }
    };
};
