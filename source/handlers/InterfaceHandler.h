#pragma once

#include <comet.pch>

#include "Interface.h"

class InterfaceHandler
{
public:
    InterfaceHandler() {}
    ~InterfaceHandler() {}

    void AddInterface(Interface *interface) { m_Interfaces.insert(interface); }
    void RemoveInterface(Interface *interface) { m_Interfaces.erase(interface); }
    void DrawInterfaces()
    {
        for (const auto &interface : m_Interfaces)
        {
            interface->Draw();
        }
    }

private:
    std::unordered_set<Interface *> m_Interfaces;
};
