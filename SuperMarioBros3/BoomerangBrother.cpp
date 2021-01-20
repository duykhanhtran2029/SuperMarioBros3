#include "BoomerangBrother.h"
#include "Brick.h"
CBoomerangBrother::CBoomerangBrother()
{
	SetState(BOOMERANG_BROTHER_STATE_FORWARD);
	StartAim();
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
	if (aim_start != 0 && GetTickCount64() - aim_start >= BOOMERANG_BROTHER_AIM_TIME)
	{
		aim_start = 0;
		StartThrow();
	}
	if (throw_start != 0 && GetTickCount64() - throw_start >= BOOMERANG_BROTHER_THROW_TIME)
	{
		throw_start = 0;
		StartAim();
	}
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0)
	{
		y += dy;
		x += dx;
	}
	else
	{
		float min_tx, min_ty;
		int nx = 0, ny = 0;
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
		float mLeft, mTop, mRight, mBottom;
		float oLeft, oTop, oRight, oBottom;
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
		vx = 0;
		vy = BOOMERANG_BROTHER_DEFLECT_SPEED;
		break;
	}
}