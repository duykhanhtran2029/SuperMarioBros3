#include "FloatingWood.h"
#include "Mario.h"
#include "PlayScene.h"
CFloatingWood::CFloatingWood()
{
	SetState(FLOATING_WOOD_STATE_MOVING);
	ay = 0;
}
void CFloatingWood::GetBoundingBox(float& left, float& top,
	float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + FLOATING_WOOD_BBOX_WIDTH;
	bottom = y + FLOATING_WOOD_BBOX_HEIGHT;
}
void CFloatingWood::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);
	vy += ay * dt;
	CPlayScene* playScene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	CMario* mario = playScene->GetPlayer();
}
void CFloatingWood::Render()
{

	animation_set->at(0)->Render(x,y);
	RenderBoundingBox();
}
void CFloatingWood::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FLOATING_WOOD_STATE_MOVING:
		vx = -FLOATING_WOOD_SPEED_X;
		ay = 0;
		break;
	case FLOATING_WOOD_STATE_FALLING:
		vx = 0;
		break;
	}
}

