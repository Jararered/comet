#pragma once

#include "Layer.hpp"

#include <unordered_set>

class LayerManager
{
public:
    inline static auto& Get()
    {
        static LayerManager instance;
        return instance;
    }

    static void Initialize();
    static void AddLayer(Layer* layer) { Get().m_Layers.insert(layer); }
    static void RemoveLayer(Layer* layer) { Get().m_Layers.erase(layer); }
    static void Draw()
    {
        for (const auto& layer : Get().m_Layers)
        {
            layer->Draw();
        }
    }

private:
    LayerManager() {}
    LayerManager(LayerManager const&);
    void operator=(LayerManager const&);

    std::unordered_set<Layer*> m_Layers;
};
