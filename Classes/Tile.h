#ifndef TILE_H
#define TILE_H

#include "cocos2d.h"
#include "Building.h"
#include "Resource.h"

namespace LD38
{
	/**
	*	@class Tile
	*	@brief Tile data with sprite
	*
	*	All tile has empty tile sprite as default.
	*	Can only hold 1 builidng at a time
	*/
	class Tile
	{
	private:
		// Nullptr if there is no building
		Building* building;

		// natural resources that this tile can potentially produce
		std::vector<NaturalResource> potentialResources;
		NaturalResource* selectedNaturalResource;
		const unsigned int METAL_INDEX = 0;
		const unsigned int WOOD_INDEX = 1;
		const unsigned int FOOD_INDEX = 2;

		// resource type
		ResourceManager::ResourceType resourceType;

		// cocos2d
		cocos2d::Node* tileNode;
		cocos2d::Sprite* emptyTileSprite;
		cocos2d::ParticleSystemQuad* enrichedParticle;

		// raw data
		int row;
		int col;
		int id;
		
		// true if tile is destroyed and need to be restored
		bool destroyed;

		bool updateResource(int& point, int& resource);
	public:
		Tile();
		~Tile();

		void initTile(const int id, const int row, const int col, const cocos2d::Vec2& pos, cocos2d::Node* parent);
		// Init buildings
		void initCaslte();
		void initFarm();
		void initLumberMill();
		void initMine();
		void initVillage();

		// update tile
		void update(const float delta);

		// True if this tile has building. Else, false
		bool hasBuilding();
		// True if vec2 point is in the tile boundary. Else, false
		bool containsPoint(const cocos2d::Vec2& point);
		// get tile id
		int getId();
		// get tile node position
		cocos2d::Vec2 getTilePosition();
		// show / hide hp bar
		void showHpBar(const bool selected);
		void hideHpBar();
		void showRsBar();
		void hideRsBar();
		// check if tile is empty
		bool isEmptyTile();
		bool isCastle();

		ResourceGenBuilding::BuildingType getBuildingType();

		int getBuildingMaxHp();
		int getBuildingCurHp();

		float getBuildingProductionRate();
		int getBuildingUsageRate(ResourceManager::ResourceType type);
		std::string getNaturalResourcesAsStr();
		void makeNaturalResourceEnriched(const int index);
		int hasEnrichedNR();
	};
}

#endif