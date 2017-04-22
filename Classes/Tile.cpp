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
					//this->building->update(delta);
					int point = resourceGenBuilding->getProducedResources();
					if (point > 0)
					{
						rm->addPopulation(point);
						cocos2d::log("Tile #%d generated %d population.", this->id, point);
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
							}
						}
						// No need to update natrual resource if it's depleted

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

void LD38::Tile::showHpBar()
{
	if (this->building)
	{
		this->building->setHpBarVisibility(true);
	}
}

void LD38::Tile::hideHpBar()
{
	if (this->building)
	{
		this->building->setHpBarVisibility(false);
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
