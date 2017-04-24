#ifndef BUILDING_H
#define BUILDING_H

#include "cocos2d.h"
#include "Resource.h"

class Building
{
public:
	enum class BuildingType : int
	{
		NONE = 0,
		POPULATION,
		FOOD,
		WOOD,
		METAL,
		CASTLE,
		EMPTY,
	};
public:
	virtual ~Building() = default;
protected:
	Building(const int hitPoint, const bool destructable);

	// Castle can't be destructable by player
	bool destructable;
	// All buildings have hit point. Building gets destroyed if hp reaches 0.
	int hitPoint;
	int maxHitPoint;

	// coocs2d
	cocos2d::Sprite* buildingSprite;
	cocos2d::Sprite* hpBarFrame;
	cocos2d::ProgressTimer* hpBar;

	bool alwaysShowHp;

	void initHpBar(cocos2d::Node* parent);

	BuildingType type;
public:
	virtual void update(const float delta) = 0;
	void setHpBarVisibility(const bool visibility);
	void updateHpBar(float percentage);
	void takeDamage(const int damage);
	virtual void setRsBarVisibility(const bool visibility) {};
	void lockHpBarVisibility(const bool lock);
	BuildingType getType();
	int getMaxHp();
	int getCurHp();
	virtual float getProductionRate() { return 0; }
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
	float progressBarTracker;

	// Resources that are used for this building
	int populationUsageRate;
	int foodUsageRate;
	int woodUsageRate;
	int metalUsageRate;

	int populationUsageTracker;
	int foodUsageTracker;
	int woodUsageTracker;
	int metalUsageTracker;

	//cocos2d
	cocos2d::Sprite* productionProgressBg;
	cocos2d::ProgressTimer* productionProgressBar;

	cocos2d::Node* rsGainIcon;
	cocos2d::Label* rsGainLabel;
	cocos2d::Sequence* rsGainAnimSeq;

	cocos2d::Node* rsUseIcon;
	cocos2d::Sequence* rsUseAnimSeq;

	cocos2d::Sprite* rsBarFrame;
	cocos2d::ProgressTimer* rsBar;
private:
	ResourceGenBuilding(const float productionRate, const int hp);

	static ResourceGenBuilding* create(const std::string& key, cocos2d::Node* parent);

	void initProductionProgressTimer(cocos2d::Node* parent);
	void initRsGainUseIcons(cocos2d::Node* parent, const std::string& spriteType);
	void initResourcePogressBar(cocos2d::Node* parent);
	void initRsUseIcons(const std::string& iconName, const int tag);
	void initRsUsageRate(const int pr, const int wr, const int fr, const int mr);
public:
	~ResourceGenBuilding();

	static ResourceGenBuilding* createFarm(const bool boosted, cocos2d::Node* parent);
	static ResourceGenBuilding* createLumberMill(const bool boosted, cocos2d::Node* parent);
	static ResourceGenBuilding* createMine(const bool boosted, cocos2d::Node* parent);
	static ResourceGenBuilding* createVillage(cocos2d::Node* parent);

	void update(const float delta) override;

	int getProducedResources();

	// modifier
	void reduceProductionRateBy(const float rate);

	void playRsGainAnim(const cocos2d::Vec2& startPos, const int point);
	void playRsUseAnim(const cocos2d::Vec2& startPos, const int tag, const int point);
	void clearCurrentProduction();
	virtual void setRsBarVisibility(const bool visibility) override;
	virtual float getProductionRate() override;
	void updateRsBar(float percentage);
	void returnPoint(const int point);

	int getUsage(ResourceManager::ResourceType type);
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