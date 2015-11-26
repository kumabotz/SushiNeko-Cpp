//
//  Character.cpp
//  SushiNeko
//
//  Created by Tery Lim on 25/11/15.
//
//

#include "Character.hpp"

void Character::setSide(Side side)
{
    if (this->side == Side::Right)
    {
        this->setScaleX(-1.0f);
    }
    else
    {
        this->setScaleX(1.0f);
    }
}

Side Character::getSide()
{
    return this->side;
}
