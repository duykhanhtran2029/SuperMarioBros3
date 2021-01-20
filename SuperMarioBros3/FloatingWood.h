#pragma once
#include "GameObject.h"
#define FLOATING_WOOD_BBOX_WIDTH		48
#define FLOATING_WOOD_BBOX_HEIGHT	16

#define FLOATING_WOOD_SPEED		0.03f
#define FLOATING_WOOD_GRAVITY		0.0001f

#define FLOATING_WOOD_STATE_MOVING	0
#define FLOATING_WOOD_STATE_FALLING	1
#define FLOATING_WOOD_INACTIVE	2

#define MOVING_PLAT_FORM_ANI			0
class CFloatingWood :
    public CGameObject
{
public:
	CFloatingWood();
	bool isBeingTouched = false;

	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};

