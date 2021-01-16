#pragma once
#include "GameObject.h"
#define TAIL_BBOX_WIDTH		8
#define TAIL_BBOX_HEIGHT	6
#define TAIL_HIT_SPRITE_ID	10998	

#define TAIL_HIT_TIME		25
class CTail : public CGameObject
{
	DWORD hit_start = 0;
public:
	bool isAppear = false;
	bool hit_times = 0;
	CTail(float x, float y) { this->x = x; this->y = y; }
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void ShowHitEffect() { hit_start = GetTickCount64(); }
};

