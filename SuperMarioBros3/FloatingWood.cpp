#include "FloatingWood.h"
#include "Mario.h"
#include "PlayScene.h"
CFloatingWood::CFloatingWood()
{
	type = IGNORE;
	SetState(FLOATING_WOOD_STATE_MOVING);
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
	if (vy >= MARIO_JUMP_SPEED_MAX)
		vy = MARIO_JUMP_SPEED_MAX;
	x += dx;
	y += dy;
	if (isBeingTouched)
	{
		CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		CMario* mario = scene->GetPlayer();
		if (mario->isTouchingWood)
		{
			float mLeft, mTop, mRight, mBottom;
			float oLeft, oTop, oRight, oBottom;
			mario->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			GetBoundingBox(oLeft, oTop, oRight, oBottom);
			if(mRight < oLeft && mLeft > oRight || oTop - mBottom >= 1.0f)
			{
				mario->vy = MARIO_JUMP_SPEED_MAX;
				mario->ay = MARIO_GRAVITY;
				mario->isTouchingWood = false;
				isBeingTouched = false;
			}
			else if(oTop - mBottom < 1.0f && mRight >= oLeft && mLeft <= oRight)
			{
				if(mario->vy >= 0)
					mario->vy = this->vy;
				mario->isOnGround = true;
			}
		}
		else
			isBeingTouched = false;
	}
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
		vx = -FLOATING_WOOD_SPEED;
		ay = 0;
		break;
	case FLOATING_WOOD_STATE_FALLING:
		vx = 0;
		ay = FLOATING_WOOD_GRAVITY;
		break;
	}
}

