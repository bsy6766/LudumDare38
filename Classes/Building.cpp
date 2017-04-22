#include "Building.h"

Building::Building(const int hitPoint, const bool destructable)
	: destructable(destructable)
	, hitPoint(hitPoint)
	, buildingSprite(nullptr)
	, hpBarFrame(nullptr)
{}

void Building::setHpBarVisibility(const bool visibility)
{
	if (this->hpBarFrame)
	{
		this->hpBarFrame->setVisible(visibility);
	}
}



Castle::Castle(cocos2d::Node* parent)
	: Building(500, false)
{
	this->buildingSprite = cocos2d::Sprite::createWithSpriteFrameName("castleBuilding.png");
	this->buildingSprite->setPosition(cocos2d::Vec2(0, 8.0f));
	parent->addChild(this->buildingSprite);

	// add flag animation
	cocos2d::Vector<cocos2d::SpriteFrame*> animFrame(4);
	auto cache = cocos2d::SpriteFrameCache::getInstance();

	for (int i = 0; i < 4; i++)
	{
		auto spriteFrame = cache->getSpriteFrameByName("castleFlag_" + std::to_string(i) + ".png");
		spriteFrame->setAnchorPoint(cocos2d::Vec2(0.1f, 0.1f));
		animFrame.pushBack(spriteFrame);
	}

	auto castleFlagSprite = cocos2d::Sprite::createWithSpriteFrame(animFrame.front());
	castleFlagSprite->setPosition(cocos2d::Vec2(-8.0f, 34.0f));

	parent->addChild(castleFlagSprite);

	auto animation = cocos2d::Animation::createWithSpriteFrames(animFrame, 0.1f);
	castleFlagSprite->runAction(cocos2d::RepeatForever::create(cocos2d::Animate::create(animation)));

	this->hpBarFrame = cocos2d::Sprite::createWithSpriteFrameName("hpBarFrame.png");
	this->hpBarFrame->setColor(cocos2d::Color3B(11, 212, 0));
	this->hpBarFrame->setPosition(cocos2d::Vec2(0.0f, 20.0f));
	parent->addChild(this->hpBarFrame);

	this->hpBar = cocos2d::ProgressTimer::create(cocos2d::Sprite::createWithSpriteFrameName("hpBar.png"));
	this->hpBar->setType(cocos2d::ProgressTimer::Type::BAR);
	this->hpBar->setMidpoint(cocos2d::Vec2(0, 0.5f));
	this->hpBar->setBarChangeRate(cocos2d::Vec2(1.0f, 0.0f));
	this->hpBar->setColor(cocos2d::Color3B(11, 212, 0));
	this->hpBar->setPercentage(100.0f);
	this->hpBar->setPosition(cocos2d::Vec2(70.0f, 12.0f));
	this->hpBarFrame->addChild(this->hpBar);

	this->hpBarFrame->setVisible(false);
}

void Castle::update(const float delta)
{
	// Slowly regenrates health ?
}



ResourceGenBuilding::ResourceGenBuilding(const float productionRate, const int hp)
	: Building(hp, true)
	, productionRate(productionRate)
	, producedResources(0)
{
}

ResourceGenBuilding * ResourceGenBuilding::create(const float productionRate, const int hp, const std::string & spriteName, cocos2d::Node * parent)
{
	auto newBuilding = new ResourceGenBuilding(productionRate, hp);
	newBuilding->buildingSprite = cocos2d::Sprite::createWithSpriteFrameName(spriteName);
	parent->addChild(newBuilding->buildingSprite);
	return newBuilding;
}

ResourceGenBuilding * ResourceGenBuilding::createFarm(const bool boosted, cocos2d::Node* parent)
{
	return ResourceGenBuilding::create(boosted ? 2.0f : 1.0f, 50.0f, "farmBuilding.png", parent);
}

ResourceGenBuilding * ResourceGenBuilding::createLumberMill(const bool boosted, cocos2d::Node * parent)
{
	return ResourceGenBuilding::create(boosted ? 2.0f : 1.0f, 100.0f, "lumbermillBuilding.png", parent);
}

ResourceGenBuilding * ResourceGenBuilding::createMine(const bool boosted, cocos2d::Node * parent)
{
	return ResourceGenBuilding::create(boosted ? 2.0f : 1.0f, 120.0f, "mineBuilding.png", parent);
}

ResourceGenBuilding * ResourceGenBuilding::createVillage(cocos2d::Node * parent)
{
	return ResourceGenBuilding::create(2.0f, 70.0f, "villageBuilding.png", parent);
}

void ResourceGenBuilding::update(const float delta)
{
	this->producedResources += (this->productionRate * delta);
}

int ResourceGenBuilding::getProducedResources()
{
	int point = static_cast<int>(this->producedResources);
	if (point >= 1)
	{
		float pointf = static_cast<float>(point);
		this->producedResources -= pointf;

		//cocos2d::log("intpart = %d, fractPart = %f", point, this->producedResources);

		return point;
	}
	else
	{
		return 0;
	}

	/*
	float fractPart = modf(this->producedResources, &intPart);
	if (intPart >= 1.0f)
	{
		this->producedResources = fractPart;
		cocos2d::log("intpart = %d, fractPart = %f", static_cast<int>(intPart), fractPart);
		return static_cast<int>(intPart);
	}
	else
	{
		return 0;
	}
	*/
}

void ResourceGenBuilding::reduceProductionRateBy(const float rate)
{
	this->productionRate *= rate;
}
