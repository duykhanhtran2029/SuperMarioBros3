#pragma once
#include "GameObject.h"
#include "Game.h"
#include "FireBullet.h"
#include "Utils.h"
#include "Portal.h"


#define PLAYER_SPEED	0.1f

#define PLAYER_STATE_IDLE			0
#define PLAYER_STATE_RIGHT			100
#define PLAYER_STATE_LEFT			111
#define PLAYER_STATE_UP				200
#define PLAYER_STATE_DOWN			201

#define PLAYER_BBOX_WIDTH	8	

// SMALL
#define MARIO_ANI_SMALL_IDLE_RIGHT			0
//BIG
#define MARIO_ANI_BIG_IDLE_RIGHT			14
//TAIL
#define MARIO_ANI_TAIL_IDLE_RIGHT			30
//FIRE
#define MARIO_ANI_FIRE_IDLE_RIGHT			46

#define MARIO	0
#define LUIGI	1

class CWorldPlayer : public CGameObject
{

	float start_x;			// initial position of Mario at scene
	float start_y;
	int level;
public:
	bool cgLeft, cgRight, cgUp, cgDown;
	int sceneId;
	CWorldPlayer(float x = 0.0f, float y = 0.0f);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) 
	{ left = x + 2; top = y + 2 ; right = left + PLAYER_BBOX_WIDTH - 4; bottom = top + PLAYER_BBOX_WIDTH - 4; };
	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);
	void SetState(int vState);
	void SetLevel(int l) { level = l; };
	void SetMove(bool cLeft, bool cUp, bool cRight, bool cDown)
	{
		cgLeft = cLeft;
		cgRight = cRight;
		cgUp = cUp;
		cgDown = cDown;
	};
	void ChooseScene()
	{
		CGame::GetInstance()->SwitchScene(sceneId);
	}
};