#pragma once

#include <raylib.h>

namespace Comet
{
    namespace Clock
    {
        static double Dt = 0.0;
        static void Reset()
        {
            Dt = GetTime();
        }
        static double Time()
        {
            return GetTime() - Dt;
        }
    };
};