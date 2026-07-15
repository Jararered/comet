#pragma once

#include "Layer.h"

#include <unordered_set>

class LayerManager
{
public:
    void AddLayer(Layer* layer) { m_Layers.insert(layer); }
    void RemoveLayer(Layer* layer) { m_Layers.erase(layer); }
    void Draw()
    {
        for (const auto& layer : m_Layers)
        {
            layer->Draw();
        }
    }

private:
    std::unordered_set<Layer*> m_Layers;
};
