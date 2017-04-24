#include "GameScene.h"
#include "DataManager.h"
#include "Utility.h"

USING_NS_CC;

GameScene* GameScene::createScene()
{
	GameScene* newGameScene = GameScene::create();
	return newGameScene;
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
	this->pause = false;
	this->rpState = RIGHT_PANEL_STATE::NONE;

	this->initSpriteSheets();
	this->initData();
	this->initSprites();
	this->initUI();
	this->initTiles();
	this->initResources();
	this->initInstances();

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

	this->selectingSeq = cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.8f), cocos2d::DelayTime::create(0.02f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr);
	this->selectingSeq->retain();

	auto winSize = cocos2d::Director::getInstance()->getVisibleSize();
	float iconX = 20.0f;
	float labelX = 46.0f;

	int z = static_cast<int>(UI_Z_ORDER::ICONS);

	{
		//population
		float populationY = winSize.height - 38.0f;

		this->populationIcon = cocos2d::Sprite::createWithSpriteFrameName("populationIcon.png");
		this->populationIcon->setPosition(cocos2d::Vec2(iconX, populationY));
		this->uiNode->addChild(this->populationIcon, z);

		this->populationLabel = cocos2d::Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50);
		this->populationLabel->setScale(0.5f);
		this->populationLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
		this->populationLabel->setPosition(cocos2d::Vec2(labelX, populationY));
		this->populationLabel->enableOutline(cocos2d::Color4B::BLACK, 8.0f);
		this->uiNode->addChild(this->populationLabel, z);
	}

	{
		//food
		float foodY = winSize.height - 140.0f;

		this->foodIcon = cocos2d::Sprite::createWithSpriteFrameName("foodIcon.png");
		this->foodIcon->setPosition(cocos2d::Vec2(iconX, foodY));
		this->uiNode->addChild(this->foodIcon, z);

		this->foodLabel = cocos2d::Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50);
		this->foodLabel->setScale(0.5f);
		this->foodLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
		this->foodLabel->setPosition(cocos2d::Vec2(labelX, foodY));
		this->foodLabel->enableOutline(cocos2d::Color4B::BLACK, 8.0f);
		this->uiNode->addChild(this->foodLabel, z);
	}

	{
		//wood
		float woodY = winSize.height - 73.0f;

		this->woodIcon = cocos2d::Sprite::createWithSpriteFrameName("woodIcon.png");
		this->woodIcon->setPosition(cocos2d::Vec2(iconX, woodY));
		this->uiNode->addChild(this->woodIcon, z);

		this->woodLabel = cocos2d::Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50);
		this->woodLabel->setScale(0.5f);
		this->woodLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
		this->woodLabel->setPosition(cocos2d::Vec2(labelX, woodY));
		this->woodLabel->enableOutline(cocos2d::Color4B::BLACK, 8.0f);
		this->uiNode->addChild(this->woodLabel, z);
	}

	{
		//metal
		float metalY = winSize.height - 106.0f;

		this->metalIcon = cocos2d::Sprite::createWithSpriteFrameName("metalIcon.png");
		this->metalIcon->setPosition(cocos2d::Vec2(iconX, metalY));
		this->uiNode->addChild(this->metalIcon, z);

		this->metalLabel = cocos2d::Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50);
		this->metalLabel->setScale(0.5f);
		this->metalLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
		this->metalLabel->setPosition(cocos2d::Vec2(labelX, metalY));
		this->metalLabel->enableOutline(cocos2d::Color4B::BLACK, 8.0f);
		this->uiNode->addChild(this->metalLabel, z);
	}

	// panel
	this->leftPanel = cocos2d::Sprite::createWithSpriteFrameName("leftPanel.png");
	this->leftPanel->setPosition(cocos2d::Vec2(108.5f, winSize.height * 0.5f));
	this->uiNode->addChild(this->leftPanel, static_cast<int>(UI_Z_ORDER::PANEL));

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
		this->titleDivider->setTag(RIGHT_PANEL_TAG::TITLE_DIVIDER);
		this->titleDivider->setVisible(false);
		this->tileInfoPanelNode->addChild(this->titleDivider);

		this->secondDivider = cocos2d::Sprite::createWithSpriteFrameName("panelDivider.png");
		this->secondDivider->setTag(RIGHT_PANEL_TAG::SECOND_DIVIDER);
		this->secondDivider->setVisible(false);
		this->tileInfoPanelNode->addChild(this->secondDivider);

		this->thirdDivider = cocos2d::Sprite::createWithSpriteFrameName("panelDivider.png");
		this->thirdDivider->setTag(RIGHT_PANEL_TAG::THIRD_DIVIDER);
		this->thirdDivider->setVisible(false);
		this->tileInfoPanelNode->addChild(this->thirdDivider);
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
		this->genLabel->setPosition(cocos2d::Vec2(winSize.width - 158.0f, winSize.height - 148.0f));
		this->genLabel->setTag(RIGHT_PANEL_TAG::GENERATES_LABEL);
		this->genLabel->setLineSpacing(10.0f);
		this->genLabel->enableOutline(cocos2d::Color4B::BLACK, 6.0f);
		this->genLabel->setVisible(false);
		this->tileInfoPanelNode->addChild(this->genLabel);

		this->usesLabel = cocos2d::Label::createWithTTF("Uses", "fonts/Marker Felt.ttf", 40.0f, cocos2d::Size::ZERO, cocos2d::TextHAlignment::RIGHT);
		this->usesLabel->setScale(0.5f);
		this->usesLabel->setPosition(cocos2d::Vec2(winSize.width - 5.0f, winSize.height - 148.0f));
		this->usesLabel->setTag(RIGHT_PANEL_TAG::USES_LABEL);
		this->usesLabel->setAnchorPoint(cocos2d::Vec2(1.0f, 0.5f));
		this->usesLabel->enableOutline(cocos2d::Color4B::BLACK, 6.0f);
		this->usesLabel->setVisible(false);
		this->usesLabel->setLineSpacing(10.0f);
		this->tileInfoPanelNode->addChild(this->usesLabel);

		this->nrTitle = cocos2d::Label::createWithTTF("Natural Resources", "fonts/Marker Felt.ttf", 40.0f);
		this->nrTitle->setScale(0.5f);
		this->nrTitle->setVisible(false);
		this->nrTitle->setPosition(cocos2d::Vec2(winSize.width - 100.0f, winSize.height - 82.0f));
		this->nrTitle->setTag(RIGHT_PANEL_TAG::NATURAL_RESOURCES_TITLE);
		this->nrTitle->enableOutline(cocos2d::Color4B::BLACK, 6.0f);
		this->tileInfoPanelNode->addChild(this->nrTitle);

		this->nrLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 40.0f);
		this->nrLabel->setScale(0.5f);
		this->nrLabel->setVisible(false);
		this->nrLabel->setPosition(cocos2d::Vec2(winSize.width - 70.0f, winSize.height - 155.0f));
		this->nrLabel->setTag(RIGHT_PANEL_TAG::NATURAL_RESOURCES_NUMBERS);
		this->nrLabel->enableOutline(cocos2d::Color4B::BLACK, 6.0f);
		this->nrLabel->setLineSpacing(10.0f);
		this->tileInfoPanelNode->addChild(this->nrLabel);

		castleHpLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 40.0f);
		castleHpLabel->setScale(0.5f);
		castleHpLabel->setVisible(false);
		castleHpLabel->setPosition(cocos2d::Vec2(winSize.width - 100.0f, winSize.height - 82.0f));
		castleHpLabel->setTag(RIGHT_PANEL_TAG::CASTLE_HP_LABEL);
		this->tileInfoPanelNode->addChild(castleHpLabel);
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
		this->nrIcon->setPosition(cocos2d::Vec2(winSize.width - 129.0f, winSize.height - 155.0f));
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

	// pause Node
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

	// Pick random 2 enriched tiles
	int randFirst = Utility::Random::randomInt(0, 11);
	int randSecond = Utility::Random::randomInt(13, 24);

	this->tileGrid.at(randFirst / 5).at(randFirst % 5)->makeNaturalResourceEnriched(Utility::Random::randomInt(0, 2));
	this->tileGrid.at(randSecond / 5).at(randSecond % 5)->makeNaturalResourceEnriched(Utility::Random::randomInt(0, 2));

	this->tileGrid.at(0).at(0)->initFarm();
	this->tileGrid.at(1).at(1)->initLumberMill();
	this->tileGrid.at(3).at(0)->initMine();
	this->tileGrid.at(4).at(2)->initVillage();
}

void GameScene::initResources()
{
	auto r = ResourceManager::getInstance();
	r->initResources(20, 50, 50, 50, 0);
	r->initResourcesCap(50, 100, 100, 100, 50);
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
	float gradiantDist = 254.0f;

	this->leftGradiant = cocos2d::Sprite::createWithSpriteFrameName("fadeGradiant.png");
	this->leftGradiant->setPosition(cocos2d::Vec2(gradiantDist, y));
	this->addChild(this->leftGradiant, static_cast<int>(Z_ORDER::GRADIANTS));

	this->rightGradiant = cocos2d::Sprite::createWithSpriteFrameName("fadeGradiant.png");
	this->rightGradiant->setPosition(cocos2d::Vec2(winSize.width - gradiantDist, y));
	this->rightGradiant->setScaleX(-1.0f);
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

void GameScene::onEnter()
{
	cocos2d::Scene::onEnter();

	initInputListeners();
}

void GameScene::update(float delta)
{
	if (!this->pause)
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
		}
	}
}

void GameScene::updateResourceUI(ResourceManager* rm)
{
	this->populationLabel->setString(std::to_string(rm->getPopulation()) + " / " + std::to_string(rm->getPopulationCap()));
	this->foodLabel->setString(std::to_string(rm->getFoods()) + " / " + std::to_string(rm->getFoodsCap()));
	this->woodLabel->setString(std::to_string(rm->getWoods()) + " / " + std::to_string(rm->getWoodsCap()));
	this->metalLabel->setString(std::to_string(rm->getMetals()) + " / " + std::to_string(rm->getMetalsCap()));
	//cocos2d::log("Updating resources");
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
	auto label = dynamic_cast<cocos2d::Label*>(this->tileInfoPanelNode->getChildByTag(RIGHT_PANEL_TAG::TILE_TITLE));
	auto tileStr = std::string();
	auto winSize = cocos2d::Director::getInstance()->getVisibleSize();


	if (tile->isEmptyTile())
	{
		tileStr = "Empty Tile";

		nrLabel->setString(tile->getNaturalResourcesAsStr());
		nrLabel->setVisible(true);

		int enrichedIndex = tile->hasEnrichedNR();

		if(enrichedIndex != -1)
		{
			float x = winSize.width - 30.0f;
			float y = 0;
			if (enrichedIndex == 0)
			{
				y = winSize.height - 120.0f;
			}
			else if (enrichedIndex == 1)
			{
				y = winSize.height - 153.0f;
			}
			else if (enrichedIndex == 2)
			{
				y = winSize.height - 185.0f;
			}

			if (y != 0.0f)
			{
				this->enrichIcon->setVisible(true);
				this->enrichIcon->setPosition(cocos2d::Vec2(x, y));
			}
		}

		nrTitle->setVisible(true);
		nrLabel->setVisible(true);
		nrIcon->setVisible(true);

		secondDivider->setVisible(true);
		secondDivider->setPosition(winSize.width - 107.0f, winSize.height - 210.0f);

		buildIcon->setVisible(true);
		cleanIcon->setVisible(true);
		destroyIcon->setVisible(true);

		this->buildIcon->setPosition(cocos2d::Vec2(winSize.width - 150.0f, winSize.height - 270.0f));
		this->cleanIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, winSize.height - 270.0f));
		this->destroyIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, winSize.height - 360.0f));

		thirdDivider->setVisible(true);
		thirdDivider->setPosition(winSize.width - 107.0f, 310.0f);

		this->rpState = RIGHT_PANEL_STATE::EMPTY_TILE;
	}
	else
	{
		if (tile->isCastle())
		{
			tileStr = "Castle";

			castleHpLabel->setVisible(true);
			castleHpLabel->setString("HP: " + std::to_string(tile->getBuildingCurHp()) + " / " + std::to_string(tile->getBuildingMaxHp()));

			secondDivider->setVisible(true);
			secondDivider->setPosition(winSize.width - 107.0f, winSize.height - 98.0f);

			this->rpState = RIGHT_PANEL_STATE::CASTLE;
		}
		else
		{
			// resource generator
			this->genLabel->setVisible(true);
			this->usesLabel->setVisible(true);

			std::string genStr = "Generates\n";
			std::string usesStr = "Uses\n";

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

			verticalIcon->setVisible(true);


			secondDivider->setVisible(true);
			secondDivider->setPosition(winSize.width - 107.0f, winSize.height - 231.0f);

			buildIcon->setVisible(true);
			cleanIcon->setVisible(true);
			destroyIcon->setVisible(true);

			this->buildIcon->setPosition(cocos2d::Vec2(winSize.width - 150.0f, winSize.height - 290.0f));
			this->cleanIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, winSize.height - 290.0f));
			this->destroyIcon->setPosition(cocos2d::Vec2(winSize.width - 55.0f, winSize.height - 380.0f));

			thirdDivider->setVisible(true);
			thirdDivider->setPosition(winSize.width - 107.0f, 290.0f);

			this->rpState = RIGHT_PANEL_STATE::RS_GEN_TILE;
		}
	}

	this->titleDivider->setVisible(true);

	label->setString(tileStr);
	label->setVisible(true);
}

void GameScene::toggleBuildingIcons(const bool visibility)
{
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

	if (this->buildIcon->getBoundingBox().containsPoint(point))
	{
		this->buildIcon->setScale(1.1f);
	}
	else if (this->cleanIcon->getBoundingBox().containsPoint(point))
	{
		this->cleanIcon->setScale(1.1f);
	}
	else if (this->destroyIcon->getBoundingBox().containsPoint(point))
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
	if (this->buildIcon->getBoundingBox().containsPoint(point))
	{
		this->buildIcon->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.9f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr));
		this->toggleBuildingIcons(true);
		this->rpState = RIGHT_PANEL_STATE::BUILD_MODE;
		cocos2d::log("Build mode");
		return true;
	}
	else if (this->cleanIcon->getBoundingBox().containsPoint(point))
	{
		this->cleanIcon->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.9f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr));
		this->toggleBuildingIcons(false);
		this->rpState = RIGHT_PANEL_STATE::CLEAN_MODE;
		cocos2d::log("clean mode");
		return true;
	}
	else if (this->destroyIcon->getBoundingBox().containsPoint(point))
	{
		this->destroyIcon->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.9f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr));
		this->toggleBuildingIcons(false);
		this->rpState = RIGHT_PANEL_STATE::DESTROY_MODE;
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
}

bool GameScene::checkBuildingIconMouseClick(const cocos2d::Vec2 & point)
{
	if (this->farmIcon->getBoundingBox().containsPoint(point))
	{
		this->farmIcon->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.9f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr));
		this->rpState = RIGHT_PANEL_STATE::BUILD_FARM;
		cocos2d::log("Build farm mode");
		return true;
	}
	else if (this->villageIcon->getBoundingBox().containsPoint(point))
	{
		this->villageIcon->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.9f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr));
		this->rpState = RIGHT_PANEL_STATE::BUILD_VILLAGE;
		cocos2d::log("Build village mode");
		return true;
	}
	else if (this->lumbermillIcon->getBoundingBox().containsPoint(point))
	{
		this->lumbermillIcon->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.9f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr));
		this->rpState = RIGHT_PANEL_STATE::BUILD_LM;
		cocos2d::log("Build lumber mill mode");
		return true;
	}
	else if (this->mineIcon->getBoundingBox().containsPoint(point))
	{
		this->mineIcon->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05f, 0.9f), cocos2d::ScaleTo::create(0.05f, 1.0f), nullptr));
		this->rpState = RIGHT_PANEL_STATE::BUILD_MINE;
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
	/*auto bb = this->rightPanel->getBoundingBox();
	bb.origin += this->tileInfoPanelNode->getPosition();
	return bb.containsPoint(point);*/
	return this->rightPanel->getBoundingBox().containsPoint(point);
}

void GameScene::clearRightPanel()
{
	auto children = this->tileInfoPanelNode->getChildren();

	for (auto child : children)
	{
		child->setVisible(false);
	}

	this->rpState = RIGHT_PANEL_STATE::NONE;
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

	bool hovering = false;

	if (this->checkRightPanelMouse(point))
	{
		if (this->rpState == RIGHT_PANEL_STATE::EMPTY_TILE ||
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
				this->checkBuildingIconMouseHover(point);
			}
		}

		return;
	}
	else if (this->gs->mouseHover(point))
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

	if (this->checkRightPanelMouse(point))
	{
		if (this->rpState == RIGHT_PANEL_STATE::EMPTY_TILE ||
			this->rpState == RIGHT_PANEL_STATE::CLEAN_MODE ||
			this->rpState == RIGHT_PANEL_STATE::DESTROY_MODE)
		{
			// Only 3 icons can be clicked on this case
			bool clickedMode = this->checkModeIconMouseClick(point);
			if (clickedMode) return;
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
				return;
			}
			else
			{
				// Didn't select any other modes. Check building icons
				// Check if it clicked building icons
				bool clickedBuildingIcon = this->checkBuildingIconMouseClick(point);
				if (clickedBuildingIcon) return;
			}
		}

		return;
	}
	else if(this->gs->mouseClick(point))
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

			this->clearRightPanel();
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
				this->tileSelectSprite->runAction(this->selectingSeq->clone());
				this->tileHoverSprite->setPosition(cocos2d::Vec2(-200, -200));
				tile->showHpBar(true);
				tile->showRsBar();

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

					this->updateRightPanel(tile);
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
					this->tileSelectSprite->runAction(this->selectingSeq->clone());
					tile->showHpBar(true);
					tile->showRsBar();

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
		cocos2d::Director::getInstance()->end();
	}

	if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_SPACE)
	{
		this->pause = !this->pause;
		this->pauseNode->setVisible(this->pause);
	}

	if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_R)
	{
		ResourceManager::getInstance()->print();
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

	if (this->cursor)
	{
		delete cursor;
	}

	if (this->gs)
	{
		delete gs;
	}
}