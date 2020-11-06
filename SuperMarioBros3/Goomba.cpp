#include "Goomba.h"
#include "Brick.h"
CGoomba::CGoomba(int ctype)
{
	type = ctype;
	SetState(GOOMBA_STATE_WALKING);
}
void CGoomba::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents)
{
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f)
		{
			coEvents.push_back(e);
		}
		else
			delete e;
	}

	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}
void CGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{

	if (state == GOOMBA_STATE_DIE_BY_KICK || state == GOOMBA_STATE_DISAPPEAR)
	{
		left = top = right = bottom = 0;
		return;
	}
	left = x;
	top = y;
	right = x + GOOMBA_NORMAL_BBOX_WIDTH;
	if (state == GOOMBA_STATE_DIE)
		bottom = y + GOOMBA_BBOX_HEIGHT_DIE;
	else
		bottom = y + GOOMBA_NORMAL_BBOX_HEIGHT;
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == GOOMBA_STATE_DISAPPEAR)
		return;
	CGameObject::Update(dt, coObjects);

	//
	// TO-DO: make sure Goomba can interact with the world and to each of them too!
	// 
	// Simple fall down
	vy += GOOMBA_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	// turn off collision when goomba kicked 
	if (state != GOOMBA_STATE_DIE_BY_KICK && state != GOOMBA_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);



	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{

		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// block 
		x += min_tx * dx + nx * 0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
		y += min_ty * dy + ny * 0.4f;

		if (ny != 0) vy = 0;

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			float oLeft, oTop, oRight, oBottom;
			float mLeft, mTop, mRight, mBottom;
			if (e->obj->isDestroyed == true)
				continue;
			GetBoundingBox(mLeft, mTop, mRight, mBottom);
			if (dynamic_cast<CGoomba*>(e->obj)) // if e->obj is Goomba 
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				if (e->nx != 0)
				{
					if (goomba->GetState() != GOOMBA_STATE_DIE)
					{
						goomba->vx = -goomba->vx;
						this->vx = -this->vx;
					}

				}
			}
			else if (dynamic_cast<CBrick*>(e->obj))
			{
				CBrick* object = dynamic_cast<CBrick*>(e->obj);
				object->GetBoundingBox(oLeft, oTop, oRight, oBottom);

				if (e->ny != 0) vy = 0;
				if (e->nx != 0)
				{
					if (ceil(mBottom) != oTop)
						vx = - vx;
					if (ceil(mRight) == oLeft && e->nx < 0)
						x = mLeft - 1;
					if (ceil(mLeft) == oRight && e->nx > 0)
						x = mLeft + 1;
					//DebugOut(L"[RESULT]	e->nx: %f\t mBottom: %f\toTop: %f\t\n",e->nx, mBottom, oTop);
				}
			}

		}
	}
	if (vx < 0 && x < 0) {
		x = 0;
		vx = -vx;
	}


	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

}

void CGoomba::Render()
{
	int ani = 0;
	switch (type)
	{
	case GOOMBA_NORMAL:
		ani = GOOMBA_NORMAL_ANI_WALKING;
		if (state == GOOMBA_STATE_DISAPPEAR)
		{
			SetIsDestroyed(true);
			return;
		}
		else if (state == GOOMBA_STATE_DIE) {
			ani = GOOMBA_NORMAL_ANI_DIE;
			state = GOOMBA_STATE_DISAPPEAR;
		}
		else if (state == GOOMBA_STATE_DIE_BY_KICK) {
			ani = GOOMBA_NORMAL_ANI_WALKING;
		}
		break;
	}
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case GOOMBA_STATE_DIE:
		y += GOOMBA_NORMAL_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
		vx = 0;
		vy = 0;
		break;
	case GOOMBA_STATE_DIE_BY_KICK:
		vy = -GOOMBA_DIE_DEFLECT_SPEED;
		vx = -vx;
		break;
	case GOOMBA_STATE_RED_LOSE_WINGS:
		vy = 0;
		break;
	case GOOMBA_STATE_WALKING:
		vx = -GOOMBA_WALKING_SPEED;
		break;
	}
}
