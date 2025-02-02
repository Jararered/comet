#pragma once

namespace Comet
{
    namespace Clock
    {
        static double Dt = 0.0;
        static void Reset()
        {
            Dt = glfwGetTime();
        }
        static double Time()
        {
            return glfwGetTime() - Dt;
        }
    };
};
