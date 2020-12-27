#pragma once
#include "Brick.h"
#include "Coin.h"
#include "Utils.h"
#include "Piece.h"
class CBreakableBrick :
	public CBrick
{
public:
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
};

