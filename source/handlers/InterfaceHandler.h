#pragma once

#include <comet.pch>

#include "Interface.h"

class InterfaceHandler
{
public:
    inline static auto &Instance()
    {
        static InterfaceHandler instance;
        return instance;
    }

    static void Initialize();
    static void AddInterface(Interface *interface) { Instance().m_Interfaces.insert(interface); }
    static void RemoveInterface(Interface *interface) { Instance().m_Interfaces.erase(interface); }
    static void DrawInterfaces()
    {
        for (const auto &interface : Instance().m_Interfaces)
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
