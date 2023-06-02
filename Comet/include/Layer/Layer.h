#pragma once

#include <imgui.h>
#include <imgui_internal.h>

class Layer
{
public:
    Layer() = default;
    virtual ~Layer() = default;

    virtual void Initialize() {}
    virtual void Draw() {}
    virtual void Finalize() {}
};
