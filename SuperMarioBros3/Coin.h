#pragma once

#include "GameObject.h"
#include "algorithm"
#include "Brick.h"
#include "PlayScence.h"

#define COIN_BBOX_WIDTH  16
#define COIN_BBOX_HEIGHT 15


#define COIN_STATE_IDLE		0
#define COIN_STATE_UP		100
#define COIN_STATE_DOWN		200

#define COIN_TYPE_NORMAL	6
#define COIN_TYPE_INBRICK	61

#define COIN_FALLING_TIME		300

class CCoin : public CGameObject
{
	bool isAppear;
	DWORD timing_start;
public:
	CCoin(int tag = 6);
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void SetState(int state);
	//void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);
	void StartTiming()
	{
		timing_start = GetTickCount64();
	}
	void SetIsAppear(bool isAppearBool)
	{
		isAppear = isAppearBool;
	}
	bool GetIsAppear()
	{
		return isAppear;
	}
};