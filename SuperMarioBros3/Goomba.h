#pragma once
#include "GameObject.h"
#include "algorithm"
#include "Koopas.h"

#define GOOMBA_WALKING_SPEED 0.05f
#define GOOMBA_GRAVITY		 0.002f
#define GOOMBA_DIE_DEFLECT_SPEED	0.5f
#define GOOMBA_JUMP_SPEED		0.35f
#define GOOMBA_TIME_JUMPING		900
#define GOOMBA_NORMAL_BBOX_WIDTH 16
#define GOOMBA_NORMAL_BBOX_HEIGHT 15
#define GOOMBA_RED_BBOX_WIDTH 20
#define GOOMBA_RED_BBOX_HEIGHT 24
#define GOOMBA_BBOX_HEIGHT_DIE		9

#define GOOMBA_STATE_WALKING	100
#define GOOMBA_STATE_DIE		200
#define GOOMBA_STATE_DISAPPEAR  300
#define GOOMBA_STATE_DIE_BY_KICK 400
#define GOOMBA_STATE_RED_LOSE_WINGS 500

#define GOOMBA_NORMAL_ANI_WALKING		0
#define GOOMBA_NORMAL_ANI_DIE			1
#define GOOMBA_RED_ANI_WALKING		2
#define GOOMBA_RED_ANI_WITHOUT_WINGS	3
#define GOOMBA_RED_ANI_DIE			4

#define GOOMBA_NORMAL	0
#define GOOMBA_RED_FLY	1


class CGoomba : public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);
	int type;
	DWORD  jumpingStart = 0;
public:
	CGoomba(int ctype);
	int GetType()
	{
		return type;
	}
	virtual void SetState(int state);
};