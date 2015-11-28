//
//  Piece.cpp
//  SushiNeko
//
//  Created by Tery Lim on 25/11/15.
//
//

#include "Piece.hpp"

using namespace cocos2d;

float Piece::getSpriteHeight()
{
    // first grab a reference to the roll sprite
    Sprite* roll = this->getChildByName<Sprite*>("roll");

    // then return the roll sprite's height
    return roll->getContentSize().height;
}

void Piece::setObstacleSide(Side side)
{
    this->obstacleSide = side;

    Sprite* roll = this->getChildByName<Sprite*>("roll");
    Sprite* leftChopstick = roll->getChildByName<Sprite*>("leftChopstick");
    Sprite* rightChopstick = roll->getChildByName<Sprite*>("rightChopstick");

    switch (this->obstacleSide)
    {
        case Side::None:
            leftChopstick->setVisible(false);
            rightChopstick->setVisible(false);
            break;

        case Side::Left:
            leftChopstick->setVisible(true);
            rightChopstick->setVisible(false);
            break;

        case Side::Right:
            leftChopstick->setVisible(false);
            rightChopstick->setVisible(true);
            break;
    }
}

Side Piece::getObstacleSide()
{
    return this->obstacleSide;
}
