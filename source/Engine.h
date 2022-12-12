#pragma once

#include <comet.pch>

#include "handlers/EntityHandler.h"

#include "input/Input.h"

#include "render/Shader.h"
#include "render/Texture.h"
#include "render/TextureMap.h"

#include "Camera.h"
#include "Renderer.h"
#include "Timer.h"
#include "Utilities.h"

class Engine
{
public:
    inline static auto &Get()
    {
        static Engine instance;
        return instance;
    }

    static void Initialize();
    static void Thread();
    static void Finalize();

private:
    Engine() {}
    Engine(Engine const &);
    void operator=(Engine const &);

    bool m_ShouldClose = false;

public:
    static bool IsShouldClose() { return Get().m_ShouldClose; }
    static void SetShouldClose(bool ShouldClose) { Get().m_ShouldClose = ShouldClose; }
};
