#include "BoomerangBrother.h"
#include "Brick.h"
#include "Mario.h"
#include "PlayScene.h"
CBoomerangBrother::CBoomerangBrother()
{
	SetState(BOOMERANG_BROTHER_STATE_FORWARD);
	nx = 1;
	StartReload();
	for (int i = 0; i < BOOMERANG_BROTHER_BOOMERANGS; i++)
	{
		boomerangs.push_back(new CBoomerang(x, y));
		((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->PushBack(boomerangs[i]);
	}
}
void CBoomerangBrother::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = left + BOOMERANG_BROTHER_BBOX_WIDTH;
	bottom = top + BOOMERANG_BROTHER_BBOX_HEIGHT;
}
void CBoomerangBrother::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
	vy += BOOMERANG_BROTHER_GRAVITY * dt;
	if (x < start_x)
		vx = BOOMERANG_BROTHER_SPEED;
	if (x > start_x + BOOMERANG_BROTHER_LIMIT_RANGE)
		vx = -BOOMERANG_BROTHER_SPEED;
	if (state == BOOMERANG_BROTHER_STATE_DIE)
	{
		x += dx;
		y += dy;
		for (int i = 0; i < BOOMERANG_BROTHER_BOOMERANGS; i++)
			if (boomerangs[i]->state == BOOMERANG_STATE_IDLE)
			{
				boomerangs[i]->isDestroyed = true;
				boomerangs[i]->isAppear = false;
			}
		return;
	}
	CMario* mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	float mLeft, mTop, mRight, mBottom;
	float oLeft, oTop, oRight, oBottom;
	if (mario != NULL && state != BOOMERANG_BROTHER_STATE_DIE)
	{
		if (mario->isTurningTail && mario->level == MARIO_LEVEL_TAIL)
		{
			mario->getTail()->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			GetBoundingBox(oLeft, oTop, oRight, oBottom);
			if (isColliding(floor(mLeft), mTop, ceil(mRight), mBottom))
			{
				mario->AddScore(x, y, 1000, true);
				SetState(BOOMERANG_BROTHER_STATE_DIE);
				mario->getTail()->ShowHitEffect();
				return;
			}
		}
		if (mario->x > x)
			nx = 1;
		else
			nx = -1;
		for (int i = 0; i < BOOMERANG_BROTHER_BOOMERANGS; i++)
			if (boomerangs[i]->state == BOOMERANG_STATE_IDLE)
				boomerangs[i]->nx = nx;

	}
	for (int i = 0; i < BOOMERANG_BROTHER_BOOMERANGS; i++)
	{
		if (boomerangs[i]->state >= BOOMERANG_STATE_2)
		{
			boomerangs[i]->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			GetBoundingBox(oLeft, oTop, oRight, oBottom);
			if (isColliding(floor(mLeft), floor(mTop), ceil(mRight), ceil(mBottom)))
			{
				if (i == 1)
					StartReload();
				boomerangs[i]->SetState(BOOMERANG_STATE_IDLE);
				boomerangs[i]->isAppear = false;
			}
		}

	}
	if (reload_start != 0 && GetTickCount64() - reload_start >= BOOMERANG_BROTHER_RELOAD_TIME 
		&& abs(x - mario->x) <= BOOMERANG_BROTHER_ACTIVE_RANGE)
	{
		reload_start = 0;
		StartAim();
	}
	if (aim_start != 0 && GetTickCount64() - aim_start >= BOOMERANG_BROTHER_AIM_TIME)
	{
		aim_start = 0;
		StartThrow();
		boomerangs[bmrIndex]->nx = nx;
		if (bmrIndex == 0)
			StartChange();
		boomerangs[bmrIndex++]->SetState(BOOMERANG_STATE_1);
		if (bmrIndex == BOOMERANG_BROTHER_BOOMERANGS)
			bmrIndex = 0;
	}
	if (aim_start > 0)
	{
		float bx = x + BOOMERANG_BBOX_WIDTH - nx * BOOMERANG_DIFF * 2;
		if (nx > 0)
			bx = x - nx * BOOMERANG_DIFF * 2;
		boomerangs[bmrIndex]->SetState(BOOMERANG_STATE_IDLE);
		boomerangs[bmrIndex]->SetPosition(bx, y);
		boomerangs[bmrIndex]->isAppear = true;
	}
	if (throw_start != 0 && GetTickCount64() - throw_start >= BOOMERANG_BROTHER_THROW_TIME)
	{
		throw_start = 0;
	}
	if (change_start != 0 && GetTickCount64() - change_start >= BOOMERANG_BROTHER_CHANGE_TIME 
		&& abs(x - mario->x) <= BOOMERANG_BROTHER_ACTIVE_RANGE)
	{
		change_start = 0;
		StartAim();
	}
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	if (state != BOOMERANG_BROTHER_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0)
	{
		y += dy;
		x += dx;
	}
	else
	{
		float min_tx, min_ty;
		float nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		float x0 = x, y0 = y;
		x = x0 + min_tx * dx + nx * PUSHBACK;
		y = y0 + min_ty * dy + ny * PUSHBACK;
		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			GetBoundingBox(mLeft, mTop, mRight, mBottom);
			e->obj->GetBoundingBox(oLeft, oTop, oRight, oBottom);
			if (dynamic_cast<CBrick*>(e->obj))
			{
				if (e->ny < 0)
					vy = 0;
				if (e->nx != 0)
				{
					if (ceil(mBottom) != oTop)
						vx = -vx;
				}
			}
		}
	}
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++)
		delete coEvents[i];
}
void CBoomerangBrother::Render()
{
	int ani = 0;
	if (nx > 0)
	{
		if (aim_start != 0)
			ani = BOOMERANG_BROTHER_ANI_AIM_RIGHT;
		else
			ani = BOOMERANG_BROTHER_ANI_THROW_RIGHT;
	}
	else
	{
		if (aim_start != 0)
			ani = BOOMERANG_BROTHER_ANI_AIM_LEFT;
		else
			ani = BOOMERANG_BROTHER_ANI_THROW_LEFT;
	}
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}
void CBoomerangBrother::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case BOOMERANG_BROTHER_STATE_FORWARD:
		vx = BOOMERANG_BROTHER_SPEED;
		break;
	case BOOMERANG_BROTHER_STATE_DIE:
		vy = -BOOMERANG_BROTHER_DEFLECT_SPEED;
		type = IGNORE;
		break;
	}
}