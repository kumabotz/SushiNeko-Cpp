#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include "Piece.hpp"
#include "Constants.h"

class Character;

class MainScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);

protected:
    Character* character;
    Side lastObstacleSide;
    int pieceIndex;
    void onEnter() override;
    void setupTouchHandling();
    void stepTower();
    Side getSideForObstacle(Side lastSide);

private:
    cocos2d::Node* pieceNode;
    cocos2d::Vector<Piece*> pieces;
};

#endif // __MAIN_SCENE_H__
