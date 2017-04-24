#include "GameSpeed.h"



GameSpeed::GameSpeed()
	: speedIconNode(nullptr)
	, speedx0(nullptr)
	, speedx1(nullptr)
	, speedx2(nullptr)
	, speedx3(nullptr)
	, curSprite(CUR_SPRITE::X1)
	, speed(1.0f)
{}

GameSpeed::~GameSpeed()
{
}

void GameSpeed::init(cocos2d::Node * parent, const int z)
{
	auto x = cocos2d::Director::getInstance()->getVisibleSize().width * 0.5f;

	this->speedIconNode = cocos2d::Node::create();
	this->speedIconNode->setPosition(cocos2d::Vec2(x, 25.0f));
	parent->addChild(this->speedIconNode, z);

	this->speedx0 = cocos2d::Sprite::createWithSpriteFrameName("speedX0.png");
	this->speedx0->setVisible(false);
	this->speedIconNode->addChild(this->speedx0);

	this->speedx1 = cocos2d::Sprite::createWithSpriteFrameName("speedX1.png");
	this->speedx1->setVisible(true);
	this->speedIconNode->addChild(this->speedx1);

	this->speedx2 = cocos2d::Sprite::createWithSpriteFrameName("speedX2.png");
	this->speedx2->setVisible(false);
	this->speedIconNode->addChild(this->speedx2);

	this->speedx3 = cocos2d::Sprite::createWithSpriteFrameName("speedX3.png");
	this->speedx3->setVisible(false);
	this->speedIconNode->addChild(this->speedx3);

}

void GameSpeed::setSpeed(const float speed)
{
	if (speed < 0.0f)
	{
		return;
	}
	else
	{
		this->speed = speed;
	}
}

bool GameSpeed::mouseHover(const cocos2d::Vec2 & point)
{
	auto shift = this->speedIconNode->getPosition();

	switch (this->curSprite)
	{
	case GameSpeed::CUR_SPRITE::PAUSED:
	{
		auto bb = this->speedx0->getBoundingBox();
		bb.origin += shift;
		if (bb.containsPoint(point))
		{
			this->speedx0->setScale(1.1f);
			return true;
		}
		else
		{
			this->speedx0->setScale(1.0f);
			return false;
		}
	}
		break;
	case GameSpeed::CUR_SPRITE::X1:
	{
		auto bb = this->speedx1->getBoundingBox();
		bb.origin += shift;
		if (bb.containsPoint(point))
		{
			this->speedx1->setScale(1.1f);
			return true;
		}
		else
		{
			this->speedx1->setScale(1.0f);
			return false;
		}
	}
		break;
	case GameSpeed::CUR_SPRITE::X2:
	{
		auto bb = this->speedx2->getBoundingBox();
		bb.origin += shift;
		if (bb.containsPoint(point))
		{
			this->speedx2->setScale(1.1f);
			return true;
		}
		else
		{
			this->speedx2->setScale(1.0f);
			return false;
		}
	}
		break;
	case GameSpeed::CUR_SPRITE::X3:
	{
		auto bb = this->speedx3->getBoundingBox();
		bb.origin += shift;
		if (bb.containsPoint(point))
		{
			this->speedx3->setScale(1.1f);
			return true;
		}
		else
		{
			this->speedx3->setScale(1.0f);
			return false;
		}
	}
		break;
	default:
		break;
	}

	return false;
}

bool GameSpeed::mouseClick(const cocos2d::Vec2 & point)
{
	auto shift = this->speedIconNode->getPosition();

	switch (this->curSprite)
	{
	case GameSpeed::CUR_SPRITE::PAUSED:
	{
		auto bb = this->speedx0->getBoundingBox();
		bb.origin += shift;
		if (bb.containsPoint(point))
		{
			this->speedx0->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.9f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr));
			this->curSprite = CUR_SPRITE::X1;
			this->speedx0->setVisible(false);
			this->speedx1->setVisible(true);
			this->setSpeed(1.0f);
			return true;
		}
	}
	break;
	case GameSpeed::CUR_SPRITE::X1:
	{
		auto bb = this->speedx1->getBoundingBox();
		bb.origin += shift;
		if (bb.containsPoint(point))
		{
			this->speedx1->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.9f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr));
			this->curSprite = CUR_SPRITE::X2;
			this->speedx1->setVisible(false);
			this->speedx2->setVisible(true);
			this->setSpeed(2.0f);
			return true;
		}
	}
	break;
	case GameSpeed::CUR_SPRITE::X2:
	{
		auto bb = this->speedx2->getBoundingBox();
		bb.origin += shift;
		if (bb.containsPoint(point))
		{
			this->speedx2->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.9f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr));
			this->curSprite = CUR_SPRITE::X3;
			this->speedx2->setVisible(false);
			this->speedx3->setVisible(true);
			this->setSpeed(3.0f);
			return true;
		}
	}
	break;
	case GameSpeed::CUR_SPRITE::X3:
	{
		auto bb = this->speedx3->getBoundingBox();
		bb.origin += shift;
		if (bb.containsPoint(point))
		{
			this->speedx3->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.9f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr));
			this->curSprite = CUR_SPRITE::PAUSED;
			this->speedx3->setVisible(false);
			this->speedx0->setVisible(true);
			this->setSpeed(0.0f);
			return true;
		}
	}
	break;
	default:
		break;
	}

	return false;
}
