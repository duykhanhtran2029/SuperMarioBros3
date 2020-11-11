#include "FireBullet.h"
#include "Utils.h"

CFireBullet::CFireBullet(float x, float y) : CGameObject()
{
	vx = vy = FIRE_BULLET_FLYING_SPEED;
	this->x = x;
	this->y = y;
}
int tempHeight = -1;

void CFireBullet::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{

	CGameObject::Update(dt);
	if (!this->isBeingUsed)
		return;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
		if (y <= tempHeight - FIRE_BULLET_LIMITED_HEIGHT)
			vy = -vy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (e->obj != NULL)
				if (e->obj->isDestroyed == true)
					continue;
			if (dynamic_cast<CBrick*>(e->obj))
			{
				CBrick* obj = dynamic_cast<CBrick*>(e->obj);
				y += min_ty * dy + ny * 0.4f;
				x += min_tx * dx + nx * 0.4f;
				if (nx != 0)
				{
					isBeingUsed = false;
					x = 1;
					y = -1;
				}
				if (ny != 0)
					vy = -vy;
				tempHeight = y;
			}
			else
			{
				x += dx;
				y += dy;
			}
		}
		//DebugOut(L"%f %f\n", tempHeight - FIRE_BULLET_LIMITED_HEIGHT, y);
	}

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}


void CFireBullet::Render()
{
	if (!isBeingUsed)
		return;
	int ani = 0;
	if (vx > 0)
	{
		ani = FIRE_BULLET_ANI_RIGHT;
	}
	else
		ani = FIRE_BULLET_ANI_LEFT;
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

void CFireBullet::GetBoundingBox(float& l, float& t, float& r, float& b)
{

	l = x;
	t = y;
	r = x + FIRE_BULLET_BBOX_WIDTH;
	b = y + FIRE_BULLET_BBOX_HEIGHT;
}