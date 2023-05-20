#pragma once

#include "Entities/EntityManager.hpp"

#include "Input/Input.hpp"

#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/TextureMap.hpp"

#include "Camera.hpp"
#include "Renderer.hpp"
#include "Timer.hpp"
#include "Utilities.hpp"

class Engine
{
public:
    inline static auto& Get()
    {
        static Engine instance;
        return instance;
    }

    static void Initialize();
    static void Thread();
    static void Finalize();

private:
    Engine() {}
    Engine(Engine const&);
    void operator=(Engine const&);

    bool m_ShouldClose = false;

public:
    static bool IsShouldClose() { return Get().m_ShouldClose; }
    static void SetShouldClose(bool ShouldClose) { Get().m_ShouldClose = ShouldClose; }
};
