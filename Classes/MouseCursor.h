#ifndef MOUSE_CURSOR_H
#define MOUSE_CURSOR_H

#include "cocos2d.h"

class MouseCursor
{
public:
	enum class CURSOR_TYPE
	{
		DEFAULT,
		BUILD_FARM,
		BUILD_VILLAGE,
		BUILD_MINE,
		BUILD_LM,
		CLEAN,
		DESTROY
	};
private:
	CURSOR_TYPE type;

	cocos2d::Node* cursorNode;
	cocos2d::Sprite* defaultCursor;		// Default arrow cursor
	cocos2d::Sprite* cleanCursor;		// Shovel cursor
	cocos2d::Sprite* destroyCursor;		// Red X cursor
	cocos2d::Sprite* buildFarmCursor;	// farm preview cursor
	cocos2d::Sprite* buildVillageCursor;
	cocos2d::Sprite* buildLumbermillCursor;
	cocos2d::Sprite* buildMineCursor;

	void hideAllCursor();
public:
	MouseCursor();
	~MouseCursor();

	void initCursors(cocos2d::Node* parent, const int z);

	void setType(CURSOR_TYPE type);
	void setColor(const cocos2d::Color3B& color);
	void updatePosition(const cocos2d::Vec2& position);
};

#endif