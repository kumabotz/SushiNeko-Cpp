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
