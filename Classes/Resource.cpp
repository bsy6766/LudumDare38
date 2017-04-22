#include "Resource.h"
#include "Utility.h"

void NaturalResource::init()
{
	this->enriched = false;
	this->totalAmount = Utility::Random::randomInt(40, 60);
	this->currentAmount = this->totalAmount;
}

void NaturalResource::init(NaturalResource& nr)
{
	this->enriched = nr.enriched;
	this->totalAmount = nr.totalAmount;
	this->currentAmount = this->totalAmount;
}

bool NaturalResource::isEnriched()
{
	return this->enriched;
}

void NaturalResource::makeEnriched()
{
	this->enriched = true;
}

bool NaturalResource::isDepleted()
{
	return this->currentAmount <= 0;
}

void NaturalResource::update(int& point)
{
	if (point > this->currentAmount)
	{
		point = this->currentAmount;
		this->currentAmount = 0;
	}
	else
	{
		this->currentAmount -= point;
	}
}

int NaturalResource::getRemaining()
{
	return this->currentAmount;
}

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager()
	: population(0)
	, woods(0)
	, metals(0)
	, foods(0)
	, troops(0)
	, populationCap(0)
	, woodsCap(0)
	, metalsCap(0)
	, foodsCap(0)
	, troopsCap(0)
	, updateUI(false)
{}

ResourceManager::~ResourceManager()
{}

void ResourceManager::needToUpdateUI()
{
	this->updateUI = true;
}

ResourceManager* ResourceManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new ResourceManager();
	}

	return instance;
}

void ResourceManager::deleteInstance()
{
	//check if instance is alive pointer
	if (instance != nullptr)
	{
		delete instance;
	}
}

void ResourceManager::initResources(const int population, const int woods, const int metals, const int foods, const int troops)
{
	this->population = population;
	this->woods = woods;
	this->metals = metals;
	this->foods = foods;
	this->troops = troops;

	this->updateUI = true;
}

void ResourceManager::initResourcesCap(const int populationCap, const int woodsCap, const int metalsCap, const int foodsCap, const int troopsCap)
{
	this->populationCap = populationCap;
	this->woodsCap = woodsCap;
	this->metalsCap = metalsCap;
	this->foodsCap = foodsCap;
	this->troopsCap = troopsCap;
}

int ResourceManager::getPopulation()
{
	return this->population;
}

int ResourceManager::getFoods()
{
	return this->foods;
}

int ResourceManager::getWoods()
{
	return this->woods;
}

int ResourceManager::getMetals()
{
	return this->metals;
}

int ResourceManager::getTroops()
{
	return this->troops;
}

int ResourceManager::getPopulationCap()
{
	return this->populationCap;
}

int ResourceManager::getFoodsCap()
{
	return this->foodsCap;
}

int ResourceManager::getWoodsCap()
{
	return this->woodsCap;
}

int ResourceManager::getMetalsCap()
{
	return this->metalsCap;
}

int ResourceManager::getTroopsCap()
{
	return this->troopsCap;
}

bool ResourceManager::isPopulationFull()
{
	return this->population >= this->populationCap;
}

bool ResourceManager::isFoodsFull()
{
	return this->foods >= this->foodsCap;
}

bool ResourceManager::isWoodsFull()
{
	return this->woods >= this->woodsCap;
}

bool ResourceManager::isMetalsFull()
{
	return this->metals >= this->metalsCap;
}

void ResourceManager::addResource(int & resource, const int& resourceCap, const int amount)
{
	if (amount > 0)
	{
		resource += amount;
		if (resource > resourceCap)
		{
			resource = resourceCap;
		}
		this->needToUpdateUI();
	}
}

void ResourceManager::addFoods(const int foods)
{
	this->addResource(this->foods, this->foodsCap, foods);
}

void ResourceManager::addWoods(const int woods)
{
	this->addResource(this->woods, this->woodsCap, woods);
}

void ResourceManager::addMetals(const int metals)
{
	this->addResource(this->metals, this->metalsCap, metals);
}

void ResourceManager::addPopulation(const int population)
{
	this->addResource(this->population, this->populationCap, population);
}

bool ResourceManager::doesNeedToUpdateUI()
{
	return this->updateUI;
}

void ResourceManager::markAsUIUpdated()
{
	this->updateUI = false;
}

void ResourceManager::print()
{
	cocos2d::log("ResourceManager...");
	cocos2d::log("population = %d", this->population);
	cocos2d::log("foods = %d", this->foods);
	cocos2d::log("woods = %d", this->woods);
	cocos2d::log("metals = %d", this->metals);
	cocos2d::log("troops = %d", this->troops);
}
