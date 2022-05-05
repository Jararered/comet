#pragma once

#include <comet.pch>

#include "Interface.h"

class InterfaceHandler
{
public:
    inline static auto &Get()
    {
        static InterfaceHandler instance;
        return instance;
    }

    static void Initialize();
    static void AddInterface(Interface *interface) { Get().m_Interfaces.insert(interface); }
    static void RemoveInterface(Interface *interface) { Get().m_Interfaces.erase(interface); }
    static void DrawInterfaces()
    {
        for (const auto &interface : Get().m_Interfaces)
        {
            interface->Draw();
        }
    }

private:
    InterfaceHandler() {}
    InterfaceHandler(InterfaceHandler const &);
    void operator=(InterfaceHandler const &);

    std::unordered_set<Interface *> m_Interfaces;
};
