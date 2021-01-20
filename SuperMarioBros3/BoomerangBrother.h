#pragma once
#include "GameObject.h"

#define BOOMERANG_BROTHER_BBOX_WIDTH    16
#define BOOMERANG_BROTHER_BBOX_HEIGHT   24

#define BOOMERANG_BROTHER_STATE_FORWARD		1
#define BOOMERANG_BROTHER_STATE_DIE			2

#define BOOMERANG_BROTHER_ANI_AIM_LEFT		0
#define BOOMERANG_BROTHER_ANI_THROW_LEFT	1
#define BOOMERANG_BROTHER_ANI_AIM_RIGHT		2
#define BOOMERANG_BROTHER_ANI_THROW_RIGHT	3
#define BOOMERANG_BROTHER_ANI_DIE			4

#define BOOMERANG_BROTHER_SPEED				0.02f
#define BOOMERANG_BROTHER_GRAVITY			0.001f
#define BOOMERANG_BROTHER_DEFLECT_SPEED		0.35f
#define BOOMERANG_BROTHER_LIMIT_RANGE		48
#define BOOMERANG_BROTHER_AIM_TIME			1000
#define BOOMERANG_BROTHER_THROW_TIME		500
class CBoomerangBrother :
    public CGameObject
{
	DWORD aim_start = 0;
	DWORD throw_start = 0;
public:
	float start_x = 0;
	CBoomerangBrother();
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void StartAim() { aim_start = GetTickCount64(); }
	void StartThrow() { throw_start = GetTickCount64(); }
};

