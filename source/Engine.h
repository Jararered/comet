#pragma once

#include <comet.pch>

#include "handlers/EntityHandler.h"
#include "handlers/EventHandler.h"
#include "handlers/KeyboardHandler.h"
#include "handlers/MouseHandler.h"

#include "Camera.h"
#include "Renderer.h"
#include "render/ShaderProgram.h"
#include "render/Texture.h"
#include "render/TextureMap.h"

#include "Timer.h"

class Engine
{
public:
    inline static auto &Instance()
    {
        static Engine instance;
        return instance;
    }

    static void Initialize();
    static void Finalize();

    static void Thread();

private:
    Engine() {}
    Engine(Engine const &);
    void operator=(Engine const &);

    bool m_ShouldClose = false;
    double m_TimeDelta = 0.0; // ms
    double m_TimeLast = 0.0;  // ms

public:
    static bool IsShouldClose() { return Instance().m_ShouldClose; }
    static void SetShouldClose(bool ShouldClose) { Instance().m_ShouldClose = ShouldClose; }

    static double TimeDelta() { return Instance().m_TimeDelta; }
    static void SetTimeDelta(double TimeDelta) { Instance().m_TimeDelta = TimeDelta; }

    static double TimeLast() { return Instance().m_TimeLast; }
    static void SetTimeLast(double TimeLast) { Instance().m_TimeLast = TimeLast; }
};
