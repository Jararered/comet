#pragma once

#include <Layer/Layer.h>
#include <Layer/LayerManager.h>

#include <raylib.h>

#include "ResourcePaths.h"

class Crosshair : public Layer
{
public:
    Crosshair();
    ~Crosshair() override = default;

    void Draw() override;

private:
    ::Texture2D m_Texture;
};