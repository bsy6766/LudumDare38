#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "cocos2d.h"
#include "Tile.h"
#include "Resource.h"
#include "MouseCursor.h"
#include "GameSpeed.h"

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
		PARTICLES,
		UI
	};

	enum class UI_Z_ORDER
	{
		BG = 0,
		TILE_HOVER,
		TILE_SELECT,
		PANEL,
		INFO,
		ICONS,
		PAUSE,
		MOUSE_CURSOR
	};

	enum RIGHT_PANEL_TAG
	{
		TILE_TITLE,
		TITLE_DIVIDER,
		NATURAL_RESOURCES_TITLE,
		NATURAL_RESOURCES_NUMBERS,
		NATURAL_RESOURCES_ICONS,
		ENRICH_ICON,
		GENERATES_LABEL,
		USES_LABEL,
		VERTICAL_ICON_PACK,
		CASTLE_HP_LABEL,
		SECOND_DIVIDER,
		BUILD_ICON,
		CLEAR_ICON,
		DESTROY_ICON,
		THIRD_DIVIDER,
		FARM_ICON,
		VILLAGE_ICON,
		MINE_ICON,
		LUMBERMILL_ICON
	};

	enum class RIGHT_PANEL_STATE
	{
		NONE = 0,
		IDLE,
		EMPTY_TILE,
		RS_GEN_TILE,
		CASTLE,
		BUILD_MODE,
		CLEAN_MODE,
		DESTROY_MODE,
		BUILD_FARM,
		BUILD_MINE,
		BUILD_LM,
		BUILD_VILLAGE,
	};

	RIGHT_PANEL_STATE rpState;

	// instances
	MouseCursor* cursor;
	GameSpeed* gs;

	// flag
	bool pause;

	// debug
	float deltaModifier;

	// cocos2d
	cocos2d::Node* uiNode;

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

	//Right panel
	cocos2d::Node* tileInfoPanelNode;

	// dividers
	cocos2d::Sprite* titleDivider;
	cocos2d::Sprite* secondDivider;
	cocos2d::Sprite* thirdDivider;
	cocos2d::Sprite* lastDivider;

	// Labels
	cocos2d::Label* tileTitleLabel;
	cocos2d::Label* genLabel;
	cocos2d::Label* usesLabel;
	cocos2d::Sprite* genUsesLabel;
	cocos2d::Label* castleHpLabel;

	cocos2d::Sprite* nrTitle;
	cocos2d::Label* nrLabel;

	cocos2d::Sprite* costLabel;
	cocos2d::Label* costNumberLabel;

	cocos2d::Sprite* rsLeftLabel;
	cocos2d::Label* rsLeftNumber;

	// icons
	cocos2d::Sprite* buildIcon;
	cocos2d::Sprite* cleanIcon;
	cocos2d::Sprite* destroyIcon;
	
	cocos2d::Sprite* farmIcon;
	cocos2d::Sprite* villageIcon;
	cocos2d::Sprite* lumbermillIcon;
	cocos2d::Sprite* mineIcon;

	cocos2d::Sprite* nrIcon;
	cocos2d::Sprite* enrichIcon;
	cocos2d::Sprite* verticalIcon;
	cocos2d::Sprite* costIcon;

	cocos2d::Sprite* buildConfirmIcon;

	cocos2d::Sprite* tileHoverSprite;
	cocos2d::Sprite* tileSelectSprite;

	cocos2d::Sprite* buildPreview;

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
	void initData();
	void initInstances();
	void initAudio();

	void updateResourceUI(ResourceManager* rm);
	void updateResourceLabel(cocos2d::Label* label, const int amount, const int cap);
	void clearRightPanel();
	void updateRightPanel(LD38::Tile* tile);
	void toggleBuildingIcons(const bool visibility);
	bool checkModeIconMouseHover(const cocos2d::Vec2& point);
	bool checkModeIconMouseClick(const cocos2d::Vec2& point);
	bool checkBuildingIconMouseClick(const cocos2d::Vec2& point);
	bool checkBuildingIconMouseHover(const cocos2d::Vec2& point);
	bool checkRightPanelMouse(const cocos2d::Vec2& point);
	void toggleBuildingPreview(LD38::Tile* tile);
	void playSelectSound(LD38::Tile* tile);
	void playButtonClickSFX();

	LD38::Tile* getTileById(const int tileId);

public:
	//simple creator func
	static GameScene* createScene();

	//default destructor
	~GameScene() {};

	//Cocos2d Macro
	CREATE_FUNC(GameScene);

	void requestRightPanelUpdate(const int tileId);
};

#endif