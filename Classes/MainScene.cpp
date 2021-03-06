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
    this->gameState = GameState::Title;

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
    this->triggerTitle();
    this->flyingPiecePosition = this->pieceNode->getPosition();
    this->scheduleUpdate();
}

void MainScene::setupTouchHandling()
{
    auto touchListener = EventListenerTouchOneByOne::create();

    touchListener->onTouchBegan = [&](Touch* touch, Event* event)
    {
        switch (this->gameState)
        {
            case GameState::Title:
            {
                this->triggerReady();
                break;
            }

            case GameState::Ready:
            {
                this->triggerPlaying();
                // no break here!
            }

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

                this->character->runChopAnimation();
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
                this->triggerReady();
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
    this->animateHitPiece(currentPiece->getObstacleSide());

    // move the lowest piece to the top of the tower
    currentPiece->setPosition(currentPiece->getPosition() + Vec2(0.0f, currentPiece->getSpriteHeight() / 2.0f * 10.0f));

    // set the zOrder of the piece so that it appears on top of the others
    currentPiece->setLocalZOrder(currentPiece->getLocalZOrder() + 1);

    // set the side of the obstacle, based on the side of the obstacle of the piece right before this one
    currentPiece->setObstacleSide(this->getSideForObstacle(this->lastObstacleSide));
    this->lastObstacleSide = currentPiece->getObstacleSide();

    // move pieceNode down so that the whole tower moves down
    cocos2d::MoveBy* moveAction = cocos2d::MoveBy::create(0.15f, Vec2(0.0f, -1.0f * currentPiece->getSpriteHeight() / 2.0f));
    this->pieceNode->runAction(moveAction);

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
    this->scoreLabel->setVisible(false);
    this->setScore(0);
    this->setTimeLeft(5.0f);
}

void MainScene::triggerGameOver()
{
    this->gameState = GameState::GameOver;
    this->setTimeLeft(0.0f);

    // get a reference to the top-most node
    auto scene = this->getChildByName("Scene");

    // get a reference to the mat sprite
    auto mat = scene->getChildByName("mat");

    // get a reference to the game over score label
    cocos2d::ui::Text* gameOverScoreLabel = mat->getChildByName<cocos2d::ui::Text*>("gameOverScoreLabel");

    // set the score label to the user's score
    gameOverScoreLabel->setString(std::to_string(this->score));

    // load and run the game over animations
    cocostudio::timeline::ActionTimeline* gameOverTimeline = CSLoader::createTimeline("MainScene.csb");
    this->stopAllActions();
    this->runAction(gameOverTimeline);
    gameOverTimeline->play("gameOver", false);
}

void MainScene::triggerPlaying()
{
    this->gameState = GameState::Playing;

    // get a reference to the top-most node
    auto scene = this->getChildByName("Scene");

    // get a reference to the left and right tap sprite
    cocos2d::Sprite* tapLeft = scene->getChildByName<cocos2d::Sprite*>("tapLeft");
    cocos2d::Sprite* tapRight = scene->getChildByName<cocos2d::Sprite*>("tapRight");

    // create two fade actions
    cocos2d::FadeOut* leftFade = cocos2d::FadeOut::create(0.35f);
    cocos2d::FadeOut* rightFade = cocos2d::FadeOut::create(0.35f);

    // run the fade actions
    tapLeft->runAction(leftFade);
    tapRight->runAction(rightFade);

    this->scoreLabel->setVisible(true);
}

void MainScene::triggerTitle()
{
    this->gameState = GameState::Title;
    cocostudio::timeline::ActionTimeline* titleTimeline = CSLoader::createTimeline("MainScene.csb");
    this->stopAllActions();
    this->runAction(titleTimeline);
    titleTimeline->play("title", false);
}

void MainScene::triggerReady()
{
    this->gameState = GameState::Ready;

    // get a reference to the top-most node
    auto scene = this->getChildByName("Scene");

    // get a reference to the left and right tap sprite
    cocos2d::Sprite* tapLeft = scene->getChildByName<cocos2d::Sprite*>("tapLeft");
    cocos2d::Sprite* tapRight = scene->getChildByName<cocos2d::Sprite*>("tapRight");

    // make sure the sprites are visible
    tapLeft->setOpacity(255);
    tapRight->setOpacity(255);

    cocostudio::timeline::ActionTimeline* readyTimeline = CSLoader::createTimeline("MainScene.csb");
    this->stopAllActions();
    this->runAction(readyTimeline);
    readyTimeline->play("ready", true);
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

void MainScene::update(float dt)
{
    // update is called before every new frame is rendered
    // dt is the amount of time elapsed (in seconds) between this update call and the previous one
    // call the superclass method update
    Layer::update(dt);

    if (this->gameState == GameState::Playing)
    {
        // if the game is being played
        // reduce the timer by the amount of the time elapsed
        this->setTimeLeft(timeLeft - dt);

        if (this->timeLeft <= 0.0f)
        {
            this->triggerGameOver();
        }
    }
}

void MainScene::animateHitPiece(Side obstacleSide)
{
    // load a new piece from CSLoader
    Piece* flyingPiece = dynamic_cast<Piece*>(CSLoader::createNode("Piece.csb"));

    // make sure the flying piece obstacle matches the correct side of the real one
    flyingPiece->setObstacleSide(obstacleSide);

    // set the position and add it to the scene
    flyingPiece->setPosition(this->flyingPiecePosition);
    this->addChild(flyingPiece);

    // load the piece's animation timeline
    cocostudio::timeline::ActionTimeline* pieceTimeline = CSLoader::createTimeline("Piece.csb");

    // get the side the character is on
    Side characterSide = this->character->getSide();

    // if the character is on the left, animate the piece to the right and vice-versa
    std::string animationName = (characterSide == Side::Left) ? std::string("moveRight") : std::string("moveLeft");

    // run the action so the timeline gets update ticks
    flyingPiece->runAction(pieceTimeline);

    // tell the timeline to play the animation
    pieceTimeline->play(animationName, false);

    // on the last frame of the animation, remove the piece from the scene
    pieceTimeline->setLastFrameCallFunc([this, &flyingPiece]() {
        this->removeChild(flyingPiece);
    });
}
