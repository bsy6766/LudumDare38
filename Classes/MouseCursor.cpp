#include "MouseCursor.h"

MouseCursor::MouseCursor()
	: cursorNode(nullptr)
	, defaultCursor(nullptr)
	, cleanCursor(nullptr)
	, destroyCursor(nullptr)
	, buildFarmCursor(nullptr)
	, buildVillageCursor(nullptr)
	, buildLumbermillCursor(nullptr)
	, buildMineCursor(nullptr)
{
}

MouseCursor::~MouseCursor()
{
}

void MouseCursor::initCursors(cocos2d::Node * parent, const int z)
{
	this->cursorNode = cocos2d::Node::create();
	parent->addChild(this->cursorNode, z);

	this->defaultCursor = cocos2d::Sprite::createWithSpriteFrameName("defaultCursor.png");
	this->defaultCursor->setAnchorPoint(cocos2d::Vec2(0.0f, 1.0f));
	this->cursorNode->addChild(this->defaultCursor);

	this->cleanCursor = cocos2d::Sprite::createWithSpriteFrameName("cleanCursor.png");
	this->cleanCursor->setAnchorPoint(cocos2d::Vec2(0.0f, 1.0f));
	this->cleanCursor->setVisible(false);
	this->cursorNode->addChild(this->cleanCursor);

	this->destroyCursor = cocos2d::Sprite::createWithSpriteFrameName("destroyCursor.png");
	this->destroyCursor->setAnchorPoint(cocos2d::Vec2(0.0f, 1.0f));
	this->destroyCursor->setVisible(false);
	this->cursorNode->addChild(this->destroyCursor);

	this->buildFarmCursor = cocos2d::Sprite::createWithSpriteFrameName("farmBuilding.png");
	this->buildFarmCursor->setOpacity(200);
	this->buildFarmCursor->setVisible(false);
	this->cursorNode->addChild(this->buildFarmCursor);

	this->buildVillageCursor = cocos2d::Sprite::createWithSpriteFrameName("villageBuilding.png");
	this->buildVillageCursor->setOpacity(200);
	this->buildVillageCursor->setVisible(false);
	this->cursorNode->addChild(this->buildVillageCursor);

	this->buildLumbermillCursor = cocos2d::Sprite::createWithSpriteFrameName("lumbermillBuilding.png");
	this->buildLumbermillCursor->setOpacity(200);
	this->buildLumbermillCursor->setVisible(false); 
	this->cursorNode->addChild(this->buildLumbermillCursor);

	this->buildMineCursor = cocos2d::Sprite::createWithSpriteFrameName("mineBuilding.png");
	this->buildMineCursor->setOpacity(200);
	this->buildMineCursor->setVisible(false);
	this->cursorNode->addChild(this->buildMineCursor);

	this->type = CURSOR_TYPE::DEFAULT;
}

void MouseCursor::setType(CURSOR_TYPE type)
{
	hideAllCursor();

	switch (type)
	{
	case MouseCursor::CURSOR_TYPE::BUILD_FARM:
	{
		buildFarmCursor->setVisible(true);
		this->type = type;
	}
	break;
	case MouseCursor::CURSOR_TYPE::BUILD_VILLAGE:
	{
		buildVillageCursor->setVisible(true);
		this->type = type;
	}
	break;
	case MouseCursor::CURSOR_TYPE::BUILD_MINE:
	{
		buildMineCursor->setVisible(true);
		this->type = type;
	}
	break;
	case MouseCursor::CURSOR_TYPE::BUILD_LM:
	{
		buildLumbermillCursor->setVisible(true);
		this->type = type;
	}
	break;
	case MouseCursor::CURSOR_TYPE::CLEAN:
	{
		cleanCursor->setVisible(true);
		this->type = type;
	}
	break;
	case MouseCursor::CURSOR_TYPE::DESTROY:
	{
		destroyCursor->setVisible(true);
		this->type = type;
	}
	break;
	case MouseCursor::CURSOR_TYPE::DEFAULT:
	default:
	{
		defaultCursor->setVisible(true);
		this->type = MouseCursor::CURSOR_TYPE::DEFAULT;
	}
		break;
	}
}

void MouseCursor::setColor(const cocos2d::Color3B & color)
{
	switch (this->type)
	{
	case MouseCursor::CURSOR_TYPE::BUILD_FARM:
	{
		buildFarmCursor->setColor(color);
	}
	break;
	case MouseCursor::CURSOR_TYPE::BUILD_VILLAGE:
	{
		buildVillageCursor->setColor(color);
	}
	break;
	case MouseCursor::CURSOR_TYPE::BUILD_MINE:
	{
		buildMineCursor->setColor(color);
	}
	break;
	case MouseCursor::CURSOR_TYPE::BUILD_LM:
	{
		buildLumbermillCursor->setColor(color);
	}
	break;
	case MouseCursor::CURSOR_TYPE::CLEAN:
	{
		cleanCursor->setColor(color);
	}
	break;
	case MouseCursor::CURSOR_TYPE::DESTROY:
	{
		destroyCursor->setColor(color);
	}
	break;
	case MouseCursor::CURSOR_TYPE::DEFAULT:
	default:
	{
		defaultCursor->setColor(color);
	}
	break;
	}
}

void MouseCursor::updatePosition(const cocos2d::Vec2 & position)
{
	this->cursorNode->setPosition(position);
}

void MouseCursor::hideAllCursor()
{
	defaultCursor->setVisible(false);
	cleanCursor->setVisible(false);
	destroyCursor->setVisible(false);
	buildFarmCursor->setVisible(false);
	buildVillageCursor->setVisible(false);
	buildLumbermillCursor->setVisible(false);
	buildMineCursor->setVisible(false);
}