#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include "SimpleAudioEngine.h"
#include "cocos2d.h"

class SoundManager
{
private:
	SoundManager();
	~SoundManager();

	static SoundManager* instance;

	CocosDenshion::SimpleAudioEngine* ss;

	cocos2d::Sprite* musicOn;
	cocos2d::Sprite* musicOff;

	cocos2d::Sprite* sfxOn;
	cocos2d::Sprite* sfxOff;

	bool music;
	bool sfx;
public:
	// Get instance
	static SoundManager* getInstance();

	// Destroy instance.
	static void deleteInstance();

	// Prevent copying or assigning instance
	SoundManager(SoundManager const&) = delete;
	void operator=(SoundManager const&) = delete;

	void init();
	void initIcons(cocos2d::Node* parent, const int z);

	bool mouseHover(const cocos2d::Vec2& point);
	bool mouseClick(const cocos2d::Vec2& point);

	void playMusic();
	void stopMusic();
	void muteSFX();
	void unmuteSFX();
	void playButtonClick();
	void playTileDeselect();
	void playVillageSelect();
	void playLumbermillSelect();
};


#endif