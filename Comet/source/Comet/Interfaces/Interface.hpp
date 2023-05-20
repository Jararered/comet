#pragma once

#include <imgui.h>
#include <imgui_internal.h>

class Interface
{
public:
    Interface() = default;
    virtual ~Interface() = default;

    virtual void Initialize() {}
    virtual void Update() {}
    virtual void Finalize() {}
};
