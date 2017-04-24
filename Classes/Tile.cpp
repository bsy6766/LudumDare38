#include "Tile.h"
#include "Utility.h"
#include "GameScene.h"

LD38::Tile::Tile()
	: row(-1)
	, col(-1)
	, id(-1)
	, tileNode(nullptr)
	, emptyTileSprite(nullptr)
	, building(nullptr)
	, destroyed(false)
	, cleanElapsedTime(0)
	, cleanDuration(30)
	, buildElapsedTime(0)
	, buildDuration(15)
	, resourceType(ResourceManager::ResourceType::NONE)
	, selectedNaturalResource(nullptr)
	, dustParticle(nullptr)
	, enrichedParticle(nullptr)
	, state(STATE::IDLE)
{}

LD38::Tile::~Tile()
{
	if (this->building)
	{
		delete this->building;
		this->building = nullptr;
	}

	if (this->selectedNaturalResource)
	{
		delete this->selectedNaturalResource;
		this->selectedNaturalResource = nullptr;
	}
}

void LD38::Tile::initTile(const int id, const int row, const int col, const cocos2d::Vec2& pos, cocos2d::Node* parent)
{
	this->id = id;

	this->row = row;
	this->col = col;

	this->initNaturalResources();

	this->tileNode = cocos2d::Node::create();
	this->tileNode->setPosition(pos);
	parent->addChild(this->tileNode, 5 - row);

	std::string emptyTileSpriteName;

	int rand = Utility::Random::randomInt(0, 100);

	if (rand < 60.0f)
	{
		emptyTileSpriteName = "emptyTile0.png";
	}
	else if (rand >= 60.0f && rand <= 80.0f)
	{
		emptyTileSpriteName = "emptyTile1.png";
	}
	else
	{
		emptyTileSpriteName = "emptyTile2.png";
	}

	this->emptyTileSprite = cocos2d::Sprite::createWithSpriteFrameName(emptyTileSpriteName);
	this->tileNode->addChild(this->emptyTileSprite, static_cast<int>(Z_ORDER::BG));

	this->buildingWreckage = cocos2d::Sprite::createWithSpriteFrameName("buildingWreckage.png");
	this->buildingWreckage->setVisible(false);
	this->tileNode->addChild(this->buildingWreckage, static_cast<int>(Z_ORDER::WRECKAGE));

	this->cleaningBarFrame = cocos2d::Sprite::createWithSpriteFrameName("cleaningBarFrame.png");
	this->cleaningBarFrame->setPosition(cocos2d::Vec2(0.0f, 10.0f));
	this->tileNode->addChild(this->cleaningBarFrame, static_cast<int>(Z_ORDER::PROGRESS_BAR));

	this->cleaningBar = cocos2d::ProgressTimer::create(cocos2d::Sprite::createWithSpriteFrameName("cleaningBar.png"));
	this->cleaningBar->setType(cocos2d::ProgressTimer::Type::BAR);
	this->cleaningBar->setMidpoint(cocos2d::Vec2(0, 0.5f));
	this->cleaningBar->setBarChangeRate(cocos2d::Vec2(1.0f, 0.0f));
	this->cleaningBar->setPercentage(0.0f);
	this->cleaningBar->setPosition(cocos2d::Vec2(61.0f, 10.0f));
	this->cleaningBarFrame->addChild(this->cleaningBar);

	this->cleaningBarFrame->setVisible(false);

	this->buildingBarFrame = cocos2d::Sprite::createWithSpriteFrameName("buildingBarFrame.png");
	this->buildingBarFrame->setPosition(cocos2d::Vec2(0.0f, 10.0f));
	this->tileNode->addChild(this->buildingBarFrame, static_cast<int>(Z_ORDER::PROGRESS_BAR));

	this->buildingBar = cocos2d::ProgressTimer::create(cocos2d::Sprite::createWithSpriteFrameName("buildingBar.png"));
	this->buildingBar->setType(cocos2d::ProgressTimer::Type::BAR);
	this->buildingBar->setMidpoint(cocos2d::Vec2(0, 0.5f));
	this->buildingBar->setBarChangeRate(cocos2d::Vec2(1.0f, 0.0f));
	this->buildingBar->setPercentage(0.0f);
	this->buildingBar->setPosition(cocos2d::Vec2(61.0f, 10.0f));
	this->buildingBarFrame->addChild(this->buildingBar);

	this->buildingBarFrame->setVisible(false);

	this->enrichedParticle = nullptr;
}

void LD38::Tile::initCaslte()
{
	if (this->building == nullptr)
	{
		this->building = new Castle(this->tileNode);
	}
}

void LD38::Tile::initFarm()
{
	this->selectedNaturalResource = new NaturalResource();
	this->selectedNaturalResource->init(this->potentialResources.at(FOOD_INDEX));

	if (this->potentialResources.at(FOOD_INDEX).isEnriched() == false)
	{
		if (this->enrichedParticle)
		{
			this->enrichedParticle->pauseEmissions();
		}
	}

	this->building->setOpacity(255);
}

void LD38::Tile::initLumberMill()
{
	this->selectedNaturalResource = new NaturalResource();
	this->selectedNaturalResource->init(this->potentialResources.at(WOOD_INDEX));

	if (this->potentialResources.at(WOOD_INDEX).isEnriched() == false)
	{
		if (this->enrichedParticle)
		{
			this->enrichedParticle->pauseEmissions();
		}
	}

	this->building->setOpacity(255);
}

void LD38::Tile::initMine()
{
	this->selectedNaturalResource = new NaturalResource();
	this->selectedNaturalResource->init(this->potentialResources.at(METAL_INDEX));

	if (this->potentialResources.at(METAL_INDEX).isEnriched() == false)
	{
		if (this->enrichedParticle)
		{
			this->enrichedParticle->pauseEmissions();
		}
	}

	this->building->setOpacity(255);
}

void LD38::Tile::initVillage()
{
	if (this->enrichedParticle)
	{
		this->enrichedParticle->pauseEmissions();
	}

	this->building->setOpacity(255);
}

void LD38::Tile::update(const float delta)
{
	if (this->state == STATE::IDLE)
	{
		if (this->hasBuilding())
		{
			auto resourceGenBuilding = dynamic_cast<ResourceGenBuilding*>(this->building);
			if (resourceGenBuilding)
			{
				auto rm = ResourceManager::getInstance();

				if (this->resourceType == ResourceManager::ResourceType::POPULATION)
				{
					// population can always growth till cap 
					if (rm->isPopulationFull() == false)
					{
						this->building->update(delta);
						int point = resourceGenBuilding->getProducedResources();
						if (point > 0)
						{
							bool success = rm->addPopulation(point);
							if (success)
							{
								resourceGenBuilding->playRsGainAnim(cocos2d::Vec2(-30.0f, -34.0f), point);
								resourceGenBuilding->playRsUseAnim(cocos2d::Vec2(30.0f, -14.0f), static_cast<int>(ResourceManager::ResourceType::FOOD), 3);
								//cocos2d::log("Tile #%d generated %d population.", this->id, point);
							}
							else
							{
								//resourceGenBuilding->returnPoint(point);
								//cocos2d::log("Tile #%d failed to generate %d population due to low food");
								resourceGenBuilding->takeDamage(10);
							}
						}
					}
				}
				else
				{
					// woods, foods, metals can be growth till cap. Production rate will be halved if natural resource depletes
					bool capped = false;
					// But check if resource is capped.
					switch (this->resourceType)
					{
					case ResourceManager::ResourceType::FOOD:
					{
						capped = rm->isFoodsFull();
					}
					break;
					case ResourceManager::ResourceType::WOOD:
					{
						capped = rm->isWoodsFull();
					}
					break;
					case ResourceManager::ResourceType::METAL:
					{
						capped = rm->isMetalsFull();
					}
					break;
					default:
						return;
						break;
					}

					if (capped == false)
					{
						// not capped yet
						this->building->update(delta);

						int point = resourceGenBuilding->getProducedResources();
						if (point > 0)
						{
							if (this->selectedNaturalResource->isDepleted() == false)
							{
								// natural resource not depleted yet. Update amount
								this->selectedNaturalResource->update(point);

								// check again and if it's depleted, reduce produiction rate by half
								if (this->selectedNaturalResource->isDepleted())
								{
									resourceGenBuilding->reduceProductionRateBy(0.15f);
									if (this->enrichedParticle)
									{
										this->enrichedParticle->pauseEmissions();
									}
								}
							}
							// No need to update natrual resource if it's depleted

							point *= this->selectedNaturalResource->getEnrichedModifier();

							switch (this->resourceType)
							{
							case ResourceManager::ResourceType::FOOD:
							{
								cocos2d::log("Tile #%d generated %d foods. %d foods remaining.", this->id, point, this->selectedNaturalResource->getRemaining());
								ResourceManager::getInstance()->addFoods(point);
							}
							break;
							case ResourceManager::ResourceType::WOOD:
							{
								cocos2d::log("Tile #%d generated %d wood. %d wood remaining.", this->id, point, this->selectedNaturalResource->getRemaining());
								ResourceManager::getInstance()->addWoods(point);
							}
							break;
							case ResourceManager::ResourceType::METAL:
							{
								cocos2d::log("Tile #%d generated %d metal. %d metal remaining.", this->id, point, this->selectedNaturalResource->getRemaining());
								ResourceManager::getInstance()->addMetals(point);
							}
							break;
							default:
								break;
							}

							resourceGenBuilding->playRsGainAnim(cocos2d::Vec2(-30.0f, -34.0f), point);
							resourceGenBuilding->updateRsBar(static_cast<float>(this->selectedNaturalResource->getRemaining()) / static_cast<float>(this->selectedNaturalResource->getTotal()) * 100.0f);

							// Check cap again
							switch (this->resourceType)
							{
							case ResourceManager::ResourceType::FOOD:
							{
								if (rm->isFoodsFull())
								{
									resourceGenBuilding->clearCurrentProduction();
								}
							}
							break;
							case ResourceManager::ResourceType::WOOD:
							{
								if (rm->isWoodsFull())
								{
									resourceGenBuilding->clearCurrentProduction();
								}
							}
							break;
							case ResourceManager::ResourceType::METAL:
							{
								if (rm->isMetalsFull())
								{
									resourceGenBuilding->clearCurrentProduction();
								}
							}
							break;
							default:
								break;
							}
						}
					}
				}
			}
			else
			{
				// Not resource generator
				this->building->update(delta);
			}
		}
	}
	else if (this->state == STATE::CLEANING)
	{
		this->cleanElapsedTime += delta;
		if (this->cleanElapsedTime >= this->cleanDuration)
		{
			this->cleaningBar->setPercentage(100.0f);
			this->onCleanUpFinished();
			this->dustParticle->pauseEmissions();
			this->cleanElapsedTime = 0;
			this->state = STATE::IDLE;
		}
		else
		{
			this->cleaningBar->setPercentage(this->cleanElapsedTime / this->cleanDuration * 100.0f);
		}
	}
	else if (this->state == STATE::BUILDING)
	{
		this->buildElapsedTime += delta;
		if (this->buildElapsedTime >= this->buildDuration)
		{
			this->buildingBar->setPercentage(100.0f);
			this->onBuildFinished();
			this->dustParticle->pauseEmissions();
			this->buildElapsedTime = 0;
			this->state = STATE::IDLE;
		}
		else
		{
			this->buildingBar->setPercentage(this->buildElapsedTime / this->buildDuration * 100.0f);
		}
	}
}

bool LD38::Tile::hasBuilding()
{
	return this->building != nullptr;
}

bool LD38::Tile::containsPoint(const cocos2d::Vec2 & point)
{
	auto bb = this->emptyTileSprite->getBoundingBox();
	bb.origin += this->tileNode->getPosition();
	return bb.containsPoint(point);
}

int LD38::Tile::getId()
{
	return this->id;
}

cocos2d::Vec2 LD38::Tile::getTilePosition()
{
	return this->tileNode->getPosition();
}

void LD38::Tile::showHpBar(const bool selected)
{
	if (this->building)
	{
		this->building->setHpBarVisibility(true);
		this->building->lockHpBarVisibility(selected);
	}
}

void LD38::Tile::hideHpBar()
{
	if (this->building)
	{
		this->building->lockHpBarVisibility(false);
		this->building->setHpBarVisibility(false);
	}
}

void LD38::Tile::flashHpBar()
{
	if (this->building)
	{
		this->building->flashHpBar();
	}
}

void LD38::Tile::showRsBar()
{
	if (this->building && this->resourceType != ResourceManager::ResourceType::POPULATION)
	{
		this->building->setRsBarVisibility(true);
	}
}

void LD38::Tile::hideRsBar()
{
	if (this->building && this->resourceType != ResourceManager::ResourceType::POPULATION)
	{
		this->building->setRsBarVisibility(false);
	}
}

bool LD38::Tile::isEmptyTile()
{
	return this->building == nullptr;
}

bool LD38::Tile::isCastle()
{
	return this->id == 12 && this->row == 2 && this->col == 2;
}

ResourceGenBuilding::BuildingType LD38::Tile::getBuildingType()
{
	if (this->building)
	{
		return this->building->getType();
	}
	else
	{
		return Building::BuildingType::NONE;
	}
}

int LD38::Tile::getBuildingMaxHp()
{
	if (this->building)
	{
		return this->building->getMaxHp();
	}
	else
	{
		return 0;
	}
}

int LD38::Tile::getBuildingCurHp()
{
	if (this->building)
	{
		return this->building->getCurHp();
	}
	else
	{
		return 0;
	}
}

float LD38::Tile::getBuildingProductionRate()
{
	if (this->building)
	{
		return this->building->getProductionRate();
	}
	else
	{
		return 0;
	}
}

int LD38::Tile::getBuildingUsageRate(ResourceManager::ResourceType type)
{
	if (this->building)
	{
		if (this->isCastle())
		{
			return 0;
		}

		auto rsGenBuilding = dynamic_cast<ResourceGenBuilding*>(this->building);
		if (rsGenBuilding)
		{
			return rsGenBuilding->getUsage(type);
		}
		else
		{
			return 0;
		}
	}

	return 0;
}

std::string LD38::Tile::getNaturalResourcesAsStr()
{
	if (this->destroyed)
	{
		return "0\n0\n0";
	}
	else
	{
		std::string str;
		str += std::to_string(this->potentialResources.at(WOOD_INDEX).getTotal()) + "\n";
		str += std::to_string(this->potentialResources.at(METAL_INDEX).getTotal()) + "\n";
		str += std::to_string(this->potentialResources.at(FOOD_INDEX).getTotal());

		return str;
	}
}

void LD38::Tile::makeNaturalResourceEnriched(const int index)
{
	if (this->enrichedParticle == nullptr)
	{
		// lazy loading
		this->enrichedParticle = cocos2d::ParticleSystemQuad::create("particles/enriched.plist");
		this->tileNode->addChild(this->enrichedParticle, static_cast<int>(Z_ORDER::PARTICLES));
	}

	this->enrichedParticle->resumeEmissions();

	this->potentialResources.at(index).makeEnriched(2);

	cocos2d::log("tile #%d is enriched! index = %d", this->id, index);
}

int LD38::Tile::hasEnrichedNR()
{
	int counter = 0;
	for (auto nr : this->potentialResources)
	{
		if (nr.isEnriched())
		{
			return counter;
		}

		counter++;
	}

	return -1;
}

const std::string LD38::Tile::getBuildingCostAsStr()
{
	if (this->building)
	{
		return this->building->getCostsAsStr();
	}
	else
	{
		return "";
	}
}

void LD38::Tile::buildFarm()
{
	this->clearBuilding();
	this->clearNaturalResource();

	this->resourceType = ResourceManager::ResourceType::FOOD;

	initBuild();
}

void LD38::Tile::buildVillage()
{
	this->clearBuilding();
	this->clearNaturalResource();

	this->resourceType = ResourceManager::ResourceType::POPULATION;

	initBuild();
}

void LD38::Tile::buildMine()
{
	this->clearBuilding();
	this->clearNaturalResource();

	this->resourceType = ResourceManager::ResourceType::METAL;

	initBuild();
}

void LD38::Tile::buildLumbermill()
{
	this->clearBuilding();
	this->clearNaturalResource();

	this->resourceType = ResourceManager::ResourceType::WOOD;

	initBuild();
}

void LD38::Tile::destroyBuilding()
{
	cocos2d::log("Tile #%d building is destroyed", this->id);
	this->clearBuilding();
	this->clearNaturalResource();
	this->destroyed = true;
	this->buildingWreckage->setVisible(true);
	this->resourceType = ResourceManager::ResourceType::NONE;
}

bool LD38::Tile::isDestroyed()
{
	return this->destroyed;
}

bool LD38::Tile::isBuilding()
{
	return this->state == STATE::BUILDING;
}

void LD38::Tile::cleanUp()
{
	this->cleaningBarFrame->setVisible(true);
	this->state = STATE::CLEANING;

	//auto seq = cocos2d::Sequence::create(cocos2d::ProgressFromTo::create(20.0f, 0.0f, 100.0f), cocos2d::CallFunc::create(CC_CALLBACK_0(LD38::Tile::onCleanUpFinished, this)), nullptr);
	//this->cleaningBar->runAction(seq);

	if (this->dustParticle == nullptr)
	{
		//lazy loading
		this->dustParticle = cocos2d::ParticleSystemQuad::create("particles/buildInfinite.plist");
		this->dustParticle->setPosition(cocos2d::Vec2::ZERO);
		this->tileNode->addChild(this->dustParticle, static_cast<int>(Z_ORDER::PARTICLES));
	}

	this->dustParticle->resumeEmissions();
}

int LD38::Tile::getRemainigResource()
{
	if (this->selectedNaturalResource)
	{
		return this->selectedNaturalResource->getRemaining();
	}
	else
	{
		return 0;
	}
}

bool LD38::Tile::updateResource(int & point, int & resource)
{
	if (point > resource)
	{
		point = resource;
		resource = 0;
		return true;
	}
	else
	{
		resource -= point;
		return false;
	}
}

void LD38::Tile::clearBuilding()
{
	if (this->building)
	{
		// Destroy existing building
		this->building->destroy();
		delete this->building;
		this->building = nullptr;
	}
}

void LD38::Tile::clearNaturalResource()
{
	if (this->selectedNaturalResource)
	{
		delete this->selectedNaturalResource;
		this->selectedNaturalResource = nullptr;
	}
}

void LD38::Tile::initNaturalResources()
{
	this->potentialResources.clear();

	for (int i = 0; i < 3; i++)
	{
		this->potentialResources.push_back(NaturalResource());
		this->potentialResources.back().init();
	}
}

void LD38::Tile::onCleanUpFinished()
{
	this->state = STATE::IDLE;

	this->cleaningBarFrame->runAction(cocos2d::Sequence::create(cocos2d::FadeTo::create(0.5f, 0), cocos2d::Hide::create(), cocos2d::FadeTo::create(0, 255), nullptr));
	this->cleaningBar->runAction(cocos2d::Sequence::create(cocos2d::FadeTo::create(0.5f, 0), cocos2d::Hide::create(), cocos2d::FadeTo::create(0, 255), nullptr));

	this->destroyed = false;
	this->buildingWreckage->runAction(cocos2d::Sequence::create(cocos2d::FadeTo::create(0.5f, 0), cocos2d::Hide::create(), cocos2d::FadeTo::create(0, 255), nullptr));

	this->rerollNaturalResources();

	dynamic_cast<GameScene*>(cocos2d::Director::getInstance()->getRunningScene())->requestRightPanelUpdate(this->id);
}

void LD38::Tile::onBuildFinished()
{
	this->buildingBarFrame->runAction(cocos2d::Sequence::create(cocos2d::FadeTo::create(0.5f, 0), cocos2d::Hide::create(), cocos2d::FadeTo::create(0, 255), nullptr));
	this->buildingBar->runAction(cocos2d::Sequence::create(cocos2d::FadeTo::create(0.5f, 0), cocos2d::Hide::create(), cocos2d::FadeTo::create(0, 255), nullptr));

	switch (this->resourceType)
	{
	case ResourceManager::ResourceType::POPULATION:
		this->initVillage();
		break;
	case ResourceManager::ResourceType::FOOD:
		this->initFarm();
		break;
	case ResourceManager::ResourceType::WOOD:
		this->initLumberMill();
		break;
	case ResourceManager::ResourceType::METAL:
		this->initMine();
		break;
	default:
		break;
	}

	this->state = STATE::IDLE;

	dynamic_cast<GameScene*>(cocos2d::Director::getInstance()->getRunningScene())->requestRightPanelUpdate(this->id);
}

void LD38::Tile::rerollNaturalResources()
{
	this->potentialResources.clear();

	for (int i = 0; i < 3; i++)
	{
		this->potentialResources.push_back(NaturalResource());
		this->potentialResources.back().init();
	}

	if (Utility::Random::randomIntRollCheck(20))
	{
		this->makeNaturalResourceEnriched(Utility::Random::randomInt(0, 2));
	}
}

void LD38::Tile::initBuild()
{
	this->buildingBarFrame->setVisible(true);
	this->state = STATE::BUILDING;

	if (this->dustParticle == nullptr)
	{
		//lazy loading
		this->dustParticle = cocos2d::ParticleSystemQuad::create("particles/buildInfinite.plist");
		this->dustParticle->setPosition(cocos2d::Vec2::ZERO);
		this->tileNode->addChild(this->dustParticle, static_cast<int>(Z_ORDER::PARTICLES));
	}

	this->dustParticle->resumeEmissions();


	// create at first
	switch (this->resourceType)
	{
	case ResourceManager::ResourceType::POPULATION:
		this->building = ResourceGenBuilding::createVillage(this->tileNode);
		break;
	case ResourceManager::ResourceType::FOOD:
		this->building = ResourceGenBuilding::createFarm(this->tileNode);
		break;
	case ResourceManager::ResourceType::WOOD:
		this->building = ResourceGenBuilding::createLumberMill(this->tileNode);
		break;
	case ResourceManager::ResourceType::METAL:
		this->building = ResourceGenBuilding::createMine(this->tileNode);
		break;
	default:
		break;
	}

	this->building->setOpacity(180);
}
