//
//  Character.hpp
//  SushiNeko
//
//  Created by Tery Lim on 25/11/15.
//
//

#ifndef Character_hpp
#define Character_hpp

#include "cocos2d.h"
#include "Constants.h"

class Character : public cocos2d::Node
{
public:
    CREATE_FUNC(Character);
    void setSide(Side side);
    Side getSide();

protected:
    Side side;
};

#endif /* Character_hpp */
