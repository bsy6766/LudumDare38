#include "GameScene.h"
#include "DataManager.h"
#include "Utility.h"
#include "SoundManager.h"
#include <unordered_set>

USING_NS_CC;

GameScene* GameScene::createScene()
{
	GameScene* newGameScene = GameScene::create();
	return newGameScene;
}

void GameScene::requestRightPanelUpdate(const int tileId)
{
	if (this->curSelectingTileId != -1)
	{
		if (this->curSelectingTileId == tileId)
		{
			auto tile = this->getTileById(this->curSelectingTileId);
			this->updateRightPanel(tile);
			tile->showHpBar(true);
		}
		else
		{
			auto tile = this->getTileById(tileId);
			tile->flashHpBar();
		}
	}
}

bool GameScene::init()
{
	if (!cocos2d::Scene::init())
	{
		return false;
	}

	// Uncomment this to activate update(float) function
	this->scheduleUpdate();

	this->curHoveringTileId = -1;
	this->curSelectingTileId = -1;
	this->deltaModifier = 1.0f;
	this->rpState = RIGHT_PANEL_STATE::IDLE;
	this->buildPreview = nullptr;
	this->viewingMenu = false;

	this->initSpriteSheets();
	this->initData();
	this->initSprites();
	this->initUI();
	this->initTiles();
	this->initResources();
	this->initInstances();
	this->initAudio();

	this->clearRightPanel();
	this->updateRightPanel(nullptr);

	return true;
}

void GameScene::initSpriteSheets()
{
	auto cache = cocos2d::SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("spritesheets/UISpriteSheet.plist");
	cache->addSpriteFramesWithFile("spritesheets/TileSpriteSheet.plist");
}

void GameScene::initUI()
{
	this->uiNode = cocos2d::Node::create();
	this->addChild(this->uiNode, static_cast<int>(Z_ORDER::UI));

	this->tileHoverSprite = cocos2d::Sprite::createWithSpriteFrameName("tileHover.png");
	this->tileHoverSprite->setPosition(-200.0f, -200.0f);
	this->tileHoverSprite->setOpacity(180);
	// We don't toggle visibility of hover sprite.  We simply move out of screen.
	this->uiNode->addChild(this->tileHoverSprite, static_cast<int>(UI_Z_ORDER::TILE_HOVER));

	this->tileSelectSprite = cocos2d::Sprite::createWithSpriteFrameName("tileSelect.png");
	this->tileSelectSprite->setPosition(-200.0f, -200.0f);
	this->tileSelectSprite->setOpacity(180);
	this->uiNode->addChild(this->tileSelectSprite, static_cast<int>(UI_Z_ORDER::TILE_HOVER));

	this->selectingSeq = cocos2d::Sequence::create(cocos2d::MoveBy::create(0.05f, cocos2d::Vec2(0, -3.0f)), cocos2d::MoveBy::create(0.05f, cocos2d::Vec2(0, 3.0f)), nullptr);
	this->selectingSeq->retain();

	auto winSize = cocos2d::Director::getInstance()->getVisibleSize();
	float iconX = 20.0f;
	float labelX = 60.0f;

	int z = static_cast<int>(UI_Z_ORDER::ICONS);

	{
		//population
		float populationY = winSize.height - 64.0f;

		//this->populationIcon = cocos2d::Sprite::createWithSpriteFrameName("populationIcon.png");
		//this->populationIcon->setPosition(cocos2d::Vec2(iconX, populationY));
		//this->uiNode->addChild(this->populationIcon, z);

		this->populationLabel = cocos2d::Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50);
		this->populationLabel->setScale(0.5f);
		this->populationLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
		this->populationLabel->setPosition(cocos2d::Vec2(labelX, populationY));
		this->populationLabel->enableOutline(cocos2d::Color4B::BLACK, 8.0f);
		this->uiNode->addChild(this->populationLabel, z);
	}

	{
		//food
		float foodY = winSize.height - 160.0f;

		//this->foodIcon = cocos2d::Sprite::createWithSpriteFrameName("foodIcon.png");
		//this->foodIcon->setPosition(cocos2d::Vec2(iconX, foodY));
		//this->uiNode->addChild(this->foodIcon, z);

		this->foodLabel = cocos2d::Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50);
		this->foodLabel->setScale(0.5f);
		this->foodLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
		this->foodLabel->setPosition(cocos2d::Vec2(labelX, foodY));
		this->foodLabel->enableOutline(cocos2d::Color4B::BLACK, 8.0f);
		this->uiNode->addChild(this->foodLabel, z);
	}

	{
		//wood
		float woodY = winSize.height - 95.0f;

		//this->woodIcon = cocos2d::Sprite::createWithSpriteFrameName("woodIcon.png");
		//this->woodIcon->setPosition(cocos2d::Vec2(iconX, woodY));
		//this->uiNode->addChild(this->woodIcon, z);

		this->woodLabel = cocos2d::Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50);
		this->woodLabel->setScale(0.5f);
		this->woodLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
		this->woodLabel->setPosition(cocos2d::Vec2(labelX, woodY));
		this->woodLabel->enableOutline(cocos2d::Color4B::BLACK, 8.0f);
		this->uiNode->addChild(this->woodLabel, z);
	}

	{
		//metal
		float metalY = winSize.height - 128.0f;

		//this->metalIcon = cocos2d::Sprite::createWithSpriteFrameName("metalIcon.png");
		//this->metalIcon->setPosition(cocos2d::Vec2(iconX, metalY));
		//this->uiNode->addChild(this->metalIcon, z);

		this->metalLabel = cocos2d::Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50);
		this->metalLabel->setScale(0.5f);
		this->metalLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
		this->metalLabel->setPosition(cocos2d::Vec2(labelX, metalY));
		this->metalLabel->enableOutline(cocos2d::Color4B::BLACK, 8.0f);
		this->uiNode->addChild(this->metalLabel, z);
	}

	// panel
	this->leftPanel = cocos2d::Sprite::createWithSpriteFrameName("leftPanel1.png");
	this->leftPanel->setPosition(cocos2d::Vec2(108.5f, winSize.height * 0.5f));
	this->uiNode->addChild(this->leftPanel, static_cast<int>(UI_Z_ORDER::PANEL));

	// add fixed divider
	//auto resourceDivider = cocos2d::Sprite::createWithSpriteFrameName("panelDividerFlipped.png");
	//resourceDivider->setPosition(cocos2d::Vec2(winSize.width - 108.0f, winSize.height - 174.0f));
	//this->leftPanel->addChild(resourceDivider);

	this->rightPanel = cocos2d::Sprite::createWithSpriteFrameName("rightPanel.png");
	this->rightPanel->setPosition(cocos2d::Vec2(winSize.width - 108.5f, winSize.height * 0.5f));
	this->uiNode->addChild(this->rightPanel, static_cast<int>(UI_Z_ORDER::PANEL));

	// init empty tile info panel
	this->tileInfoPanelNode = cocos2d::Node::create();
	//this->tileInfoPanelNode->setVisible(false);
	this->uiNode->addChild(this->tileInfoPanelNode, static_cast<int>(UI_Z_ORDER::INFO));

	{
		// Dividers
		this->titleDivider = cocos2d::Sprite::createWithSpriteFrameName("panelDivider.png");
		this->titleDivider->setPosition(cocos2d::Vec2(winSize.width - 108.0f, winSize.height - 56.0f));
		//this->titleDivider->setTag(RIGHT_PANEL_TAG::TITLE_DIVIDER);
		this->titleDivider->setVisible(false);
		this->tileInfoPanelNode->addChild(this->titleDivider);

		this->secondDivider = cocos2d::Sprite::createWithSpriteFrameName("panelDivider.png");
		//this->secondDivider->setTag(RIGHT_PANEL_TAG::SECOND_DIVIDER);
		this->secondDivider->setVisible(false);
		this->tileInfoPanelNode->addChild(this->secondDivider);

		this->thirdDivider = cocos2d::Sprite::createWithSpriteFrameName("panelDivider.png");
		//this->thirdDivider->setTag(RIGHT_PANEL_TAG::THIRD_DIVIDER);
		this->thirdDivider->setVisible(false);
		this->tileInfoPanelNode->addChild(this->thirdDivider);

		this->lastDivider = cocos2d::Sprite::createWithSpriteFrameName("panelDivider.png");
		//this->thirdDivider->setTag(RIGHT_PANEL_TAG::THIRD_DIVIDER);
		this->lastDivider->setVisible(false);
		this->tileInfoPanelNode->addChild(this->lastDivider);
	}

	{
		// Labels
		this->tileTitleLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 60.0f);
		this->tileTitleLabel->setScale(0.5f);
		this->tileTitleLabel->setPosition(winSize.width - 100.0f, winSize.height - 35.0f);
		this->tileTitleLabel->setTag(RIGHT_PANEL_TAG::TILE_TITLE);
		this->tileTitleLabel->setVisible(false);
		this->tileTitleLabel->enableOutline(cocos2d::Color4B::BLACK, 10.0f);
		this->tileInfoPanelNode->addChild(this->tileTitleLabel);

		this->genLabel = cocos2d::Label::createWithTTF("Generates", "fonts/Marker Felt.ttf", 40.0f);
		this->genLabel->setScale(0.5f);
		this->genLabel->setPosition(cocos2d::Vec2(winSize.width - 195.0f, winSize.height - 160.0f));
		this->genLabel->setAnchorPoint(cocos2d::Vec2(0.0f, 0.5f));
		this->genLabel->setTag(RIGHT_PANEL_TAG::GENERATES_LABEL);
		this->genLabel->setLineSpacing(10.0f);
		this->genLabel->enableOutline(cocos2d::Color4B::BLACK, 6.0f);
		this->genLabel->setVisible(false);
		this->tileInfoPanelNode->addChild(this->genLabel);

		this->usesLabel = cocos2d::Label::createWithTTF("Uses", "fonts/Marker Felt.ttf", 40.0f, cocos2d::Size::ZERO, cocos2d::TextHAlignment::RIGHT);
		this->usesLabel->setScale(0.5f);
		this->usesLabel->setPosition(cocos2d::Vec2(winSize.width - 7.0f, winSize.height - 160.0f));
		this->usesLabel->setTag(RIGHT_PANEL_TAG::USES_LABEL);
		this->usesLabel->setAnchorPoint(cocos2d::Vec2(1.0f, 0.5f));
		this->usesLabel->enableOutline(cocos2d::Color4B::BLACK, 6.0f);
		this->usesLabel->setVisible(false);
		this->usesLabel->setLineSpacing(10.0f);
		this->tileInfoPanelNode->addChild(this->usesLabel);

		this->genUsesLabel = cocos2d::Sprite::createWithSpriteFrameName("genUsesLabel.png");
		this->genUsesLabel->setPosition(cocos2d::Vec2(winSize.width - 100.0f, winSize.height - 77.0f));
		this->genUsesLabel->setVisible(false);
		this->tileInfoPanelNode->addChild(this->genUsesLabel);

		this->nrTitle = cocos2d::Sprite::createWithSpriteFrameName("naturalResourcesLabel.png");
		this->nrTitle->setPosition(cocos2d::Vec2(winSize.width - 100.0f, winSize.height - 78.0f));
		this->nrTitle->setVisible(false);
		this->tileInfoPanelNode->addChild(this->nrTitle);

		this->nrLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 40.0f);
		this->nrLabel->setScale(0.5f);
		this->nrLabel->setVisible(false);
		this->nrLabel->setPosition(cocos2d::Vec2(winSize.width - 70.0f, winSize.height - 142.0f));
		this->nrLabel->setTag(RIGHT_PANEL_TAG::NATURAL_RESOURCES_NUMBERS);
		this->nrLabel->enableOutline(cocos2d::Color4B::BLACK, 6.0f);
		this->nrLabel->setLineSpacing(10.0f);
		this->tileInfoPanelNode->addChild(this->nrLabel);

		this->castleHpLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 40.0f);
		this->castleHpLabel->setScale(0.5f);
		this->castleHpLabel->setVisible(false); 
		this->castleHpLabel->setPosition(cocos2d::Vec2(winSize.width - 100.0f, winSize.height - 82.0f));
		this->castleHpLabel->setTag(RIGHT_PANEL_TAG::CASTLE_HP_LABEL);
		this->castleHpLabel->enableOutline(cocos2d::Color4B::BLACK, 6.0f);
		this->tileInfoPanelNode->addChild(this->castleHpLabel);

		this->costLabel = cocos2d::Sprite::createWithSpriteFrameName("costsLabel.png");
		this->costLabel->setPosition(cocos2d::Vec2(winSize.width - 100.0f, 125.0f));
		this->costLabel->setVisible(false);
		this->tileInfoPanelNode->addChild(this->costLabel);

		this->costNumberLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 40.0f);
		this->costNumberLabel->setScale(0.5f);
		this->costNumberLabel->setVisible(false);
		this->costNumberLabel->setPosition(cocos2d::Vec2(winSize.width - 150.0f, 75.0f));
		this->costNumberLabel->enableOutline(cocos2d::Color4B::BLACK, 6.0f);
		this->costNumberLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
		this->tileInfoPanelNode->addChild(this->costNumberLabel);

		this->rsLeftLabel = cocos2d::Sprite::createWithSpriteFrameName("resourceLabel.png");
		this->rsLeftLabel->setPosition(cocos2d::Vec2(winSize.width - 100.0f, winSize.height - 250.0f));
		this->rsLeftLabel->setVisible(false);
		this->tileInfoPanelNode->addChild(this->rsLeftLabel);

		this->rsLeftNumber = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 40.0f);
		this->rsLeftNumber->setScale(0.5f);
		this->rsLeftNumber->setVisible(false);
		this->rsLeftNumber->setPosition(cocos2d::Vec2(winSize.width - 100.0f, winSize.height - 280.0f));
		this->rsLeftNumber->enableOutline(cocos2d::Color4B::BLACK, 6.0f);
		this->tileInfoPanelNode->addChild(this->rsLeftNumber);
	}

	{
		// icons
		this->buildIcon = cocos2d::Sprite::createWithSpriteFrameName("buildIcon.png");
		this->buildIcon->setTag(RIGHT_PANEL_TAG::BUILD_ICON);
		this->buildIcon->setPosition(cocos2d::Vec2(winSize.width - 150.0f, winSize.height - 270.0f));
		this->buildIcon->setVisible(false);
		this->tileInfoPanelNode->addChild(this->buildIcon);

		this->cleanIcon = cocos2d::Sprite::createWithSpriteFrameName("cleanIcon.png");
		this->cleanIcon->setTag(RIGHT_PANEL_TAG::CLEAR_ICON);
		this->cleanIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, winSize.height - 270.0f));
		this->cleanIcon->setVisible(false);
		this->tileInfoPanelNode->addChild(this->cleanIcon);

		this->destroyIcon = cocos2d::Sprite::createWithSpriteFrameName("destroyIcon.png");
		this->destroyIcon->setTag(RIGHT_PANEL_TAG::DESTROY_ICON);
		this->destroyIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, winSize.height - 350.0f));
		this->destroyIcon->setVisible(false);
		this->tileInfoPanelNode->addChild(this->destroyIcon);

		this->nrIcon = cocos2d::Sprite::createWithSpriteFrameName("resourceIconVerticalPackWOPopulation.png");
		this->nrIcon->setPosition(cocos2d::Vec2(winSize.width - 129.0f, winSize.height - 145.0f));
		this->nrIcon->setTag(RIGHT_PANEL_TAG::NATURAL_RESOURCES_ICONS);
		this->nrIcon->setVisible(false);
		this->tileInfoPanelNode->addChild(this->nrIcon);

		this->enrichIcon = cocos2d::Sprite::createWithSpriteFrameName("enrichIcon.png");
		this->enrichIcon->setTag(RIGHT_PANEL_TAG::GENERATES_LABEL);
		this->enrichIcon->setVisible(false);
		this->tileInfoPanelNode->addChild(this->enrichIcon);

		this->verticalIcon = cocos2d::Sprite::createWithSpriteFrameName("resourceIconVerticalPack.png");
		this->verticalIcon->setPosition(cocos2d::Vec2(winSize.width - 100.0f, winSize.height - 160.0f));
		this->verticalIcon->setTag(RIGHT_PANEL_TAG::VERTICAL_ICON_PACK);
		this->verticalIcon->setVisible(false);
		this->tileInfoPanelNode->addChild(this->verticalIcon);

		this->costIcon = cocos2d::Sprite::createWithSpriteFrameName("costIcon.png");
		this->costIcon->setPosition(cocos2d::Vec2(winSize.width - 145.0f, 75.0f));
		this->costIcon->setVisible(false);
		this->tileInfoPanelNode->addChild(this->costIcon);

		this->buildConfirmIcon = cocos2d::Sprite::createWithSpriteFrameName("buildConfirmIcon.png");
		this->buildConfirmIcon->setPosition(cocos2d::Vec2(winSize.width - 45.0f, 80.0f));
		this->buildConfirmIcon->setVisible(false);
		this->tileInfoPanelNode->addChild(this->buildConfirmIcon);
	}

	{
		// building icons
		this->farmIcon = cocos2d::Sprite::createWithSpriteFrameName("farmBuildingIcon.png");
		this->farmIcon->setTag(RIGHT_PANEL_TAG::FARM_ICON);
		this->farmIcon->setVisible(false);
		this->farmIcon->setPosition(cocos2d::Vec2(winSize.width - 150.0f, 245.0f));
		this->tileInfoPanelNode->addChild(this->farmIcon);

		this->villageIcon = cocos2d::Sprite::createWithSpriteFrameName("villageBuildingIcon.png");
		this->villageIcon->setTag(RIGHT_PANEL_TAG::VILLAGE_ICON);
		this->villageIcon->setVisible(false);
		this->villageIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, 243.0f));
		this->tileInfoPanelNode->addChild(this->villageIcon);

		this->lumbermillIcon = cocos2d::Sprite::createWithSpriteFrameName("lumbermillBuildingIcon.png");
		this->lumbermillIcon->setTag(RIGHT_PANEL_TAG::LUMBERMILL_ICON);
		this->lumbermillIcon->setVisible(false);
		this->lumbermillIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, 147.0f));
		this->tileInfoPanelNode->addChild(this->lumbermillIcon);

		this->mineIcon = cocos2d::Sprite::createWithSpriteFrameName("mineBuildingIcon.png");
		this->mineIcon->setTag(RIGHT_PANEL_TAG::MINE_ICON);
		this->mineIcon->setVisible(false);
		this->mineIcon->setPosition(cocos2d::Vec2(winSize.width - 150.0f, 150.0f));
		this->tileInfoPanelNode->addChild(this->mineIcon);
	}

	this->menuPanel = cocos2d::Sprite::createWithSpriteFrameName("menuPanel.png");
	this->menuPanel->setPosition(cocos2d::Vec2(winSize * 0.5f));
	this->menuPanel->setVisible(false);
	this->uiNode->addChild(this->menuPanel, static_cast<int>(UI_Z_ORDER::MENU));

	this->resumeLabel = cocos2d::Sprite::createWithSpriteFrameName("resumeLabel.png");
	this->resumeLabel->setPosition(cocos2d::Vec2(109.0f, 150.0f));
	this->menuPanel->addChild(this->resumeLabel);

	this->exitLabel = cocos2d::Sprite::createWithSpriteFrameName("exitLabel.png");
	this->exitLabel->setPosition(cocos2d::Vec2(109.0f, 100.0f));
	this->menuPanel->addChild(this->exitLabel);

	// pause Node
	/*
	this->pauseNode = cocos2d::Node::create();
	this->uiNode->addChild(this->pauseNode, static_cast<int>(UI_Z_ORDER::PAUSE));

	auto pauseBg = cocos2d::Sprite::createWithSpriteFrameName("dot.png");
	pauseBg->setColor(cocos2d::Color3B::BLACK);
	pauseBg->setOpacity(120);
	pauseBg->setScaleY(250.0f);
	pauseBg->setScaleX(winSize.width);
	pauseBg->setPosition(winSize * 0.5f);
	this->pauseNode->addChild(pauseBg, 0);

	auto pauseLabelSprite = cocos2d::Sprite::createWithSpriteFrameName("pauseLabel.png");
	pauseLabelSprite->setPosition(winSize * 0.5f);
	this->pauseNode->addChild(pauseLabelSprite, 1);

	this->pauseNode->setVisible(false);
	*/
}

void GameScene::initTiles()
{
	this->tileNode = cocos2d::Node::create();
	this->addChild(this->tileNode, static_cast<int>(Z_ORDER::TILES));

	auto winSize = cocos2d::Director::getInstance()->getVisibleSize();
	auto centerPos = cocos2d::Vec2(winSize * 0.5f);

	auto tileLength = 120.0f;

	float startX = centerPos.x - (static_cast<float>(MAX_COL_SIZE / 2) * tileLength);
	float startY = centerPos.y - (static_cast<float>(MAX_ROW_SIZE / 2) * tileLength);

	auto startPos = cocos2d::Vec2(startX, startY);

	for (int row = 0; row < MAX_ROW_SIZE; row++)
	{
		tileGrid.push_back(std::vector<LD38::Tile*>());

		for (int col = 0; col < MAX_COL_SIZE; col++)
		{
			auto newTile = new LD38::Tile();
			int tileId = row * MAX_ROW_SIZE + col;
			newTile->initTile(tileId, row, col, startPos, this->tileNode);
			tileGrid.back().push_back(newTile);
			startPos.x += tileLength;
		}

		startPos.x = startX;
		startPos.y += tileLength;
	}

	this->tileGrid.at(2).at(2)->initCaslte();

	std::vector<int> shuffledIndices;

	for (int i = 0; i < 25; i++)
	{
		if (i == 12) continue;

		shuffledIndices.push_back(i);
	}

	std::random_shuffle(shuffledIndices.begin(), shuffledIndices.end());
	std::random_shuffle(shuffledIndices.begin(), shuffledIndices.end());

	int e1 = shuffledIndices.at(0);
	int e2 = shuffledIndices.at(1);

	this->tileGrid.at(e1 / 5).at(e1 % 5)->makeNaturalResourceEnriched(Utility::Random::randomInt(0, 2));
	this->tileGrid.at(e2 / 5).at(e2 % 5)->makeNaturalResourceEnriched(Utility::Random::randomInt(0, 2));

	int d1 = shuffledIndices.at(2);
	int d2 = shuffledIndices.at(3);
	int d3 = shuffledIndices.at(4);
	int d4 = shuffledIndices.at(5);
	int d5 = shuffledIndices.at(6);
	int d6 = shuffledIndices.at(7);

	this->tileGrid.at(d1 / 5).at(d1 % 5)->destroyBuilding();
	this->tileGrid.at(d2 / 5).at(d2 % 5)->destroyBuilding();
	this->tileGrid.at(d3 / 5).at(d3 % 5)->destroyBuilding();
	this->tileGrid.at(d4 / 5).at(d4 % 5)->destroyBuilding();
	this->tileGrid.at(d5 / 5).at(d5 % 5)->destroyBuilding();
	this->tileGrid.at(d6 / 5).at(d6 % 5)->destroyBuilding();
}

void GameScene::initResources()
{
	auto r = ResourceManager::getInstance();
	r->initResources(20, 100, 100, 100, 0);
	r->initResourcesCap(200, 500, 500, 500, 500);
}

void GameScene::initSprites()
{
	auto winSize = cocos2d::Director::getInstance()->getVisibleSize();

	float y = winSize.height * 0.5f;

	/*
	auto bgColor = cocos2d::Sprite::createWithSpriteFrameName("dot.png");
	bgColor->setScaleX(winSize.width);
	bgColor->setScaleY(winSize.height);
	bgColor->setColor(cocos2d::Color3B(81, 103, 31));
	bgColor->setPosition(cocos2d::Vec2(winSize * 0.5f));
	this->addChild(bgColor, static_cast<int>(Z_ORDER::BG));
	*/

	auto gridBorder = cocos2d::Sprite::createWithSpriteFrameName("gridBorder.png");
	gridBorder->setPosition(cocos2d::Vec2(winSize * 0.5f));
	this->addChild(gridBorder, static_cast<int>(Z_ORDER::GRID_BORDER));

	//trees
	float treeDist = 246.0f;

	this->treesLeft = cocos2d::Sprite::createWithSpriteFrameName("treeLeft.png");
	this->treesLeft->setPosition(cocos2d::Vec2(treeDist, y));
	this->addChild(this->treesLeft, static_cast<int>(Z_ORDER::TREES));

	this->treesRight = cocos2d::Sprite::createWithSpriteFrameName("treeRight.png");
	this->treesRight->setPosition(cocos2d::Vec2(winSize.width - treeDist, y));
	this->addChild(this->treesRight, static_cast<int>(Z_ORDER::TREES));

	//gradiant
	float gradiantDist = 230.0f;

	this->leftGradiant = cocos2d::Sprite::createWithSpriteFrameName("fadeGradiant.png");
	this->leftGradiant->setPosition(cocos2d::Vec2(gradiantDist, y));
	this->leftGradiant->setScaleY(8);
	this->addChild(this->leftGradiant, static_cast<int>(Z_ORDER::GRADIANTS));

	this->rightGradiant = cocos2d::Sprite::createWithSpriteFrameName("fadeGradiant.png");
	this->rightGradiant->setPosition(cocos2d::Vec2(winSize.width - gradiantDist, y));
	this->rightGradiant->setScaleX(-1.0f);
	this->rightGradiant->setScaleY(8);
	this->addChild(this->rightGradiant, static_cast<int>(Z_ORDER::GRADIANTS));
}

void GameScene::initData()
{
	auto dm = DataManager::getInstance();
	dm->loadDataTree("data/buildingData", "BUILDING_DATA");
}

void GameScene::initInstances()
{
	this->cursor = new MouseCursor();
	this->cursor->initCursors(this->uiNode, static_cast<int>(UI_Z_ORDER::MOUSE_CURSOR));

	this->gs = new GameSpeed();
	this->gs->init(this->uiNode, static_cast<int>(UI_Z_ORDER::ICONS));
}

void GameScene::initAudio()
{
	SoundManager::getInstance()->init();
	SoundManager::getInstance()->initIcons(this->uiNode, static_cast<int>(UI_Z_ORDER::ICONS));
}

void GameScene::onEnter()
{
	cocos2d::Scene::onEnter();

	initInputListeners();

	//SoundManager::getInstance()->playMusic();
}

void GameScene::update(float delta)
{
	if (!this->viewingMenu)
	{
		float speed = this->gs->getSpeed();
		if (speed == 0.0f)
		{
			return;
		}

		delta *= speed;

		for (auto row : this->tileGrid)
		{
			for (auto tile : row)
			{
				tile->update(delta);
			}
		}

		auto r = ResourceManager::getInstance();
		if (r->doesNeedToUpdateUI())
		{
			this->updateResourceUI(r);
			r->markAsUIUpdated();

			if (this->rsLeftNumber->isVisible() && this->curSelectingTileId != -1)
			{
				// update number
				auto tile = this->getTileById(this->curSelectingTileId);
				if (tile->getBuildingType() != Building::BuildingType::POPULATION)
				{
					rsLeftNumber->setString(std::to_string(tile->getRemainigResource()));
				}
			}

			if (this->buildConfirmIcon->isVisible())
			{
				//update button if it has enough resource now
				auto data = DataManager::getInstance()->getData("BUILDING_DATA");

				std::string key;

				switch (this->rpState)
				{
				case RIGHT_PANEL_STATE::BUILD_VILLAGE:
					key = "village";
					break;
				case RIGHT_PANEL_STATE::BUILD_FARM:
					key = "farm";
					break;
				case RIGHT_PANEL_STATE::BUILD_LM:
					key = "lumbermill";
					break;
				case RIGHT_PANEL_STATE::BUILD_MINE:
					key = "mine";
					break;
				default:
					break;
				}

				if (!key.empty())
				{
					bool buildable = r->checkCost(data->getInt(key + ".cost.population"), data->getInt(key + ".cost.wood"), data->getInt(key + ".cost.metal"), data->getInt(key + ".cost.food"));
					if (buildable)
					{
						this->buildConfirmIcon->setOpacity(255);
					}
					else
					{
						this->buildConfirmIcon->setOpacity(100);
					}
				}
			}
		}
	}
}

void GameScene::updateResourceUI(ResourceManager* rm)
{
	this->updateResourceLabel(this->populationLabel, rm->getPopulation(), rm->getPopulationCap());
	this->updateResourceLabel(this->foodLabel, rm->getFoods(), rm->getFoodsCap());
	this->updateResourceLabel(this->woodLabel, rm->getWoods(), rm->getWoodsCap());
	this->updateResourceLabel(this->metalLabel, rm->getMetals(), rm->getMetalsCap());
	//cocos2d::log("Updating resources");
}

void GameScene::updateResourceLabel(cocos2d::Label * label, const int amount, const int cap)
{

	label->setString(std::to_string(amount) + " / " + std::to_string(cap));
	if (amount == cap)
	{
		label->setColor(cocos2d::Color3B::YELLOW);
	}
	else if (amount == 0)
	{
		label->setColor(cocos2d::Color3B::RED);
	}
	else
	{
		label->setColor(cocos2d::Color3B::WHITE);
	}

}

LD38::Tile * GameScene::getTileById(const int tileId)
{
	for (auto row : this->tileGrid)
	{
		for (auto col : row)
		{
			if (col->getId() == tileId)
			{
				return col;
			}
		}
	}

	return nullptr;
}

void GameScene::updateRightPanel(LD38::Tile * tile)
{
	this->clearRightPanel();
	auto winSize = cocos2d::Director::getInstance()->getVisibleSize();

	if (tile == nullptr)
	{
		this->rpState = RIGHT_PANEL_STATE::IDLE;

		/*
		buildIcon->setVisible(true);
		cleanIcon->setVisible(true);
		destroyIcon->setVisible(true);

		this->buildIcon->setPosition(cocos2d::Vec2(winSize.width - 150.0f, winSize.height - 65.0f));
		this->cleanIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, winSize.height - 65.0f));
		this->destroyIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, winSize.height - 155.0f));

		secondDivider->setVisible(true);
		secondDivider->setPosition(winSize.width - 107.0f, winSize.height - 200.0f);
		*/
	}
	else
	{
		auto tileStr = std::string();

		if (tile->isEmptyTile())
		{

			tileStr = "Empty Tile";

			this->nrLabel->setString(tile->getNaturalResourcesAsStr());
			this->nrLabel->setVisible(true);

			int enrichedIndex = tile->hasEnrichedNR();

			if (enrichedIndex != -1)
			{
				float x = winSize.width - 30.0f;
				float y = 0;
				if (enrichedIndex == 0)
				{
					y = winSize.height - 110.0f;
				}
				else if (enrichedIndex == 1)
				{
					y = winSize.height - 140.0f;
				}
				else if (enrichedIndex == 2)
				{
					y = winSize.height - 175.0f;
				}

				if (y != 0.0f)
				{
					this->enrichIcon->setVisible(true);
					this->enrichIcon->setPosition(cocos2d::Vec2(x, y));
				}
			}

			this->nrTitle->setVisible(true);
			this->nrLabel->setVisible(true);
			this->nrIcon->setVisible(true);

			this->secondDivider->setVisible(true);
			this->secondDivider->setPosition(winSize.width - 107.0f, winSize.height - 200.0f);

			this->buildIcon->setVisible(true);
			this->cleanIcon->setVisible(true);
			this->destroyIcon->setVisible(true);

			bool tileDestroyed = tile->isDestroyed();

			this->buildIcon->setPosition(cocos2d::Vec2(winSize.width - 150.0f, winSize.height - 250.0f));
			if (tileDestroyed)
			{
				this->buildIcon->setOpacity(100);
			}

			this->cleanIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, winSize.height - 250.0f));
			if (tileDestroyed)
			{
				this->cleanIcon->setOpacity(255);
			}
			else
			{
				this->cleanIcon->setOpacity(100);
			}

			this->destroyIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, winSize.height - 340.0f));
			this->destroyIcon->setOpacity(100);

			this->thirdDivider->setVisible(true);
			this->thirdDivider->setPosition(winSize.width - 107.0f, 330.0f);

			this->rpState = RIGHT_PANEL_STATE::EMPTY_TILE;
		}
		else
		{
			if (tile->isCastle())
			{
				tileStr = "Castle";

				this->castleHpLabel->setVisible(true);
				this->castleHpLabel->setString("HP: " + std::to_string(tile->getBuildingCurHp()) + " / " + std::to_string(tile->getBuildingMaxHp()));

				this->secondDivider->setVisible(true);
				this->secondDivider->setPosition(winSize.width - 107.0f, winSize.height - 98.0f);

				this->rpState = RIGHT_PANEL_STATE::CASTLE;
			}
			else
			{
				// resource generator
				this->genLabel->setVisible(true);
				this->usesLabel->setVisible(true);

				this->genUsesLabel->setVisible(true);

				std::string genStr = "";
				std::string usesStr = "";

				float popUsage = static_cast<float>(tile->getBuildingUsageRate(ResourceManager::ResourceType::POPULATION));
				float foodUsage = static_cast<float>(tile->getBuildingUsageRate(ResourceManager::ResourceType::FOOD));
				float woodUsage = static_cast<float>(tile->getBuildingUsageRate(ResourceManager::ResourceType::WOOD));
				float metalUsage = static_cast<float>(tile->getBuildingUsageRate(ResourceManager::ResourceType::METAL));

				auto popStr = (popUsage == 0.0f ? "0" : std::to_string(1.0f / popUsage).substr(0, 4)) + "/s\n";
				auto woodMetalStr = (woodUsage == 0.0f ? "0" : std::to_string(1.0f / woodUsage).substr(0, 4)) + "/s\n" +
					(metalUsage == 0.0f ? "0" : std::to_string(1.0f / metalUsage).substr(0, 4)) + "/s\n";
				std::string foodStr = (foodUsage == 0.0f ? "0" : std::to_string(1.0f / foodUsage).substr(0, 4)) + "/s";

				float pr = tile->getBuildingProductionRate();
				std::string prStr;
				if (pr == 1.0f || pr == 2.0f || pr == 3.0f)
				{
					prStr = std::to_string(static_cast<int>(pr));
				}
				else
				{
					prStr = std::to_string(pr).substr(0, 4);
				}

				bool isVillage = false;

				switch (tile->getBuildingType())
				{
				case Building::BuildingType::FOOD:
					tileStr = "Farm";
					genStr += ("0/s\n0/s\n0/s\n" + prStr + "/s");
					break;
				case Building::BuildingType::WOOD:
					tileStr = "Lumber Mill";
					genStr += ("0/s\n" + prStr + "/s\n0/s\n0/s");
					break;
				case Building::BuildingType::METAL:
					tileStr = "Mine";
					genStr += ("0/s\n0/s\n" + prStr + "/s\n0/s");
					break;
				case Building::BuildingType::POPULATION:
					tileStr = "Village";
					genStr += (prStr + "/s\n0/s\n0/s\n0/s");
					foodStr = "3/person";
					isVillage = true;
					break;
				case Building::BuildingType::NONE:
				case Building::BuildingType::EMPTY:
				case Building::BuildingType::CASTLE:
				default:
					this->clearRightPanel();
					return;
					break;
				}

				this->genLabel->setString(genStr);
				this->usesLabel->setString(usesStr + popStr + woodMetalStr + foodStr);

				this->secondDivider->setVisible(true);
				this->secondDivider->setPosition(winSize.width - 107.0f, winSize.height - 231.0f);

				this->rsLeftLabel->setVisible(true);
				this->rsLeftNumber->setVisible(true);

				if (isVillage)
				{
					this->rsLeftNumber->setString("-");
				}
				else
				{
					this->rsLeftNumber->setString(std::to_string(tile->getRemainigResource()));
				}

				this->thirdDivider->setVisible(true);
				this->thirdDivider->setPosition(winSize.width - 107.0f, winSize.height - 300.0f);

				this->verticalIcon->setVisible(true);

				this->buildIcon->setVisible(true);
				this->cleanIcon->setVisible(true);
				this->destroyIcon->setVisible(true);

				this->buildIcon->setPosition(cocos2d::Vec2(winSize.width - 150.0f, winSize.height - 345.0f));
				this->buildIcon->setOpacity(100);
				this->cleanIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, winSize.height - 345.0f));
				this->cleanIcon->setOpacity(100);

				if (tile->isBuilding())
				{
					this->destroyIcon->setOpacity(100);
				}
				this->destroyIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, winSize.height - 435.0f));

				this->lastDivider->setVisible(true);
				this->lastDivider->setPosition(cocos2d::Vec2(winSize.width - 107.0f, winSize.height - 475.0f));

				this->rpState = RIGHT_PANEL_STATE::RS_GEN_TILE;
			}
		}

		this->titleDivider->setVisible(true);

		this->tileTitleLabel->setString(tileStr);
		this->tileTitleLabel->setVisible(true);
	}
}

void GameScene::toggleBuildingIcons(const bool visibility)
{
	auto winSize = cocos2d::Director::getInstance()->getVisibleSize();

	if (this->rpState == RIGHT_PANEL_STATE::IDLE)
	{
		this->farmIcon->setPosition(cocos2d::Vec2(winSize.width - 150.0f, 455.0f));
		this->villageIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, 453.0f));
		this->lumbermillIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, 367.0f));
		this->mineIcon->setPosition(cocos2d::Vec2(winSize.width - 150.0f, 370.0f));
	}
	else if (this->rpState == RIGHT_PANEL_STATE::RS_GEN_TILE)
	{
		
	}
	else
	{
		this->farmIcon->setPosition(cocos2d::Vec2(winSize.width - 150.0f, 275.0f));
		this->villageIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, 273.0f));
		this->lumbermillIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, 187.0f));
		this->mineIcon->setPosition(cocos2d::Vec2(winSize.width - 150.0f, 190.0f));
	}

	this->farmIcon->setVisible(visibility);
	this->mineIcon->setVisible(visibility);
	this->lumbermillIcon->setVisible(visibility);
	this->villageIcon->setVisible(visibility);
}

bool GameScene::checkModeIconMouseHover(const cocos2d::Vec2 & point)
{
	this->buildIcon->setScale(1.0f);
	this->cleanIcon->setScale(1.0f);
	this->destroyIcon->setScale(1.0f);

	if (this->buildIcon->getOpacity() == 255 && this->buildIcon->getBoundingBox().containsPoint(point))
	{
		this->buildIcon->setScale(1.1f);
	}
	else if (this->cleanIcon->getOpacity() == 255 && this->cleanIcon->getBoundingBox().containsPoint(point))
	{
		this->cleanIcon->setScale(1.1f);
	}
	else if (this->destroyIcon->getOpacity() == 255 && this->destroyIcon->getBoundingBox().containsPoint(point))
	{
		this->destroyIcon->setScale(1.1f);
	}
	else
	{
		return false;
	}

	return true;
}

bool GameScene::checkModeIconMouseClick(const cocos2d::Vec2 & point)
{
	if (this->buildIcon->getOpacity() == 255 && this->buildIcon->getBoundingBox().containsPoint(point))
	{
		this->buildIcon->runAction(selectingSeq->clone());
		if (this->curSelectingTileId == -1)
		{
			this->rpState = RIGHT_PANEL_STATE::IDLE;
		}
		this->toggleBuildingIcons(true);
		this->rpState = RIGHT_PANEL_STATE::BUILD_MODE;
		cocos2d::log("Build mode");
		return true;
	}
	else if (this->cleanIcon->getOpacity() == 255 && this->cleanIcon->getBoundingBox().containsPoint(point))
	{
		this->cleanIcon->runAction(selectingSeq->clone());;
		if (this->curSelectingTileId == -1)
		{
			this->rpState = RIGHT_PANEL_STATE::IDLE;
		}
		else
		{
			auto tile = this->getTileById(this->curSelectingTileId);
			tile->cleanUp();
			this->cleanIcon->setOpacity(100);
		}

		this->toggleBuildingIcons(false);
		//this->rpState = RIGHT_PANEL_STATE::CLEAN_MODE;
		cocos2d::log("clean mode");


		return true;
	}
	else if (this->destroyIcon->getOpacity() == 255 && this->destroyIcon->getBoundingBox().containsPoint(point))
	{
		this->destroyIcon->runAction(selectingSeq->clone());;
		if (this->curSelectingTileId == -1)
		{
			this->rpState = RIGHT_PANEL_STATE::IDLE;
		}
		else
		{
			// destroy building
			auto tile = this->getTileById(this->curSelectingTileId);
			tile->destroyBuilding();

			auto buildParticle = cocos2d::ParticleSystemQuad::create("particles/build.plist");
			buildParticle->setAutoRemoveOnFinish(true);
			buildParticle->setPosition(tile->getTilePosition());
			this->addChild(buildParticle, static_cast<int>(Z_ORDER::PARTICLES));

			this->updateRightPanel(tile);
		}

		this->toggleBuildingIcons(false);
		//this->rpState = RIGHT_PANEL_STATE::DESTROY_MODE;
		cocos2d::log("destroy mode");
		return true;
	}
	else
	{
		cocos2d::log("Didn't click any modes");
		return false;
	}
}

bool GameScene::checkBuildingIconMouseHover(const cocos2d::Vec2 & point)
{
	this->farmIcon->setScale(1.0f);
	this->villageIcon->setScale(1.0f);
	this->lumbermillIcon->setScale(1.0f);
	this->mineIcon->setScale(1.0f);

	if (this->farmIcon->getBoundingBox().containsPoint(point))
	{
		this->farmIcon->setScale(1.1f);
	}
	else if (this->villageIcon->getBoundingBox().containsPoint(point))
	{
		this->villageIcon->setScale(1.1f);
	}
	else if (this->lumbermillIcon->getBoundingBox().containsPoint(point))
	{
		this->lumbermillIcon->setScale(1.1f);
	}
	else if (this->mineIcon->getBoundingBox().containsPoint(point))
	{
		this->mineIcon->setScale(1.1f);
	}
	else
	{
		return false;
	}

	return true;
}

bool GameScene::checkBuildingIconMouseClick(const cocos2d::Vec2 & point)
{
	if (this->farmIcon->getBoundingBox().containsPoint(point))
	{
		this->farmIcon->runAction(selectingSeq->clone());
		this->rpState = RIGHT_PANEL_STATE::BUILD_FARM;
		if (this->curSelectingTileId != -1)
		{
			this->toggleBuildingPreview(this->getTileById(this->curSelectingTileId));
		}
		cocos2d::log("Build farm mode");
		return true;
	}
	else if (this->villageIcon->getBoundingBox().containsPoint(point))
	{
		this->villageIcon->runAction(selectingSeq->clone());
		this->rpState = RIGHT_PANEL_STATE::BUILD_VILLAGE;
		if (this->curSelectingTileId != -1)
		{
			this->toggleBuildingPreview(this->getTileById(this->curSelectingTileId));
		}
		cocos2d::log("Build village mode");
		return true;
	}
	else if (this->lumbermillIcon->getBoundingBox().containsPoint(point))
	{
		this->lumbermillIcon->runAction(selectingSeq->clone());
		this->rpState = RIGHT_PANEL_STATE::BUILD_LM;
		if (this->curSelectingTileId != -1)
		{
			this->toggleBuildingPreview(this->getTileById(this->curSelectingTileId));
		}
		cocos2d::log("Build lumber mill mode");
		return true;
	}
	else if (this->mineIcon->getBoundingBox().containsPoint(point))
	{
		this->mineIcon->runAction(selectingSeq->clone());
		this->rpState = RIGHT_PANEL_STATE::BUILD_MINE;
		if (this->curSelectingTileId != -1)
		{
			this->toggleBuildingPreview(this->getTileById(this->curSelectingTileId));
		}
		cocos2d::log("Build mine mode");
		return true;
	}
	else
	{
		return false;
	}
}

bool GameScene::checkRightPanelMouse(const cocos2d::Vec2 & point)
{
	return this->rightPanel->getBoundingBox().containsPoint(point);
}

void GameScene::toggleBuildingPreview(LD38::Tile * tile)
{
	if (tile == nullptr)
	{
		if (this->buildPreview)
		{
			this->buildPreview->removeFromParentAndCleanup(true);
			this->buildPreview = nullptr;
		}
	}
	else
	{
		auto spriteName = "";

		switch (this->rpState)
		{
		case RIGHT_PANEL_STATE::BUILD_FARM:
			spriteName = "farmBuilding.png";
			break;
		case RIGHT_PANEL_STATE::BUILD_VILLAGE:
			spriteName = "villageBuilding.png";
			break;
		case RIGHT_PANEL_STATE::BUILD_MINE:
			spriteName = "mineBuilding.png";
			break;
		case RIGHT_PANEL_STATE::BUILD_LM:
			spriteName = "lumbermillBuilding.png";
			break;
		default:
			return;
			break;
		}

		if (this->buildPreview != nullptr)
		{
			this->buildPreview->removeFromParent();
			this->buildPreview = nullptr;
		}
		this->buildPreview = cocos2d::Sprite::createWithSpriteFrameName(spriteName);
		this->buildPreview->setOpacity(150);
		this->buildPreview->setPosition(tile->getTilePosition());
		this->tileNode->addChild(this->buildPreview, 1000);
	}
}

void GameScene::playSelectSound(LD38::Tile * tile)
{
	if (tile->hasBuilding())
	{
		if (tile->isCastle())
		{
			// castle sound
		}
		else
		{
			switch (tile->getBuildingType())
			{
			case Building::BuildingType::FOOD:
			{
			}
				break;
			case Building::BuildingType::METAL:
			{
			}
			break;
			case Building::BuildingType::WOOD:
			{
				SoundManager::getInstance()->playLumbermillSelect();
			}
			break;
			case Building::BuildingType::POPULATION:
			{
				SoundManager::getInstance()->playVillageSelect();
			}
			break;
			default:
				break;
			}
		}
	}
	else
	{
		// field sound
	}
}

void GameScene::clearRightPanel()
{
	auto children = this->tileInfoPanelNode->getChildren();

	for (auto child : children)
	{
		child->setOpacity(255);
		child->setVisible(false);
	}
}

void GameScene::initInputListeners()
{
	this->mouseInputListener = EventListenerMouse::create();
	this->mouseInputListener->onMouseMove = CC_CALLBACK_1(GameScene::onMouseMove, this);
	this->mouseInputListener->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
	this->mouseInputListener->onMouseUp = CC_CALLBACK_1(GameScene::onMouseUp, this);
	this->mouseInputListener->onMouseScroll = CC_CALLBACK_1(GameScene::onMouseScroll, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(this->mouseInputListener, this);

	this->keyInputListener = EventListenerKeyboard::create();
	this->keyInputListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
	this->keyInputListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(this->keyInputListener, this);
}

void GameScene::onMouseMove(cocos2d::Event* event) 
{
	auto mouseEvent = static_cast<EventMouse*>(event);
	float x = mouseEvent->getCursorX();
	float y = mouseEvent->getCursorY();

	auto point = cocos2d::Vec2(x, y);

	this->cursor->updatePosition(point);

	if (this->viewingMenu) return;

	bool hovering = false;

	if (this->checkRightPanelMouse(point))
	{
		if (this->rpState == RIGHT_PANEL_STATE::EMPTY_TILE ||
			this->rpState == RIGHT_PANEL_STATE::IDLE ||
			this->rpState == RIGHT_PANEL_STATE::RS_GEN_TILE ||
			this->rpState == RIGHT_PANEL_STATE::BUILD_MODE ||
			this->rpState == RIGHT_PANEL_STATE::CLEAN_MODE ||
			this->rpState == RIGHT_PANEL_STATE::DESTROY_MODE ||
			this->rpState == RIGHT_PANEL_STATE::BUILD_FARM ||
			this->rpState == RIGHT_PANEL_STATE::BUILD_MINE ||
			this->rpState == RIGHT_PANEL_STATE::BUILD_LM ||
			this->rpState == RIGHT_PANEL_STATE::BUILD_VILLAGE)
		{
			// Check mouse collision for icons
			bool hovered = this->checkModeIconMouseHover(point);
			if (hovered)
			{
				return;
			}
			else
			{
				// didn't hover mode icons
				hovered = this->checkBuildingIconMouseHover(point);
				if (hovered) return;
			}
		}

		// Didnt' hovered any. check confirm button
		if (this->buildConfirmIcon->getOpacity() == 255 && this->buildConfirmIcon->getBoundingBox().containsPoint(point))
		{
			this->buildConfirmIcon->setScale(1.1f);
		}
		else
		{
			this->buildConfirmIcon->setScale(1.0f);
		}

		return;
	}
	else if (this->gs->mouseHover(point))
	{
		return;
	}
	else if (SoundManager::getInstance()->mouseHover(point))
	{
		return;
	}

	for (auto row : this->tileGrid)
	{
		for (auto col : row)
		{
			if (col->containsPoint(point))
			{
				hovering = true;

				bool tileChanged = false;

				int hoveringTileId = col->getId();

				if (hoveringTileId == curSelectingTileId)
				{
					// hovering selected tile. 
					this->tileHoverSprite->setPosition(cocos2d::Vec2(-200, -200));
					if (this->curHoveringTileId != hoveringTileId && this->curHoveringTileId != -1)
					{
						auto tile = this->getTileById(this->curHoveringTileId);
						tile->hideHpBar();
						tile->hideRsBar();
					}
					this->curHoveringTileId = hoveringTileId;
					return;
				}

				if (this->curHoveringTileId == -1)
				{
					// Newly hovering id
					//cocos2d::log("Currently hovering tile id = %d", this->curHoveringTileId);
					//cocos2d::log("Newly hovering tile id = %d", hoveringTileId);

					this->curHoveringTileId = hoveringTileId;
					tileChanged = true;
				}
				else if (this->curHoveringTileId != hoveringTileId)
				{
					//cocos2d::log("Currently hovering tile id = %d", this->curHoveringTileId);
					//cocos2d::log("Newly hovering tile id = %d", hoveringTileId);

					if (this->curSelectingTileId != this->curHoveringTileId)
					{
						auto tile = this->getTileById(this->curHoveringTileId);
						tile->hideHpBar();
						tile->hideRsBar();
					}
					this->curHoveringTileId = hoveringTileId;
					tileChanged = true;
				}

				if (tileChanged)
				{
					this->tileHoverSprite->setPosition(col->getTilePosition());
					auto tile = this->getTileById(this->curHoveringTileId);
					tile->showHpBar(false);
					tile->showRsBar();

					/*
					if (this->rpState == RIGHT_PANEL_STATE::BUILD_FARM ||
						this->rpState == RIGHT_PANEL_STATE::BUILD_MINE ||
						this->rpState == RIGHT_PANEL_STATE::BUILD_LM ||
						this->rpState == RIGHT_PANEL_STATE::BUILD_VILLAGE)
					{
						if (tile->isCastle())
						{
							this->cursor->setColor(cocos2d::Color3B::RED);
						}
						else if (tile->hasBuilding())
						{
							this->cursor->setColor(cocos2d::Color3B::YELLOW);
						}
						else
						{
							this->cursor->setColor(cocos2d::Color3B::GREEN);
						}
					}
					*/
				}

				return;
			}
		}
	}

	if (!hovering)
	{
		// Not hovering any tile
		if (this->curHoveringTileId != -1)
		{
			auto tile = this->getTileById(this->curHoveringTileId);
			tile->hideHpBar();
			tile->hideRsBar();
		}
		this->curHoveringTileId = -1;
		this->tileHoverSprite->setPosition(cocos2d::Vec2(-200, -200));
	}
}

void GameScene::onMouseDown(cocos2d::Event* event) 
{
	auto mouseEvent = static_cast<EventMouse*>(event);
	float x = mouseEvent->getCursorX();
	float y = mouseEvent->getCursorY();
	
	auto point = cocos2d::Vec2(x, y);

	if (this->viewingMenu)
	{
		auto shift = this->menuPanel->getPosition() - (this->menuPanel->getContentSize() * 0.5f);

		auto resumeBB = this->resumeLabel->getBoundingBox();
		resumeBB.origin += shift;

		auto exitBB = this->exitLabel->getBoundingBox();
		exitBB.origin += shift;

		if (resumeBB.containsPoint(point))
		{
			this->viewingMenu = false;
			this->menuPanel->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0, 1.0), cocos2d::ScaleTo::create(0.1f, 0.0f), cocos2d::Hide::create(), nullptr));
			return;
		}

		if (exitBB.containsPoint(point))
		{
			cocos2d::Director::getInstance()->end();
		}

		return;
	}

	if (this->checkRightPanelMouse(point))
	{
		if (this->rpState == RIGHT_PANEL_STATE::EMPTY_TILE ||
			this->rpState == RIGHT_PANEL_STATE::IDLE ||
			this->rpState == RIGHT_PANEL_STATE::RS_GEN_TILE ||
			this->rpState == RIGHT_PANEL_STATE::CLEAN_MODE ||
			this->rpState == RIGHT_PANEL_STATE::DESTROY_MODE)
		{
			// Only 3 icons can be clicked on this case
			bool clickedMode = this->checkModeIconMouseClick(point);
			if (clickedMode)
			{
				SoundManager::getInstance()->playButtonClick();
				return;
			}
		}
		else if (this->rpState == RIGHT_PANEL_STATE::BUILD_MODE ||
					this->rpState == RIGHT_PANEL_STATE::BUILD_FARM ||
					this->rpState == RIGHT_PANEL_STATE::BUILD_MINE ||
					this->rpState == RIGHT_PANEL_STATE::BUILD_LM ||
					this->rpState == RIGHT_PANEL_STATE::BUILD_VILLAGE)
		{
			// Player can either click building icons or other mode icons
			// first check if user clicked other mode icons
			bool clickedMode = this->checkModeIconMouseClick(point);
			if (clickedMode)
			{
				SoundManager::getInstance()->playButtonClick();
				return;
			}
			else
			{
				// Didn't select any other modes. Check building icons
				// Check if it clicked building icons
				bool clickedBuildingIcon = this->checkBuildingIconMouseClick(point);
				if (clickedBuildingIcon)
				{
					SoundManager::getInstance()->playButtonClick();
					if (this->curSelectingTileId != -1)
					{
						auto tile = this->getTileById(this->curSelectingTileId);
						cocos2d::log("Trying to build on tile #%d", tile->getId());
						this->costIcon->setVisible(true);
						this->costLabel->setVisible(true);

						std::string costStr = "";
						auto data = DataManager::getInstance()->getData("BUILDING_DATA");

						std::string key;

						switch (this->rpState)
						{
						case RIGHT_PANEL_STATE::BUILD_VILLAGE:
							key = "village";
							break;
						case RIGHT_PANEL_STATE::BUILD_FARM:
							key = "farm";
							break;
						case RIGHT_PANEL_STATE::BUILD_LM:
							key = "lumbermill";
							break;
						case RIGHT_PANEL_STATE::BUILD_MINE:
							key = "mine";
							break;
						default:
							break;
						}

						if (key.empty() == false)
						{
							auto pCost = data->getInt(key + ".cost.population");
							auto wCost = data->getInt(key + ".cost.wood");
							auto fCost = data->getInt(key + ".cost.food");
							auto mCost = data->getInt(key + ".cost.metal");

							bool hasResource = ResourceManager::getInstance()->checkCost(pCost, wCost, mCost, fCost);
							if (hasResource)
							{
								this->buildConfirmIcon->setOpacity(255);
							}
							else
							{
								this->buildConfirmIcon->setOpacity(100);
							}

							auto pCostStr = std::to_string(pCost);
							if (pCostStr.empty()) pCostStr = "0";
							if (pCostStr.length() == 1) pCostStr += " ";
							auto wCostStr = std::to_string(wCost);
							if (wCostStr.empty()) wCostStr = "0";
							if (wCostStr.length() == 1) wCostStr += " ";
							auto fCostStr = std::to_string(fCost);
							if (fCostStr.empty()) fCostStr = "0";
							if (fCostStr.length() == 1) fCostStr += " ";
							auto mCostStr = std::to_string(mCost);
							if (mCostStr.empty()) mCostStr = "0";
							if (mCostStr.length() == 1) mCostStr += " ";

							costStr = pCostStr + "        " + mCostStr + "\n" + wCostStr + "        " + fCostStr;

							this->costNumberLabel->setVisible(true);
							this->costNumberLabel->setString(costStr);
							this->buildConfirmIcon->setVisible(true);
						}
					}
					else
					{
						// build it right away
						cocos2d::log("select tile to build");
					}
					return;
				}
			}
		}

		if (this->buildConfirmIcon->isVisible())
		{
			if (this->buildConfirmIcon->getOpacity() == 255)
			{
				if (this->buildConfirmIcon->getBoundingBox().containsPoint(point))
				{
					//build
					if (this->curSelectingTileId != -1)
					{
						auto tile = this->getTileById(this->curSelectingTileId);
						std::string key = "";

						switch (this->rpState)
						{
						case RIGHT_PANEL_STATE::BUILD_FARM:
							tile->buildFarm();
							key = "farm";
							break;
						case RIGHT_PANEL_STATE::BUILD_VILLAGE:
							tile->buildVillage();
							key = "village";
							break;
						case RIGHT_PANEL_STATE::BUILD_LM:
							tile->buildLumbermill();
							key = "lumbermill";
							break;
						case RIGHT_PANEL_STATE::BUILD_MINE:
							tile->buildMine();
							key = "mine";
							break;
						default:
							break;
						}

						if (key.empty() == false)
						{
							auto rm = ResourceManager::getInstance();
							auto data = DataManager::getInstance()->getData("BUILDING_DATA");
							rm->usePopulation(data->getInt(key + ".cost.population"));
							rm->useWoods(data->getInt(key + ".cost.wood"));
							rm->useMetals(data->getInt(key + ".cost.metal"));
							rm->useFoods(data->getInt(key + ".cost.food"));

							tile->showHpBar(true);
							tile->showRsBar();

							this->updateRightPanel(tile);

							auto buildParticle = cocos2d::ParticleSystemQuad::create("particles/build.plist");
							buildParticle->setAutoRemoveOnFinish(true);
							buildParticle->setPosition(tile->getTilePosition());
							this->addChild(buildParticle, static_cast<int>(Z_ORDER::PARTICLES));

							this->rpState = RIGHT_PANEL_STATE::RS_GEN_TILE;

							this->buildIcon->setOpacity(100);
							this->cleanIcon->setOpacity(100);
							this->destroyIcon->setOpacity(100);
							SoundManager::getInstance()->playButtonClick();
						}

						this->toggleBuildingPreview(nullptr);
					}

					this->buildConfirmIcon->runAction(selectingSeq->clone());
				}
			}
		}

		return;
	}
	else if(this->gs->mouseClick(point))
	{
		SoundManager::getInstance()->playButtonClick();
		return;
	}
	else if (SoundManager::getInstance()->mouseClick(point))
	{
		return;
	}

	if (curHoveringTileId == -1)
	{
		// Didn't click grid
		if (curSelectingTileId != -1)
		{
			auto tile = this->getTileById(curSelectingTileId);
			tile->hideHpBar();
			tile->hideRsBar();
			curSelectingTileId = -1;
			this->tileSelectSprite->setPosition(cocos2d::Vec2(-200, -200));

			this->updateRightPanel(nullptr);

			if (this->buildPreview)
			{
				this->buildPreview->removeFromParent();
				this->buildPreview = nullptr;
			}

			SoundManager::getInstance()->playTileDeselect();
		}
	}
	else
	{
		// Clicked grid
		if (mouseEvent->getMouseButton() == cocos2d::EventMouse::MouseButton::BUTTON_LEFT)
		{
			if (curSelectingTileId == -1)
			{
				// select new
				curSelectingTileId = curHoveringTileId;
				auto tile = this->getTileById(curSelectingTileId);
				this->tileSelectSprite->setPosition(tile->getTilePosition());
				this->tileSelectSprite->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.8f), cocos2d::DelayTime::create(0.02f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr));
				this->tileHoverSprite->setPosition(cocos2d::Vec2(-200, -200));
				tile->showHpBar(true);
				tile->showRsBar();

				this->playSelectSound(tile);
				SoundManager::getInstance()->playButtonClick();

				this->updateRightPanel(tile);
			}
			else
			{
				if (curSelectingTileId == curHoveringTileId)
				{
					// Deselect
					this->tileSelectSprite->setPosition(cocos2d::Vec2(-200, -200));
					auto tile = this->getTileById(curSelectingTileId);
					this->tileHoverSprite->setPosition(tile->getTilePosition());
					curSelectingTileId = -1;
					tile->hideHpBar();
					tile->hideRsBar();

					this->updateRightPanel(nullptr);
					SoundManager::getInstance()->playTileDeselect();
				}
				else
				{
					// change select
					auto curTile = this->getTileById(this->curSelectingTileId);
					curTile->hideHpBar();
					curTile->hideRsBar();
					curSelectingTileId = curHoveringTileId;
					auto tile = this->getTileById(curSelectingTileId);
					this->tileSelectSprite->setPosition(tile->getTilePosition());
					this->tileHoverSprite->setPosition(cocos2d::Vec2(-200, -200));
					this->tileSelectSprite->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.8f), cocos2d::DelayTime::create(0.02f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr));
					tile->showHpBar(true);
					tile->showRsBar();

					if (this->buildPreview)
					{
						this->buildPreview->removeFromParent();
						this->buildPreview = nullptr;
					}

					this->playSelectSound(tile);
					SoundManager::getInstance()->playButtonClick();

					this->updateRightPanel(tile);
				}
			}
		}

	}
}

void GameScene::onMouseUp(cocos2d::Event* event) 
{
	//auto mouseEvent = static_cast<EventMouse*>(event);
	//0 = left, 1 = right, 2 = middle
	//int mouseButton = mouseEvent->getMouseButton();
	//float x = mouseEvent->getCursorX();
	//float y = mouseEvent->getCursorY();
}

void GameScene::onMouseScroll(cocos2d::Event* event) 
{
	//auto mouseEvent = static_cast<EventMouse*>(event);
	//float x = mouseEvent->getScrollX();
	//float y = mouseEvent->getScrollY();
}

void GameScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) 
{
	if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		if (this->viewingMenu)
		{
			//hide
			this->viewingMenu = false;
			this->menuPanel->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0, 1.0), cocos2d::ScaleTo::create(0.1f, 0.0f), cocos2d::Hide::create(), nullptr));
		}
		else
		{
			// show
			this->viewingMenu = true;
			this->menuPanel->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0, 0), cocos2d::Show::create(), cocos2d::ScaleTo::create(0.1f, 1.0f), nullptr));
		}
	}

	if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_SPACE)
	{
		//this->pause = !this->pause;
		//this->pauseNode->setVisible(this->pause);
		this->gs->togglePause();
	}

	/*
	if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_R)
	{
		ResourceManager::getInstance()->print();
	}
	if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_GRAVE)
	{
		ResourceManager::getInstance()->debugSetAllRsToZero();
	}
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_Q)
	{
		auto rm = ResourceManager::getInstance();
		rm->addPopulation(20);
		rm->addWoods(20);
		rm->addMetals(20);
		rm->addFoods(20);
		rm->doesNeedToUpdateUI();
	}

	*/
	if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_1)
	{
		this->gs->setSpeed(GameSpeed::SPEED_STATE::X1);
	}
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_2)
	{
		this->gs->setSpeed(GameSpeed::SPEED_STATE::X2);
	}
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_3)
	{
		this->gs->setSpeed(GameSpeed::SPEED_STATE::X3);
	}
}

void GameScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) 
{

}

void GameScene::releaseInputListeners()
{
	if(this->mouseInputListener != nullptr)
		_eventDispatcher->removeEventListener(this->mouseInputListener);
	if(this->keyInputListener != nullptr)
		_eventDispatcher->removeEventListener(this->keyInputListener);
}

void GameScene::onExit()
{
	cocos2d::Scene::onExit();

	releaseInputListeners(); 

	for (auto row : tileGrid)
	{
		for (auto col : row)
		{
			if (col)
			{
				delete col;
			}
		}
	}

	tileGrid.clear();

	this->selectingSeq->release();

	ResourceManager::deleteInstance();
	DataManager::deleteInstance();
	SoundManager::deleteInstance();

	if (this->cursor)
	{
		delete cursor;
	}

	if (this->gs)
	{
		delete gs;
	}
}