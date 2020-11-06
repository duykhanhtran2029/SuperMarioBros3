#pragma once
#include "GameObject.h"

#define BRICK_BBOX_WIDTH  16
#define BRICK_BBOX_HEIGHT 16

class CBrick : public CGameObject
{
public:
	int tag = 0;
	CBrick(int tag = 0) { this->tag = tag; }
	int getTag() { return tag; };
	virtual void Render();
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
};