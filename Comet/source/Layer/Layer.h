#pragma once

class Layer
{
public:
    Layer() = default;
    virtual ~Layer() = default;

    virtual void Initialize() {}
    virtual void Draw() {}
    virtual void Finalize() {}
};
