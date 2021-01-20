#pragma once
#include "GameObject.h"
#include <algorithm>
#include "Define.h"
#include "Brick.h"

#define COIN_BBOX_WIDTH  16
#define COIN_BBOX_HEIGHT 15

#define COIN_ANI_SET_ID		6

#define COIN_STATE_IDLE		0
#define COIN_STATE_UP		100
#define COIN_STATE_DOWN		111

#define COIN_TYPE_NORMAL	6
#define COIN_TYPE_INBRICK	61
#define COIN_TYPE_TRANSFORM	60

#define COIN_FALLING_TIME			300
#define COIN_BEING_PUSHEDUP_TIME	300
#define COIN_EXIST_TIME				10000

class CCoin : public CGameObject
{
	DWORD timing_start = 0;
	DWORD exist_start = 0;
public:
	bool isAppear = true;
	CCoin(int tag = 6);
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	void SetState(int state);
	void StartTiming()
	{
		timing_start = GetTickCount64();
	}
	void StartExist()
	{
		exist_start = GetTickCount64();
	}
	void SetIsAppear(bool isAppearBool)
	{
		isAppear = isAppearBool;
	}
};