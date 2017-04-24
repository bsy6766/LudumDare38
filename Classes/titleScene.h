#ifndef TITLESCENE_H
#define TITLESCENE_H

#include "cocos2d.h"

class titleScene : public cocos2d::Scene
{
private:
	//default constructor
	titleScene() {};

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

	bool blockClick = false;

	cocos2d::Sprite* sky;
	cocos2d::Sprite* title;
	cocos2d::Sprite* clickToPlay;
	cocos2d::Sprite* castle;

	cocos2d::Sprite* grass0;
	cocos2d::Sprite* grass1;
	cocos2d::Sprite* grass2;
	cocos2d::Sprite* grass3;

	cocos2d::Sprite* cursor;
public:
	//simple creator func
	static titleScene* createScene();

	//default destructor
	~titleScene() {};

	//Cocos2d Macro
	CREATE_FUNC(titleScene);
};

#endif