#pragma once

#include <comet.pch>

#include "Camera.h"
#include "Renderer.h"
#include "render/ShaderProgram.h"
#include "render/Texture.h"
#include "render/TextureMap.h"

#include "Timer.h"

class EntityHandler;
class ErrorHandler;
class EventHandler;
class KeyboardHandler;
class MouseHandler;
class InterfaceHandler;

class Engine
{
public:
    Engine();
    ~Engine();

    void Thread();

private:
    bool m_ShouldClose = false;
    double m_TimeDelta = 0.0; // ms
    double m_TimeLast = 0.0;  // ms

    WindowHandler *m_WindowHandler;
    EventHandler *m_EventHandler;
    KeyboardHandler *m_KeyboardHandler;
    MouseHandler *m_MouseHandler;
    ErrorHandler *m_ErrorHandler;
    EntityHandler *m_EntityHandler;
    InterfaceHandler *m_InterfaceHandler;

    TextureMap *m_TextureMap;
    Renderer *m_Renderer;
    Camera *m_Camera;

public:
    bool IsShouldClose() { return m_ShouldClose; }
    void SetShouldClose(bool ShouldClose) { m_ShouldClose = ShouldClose; }

    double TimeDelta() { return m_TimeDelta; }
    void SetTimeDelta(double TimeDelta) { m_TimeDelta = TimeDelta; }

    double TimeLast() { return m_TimeLast; }
    void SetTimeLast(double TimeLast) { m_TimeLast = TimeLast; }

    WindowHandler *GetWindowHandler() const { return m_WindowHandler; }
    EventHandler *GetEventHandler() const { return m_EventHandler; }
    KeyboardHandler *GetKeyboardHandler() const { return m_KeyboardHandler; }
    MouseHandler *GetMouseHandler() const { return m_MouseHandler; }
    ErrorHandler *GetErrorHandler() const { return m_ErrorHandler; }
    EntityHandler *GetEntityHandler() const { return m_EntityHandler; }
    InterfaceHandler *GetInterfaceHandler() const { return m_InterfaceHandler; }
    TextureMap *GetTextureMap() const { return m_TextureMap; }
    Renderer *GetRenderer() const { return m_Renderer; }
    Camera *GetCamera() const { return m_Camera; }
};
