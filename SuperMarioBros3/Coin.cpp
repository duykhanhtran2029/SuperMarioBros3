#include "Coin.h"

CCoin::CCoin(int tag) {
	CGameObject::SetTag(tag);
	if (tag == COIN_TYPE_INBRICK)
		isAppear = false;
	else
		isAppear = true;
	state = COIN_STATE_IDLE;
}
void CCoin::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
	y += dy;
	float oLeft, oTop, oRight, oBottom;
	if(tag == COIN_TYPE_INBRICK)
		for (UINT i = 0; i < coObjects->size(); i++)
		{
			LPGAMEOBJECT obj = coObjects->at(i);
			if (obj != NULL)
				if (obj->isDestroyed == true)
					continue;
			obj->GetBoundingBox(oLeft, oTop, oRight, oBottom);
			if (isColliding(oLeft, oTop, oRight, oBottom))
			{
				if (dynamic_cast<CBrick*>(obj) && obj->tag == QUESTION)
				{
					CBrick* question_brick = dynamic_cast<CBrick*>(obj);
					if (question_brick->state == BRICK_STATE_QUESTION_HIT)
					{
						if (!isAppear)
						{
							SetState(COIN_STATE_UP);
							StartTiming();
							isAppear = true;
							DebugOut(L"[INBRICK]");
						}
						else
							isAppear = false;
					}
				}
			}

		}
	if (state == COIN_STATE_UP)
	{
		if (GetTickCount64() - timing_start >= COIN_FALLING_TIME)
		{
			SetState(COIN_STATE_DOWN);
			StartTiming();
		}
	}
	if (state == COIN_STATE_DOWN)
	{
		if (GetTickCount64() - timing_start >= COIN_FALLING_TIME)
		{
			isAppear = false;
			SetState(COIN_STATE_IDLE);
			x = y = -20;
			isDestroyed = true;
		}
	}

	// clean up collision events
}

void CCoin::Render()
{
	if (!isAppear)
		return;
	animation_set->at(0)->Render(x, y);
	RenderBoundingBox();
}

void CCoin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + COIN_BBOX_WIDTH;
	b = y + COIN_BBOX_HEIGHT;
}

void CCoin::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case COIN_STATE_IDLE:
		vx = vy = 0;
		break;
	case COIN_STATE_UP:
		vy = -0.2f;
		break;
	case COIN_STATE_DOWN:
		vy = 0.2f;
		break;
	}
}