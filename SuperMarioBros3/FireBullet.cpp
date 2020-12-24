#include "FireBullet.h"
#include "Utils.h"

CFireBullet::CFireBullet(float x, float y) : CGameObject()
{
	vx = vy = 0;
	this->x = x;
	this->y = y;
}

void CFireBullet::FilterCollision(
	vector<LPCOLLISIONEVENT>& coEvents,
	vector<LPCOLLISIONEVENT>& coEventsResult,
	float& min_tx, float& min_ty,
	int& nx, int& ny, float& rdx, float& rdy)
{
	min_tx = 1.0f;
	min_ty = 1.0f;
	int min_ix = -1;
	int min_iy = -1;

	nx = 0;
	ny = 0;

	coEventsResult.clear();

	for (UINT i = 0; i < coEvents.size(); i++)
	{
		LPCOLLISIONEVENT c = coEvents[i];

		if (c->t < min_tx && c->nx != 0) {
			min_tx = c->t; min_ix = i; rdx = c->dx;
			if(ny == 0)
				nx = c->nx;
		}

		if (c->t < min_ty && c->ny != 0) {
			min_ty = c->t; min_iy = i; rdy = c->dy;
			if(nx == 0)
				ny = c->ny;
		}
	}

	if (min_ix >= 0) coEventsResult.push_back(coEvents[min_ix]);
	if (min_iy >= 0) coEventsResult.push_back(coEvents[min_iy]);
}

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
	}
	else
	{
		float min_tx, min_ty;
		int nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;
		float x0, y0;

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
				x0 = x;
				y0 = y;
				x += min_ty * dy + ny * 0.4f;
				y += min_tx * dx + nx * 0.4f;
				CBrick* obj = dynamic_cast<CBrick*>(e->obj);

				if (ny != 0)
				{
					tempHeight = y;
					vy = -FIRE_BULLET_SPEED_Y;
					y = y0;
				}

				if (nx != 0)
				{
					isBeingUsed = false;
					x = y = -1;
				}
			}
			else
			{
				x += dx;
				y += dy;
			}
		}
		//DebugOut(L"%f %f\n", tempHeight - FIRE_BULLET_LIMITED_HEIGHT, y);
	}
	if (y <= tempHeight - FIRE_BULLET_LIMITED_HEIGHT)
	{
		y = tempHeight - FIRE_BULLET_LIMITED_HEIGHT;
		vy = FIRE_BULLET_SPEED_Y;
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