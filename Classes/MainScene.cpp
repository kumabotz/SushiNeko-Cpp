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

    for (int i = 0; i < 10; ++i)
    {
        Piece* piece = dynamic_cast<Piece*>(CSLoader::createNode("Piece.csb"));
        float rollHeight = piece->getSpriteHeight();
        piece->setPosition(0.0f, rollHeight / 2.0f * i);
        this->pieceNode->addChild(piece);
        this->pieces.pushBack(piece);
    }

    addChild(rootNode);

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

        return true;
    };

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}
