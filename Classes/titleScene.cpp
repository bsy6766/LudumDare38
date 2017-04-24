#include "titleScene.h"
#include "GameScene.h"

USING_NS_CC;

titleScene* titleScene::createScene()
{
	titleScene* newtitleScene = titleScene::create();
	return newtitleScene;
}

bool titleScene::init()
{
	if (!cocos2d::Scene::init())
	{
		return false;
	}

	// Uncomment this to activate update(float) function
	//this->scheduleUpdate();

	auto cache = cocos2d::SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("spritesheets/titleSceneSpriteSheet.plist");

	cocos2d::Director::getInstance()->getOpenGLView()->setCursorVisible(false);

	auto winSize = cocos2d::Director::getInstance()->getVisibleSize();

	this->sky = cocos2d::Sprite::createWithSpriteFrameName("sky.png");
	this->sky->setAnchorPoint(cocos2d::Vec2(0.5f, 1.0f));
	this->sky->setPosition(cocos2d::Vec2(winSize.width * 0.5f, winSize.height));
	this->addChild(sky);

	this->grass3 = cocos2d::Sprite::createWithSpriteFrameName("grass3.png");
	this->grass3->setAnchorPoint(cocos2d::Vec2(1.0f, 0.5f));
	this->grass3->setPosition(cocos2d::Vec2(winSize.width + 20.0f, winSize.height * 0.5f));
	this->addChild(this->grass3);

	this->grass2 = cocos2d::Sprite::createWithSpriteFrameName("grass2.png");
	this->grass2->setAnchorPoint(cocos2d::Vec2(0.0f, 0.5f));
	this->grass2->setPosition(cocos2d::Vec2(-20.0f, winSize.height * 0.5f));
	this->addChild(this->grass2);

	this->castle = cocos2d::Sprite::createWithSpriteFrameName("castle.png");
	this->castle->setPosition(cocos2d::Vec2(winSize.width * 0.5f, 220.0f));
	this->addChild(this->castle);

	this->grass1 = cocos2d::Sprite::createWithSpriteFrameName("grass1.png");
	this->grass1->setAnchorPoint(cocos2d::Vec2(1.0f, 0.5f));
	this->grass1->setPosition(cocos2d::Vec2(winSize.width, 200.0f));
	this->addChild(this->grass1);

	this->grass0 = cocos2d::Sprite::createWithSpriteFrameName("grass0.png");
	this->grass0->setAnchorPoint(cocos2d::Vec2(0.0f, 0.5f));
	this->grass0->setPosition(cocos2d::Vec2(0, 190.0f));
	this->addChild(this->grass0);

	this->title = cocos2d::Sprite::createWithSpriteFrameName("title.png");
	this->title->setPosition(cocos2d::Vec2(winSize.width * 0.5f, 480.0f));
	this->addChild(this->title);

	this->clickToPlay = cocos2d::Sprite::createWithSpriteFrameName("clickToPlay.png");
	this->clickToPlay->setPosition(cocos2d::Vec2(winSize.width * 0.5f, 65.0f));
	this->addChild(this->clickToPlay);

	this->cursor = cocos2d::Sprite::create("sprites/UI/defaultCursor.png");
	this->addChild(this->cursor);

	return true;
}

void titleScene::onEnter()
{
	cocos2d::Scene::onEnter();
	// Uncomment this to enable mouse and keyboard event listeners
	initInputListeners();

	this->grass3->runAction(cocos2d::MoveBy::create(2.0f, cocos2d::Vec2(100.0f, 0)));
	this->grass2->runAction(cocos2d::MoveBy::create(2.0f, cocos2d::Vec2(-100.0f, 0)));
	this->grass1->runAction(cocos2d::MoveBy::create(2.0f, cocos2d::Vec2(150.0f, 0)));
	this->grass0->runAction(cocos2d::MoveBy::create(2.0f, cocos2d::Vec2(-150.0f, 0)));

	this->castle->runAction(cocos2d::MoveBy::create(2.0f, cocos2d::Vec2(0, 60.0f)));
	this->title->runAction(cocos2d::MoveBy::create(2.0f, cocos2d::Vec2(0, 50.0f)));
}

void titleScene::update(float delta)
{

}

void titleScene::initInputListeners()
{
	this->mouseInputListener = EventListenerMouse::create();
	this->mouseInputListener->onMouseMove = CC_CALLBACK_1(titleScene::onMouseMove, this);
	this->mouseInputListener->onMouseDown = CC_CALLBACK_1(titleScene::onMouseDown, this);
	this->mouseInputListener->onMouseUp = CC_CALLBACK_1(titleScene::onMouseUp, this);
	this->mouseInputListener->onMouseScroll = CC_CALLBACK_1(titleScene::onMouseScroll, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(this->mouseInputListener, this);

	this->keyInputListener = EventListenerKeyboard::create();
	this->keyInputListener->onKeyPressed = CC_CALLBACK_2(titleScene::onKeyPressed, this);
	this->keyInputListener->onKeyReleased = CC_CALLBACK_2(titleScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(this->keyInputListener, this);
}

void titleScene::onMouseMove(cocos2d::Event* event) 
{
	auto mouseEvent = static_cast<EventMouse*>(event);
	float x = mouseEvent->getCursorX();
	float y = mouseEvent->getCursorY();

	auto point = cocos2d::Vec2(x, y);
	this->cursor->setPosition(point);
}

void titleScene::onMouseDown(cocos2d::Event* event) 
{
	//auto mouseEvent = static_cast<EventMouse*>(event);
	//0 = left, 1 = right, 2 = middle
	//int mouseButton = mouseEvent->getMouseButton();
	//float x = mouseEvent->getCursorX();
	//float y = mouseEvent->getCursorY();
	if (!this->blockClick)
	{
		cocos2d::Director::getInstance()->replaceScene(cocos2d::TransitionFade::create(1.0f, GameScene::createScene()));
		this->blockClick = true;
		cocos2d::log("Scene transitioning");
	}
	//releaseInputListeners();
}

void titleScene::onMouseUp(cocos2d::Event* event) 
{
	//auto mouseEvent = static_cast<EventMouse*>(event);
	//0 = left, 1 = right, 2 = middle
	//int mouseButton = mouseEvent->getMouseButton();
	//float x = mouseEvent->getCursorX();
	//float y = mouseEvent->getCursorY();
}

void titleScene::onMouseScroll(cocos2d::Event* event) 
{
	//auto mouseEvent = static_cast<EventMouse*>(event);
	//float x = mouseEvent->getScrollX();
	//float y = mouseEvent->getScrollY();
}

void titleScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) 
{

}

void titleScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) 
{

}

void titleScene::releaseInputListeners()
{
	if(this->mouseInputListener != nullptr)
		_eventDispatcher->removeEventListener(this->mouseInputListener);
	if(this->keyInputListener != nullptr)
		_eventDispatcher->removeEventListener(this->keyInputListener);
}

void titleScene::onExit()
{
	cocos2d::Scene::onExit();
	// Uncomment this if you are using initInputListeners()
	releaseInputListeners(); 
}