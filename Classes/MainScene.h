#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include "Piece.hpp"
#include "Constants.h"
#include "ui/CocosGUI.h"

class Character;

enum class GameState
{
    Title,
    Ready,
    Playing,
    GameOver
};

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
    void triggerGameOver();
    void resetGameState();
    void triggerPlaying();
    void triggerTitle();
    void triggerReady();
    Side getSideForObstacle(Side lastSide);

private:
    cocos2d::Node* pieceNode;
    cocos2d::Vector<Piece*> pieces;
    cocos2d::ui::Text* scoreLabel;
    cocos2d::Sprite* timeBar;
    GameState gameState;
    int score;
    float timeLeft;
    cocos2d::Vec2 flyingPiecePosition;

    void setScore(int score);
    bool isGameOver();
    void setTimeLeft(float timeLeft);
    void update(float dt) override;
    void animateHitPiece(Side obstacleSide);
};

#endif // __MAIN_SCENE_H__
