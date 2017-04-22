#ifndef BUILDING_H
#define BUILDING_H

#include "cocos2d.h"

class ResourceManager;

class Building
{
public:
	virtual ~Building() = default;
protected:
	Building(const int hitPoint, const bool destructable);

	// Castle can't be destructable by player
	bool destructable;
	// All buildings have hit point. Building gets destroyed if hp reaches 0.
	int hitPoint;

	// coocs2d
	cocos2d::Sprite* buildingSprite;
	cocos2d::Sprite* hpBarFrame;
	cocos2d::ProgressTimer* hpBar;
public:
	virtual void update(const float delta) = 0;
	void setHpBarVisibility(const bool visibility);
};

/**
*	@class ResourceGenBuilding
*	@brief This class generates resources
*/
class ResourceGenBuilding : public Building
{
protected:
	// Amount of resouce this building 
	float productionRate;
	float producedResources;
private:
	ResourceGenBuilding(const float productionRate, const int hp);

	static ResourceGenBuilding* create(const float productionRate, const int hp, const std::string& spriteName, cocos2d::Node* parent);
public:
	~ResourceGenBuilding() = default;

	static ResourceGenBuilding* createFarm(const bool boosted, cocos2d::Node* parent);
	static ResourceGenBuilding* createLumberMill(const bool boosted, cocos2d::Node* parent);
	static ResourceGenBuilding* createMine(const bool boosted, cocos2d::Node* parent);
	static ResourceGenBuilding* createVillage(cocos2d::Node* parent);

	void update(const float delta) override;

	int getProducedResources();

	// modifier
	void reduceProductionRateBy(const float rate);
};

/**
*	@class Castle
*	@brief Castle is a starter/permanent building that pretty much does nothing and can't be removed by player. It represents the total health of player.
*/
class Castle : public Building
{
public:
	Castle(cocos2d::Node* parent);
	~Castle() = default;
	
	void update(const float delta) override;
};

#endif BUILDING_H