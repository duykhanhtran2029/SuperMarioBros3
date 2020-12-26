#include "Coin.h"
#include "Utils.h"
#include "PlayScence.h"
CCoin::CCoin(int tag) : CGameObject() {
	CGameObject::SetTag(tag);
	if (tag == COIN_TYPE_INBRICK)
		isAppear = false;
	else
		isAppear = true;
	state = COIN_STATE_IDLE;
}
CMario* cmario;
CPlayScene* cscence;
void CCoin::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isDestroyed)
		return;
	CGameObject::Update(dt);
	y += dy;
	if (state == COIN_STATE_IDLE)
	{
		float mLeft, mTop, mRight, mBottom;
		cscence = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		if (cscence != NULL)
			cmario = ((CPlayScene*)cscence)->GetPlayer();
		if (cmario != NULL)
		{
			cmario->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			if (isColliding(mLeft, mTop, mRight, mBottom))
			{
				isAppear = false;
				isDestroyed = true;
				x = y = -50;
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
			x = y = -50;
			isDestroyed = true;
		}
	}
}

void CCoin::Render()
{
	if (!isAppear || isDestroyed)
		return;
	animation_set->at(0)->Render(x, y);
	RenderBoundingBox(0);
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
		StartTiming();
		break;
	case COIN_STATE_DOWN:
		vy = 0.2f;
		break;
	}
}