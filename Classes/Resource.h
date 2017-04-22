#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "cocos2d.h"

struct NaturalResource
{
private:
	bool enriched;
	int totalAmount;
	int currentAmount;
public:
	void init();
	void init(NaturalResource& nr);
	bool isEnriched();
	void makeEnriched();
	bool isDepleted();
	void update(int& point);
	int getRemaining();
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

	// add food
	void addFoods(const int foods);
	void addWoods(const int woods);
	void addMetals(const int metals);
	void addPopulation(const int population);

	// update f;ag
	bool doesNeedToUpdateUI();
	void markAsUIUpdated();

	// print resources
	void print();
};

#endif