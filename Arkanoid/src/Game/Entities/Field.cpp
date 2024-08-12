#include <Game/Entities/Field.h>

Field::Field(Graphics& gfx, ver::ph::Physics& phy)
    :
    walls
    {
        Wall{gfx, phy, 1.0f, 1.0f},
        //Wall{gfx, phy, 1.0f, 1.0f},
        //Wall{gfx, phy, 1.0f, 1.0f}
    }
{
    walls[0].SetPosition({ 0.0f, 0.0f, 20.0f });
    //walls[1].SetPosition({ 0.0f, 0.0f, 1.0f });
    //walls[2].SetPosition({ 1.0f, 0.0f, 0.0f });
}
