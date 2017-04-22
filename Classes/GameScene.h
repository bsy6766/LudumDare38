#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "cocos2d.h"
#include "Tile.h"
#include "Resource.h"

class GameScene : public cocos2d::Scene
{
private:
	//default constructor
	GameScene() {};

	//Input Listeners
	cocos2d::EventListenerMouse* mouseInputListener;
	cocos2d::EventListenerKeyboard* keyInputListener;

	//Mouse events
	void onMouseMove(cocos2d::Event* event);
	void onMouseDown(cocos2d::Event* event);
	void onMouseUp(cocos2d::Event* event);
	void onMouseScroll(cocos2d::Event* event);

	//keyboard events
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	//cocos2d virtuals
	virtual bool init() override;
	virtual void onEnter() override;
	virtual void update(float delta) override;
	virtual void onExit() override;

	void initInputListeners();
	void releaseInputListeners();

	// enums
	enum class Z_ORDER
	{
		BG = 0,
		GRID_BORDER,
		TILES,
		TREES,
		GRADIANTS,
		UI
	};

	enum class UI_Z_ORDER
	{
		BG = 0,
		TILE_HOVER,
		TILE_SELECT,
		PANEL,
		ICONS,
		PAUSE,
		MOUSE_CURSOR
	};

	// flag
	bool pause;

	// debug
	float deltaModifier;

	// cocos2d
	cocos2d::Node* uiNode;
	cocos2d::Sprite* mouseCursor;

	cocos2d::Sprite* populationIcon;
	cocos2d::Label* populationLabel;
	cocos2d::Sprite* woodIcon;
	cocos2d::Label* woodLabel;
	cocos2d::Sprite* metalIcon;
	cocos2d::Label* metalLabel;
	cocos2d::Sprite* foodIcon;
	cocos2d::Label* foodLabel;

	cocos2d::Sprite* treesLeft;
	cocos2d::Sprite* treesRight;

	cocos2d::Sprite* leftGradiant;
	cocos2d::Sprite* rightGradiant;

	cocos2d::Sprite* leftPanel;
	cocos2d::Sprite* rightPanel;

	cocos2d::Node* pauseNode;

	cocos2d::Sprite* tileHoverSprite;
	cocos2d::Sprite* tileSelectSprite;

	cocos2d::Node* tileNode;
	std::vector<std::vector<LD38::Tile*>> tileGrid;
	const int MAX_ROW_SIZE = 5;
	const int MAX_COL_SIZE = 5;

	cocos2d::Sequence* selectingSeq;

	// Resources
	int population;
	int woods;
	int metals;
	int foods;
	int troops;

	// cursor-related
	int curHoveringTileId;
	int curSelectingTileId;

	void initSpriteSheets();
	void initUI();
	void initTiles();
	void initResources();
	void initSprites();

	void updateResourceUI(ResourceManager* rm);

	LD38::Tile* getTileById(const int tileId);

public:
	//simple creator func
	static GameScene* createScene();

	//default destructor
	~GameScene() {};

	//Cocos2d Macro
	CREATE_FUNC(GameScene);
};

#endif