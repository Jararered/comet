#pragma once

#include "Layer.h"

#include <concepts>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

class LayerManager
{
public:
    ~LayerManager() { Clear(); }

    template <typename T, typename... Args>
        requires std::derived_from<T, Layer>
    T& AddLayer(std::string key, Args&&... args)
    {
        auto layer = std::make_unique<T>(std::forward<Args>(args)...);
        T& layerReference = *layer;
        const auto [entry, inserted] = m_Layers.emplace(std::move(key), std::move(layer));
        if (!inserted)
            throw std::invalid_argument("A layer with that key already exists");

        layerReference.Initialize();
        return layerReference;
    }

    bool RemoveLayer(const std::string& key)
    {
        const auto entry = m_Layers.find(key);
        if (entry == m_Layers.end())
            return false;

        entry->second->Finalize();
        m_Layers.erase(entry);
        return true;
    }

    void Clear()
    {
        for (auto& [key, layer] : m_Layers)
            layer->Finalize();
        m_Layers.clear();
    }

    void Draw()
    {
        for (const auto& [key, layer] : m_Layers)
        {
            layer->Draw();
        }
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Layer>> m_Layers;
};
