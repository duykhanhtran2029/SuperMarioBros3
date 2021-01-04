#include "Leaf.h"
#include "Utils.h"
#include "Mario.h"
#include "PlayScence.h"
#include "Map.h"

void CLeaf::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isDestroyed)
		return;
	CGameObject::Update(dt);
	x += dx;
	y += dy;
	if (state == LEAF_STATE_FALLING)
	{
		float mLeft, mTop, mRight, mBottom;
		CMap* lmap = NULL;
		CMario* lmario = {};
		CPlayScene* lscence = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		if (lscence != NULL)
			lmario = ((CPlayScene*)lscence)->GetPlayer();
		if (lmario != NULL)
		{
			lmario->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			if (isColliding(mLeft, mTop, mRight, mBottom))
			{
				lmario->SetLevel(MARIO_LEVEL_TAIL);
				isAppear = false;
				isDestroyed = true;
				lmario->AddScore(1000);
			}

		}

		if (GetTickCount64() - start_timing >= LEAF_FALLING_TIME)
		{
			vx = -vx;
			start_timing = GetTickCount64();
		}
		if (y >= lscence->GetMap()->GetMapHeight())
		{
			isAppear = false;
			isDestroyed = true;
			//x = y = -50;
		}
	}
	if (state == LEAF_STATE_UP)
	{
		if (start_y - y >= LEAF_UP_HEIGHT)
			SetState(LEAF_STATE_FALLING);
	}
}

void CLeaf::Render()
{
	if (!isAppear || isDestroyed)
		return;
	int ani = LEAF_ANI_LEFT;
	if (state == LEAF_STATE_FALLING)
	{
		if (vx >= 0)
			ani = LEAF_ANI_RIGHT;
		else
			ani = LEAF_ANI_LEFT;
	}
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

void CLeaf::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + LEAF_BBOX_WIDTH;
	b = y + LEAF_BBOX_HEIGHT;
}

void CLeaf::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case LEAF_STATE_IDLE:
		vx = vy = 0;
		break;
	case LEAF_STATE_UP:
		vx = 0;
		vy = -0.05f;
		start_y = y;
		break;
	case LEAF_STATE_FALLING:
		vy = LEAF_GRAVITY;
		vx = LEAF_SPEED;
		start_timing = GetTickCount64();
		break;
	}
}