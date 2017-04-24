#include "SoundManager.h"

SoundManager* SoundManager::instance = nullptr;

SoundManager::SoundManager()
	: ss(nullptr)
	, music(false)
	, sfx(false)
{}

SoundManager::~SoundManager()
{}

SoundManager* SoundManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new SoundManager();
	}

	return instance;
}

void SoundManager::deleteInstance()
{
	//check if instance is alive pointer
	if (instance != nullptr)
	{
		delete instance;
	}
}

void SoundManager::init()
{
	ss = CocosDenshion::SimpleAudioEngine::getInstance();

	ss->preloadBackgroundMusic("audios/bgm.wav");
	ss->preloadEffect("audios/villageSelectSFX.wav");
	ss->preloadEffect("audios/lumbermillSelectSFX.wav");

	ss->preloadEffect("audios/buttonSFX.wav");
	ss->preloadEffect("audios/tileDeselctSFX.wav");
}

void SoundManager::initIcons(cocos2d::Node * parent, const int z)
{
	this->musicOn = cocos2d::Sprite::createWithSpriteFrameName("musicOnIcon.png");
	this->musicOn->setVisible(false);
	this->musicOn->setPosition(cocos2d::Vec2(370.0f, 25.0f));
	parent->addChild(this->musicOn, z);

	this->musicOff = cocos2d::Sprite::createWithSpriteFrameName("musicOffIcon.png");
	this->musicOff->setPosition(cocos2d::Vec2(370.0f, 25.0f));
	parent->addChild(this->musicOff, z);

	this->sfxOn = cocos2d::Sprite::createWithSpriteFrameName("sfxOnIcon.png");
	this->sfxOn->setVisible(false);
	this->sfxOn->setPosition(cocos2d::Vec2(430.0f, 25.0f));
	parent->addChild(this->sfxOn, z);

	this->sfxOff = cocos2d::Sprite::createWithSpriteFrameName("sfxOffIcon.png");
	this->sfxOff->setPosition(cocos2d::Vec2(430.0f, 25.0f));
	parent->addChild(this->sfxOff, z);
}

bool SoundManager::mouseHover(const cocos2d::Vec2 & point)
{
	if (this->music)
	{
		if (this->musicOn->getBoundingBox().containsPoint(point))
		{
			this->musicOn->setScale(1.1f);
			return true;
		}
		else
		{
			this->musicOn->setScale(1.0f);
		}
	}
	else
	{
		if (this->musicOff->getBoundingBox().containsPoint(point))
		{
			this->musicOff->setScale(1.1f);
			return true;
		}
		else
		{
			this->musicOff->setScale(1.0f);
		}
	}

	if (this->sfx)
	{
		if (this->sfxOn->getBoundingBox().containsPoint(point))
		{
			this->sfxOn->setScale(1.1f);
			return true;
		}
		else
		{
			this->sfxOn->setScale(1.0f);
		}
	}
	else
	{
		if (this->sfxOff->getBoundingBox().containsPoint(point))
		{
			this->sfxOff->setScale(1.1f);
			return true;
		}
		else
		{
			this->sfxOff->setScale(1.0f);
		}
	}

	return false;
}

bool SoundManager::mouseClick(const cocos2d::Vec2 & point)
{
	if (this->music)
	{
		if (this->musicOn->getBoundingBox().containsPoint(point))
		{
			this->stopMusic();
			this->playButtonClick();
			return true;
		}
	}
	else
	{
		if (this->musicOff->getBoundingBox().containsPoint(point))
		{
			this->playMusic();
			this->playButtonClick();
			return true;
		}
	}

	if (this->sfx)
	{
		if (this->sfxOn->getBoundingBox().containsPoint(point))
		{
			this->muteSFX();
			this->playButtonClick();
			return true;
		}
	}
	else
	{
		if (this->sfxOff->getBoundingBox().containsPoint(point))
		{
			this->unmuteSFX();
			this->playButtonClick();
			return true;
		}
	}

	return false;
}

void SoundManager::playMusic()
{
	ss->playBackgroundMusic("audios/bgm.wav", true);
	this->musicOn->setVisible(true);
	this->musicOff->setVisible(false);
	this->music = true;
}

void SoundManager::stopMusic()
{
	ss->stopBackgroundMusic("audios/bgm.wav");
	this->musicOn->setVisible(false);
	this->musicOff->setVisible(true);
	this->music = false;
}

void SoundManager::muteSFX()
{
	this->sfxOn->setVisible(false);
	this->sfxOff->setVisible(true);
	this->sfx = false;
}

void SoundManager::unmuteSFX()
{
	this->sfxOn->setVisible(true);
	this->sfxOff->setVisible(false);
	this->sfx = true;
}

void SoundManager::playButtonClick()
{
	if(this->sfx) ss->playEffect("audios/buttonSFX.wav");
}

void SoundManager::playTileDeselect()
{
	if (this->sfx) ss->playEffect("audios/tileDeselctSFX.wav");
}

void SoundManager::playVillageSelect()
{
	if (this->sfx) ss->playEffect("audios/villageSelectSFX.wav");
}

void SoundManager::playLumbermillSelect()
{
	if (this->sfx) ss->playEffect("audios/lumbermillSelectSFX.wav");
}
