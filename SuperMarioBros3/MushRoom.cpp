#include "MushRoom.h"
#include "Utils.h"
#include "Mario.h"
#include "PlayScence.h"

CMario* mmario;
CPlayScene* mscence;
void CMushRoom::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isDestroyed)
		return;
	CGameObject::Update(dt);
	if (state == MUSHROOM_STATE_WALK)
	{
		float mLeft, mTop, mRight, mBottom;
		float oLeft, oTop, oRight, oBottom;
		mscence = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		if(mscence != NULL)
			mmario = ((CPlayScene*)mscence)->GetPlayer();
		if (mmario != NULL)
		{
			mmario->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			if (isColliding(mLeft, mTop, mRight, mBottom))
			{
				mmario->SetLevel(MARIO_LEVEL_BIG);
				isAppear = false;
				isDestroyed = true;
				x = y = -50;
			}
			else
			{
				vector<LPCOLLISIONEVENT> coEvents;
				vector<LPCOLLISIONEVENT> coEventsResult;

				coEvents.clear();
				CalcPotentialCollisions(coObjects, coEvents);

				if (coEvents.size() == 0)
				{
					y += dy;
					x += dx;
					vy = MUSHROOM_GRAVITY;
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
					x = x0 + min_tx * dx + nx * 0.4f;
					y = y0 + min_ty * dy + ny * 0.4f;
					//
					// Collision logic with other objects
					//
					for (UINT i = 0; i < coEventsResult.size(); i++)
					{
						LPCOLLISIONEVENT e = coEventsResult[i];
						if (e->obj != NULL)
							if (e->obj->isDestroyed == true)
							{
								x = x0 + dx;
								y = y0 + dy;
								continue;
							}
						GetBoundingBox(mLeft, mTop, mRight, mBottom);
						e->obj->GetBoundingBox(oLeft, oTop, oRight, oBottom);
						if (e->ny != 0)
						{
							vy = 0;
						}
						if (e->nx != 0)
						{
							if (ceil(mBottom) != oTop)
								vx = -vx;
						}
					}
				}
				// clean up collision events
				for (UINT i = 0; i < coEvents.size(); i++)
				{
					//coEvents[i]->obj->SetDebugAlpha(coEvents[i]->obj->DebugAlpha - 50);
					delete coEvents[i];
				}
			}

		}
		
	}
	if (state == MUSHROOM_STATE_UP)
	{
		y += dy;
		if (start_y - y >= MUSHROOM_BBOX_HEIGHT)
		{
			vy = 0;
			y = start_y - MUSHROOM_BBOX_HEIGHT - 0.1f;
			SetState(MUSHROOM_STATE_WALK);
		}
	}

}

void CMushRoom::Render()
{
	if (!isAppear || isDestroyed)
		return;
	animation_set->at(MUSHROOM_ANI_RED_ID)->Render(x, y);
	RenderBoundingBox();
}

void CMushRoom::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + MUSHROOM_BBOX_WIDTH;
	b = y + MUSHROOM_BBOX_HEIGHT;
}

void CMushRoom::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case MUSHROOM_STATE_IDLE:
		vx = vy = 0;
		break;
	case MUSHROOM_STATE_UP:
		vy = -0.05f;
		start_y = y;
		break;
	case MUSHROOM_STATE_WALK:
		mscence = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		mmario = ((CPlayScene*)mscence)->GetPlayer();
		vy = MUSHROOM_GRAVITY;
		vx = mmario->nx* MUSHROOM_SPEED;
		break;
	}
}