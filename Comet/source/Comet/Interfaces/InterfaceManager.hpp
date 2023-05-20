#pragma once

#include "Interface.hpp"

#include <unordered_set>

class InterfaceManager
{
public:
    inline static auto& Get()
    {
        static InterfaceManager instance;
        return instance;
    }

    static void Initialize();
    static void AddInterface(Interface* interface) { Get().m_Interfaces.insert(interface); }
    static void RemoveInterface(Interface* interface) { Get().m_Interfaces.erase(interface); }
    static void DrawInterfaces()
    {
        for (const auto& interface : Get().m_Interfaces)
        {
            interface->Update();
        }
    }

private:
    InterfaceManager() {}
    InterfaceManager(InterfaceManager const&);
    void operator=(InterfaceManager const&);

    std::unordered_set<Interface*> m_Interfaces;
};
