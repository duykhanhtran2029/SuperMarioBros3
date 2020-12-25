#pragma once
#include "GameObject.h"

#define BRICK_BBOX_WIDTH  16
#define BRICK_BBOX_HEIGHT 16

#define BRICK_PUSH_MAX_STACK 4
#define BRICK_PUSH_HEIGHT_STACK 2

#define PLATFORM 1
#define FLASH	12
#define PIPE	10
#define WOOD	46
#define QUESTION	42
#define CLOUDY	26

#define BRICK_ANI_IDLE				0
#define BRICK_ANI_QUESTION_NOMARK	1

#define BRICK_STATE_QUESTION_HIT	142
#define BRICK_STATE_IDLE	100

#define BRICK_PUSHED_TIME	30


class CBrick : public CGameObject
{
	DWORD pushup_start;
	bool isBeingPushedUp = false;
	bool isFallingDown = false;
	int PushedStacks = 0;
public:
	CBrick() { state = BRICK_STATE_IDLE; }
	//question mark
	void StartPushedUp() { pushup_start = GetTickCount64(); isBeingPushedUp = true; }
	void StopPushedUp() { pushup_start = GetTickCount64(); isBeingPushedUp = false; isFallingDown = true; }
	//basic
	void SetState(int vState);
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
};