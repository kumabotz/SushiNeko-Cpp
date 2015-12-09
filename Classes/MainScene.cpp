#include "MainScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "CharacterReader.hpp"
#include "PieceReader.hpp"
#include "Character.hpp"

USING_NS_CC;

using namespace cocostudio::timeline;

Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    // register the readers for our custom classes
    // be very careful to do CharacterReader::getInstance, not CharacterReader::getInstance() which will crash
    CSLoader* instance = CSLoader::getInstance();
    instance->registReaderObject("CharacterReader", (ObjectFactory::Instance) CharacterReader::getInstance);
    instance->registReaderObject("PieceReader", (ObjectFactory::Instance) PieceReader::getInstance);
    
    auto rootNode = CSLoader::createNode("MainScene.csb");

    Size size = Director::getInstance()->getVisibleSize();
    rootNode->setContentSize(size);
    ui::Helper::doLayout(rootNode);

    this->pieceNode = rootNode->getChildByName("pieceNode");
    this->character = rootNode->getChildByName<Character*>("character");
    this->scoreLabel = rootNode->getChildByName<cocos2d::ui::Text*>("scoreLabel");

    // use auto as it is not super important to understand the type of the variable
    auto lifeBG = rootNode->getChildByName("lifeBG");
    this->timeBar = lifeBG->getChildByName<Sprite*>("lifeBar");

    this->lastObstacleSide = Side::Left;
    this->pieceIndex = 0;
    this->gameState = GameState::Playing;

    for (int i = 0; i < 10; ++i)
    {
        Piece* piece = dynamic_cast<Piece*>(CSLoader::createNode("Piece.csb"));
        this->lastObstacleSide = this->getSideForObstacle(this->lastObstacleSide);
        piece->setObstacleSide(this->lastObstacleSide);

        float rollHeight = piece->getSpriteHeight();
        piece->setPosition(0.0f, rollHeight / 2.0f * i);
        this->pieceNode->addChild(piece);
        this->pieces.pushBack(piece);
    }

    addChild(rootNode);
    this->resetGameState();

    return true;
}

void MainScene::onEnter()
{
    Layer::onEnter();
    this->setupTouchHandling();
}

void MainScene::setupTouchHandling()
{
    auto touchListener = EventListenerTouchOneByOne::create();

    touchListener->onTouchBegan = [&](Touch* touch, Event* event)
    {
        switch (this->gameState)
        {
            case GameState::Playing:
            {
                // get the location of the touch in the MainScene's coordinate system
                Vec2 touchLocation = this->convertTouchToNodeSpace(touch);

                // check if the touch was on the left or right side of the screen
                // move the character to the appropriate side
                if (touchLocation.x < this->getContentSize().width / 2.0f)
                {
                    this->character->setSide(Side::Left);
                }
                else
                {
                    this->character->setSide(Side::Right);
                }

                if (this->isGameOver())
                {
                    this->triggerGameOver();
                    return true;
                }

                this->stepTower();

                if (this->isGameOver())
                {
                    this->triggerGameOver();
                    return true;
                }

                this->setScore(this->score + 1);
                this->setTimeLeft(this->timeLeft + 0.25f);

                break;
            }

            case GameState::GameOver:
            {
                this->resetGameState();
                this->triggerPlaying();
                break;
            }
        }

        return true;
    };

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

Side MainScene::getSideForObstacle(Side lastSide)
{
    Side side;

    switch (lastSide)
    {
        case Side::None:
        {
            // generate a random number between 0.0f and 1.0f
            float random = CCRANDOM_0_1();

            // if there wasn't an obstacle in the last piece then there's a 45% chance of there
            // being one on the left, 45% chance of there being one on the right and 10% chance of
            // there being no obstacle
            if (random < 0.45f)
            {
                side = Side::Left;
            }
            else if (random < 0.9f)
            {
                side = Side::Right;
            }
            else
            {
                side = Side::None;
            }
            break;
        }

            // if there was and obstacle in the last piece, then there isn't one for this piece
        case Side::Left:
        case Side::Right:
            side = Side::None;
            break;
    }

    return side;
}

void MainScene::stepTower()
{
    // get a reference to the lowest piece
    Piece* currentPiece = this->pieces.at(this->pieceIndex);

    // move the lowest piece to the top of the tower
    currentPiece->setPosition(currentPiece->getPosition() + Vec2(0.0f, currentPiece->getSpriteHeight() / 2.0f * 10.0f));

    // set the zOrder of the piece so that it appears on top of the others
    currentPiece->setLocalZOrder(currentPiece->getLocalZOrder() + 1);

    // set the side of the obstacle, based on the side of the obstacle of the piece right before this one
    currentPiece->setObstacleSide(this->getSideForObstacle(this->lastObstacleSide));
    this->lastObstacleSide = currentPiece->getObstacleSide();

    // move pieceNode down so that the whole tower moves down
    this->pieceNode->setPosition(this->pieceNode->getPosition() + Vec2(0.0f, -1.0f * currentPiece->getSpriteHeight() / 2.0f));

    // change the index referencing the lowest piece
    this->pieceIndex = (this->pieceIndex + 1) % 10;
}

bool MainScene::isGameOver()
{
    bool gameOver = false;

    // get a reference to the lowest piece
    Piece* currentPiece = this->pieces.at(this->pieceIndex);

    // if the obstacle and the character are touching, then game over
    if (currentPiece->getObstacleSide() == this->character->getSide())
    {
        gameOver = true;
    }

    return gameOver;
}

void MainScene::resetGameState(){
    // make sure the lowest piece doesn't have an obstacle when the new game starts
    Piece* currentPiece = this->pieces.at(this->pieceIndex);
    currentPiece->setObstacleSide(Side::None);
    this->setScore(0);
    this->setTimeLeft(5.0f);
}

void MainScene::triggerGameOver()
{
    this->gameState = GameState::GameOver;
    this->setTimeLeft(0.0f);
}

void MainScene::triggerPlaying()
{
    this->gameState = GameState::Playing;
}

void MainScene::setScore(int score)
{
    // update the score instance variable
    this->score = score;

    // update the score label
    this->scoreLabel->setString(std::to_string(this->score));
}

void MainScene::setTimeLeft(float timeLeft)
{
    // clamp the time left timer to between 0 and 10 seconds
    this->timeLeft = clampf(timeLeft, 0.0f, 10.0f);

    // update the UI to reflect the correct time left
    this->timeBar->setScaleX(timeLeft / 10.0f);
}
