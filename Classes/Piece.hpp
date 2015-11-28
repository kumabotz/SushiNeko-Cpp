//
//  Piece.hpp
//  SushiNeko
//
//  Created by Tery Lim on 25/11/15.
//
//

#ifndef Piece_hpp
#define Piece_hpp

#include "cocos2d.h"
#include "Constants.h"

class Piece : public cocos2d::Node
{
public:
    CREATE_FUNC(Piece);
    float getSpriteHeight();
    void setObstacleSide(Side side);
    Side getObstacleSide();

protected:
    Side obstacleSide;
};

#endif /* Piece_hpp */
