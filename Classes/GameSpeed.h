#ifndef GAME_SPEED_H
#define GAME_SPEED_H

#include "cocos2d.h"

class GameSpeed
{
public:
	enum class SPEED_STATE
	{
		PAUSED = 0,
		X1,
		X2,
		X3
	};
private:
	float speed;

	cocos2d::Node* speedIconNode;
	cocos2d::Sprite* speedx0;
	cocos2d::Sprite* speedx1;
	cocos2d::Sprite* speedx2;
	cocos2d::Sprite* speedx3;

	SPEED_STATE curSprite;

	SPEED_STATE prevSprite;

	void toggleVisibility(const bool visibility);
public:
	GameSpeed();
	~GameSpeed();

	void init(cocos2d::Node* parent, const int z);

	float getSpeed() { return speed; }
	void setSpeed(const float speed);
	void setSpeed(SPEED_STATE state);

	bool mouseHover(const cocos2d::Vec2& point);
	bool mouseClick(const cocos2d::Vec2& point);

	void togglePause();
};

#endif