#pragma once
#include "GameObject.h"
#define TAIL_BBOX_WIDTH		8
#define TAIL_BBOX_HEIGHT	6
class CTail : public CGameObject
{
public:
	bool isAppear = false;
	CTail(float x, float y) { this->x = x; this->y = y; }
	virtual void Render() { RenderBoundingBox(255); };
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};

