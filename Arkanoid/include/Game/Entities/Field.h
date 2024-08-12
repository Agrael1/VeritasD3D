#pragma once
#include <Game/Entities/Wall.h>

class Field
{
public:
    Field(Graphics& gfx, ver::ph::Physics& phy);
public:
    void AddToScene(ver::ph::Scene& scene)
    {
        for (auto& wall : walls)
        {
            wall.AddToScene(scene);
        }
    }

    void Link(RG::RenderGraph& rg)
    {
        for (auto& wall : walls)
        {
            wall.Link(rg);
        }
    }
    void Submit()
    {
        for (auto& wall : walls)
        {
            wall.Submit();
        }
    }
private:
    std::array<Wall, 1> walls;
};