#include "IntroObject.h"
#include "IntroScene.h"
#include "Brick.h"
#include "Koopas.h"
CIntroObject::CIntroObject()
{
	ax = ay = 0;
	type = IGNORE;
}
void CIntroObject::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    CGameObject::Update(dt);
    vx += ax * dt;
    vy += ay * dt;


	if (vy <= -DEFLECT_SPEED && (collision_times == 1 || collision_times == 3))
	{
		vy = -DEFLECT_SPEED;
		ay = GRAVITY;
	}
	if (vy <= -DEFLECT_SPEED/2 && collision_times == 2 && tag == BLACK_SHELL)
	{
		vy = -DEFLECT_SPEED/2;
		ay = GRAVITY;
	}
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	CalcPotentialCollisions(coObjects, coEvents);

	if (tag == BLACK_SHELL && collision_times != 3)
	{
		CKoopas* koopas = ((CIntroScene*)CGame::GetInstance()->GetCurrentScene())->shell;
		if (koopas != nullptr)
		{
			float mLeft, mTop, mRight, mBottom;
			koopas->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			if (isColliding(floor(mLeft), mTop, ceil(mRight), mBottom))
			{
				vy = 0;
				ay = -GRAVITY;
				collision_times = 3;
			}
		}
	}
	// No collision occured, proceed normally
	if (coEvents.size() == 0 || collision_times == 3)
	{
		x += dx;
		y += dy;
	}
	else
	{

		float min_tx, min_ty, x0, y0;
		float nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// block 
		x0 = x;
		y0 = y;

		x += min_tx * dx + nx * PUSHBACK;		// nx*PUSHBACK : need to push out a bit to avoid overlapping next frame
		y += min_ty * dy + ny * PUSHBACK;

		float mLeft, mTop, mRight, mBottom;
		float oLeft, oTop, oRight, oBottom;
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (e->obj != NULL)
				if (e->obj->isDestroyed == true)
					continue;
			GetBoundingBox(mLeft, mTop, mRight, mBottom);
			if (dynamic_cast<CBrick*>(e->obj))
			{
				CBrick* object = dynamic_cast<CBrick*>(e->obj);
				object->GetBoundingBox(oLeft, oTop, oRight, oBottom);

				if (e->ny != 0)
				{
					vy = 0;
					if (collision_times < 2)
					{
						ay = -GRAVITY;
						collision_times++;
					}
					else
						ay = GRAVITY;
					if (tag == STAR)
					{
						vx = MARIO_WALKING_SPEED_MAX;
						if (collision_times == 2)
							isDestroyed = true;
					}

				}
				if (e->nx != 0)
				{
					if (ceil(mBottom) != oTop)
						vx = -vx;
				}
			}
			else
			{
				x = x0;
				y = y0;
			}
		}
	}
	if (vx < 0 && x < 0) {
		x = 0;
		vx = -vx;
	}
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++)
		delete coEvents[i];
	if (y > SCREEN_HEIGHT)
		isDestroyed = true;
};
void CIntroObject::Render()
{
	if (tag == BLACK_SHELL && collision_times == 3)
		animation_set->at(1)->Render(x, y);
	else
		animation_set->at(0)->Render(x, y);
};
void CIntroObject::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x;
    t = y;
    r = l + INTROOBJECT_BBOX_WIDTH;
    b = t + INTROOBJECT_BBOX_WIDTH;
};
 