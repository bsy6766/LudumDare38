#include "GameScene.h"

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

	this->initSpriteSheets();
	this->initSprites();
	this->initUI();
	this->initTiles();
	this->initResources();

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

	auto winSize = cocos2d::Director::getInstance()->getVisibleSize();
	auto x = winSize.width * 0.5f;
	auto y = winSize.height - 20.0f;

	float iconOffset = 22.0f;
	float labelGap = 130.0f;
	float pad = 10.0f;
	
	// Temp
	/*
	auto tempbg = cocos2d::Sprite::createWithSpriteFrameName("dot.png");
	tempbg->setPosition(cocos2d::Vec2(x, y));
	tempbg->setColor(cocos2d::Color3B(162, 162, 162));
	tempbg->setScaleY(40.0f);
	tempbg->setScaleX(winSize.width);
	this->uiNode->addChild(tempbg, 0);
	*/

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

	int z = static_cast<int>(UI_Z_ORDER::ICONS);

	{
		//population
		this->populationIcon = cocos2d::Sprite::createWithSpriteFrameName("populationIcon.png");
		this->populationIcon->setPosition(cocos2d::Vec2(x + iconOffset, y));
		this->uiNode->addChild(this->populationIcon, z);

		this->populationLabel = cocos2d::Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50);
		this->populationLabel->setScale(0.5f);
		this->populationLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
		this->populationLabel->setPosition(cocos2d::Vec2(x + this->populationIcon->getContentSize().width + pad, y));
		this->uiNode->addChild(this->populationLabel, z);
	}

	{
		//food
		this->foodIcon = cocos2d::Sprite::createWithSpriteFrameName("foodIcon.png");
		this->foodIcon->setPosition(cocos2d::Vec2(x + iconOffset + labelGap, y));
		this->uiNode->addChild(this->foodIcon, z);

		this->foodLabel = cocos2d::Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50);
		this->foodLabel->setScale(0.5f);
		this->foodLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
		this->foodLabel->setPosition(cocos2d::Vec2(x + labelGap + this->foodIcon->getContentSize().width + pad, y));
		this->uiNode->addChild(this->foodLabel, z);
	}

	{
		//wood
		this->woodIcon = cocos2d::Sprite::createWithSpriteFrameName("woodIcon.png");
		this->woodIcon->setPosition(cocos2d::Vec2(x + iconOffset + (labelGap * 2.0f), y));
		this->uiNode->addChild(this->woodIcon, z);

		this->woodLabel = cocos2d::Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50);
		this->woodLabel->setScale(0.5f);
		this->woodLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
		this->woodLabel->setPosition(cocos2d::Vec2(x + (labelGap * 2.0f) + this->woodIcon->getContentSize().width + pad, y));
		this->uiNode->addChild(this->woodLabel, z);
	}

	{
		//metal
		this->metalIcon = cocos2d::Sprite::createWithSpriteFrameName("metalIcon.png");
		this->metalIcon->setPosition(cocos2d::Vec2(x + iconOffset + (labelGap * 3.0f), y));
		this->uiNode->addChild(this->metalIcon, z);

		this->metalLabel = cocos2d::Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50);
		this->metalLabel->setScale(0.5f);
		this->metalLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
		this->metalLabel->setPosition(cocos2d::Vec2(x + (labelGap * 3.0f) + this->metalIcon->getContentSize().width + pad, y));
		this->uiNode->addChild(this->metalLabel, z);
	}

	// panel
	this->leftPanel = cocos2d::Sprite::createWithSpriteFrameName("leftPanel.png");
	this->leftPanel->setPosition(cocos2d::Vec2(winSize.width - 108.5f, winSize.height * 0.5f));
	this->uiNode->addChild(this->leftPanel, static_cast<int>(UI_Z_ORDER::PANEL));

	this->rightPanel = cocos2d::Sprite::createWithSpriteFrameName("rightPanel.png");
	this->rightPanel->setPosition(cocos2d::Vec2(108.5f, winSize.height * 0.5f));
	this->uiNode->addChild(this->rightPanel, static_cast<int>(UI_Z_ORDER::PANEL));

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

	this->mouseCursor = cocos2d::Sprite::createWithSpriteFrameName("cursor.png");
	this->mouseCursor->getTexture()->setAliasTexParameters();
	this->mouseCursor->setAnchorPoint(cocos2d::Vec2(0, 1.0f));
	this->uiNode->addChild(this->mouseCursor, static_cast<int>(UI_Z_ORDER::MOUSE_CURSOR));
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

	this->tileGrid.at(0).at(0)->initFarm();
	this->tileGrid.at(0).at(1)->initLumberMill();
	this->tileGrid.at(0).at(2)->initMine();
	this->tileGrid.at(0).at(3)->initVillage();
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

void GameScene::onEnter()
{
	cocos2d::Scene::onEnter();

	initInputListeners();
}

void GameScene::update(float delta)
{
	if (!this->pause)
	{
		delta *= deltaModifier;

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
	this->mouseCursor->setPosition(point);

	bool hovering = false;

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

					this->getTileById(this->curHoveringTileId)->hideHpBar();
					this->curHoveringTileId = hoveringTileId;
					tileChanged = true;
				}

				if (tileChanged)
				{
					this->tileHoverSprite->setPosition(col->getTilePosition());
					this->getTileById(this->curHoveringTileId)->showHpBar();
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
			this->getTileById(this->curHoveringTileId)->hideHpBar();
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

	if (curHoveringTileId == -1)
	{
		// Didn't click grid
		if (curSelectingTileId != -1)
		{
			curSelectingTileId = -1;
			this->tileSelectSprite->setPosition(cocos2d::Vec2(-200, -200));
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
				}
				else
				{
					// change select
					curSelectingTileId = curHoveringTileId;
					auto tile = this->getTileById(curSelectingTileId);
					this->tileSelectSprite->setPosition(tile->getTilePosition());
					this->tileHoverSprite->setPosition(cocos2d::Vec2(-200, -200));
					this->tileSelectSprite->runAction(this->selectingSeq->clone());
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
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_GRAVE)
	{
		this->deltaModifier = 0.5f;
	}
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_1)
	{
		this->deltaModifier = 1.0f;
	}
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_2)
	{
		this->deltaModifier = 2.0f;
	}
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_3)
	{
		this->deltaModifier = 3.0f;
	}
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_4)
	{
		this->deltaModifier = 0.0f;
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
}