#pragma once
#include "GameObject.h"

#define BLOCK_BBOX_WIDTH  16
#define BLOCK_BBOX_HEIGHT 16
#define IS_LEFT_EDGE_BLOCK	2
#define IS_RIGHT_EDGE_BLOCK	3

class CBlock : public CGameObject
{
public:
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};