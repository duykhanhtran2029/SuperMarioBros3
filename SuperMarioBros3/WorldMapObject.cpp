#include "WorldMapObject.h"
#include "WorldPlayer.h"
#include "WorldScene.h"

CWorldMapObject::CWorldMapObject(int sceneId)
{
	vx = vy = 0;
	SetMove(false, false, false, false);
	this->sceneId = sceneId;
}
void CWorldMapObject::Render()
{
	animation_set->at(0)->Render(x, y);
	RenderBoundingBox();
}
void CWorldMapObject::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
	x += dx;
	y += dy;
}
void CWorldMapObject::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (tag != OBJECT_TYPE_BUSH)
	{
		left = x + 2;
		top = y + 2;
		right = left + OBJECT_BBOX_WIDTH - 4;
		bottom = top + OBJECT_BBOX_WIDTH - 4;
	}
}

