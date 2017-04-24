#include "Tile.h"
#include "Utility.h"

LD38::Tile::Tile()
	: row(-1)
	, col(-1)
	, id(-1)
	, tileNode(nullptr)
	, emptyTileSprite(nullptr)
	, building(nullptr)
	, resourceType(ResourceManager::ResourceType::NONE)
	, selectedNaturalResource(nullptr)
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

	for (int i = 0; i < 3; i++)
	{
		this->potentialResources.push_back(NaturalResource());
		this->potentialResources.back().init();
	}

	int randomEnrich = Utility::Random::randomInt(0, 2);

	//this->potentialResources.at(randomEnrich).makeEnriched();

	this->tileNode = cocos2d::Node::create();
	this->tileNode->setPosition(pos);
	parent->addChild(this->tileNode, 5 - row);

	this->emptyTileSprite = cocos2d::Sprite::createWithSpriteFrameName("emptyTile.png");
	this->tileNode->addChild(this->emptyTileSprite);

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
	if (this->building == nullptr)
	{
		this->selectedNaturalResource = new NaturalResource();
		this->selectedNaturalResource->init(this->potentialResources.at(FOOD_INDEX));

		this->building = ResourceGenBuilding::createFarm(this->selectedNaturalResource->isEnriched(), this->tileNode);

		this->resourceType = ResourceManager::ResourceType::FOOD;
	}
}

void LD38::Tile::initLumberMill()
{
	if (this->building == nullptr)
	{
		this->selectedNaturalResource = new NaturalResource();
		this->selectedNaturalResource->init(this->potentialResources.at(WOOD_INDEX));

		this->building = ResourceGenBuilding::createLumberMill(this->selectedNaturalResource->isEnriched(), this->tileNode);

		this->resourceType = ResourceManager::ResourceType::WOOD;
	}
}

void LD38::Tile::initMine()
{
	if (this->building == nullptr)
	{
		this->selectedNaturalResource = new NaturalResource();
		this->selectedNaturalResource->init(this->potentialResources.at(METAL_INDEX));

		this->building = ResourceGenBuilding::createMine(this->selectedNaturalResource->isEnriched(), this->tileNode);

		this->resourceType = ResourceManager::ResourceType::METAL;
	}
}

void LD38::Tile::initVillage()
{
	if (this->building == nullptr)
	{
		this->building = ResourceGenBuilding::createVillage(this->tileNode);
		
		this->resourceType = ResourceManager::ResourceType::POPULATION;
	}
}

void LD38::Tile::update(const float delta)
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
									this->enrichedParticle->removeFromParentAndCleanup(true);
									this->enrichedParticle = nullptr;
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
		0;
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
}

std::string LD38::Tile::getNaturalResourcesAsStr()
{
	std::string str; 
	str += std::to_string(this->potentialResources.at(METAL_INDEX).getTotal()) + "\n";
	str += std::to_string(this->potentialResources.at(WOOD_INDEX).getTotal()) + "\n";
	str += std::to_string(this->potentialResources.at(FOOD_INDEX).getTotal());

	return str;
}

void LD38::Tile::makeNaturalResourceEnriched(const int index)
{
	this->enrichedParticle = cocos2d::ParticleSystemQuad::create("particles/enriched.plist");
	this->tileNode->addChild(this->enrichedParticle, 100);

	this->potentialResources.at(index).makeEnriched(2.0f);

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
