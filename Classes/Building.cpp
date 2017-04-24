#include "Building.h"
#include "Shake.h"
#include "DataManager.h"

Building::Building(const int hitPoint, const bool destructable)
	: destructable(destructable)
	, hitPoint(hitPoint)
	, maxHitPoint(hitPoint)
	, buildingSprite(nullptr)
	, hpBarFrame(nullptr)
	, alwaysShowHp(false)
	, populationCost(0)
	, foodCost(0)
	, metalCost(0)
	, woodCost(0)
{}

void Building::initHpBar()
{
	this->hpBarFrame = cocos2d::Sprite::createWithSpriteFrameName("hpBarFrame.png");
	this->hpBarFrame->setColor(cocos2d::Color3B(11, 212, 0));
	this->hpBarFrame->setPosition(cocos2d::Vec2(0.0f, 20.0f));
	this->buildingNode->addChild(this->hpBarFrame);

	this->hpBar = cocos2d::ProgressTimer::create(cocos2d::Sprite::createWithSpriteFrameName("progressBar.png"));
	this->hpBar->setType(cocos2d::ProgressTimer::Type::BAR);
	this->hpBar->setMidpoint(cocos2d::Vec2(0, 0.5f));
	this->hpBar->setBarChangeRate(cocos2d::Vec2(1.0f, 0.0f));
	this->hpBar->setColor(cocos2d::Color3B(11, 212, 0));
	this->hpBar->setPercentage(100.0f);
	this->hpBar->setPosition(cocos2d::Vec2(70.0f, 12.0f));
	this->hpBarFrame->addChild(this->hpBar);

	this->hpBarFrame->setVisible(false);
}

void Building::initCost(const int pCost, const int fCost, const int mCost, const int wCost)
{
	this->populationCost = pCost;
	this->foodCost = fCost;
	this->metalCost = mCost;
	this->woodCost = wCost;
}

void Building::setHpBarVisibility(const bool visibility)
{
	if (this->hpBarFrame && !this->alwaysShowHp)
	{
		this->hpBarFrame->stopAllActions();
		this->hpBarFrame->setOpacity(255);
		this->hpBar->stopAllActions();
		this->hpBar->setOpacity(255);
		this->hpBar->setVisible(visibility);
		this->hpBarFrame->setVisible(visibility);
	}
}

void Building::updateHpBar(float percentage)
{
	if (this->hpBarFrame)
	{
		if (percentage <= 0.0f)
		{
			this->hpBar->setPercentage(0.0f);
		}
		else
		{
			this->hpBar->setPercentage(percentage);
			if (percentage > 60.0f)
			{
				this->hpBar->setColor(cocos2d::Color3B(11, 212, 0));
			}
			else if (percentage < 60.0f && percentage > 20.0f)
			{
				this->hpBar->setColor(cocos2d::Color3B::ORANGE);
			}
			else
			{
				this->hpBar->setColor(cocos2d::Color3B::RED);
			}
		}
	}
}

void Building::takeDamage(const int damage)
{
	if (damage > 0)
	{
		this->hitPoint -= damage;
		if (this->hitPoint < 0)
		{
			// destroy
			this->hitPoint = 0;
			this->updateHpBar(0.0f);
		}
		else
		{
			this->updateHpBar(static_cast<float>(this->hitPoint) / static_cast<float>(this->maxHitPoint) * 100.0f);
			this->buildingSprite->runAction(Shake::create(0.15f, 10.0f, 5.0f));
			if (this->alwaysShowHp == false)
			{
				this->flashHpBar();
			}
		}
	}
}

void Building::lockHpBarVisibility(const bool lock)
{
	this->alwaysShowHp = lock;
}

Building::BuildingType Building::getType()
{
	return this->type;
}

int Building::getMaxHp()
{
	return this->maxHitPoint;
}

int Building::getCurHp()
{
	return this->hitPoint;
}

std::string Building::getCostsAsStr()
{
	return std::to_string(populationCost) + "       " + std::to_string(metalCost) + "\n" + std::to_string(woodCost) + "       " + std::to_string(foodCost);
}

void Building::destroy()
{
	this->buildingNode->removeAllChildren();
	this->buildingNode->removeFromParent();
	this->buildingNode = nullptr;
}

void Building::setOpacity(const GLubyte opacity)
{
	this->buildingSprite->setOpacity(opacity);
}

void Building::flashHpBar()
{
	if (this->hpBarFrame)
	{
		auto seq = cocos2d::Sequence::create(cocos2d::Show::create(), cocos2d::FadeTo::create(0, 255), cocos2d::DelayTime::create(0.5f), cocos2d::FadeTo::create(0.2f, 0), cocos2d::Hide::create(), nullptr);
		this->hpBarFrame->runAction(seq->clone());
		this->hpBar->runAction(seq);
	}
}



Castle::Castle(cocos2d::Node* parent)
	: Building(DataManager::getInstance()->getData("BUILDING_DATA")->getInt("castle.hp"), false)
{
	this->buildingNode = cocos2d::Node::create();
	parent->addChild(this->buildingNode);

	this->buildingSprite = cocos2d::Sprite::createWithSpriteFrameName("castleBuilding.png");
	this->buildingSprite->setPosition(cocos2d::Vec2(0, 8.0f));
	//this->buildingSprite->setScale(1.2f);
	this->buildingNode->addChild(this->buildingSprite);

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
	castleFlagSprite->setPosition(cocos2d::Vec2(-8.0f, 44.0f));

	this->buildingNode->addChild(castleFlagSprite);

	auto animation = cocos2d::Animation::createWithSpriteFrames(animFrame, 0.1f);
	castleFlagSprite->runAction(cocos2d::RepeatForever::create(cocos2d::Animate::create(animation)));

	this->initHpBar();
}

void Castle::update(const float delta)
{
	// Slowly regenrates health ?
}



ResourceGenBuilding::ResourceGenBuilding(const float productionRate, const int hp)
	: Building(hp, true)
	, productionRate(productionRate)
	, producedResources(0)
	, progressBarTracker(0)
	, populationUsageRate(0)
	, foodUsageRate(0)
	, woodUsageRate(0)
	, metalUsageRate(0)
	, populationUsageTracker(0)
	, foodUsageTracker(0)
	, woodUsageTracker(0)
	, metalUsageTracker(0)
{
}

ResourceGenBuilding::~ResourceGenBuilding()
{
	this->rsGainAnimSeq->release();
	this->rsUseAnimSeq->release();
}

ResourceGenBuilding * ResourceGenBuilding::create(const std::string& key, cocos2d::Node* parent)
{
	auto bd = DataManager::getInstance()->getData("BUILDING_DATA");

	float pr = bd->getFloat(key + ".resource.productionRate");
	int hp = bd->getInt(key + ".hp");
	cocos2d::Vec2 offset = cocos2d::Vec2(bd->getFloat(key + ".sprite.offset.x"), bd->getFloat(key + ".sprite.offset.y"));
	std::string spriteType = bd->getString(key + ".sprite.typeName");

	auto newBuilding = new ResourceGenBuilding(pr, hp);

	int type = bd->getInt(key + ".type");

	if (type == 0 || type > 4)
	{
		assert(false);
	}
	else
	{
		newBuilding->type = static_cast<BuildingType>(type);
	}

	newBuilding->buildingNode = cocos2d::Node::create();
	parent->addChild(newBuilding->buildingNode);

	newBuilding->buildingSprite = cocos2d::Sprite::createWithSpriteFrameName(spriteType + "Building.png");
	newBuilding->buildingSprite->setPosition(offset);
	newBuilding->buildingNode->addChild(newBuilding->buildingSprite);

	newBuilding->initHpBar();
	newBuilding->initResourcePogressBar();
	newBuilding->initProductionProgressTimer();
	newBuilding->initRsUsageRate(bd->getInt(key + ".resource.useRate.population"), bd->getInt(key + ".resource.useRate.wood"), bd->getInt(key + ".resource.useRate.food"), bd->getInt(key + ".resource.useRate.metal"));
	newBuilding->initRsGainUseIcons(key);
	newBuilding->initCost(bd->getInt(key + ".cost.population"), bd->getInt(key + ".cost.food"), bd->getInt(key + ".cost.metal"), bd->getInt(key + ".cost.wood"));

	return newBuilding;
}

void ResourceGenBuilding::initProductionProgressTimer()
{
	this->productionProgressBg = cocos2d::Sprite::createWithSpriteFrameName("productionProgressRadialBg.png");
	this->productionProgressBg->setPosition(cocos2d::Vec2(0, 42.0f));
	this->buildingNode->addChild(this->productionProgressBg);

	this->productionProgressBar = cocos2d::ProgressTimer::create(cocos2d::Sprite::createWithSpriteFrameName("productionProgressRadial.png"));
	this->productionProgressBar->setType(cocos2d::ProgressTimer::Type::RADIAL);
	this->productionProgressBar->setColor(cocos2d::Color3B(245, 231, 51));
	this->productionProgressBar->setPosition(this->productionProgressBg->getContentSize() * 0.5f);
	this->productionProgressBg->addChild(this->productionProgressBar);
}

void ResourceGenBuilding::initRsGainUseIcons(const std::string& spriteType)
{
	this->rsGainIcon = cocos2d::Node::create();
	this->rsGainIcon->setCascadeOpacityEnabled(true);
	this->buildingNode->addChild(this->rsGainIcon);

	auto iconSprite = cocos2d::Sprite::createWithSpriteFrameName(spriteType + "Icon.png");
	iconSprite->setPosition(cocos2d::Vec2(-15.0f, 0));

	this->rsGainLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 60);
	this->rsGainLabel->setScale(0.5f);
	this->rsGainLabel->setPosition(cocos2d::Vec2(8.0f, 0));
	this->rsGainLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
	this->rsGainLabel->enableOutline(cocos2d::Color4B::BLACK, 8.0f);

	this->rsGainIcon->addChild(iconSprite);
	this->rsGainIcon->addChild(this->rsGainLabel);
	this->rsGainIcon->setScale(0.75f);

	this->rsGainAnimSeq = cocos2d::Sequence::create(cocos2d::Show::create(), cocos2d::FadeTo::create(0, 255),  cocos2d::MoveBy::create(0.75f, cocos2d::Vec2(0, 10.0f)), cocos2d::FadeTo::create(0.1f, 0), cocos2d::Hide::create(), nullptr);
	this->rsGainAnimSeq->retain();

	this->rsGainIcon->setVisible(false);



	this->rsUseIcon = cocos2d::Node::create();
	this->buildingNode->addChild(this->rsUseIcon);

	if (this->populationUsageRate > 0)
	{
		this->initRsUseIcons("villageIcon.png", static_cast<int>(ResourceManager::ResourceType::POPULATION));
	}

	if (this->foodUsageRate > 0)
	{
		this->initRsUseIcons("foodIcon.png", static_cast<int>(ResourceManager::ResourceType::FOOD));
	}
	else
	{
		if (this->type == BuildingType::POPULATION)
		{
			this->initRsUseIcons("foodIcon.png", static_cast<int>(ResourceManager::ResourceType::FOOD));
		}
	}

	if (this->woodUsageRate > 0)
	{
		this->initRsUseIcons("woodIcon.png", static_cast<int>(ResourceManager::ResourceType::WOOD));
	}

	if (this->metalUsageRate > 0)
	{
		this->initRsUseIcons("metalIcon.png", static_cast<int>(ResourceManager::ResourceType::METAL));
	}

	this->rsUseAnimSeq = cocos2d::Sequence::create(cocos2d::Show::create(), cocos2d::FadeTo::create(0, 255), cocos2d::MoveBy::create(0.75f, cocos2d::Vec2(0, -10.0f)), cocos2d::FadeTo::create(0.1f, 0), cocos2d::Hide::create(), nullptr);
	this->rsUseAnimSeq->retain();
}

void ResourceGenBuilding::initResourcePogressBar()
{
	this->rsBarFrame = cocos2d::Sprite::createWithSpriteFrameName("resourceBarFrame.png");
	this->rsBarFrame->setColor(cocos2d::Color3B(255, 140, 0));
	this->rsBarFrame->setPosition(cocos2d::Vec2(0.0f, 0.0f));
	this->buildingNode->addChild(this->rsBarFrame);

	this->rsBar = cocos2d::ProgressTimer::create(cocos2d::Sprite::createWithSpriteFrameName("progressBar.png"));
	this->rsBar->setType(cocos2d::ProgressTimer::Type::BAR);
	this->rsBar->setMidpoint(cocos2d::Vec2(0, 0.5f));
	this->rsBar->setBarChangeRate(cocos2d::Vec2(1.0f, 0.0f));
	this->rsBar->setColor(cocos2d::Color3B(255, 140, 0));
	this->rsBar->setPercentage(100.0f);
	this->rsBar->setPosition(cocos2d::Vec2(70.0f, 10.0f));
	this->rsBarFrame->addChild(this->rsBar);

	this->rsBarFrame->setVisible(false);
}

void ResourceGenBuilding::initRsUseIcons(const std::string & iconName, const int tag)
{
	// Uses population
	auto node = cocos2d::Node::create();
	node->setCascadeOpacityEnabled(true);
	node->setVisible(false);
	node->setTag(tag);
	this->rsUseIcon->addChild(node);

	auto sprite = cocos2d::Sprite::createWithSpriteFrameName(iconName);
	sprite->setPosition(cocos2d::Vec2(-15.0f, 0));
	sprite->setTag(1);

	auto label = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 60);
	label->setScale(0.5f);
	label->setPosition(cocos2d::Vec2(8.0f, 0));
	label->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
	label->enableOutline(cocos2d::Color4B::BLACK, 8.0f);
	label->setTag(0);

	node->addChild(sprite);
	node->addChild(label);
	node->setScale(0.75f);
}

void ResourceGenBuilding::initRsUsageRate(const int pr, const int wr, const int fr, const int mr)
{
	this->populationUsageRate = pr;
	this->woodUsageRate = wr;
	this->foodUsageRate = fr;
	this->metalUsageRate = mr;
}

ResourceGenBuilding * ResourceGenBuilding::createFarm(cocos2d::Node* parent)
{
	return ResourceGenBuilding::create("farm", parent);
}

ResourceGenBuilding * ResourceGenBuilding::createLumberMill(cocos2d::Node * parent)
{
	return ResourceGenBuilding::create("lumbermill", parent);
}

ResourceGenBuilding * ResourceGenBuilding::createMine(cocos2d::Node * parent)
{
	return ResourceGenBuilding::create("mine", parent);
}

ResourceGenBuilding * ResourceGenBuilding::createVillage(cocos2d::Node * parent)
{
	auto village =  ResourceGenBuilding::create("village", parent);

	village->initRsGainUseIcons("village");

	auto smokeParticle = cocos2d::ParticleSystemQuad::create("particles/smoke.plist");
	smokeParticle->setPosition(cocos2d::Vec2(-37.0f, 60.0f));
	parent->addChild(smokeParticle);

	return village;
}

void ResourceGenBuilding::update(const float delta)
{
	this->producedResources += (this->productionRate * delta);
	this->progressBarTracker += (this->productionRate * delta);

	if (this->progressBarTracker > 1.0f)
	{
		float fractal = this->progressBarTracker - static_cast<float>(static_cast<int>(this->progressBarTracker));
		this->productionProgressBar->setPercentage(fractal * 100.0f);
		this->progressBarTracker = fractal;
	}
	else
	{
		this->productionProgressBar->setPercentage(this->progressBarTracker * 100.0f);
	}

	auto rm = ResourceManager::getInstance();

	if (this->populationUsageRate > 0)
	{
		if (this->populationUsageTracker >= this->populationUsageRate)
		{
			if(rm->isPopulationEmpty())
			{
				// not enough population. Take damage
				this->takeDamage(5);
			}
			this->playRsUseAnim(cocos2d::Vec2(30.0f, -14.0f), static_cast<int>(ResourceManager::ResourceType::POPULATION), 1);
			this->populationUsageTracker -= this->populationUsageRate;
			rm->usePopulation(1);
		}
	}

	if (this->foodUsageRate > 0)
	{
		if (this->foodUsageTracker >= this->foodUsageRate)
		{
			if (rm->isFoodEmpty())
			{
				// not enough population. Take damage
				this->takeDamage(5);
			}
			this->playRsUseAnim(cocos2d::Vec2(30.0f, -14.0f), static_cast<int>(ResourceManager::ResourceType::FOOD), 1);
			this->foodUsageTracker -= this->foodUsageRate;
			rm->useFoods(1);
		}
	}

	if (this->woodUsageRate > 0)
	{
		if (this->woodUsageTracker >= this->woodUsageRate)
		{
			if (rm->isWoodEmpty())
			{
				// not enough population. Take damage
				this->takeDamage(5);
			}
			this->playRsUseAnim(cocos2d::Vec2(30.0f, -14.0f), static_cast<int>(ResourceManager::ResourceType::WOOD), 1);
			this->woodUsageTracker -= this->woodUsageRate;
			rm->useWoods(1);
		}
	}

	if (this->metalUsageRate > 0)
	{
		if (this->metalUsageTracker >= this->metalUsageRate)
		{
			if (rm->isMetalEmpty())
			{
				// not enough population. Take damage
				this->takeDamage(5);
			}
			this->playRsUseAnim(cocos2d::Vec2(30.0f, -14.0f), static_cast<int>(ResourceManager::ResourceType::METAL), 1);
			this->metalUsageTracker -= this->metalUsageRate;
			rm->useMetals(1);
		}
	}
}

int ResourceGenBuilding::getProducedResources()
{
	int point = static_cast<int>(this->producedResources);
	if (point >= 1)
	{
		float pointf = static_cast<float>(point);
		this->producedResources -= pointf;

		//cocos2d::log("intpart = %d, fractPart = %f", point, this->producedResources);

		if (populationUsageRate > 0)
		{
			this->populationUsageTracker += point;
		}

		if (foodUsageRate > 0)
		{
			this->foodUsageTracker += point;
		}

		if (metalUsageRate > 0)
		{
			this->metalUsageTracker += point;
		}

		if (woodUsageRate > 0)
		{
			this->woodUsageTracker += point;
		}


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

void ResourceGenBuilding::playRsGainAnim(const cocos2d::Vec2& startPos, const int point)
{
	this->rsGainIcon->setPosition(startPos);
	this->rsGainLabel->setString("+ " + std::to_string(point));
	this->rsGainIcon->runAction(this->rsGainAnimSeq->clone());
}

void ResourceGenBuilding::playRsUseAnim(const cocos2d::Vec2 & startPos, const int tag,  const int point)
{
	auto children = this->rsUseIcon->getChildren();

	if (children.empty())
	{
		return;
	}

	int counter = 0;
	for (auto child : children)
	{
		if (child->getNumberOfRunningActions() > 0)
		{
			counter++;
			continue;
		}
	}

	auto child = this->rsUseIcon->getChildByTag(tag);

	auto pos = cocos2d::Vec2(startPos.x, startPos.y - (static_cast<float>(counter) * 15.0f));

	if (this->type == BuildingType::POPULATION)
	{
		if (point == 3)
		{
			pos.x -= 7.0f;
		}
	}
	child->setPosition(pos);

	auto label = dynamic_cast<cocos2d::Label*>(child->getChildByTag(0));
	label->setString("- " + std::to_string(point));

	child->runAction(this->rsUseAnimSeq->clone());
}

void ResourceGenBuilding::clearCurrentProduction()
{
	this->producedResources = 0;
	this->progressBarTracker = 0;
	this->productionProgressBar->setPercentage(0);	
	
	this->populationUsageTracker = 0;
	this->foodUsageTracker = 0;
	this->woodUsageTracker = 0;
	this->metalUsageTracker = 0;
}

void ResourceGenBuilding::setRsBarVisibility(const bool visibility)
{
	if (this->rsBarFrame)
	{
		this->rsBarFrame->setVisible(visibility);
	}
}

float ResourceGenBuilding::getProductionRate()
{
	return this->productionRate;
}

void ResourceGenBuilding::updateRsBar(float percentage)
{
	if (percentage <= 0.0f)
	{
		this->rsBar->setPercentage(0.0f);
		this->rsBarFrame->setColor(cocos2d::Color3B::RED);
	}
	else
	{
		this->rsBar->setPercentage(percentage);
	}
}

void ResourceGenBuilding::returnPoint(const int point)
{
	if (point > 0)
	{
		this->producedResources += static_cast<float>(point);
	}
}

int ResourceGenBuilding::getUsage(ResourceManager::ResourceType type)
{
	switch (type)
	{
	case ResourceManager::ResourceType::POPULATION:
		return this->populationUsageRate;
		break;
	case ResourceManager::ResourceType::FOOD:
		return this->foodUsageRate;
		break;
	case ResourceManager::ResourceType::WOOD:
		return this->woodUsageRate;
		break;
	case ResourceManager::ResourceType::METAL:
		return this->metalUsageRate;
		break;
	case ResourceManager::ResourceType::NONE:
	case ResourceManager::ResourceType::TROOP:
	default:
		return 0;
		break;
	}

	return 0;
}
