#pragma once
#include "GameObject.h"
#include <algorithm>
#include "Define.h"
#include "Brick.h"

class CFireBullet : public CGameObject
{
public:
	bool isBeingUsed = false;
	int tempHeight = -1;
	void SetTemHeight(float ty) { this->tempHeight = ty; };
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	void SetIsBeingUsed(bool m) { isBeingUsed = m; };
	CFireBullet(float x = 300, float y = 300);
};
