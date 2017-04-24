#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "cocos2d.h"

struct NaturalResource
{
private:
	int enrichedModifier;
	int totalAmount;
	int currentAmount;
public:
	void init();
	void init(NaturalResource& nr);
	bool isEnriched();
	void makeEnriched(const int value);
	bool isDepleted();
	void update(int& point);
	int getRemaining();
	int getTotal();
	int getEnrichedModifier();
};

class ResourceManager
{
public:
	enum class ResourceType
	{
		NONE,
		POPULATION,
		FOOD,
		WOOD,
		METAL,
		TROOP
	};
private:
	ResourceManager();
	~ResourceManager();

	static ResourceManager* instance;

	int population;
	int woods;
	int metals;
	int foods;
	int troops;

	int populationCap;
	int woodsCap;
	int metalsCap;
	int foodsCap;
	int troopsCap;

	bool updateUI;

	void needToUpdateUI();
	void addResource(int& resource, const int& resourceCap, const int amount);
	bool useResource(int& resource, const int& resourceCap, const int amount);

	void updateCapacity();
public:
	// Get instance
	static ResourceManager* getInstance();

	// Destroy instance.
	static void deleteInstance();

	// Prevent copying or assigning instance
	ResourceManager(ResourceManager const&) = delete;
	void operator=(ResourceManager const&) = delete;

	// init
	void initResources(const int population, const int woods, const int metals, const int foods, const int troops);
	void initResourcesCap(const int populationCap, const int woodsCap, const int metalsCap, const int foodsCap, const int troopsCap);

	// getter
	int getPopulation();
	int getFoods();
	int getWoods();
	int getMetals();
	int getTroops();

	int getPopulationCap();
	int getFoodsCap();
	int getWoodsCap();
	int getMetalsCap();
	int getTroopsCap();

	// cap check
	bool isPopulationFull();
	bool isFoodsFull();
	bool isWoodsFull();
	bool isMetalsFull();

	bool isPopulationEmpty();
	bool isFoodEmpty();
	bool isWoodEmpty();
	bool isMetalEmpty();

	// add 
	void addFoods(const int foods);
	void addWoods(const int woods);
	void addMetals(const int metals);
	bool addPopulation(const int population);

	// use
	void useFoods(const int foods);
	void useWoods(const int woods);
	void useMetals(const int metals);
	void usePopulation(const int population);

	// update f;ag
	bool doesNeedToUpdateUI();
	void markAsUIUpdated();

	bool checkCost(const int pCost, const int wCost, const int mCost, const int fCost);

	/*
	void changePopulationCap(const bool increase);
	void changeFoodsCap(const bool increase);
	void changeWoodsCap(const bool increase);
	void changeMetalsCap(const bool )
	*/

	// print resources
	void print();

	void debugSetAllRsToZero();
};

#endif