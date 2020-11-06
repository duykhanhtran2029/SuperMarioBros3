#include "Koopas.h"
#include "Utils.h"
CKoopas::CKoopas()
{
	//SetState(KOOPAS_STATE_WALKING);
	SetState(KOOPAS_STATE_IN_SHELL);
	nx = -1;
}


void CKoopas::CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents)
{
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f)
			coEvents.push_back(e);
		else
			delete e;
	}

	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}


void CKoopas::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == KOOPAS_STATE_DIE)
		return;
	left = x;
	top = y;
	right = x + KOOPAS_BBOX_WIDTH;
	if (state == KOOPAS_STATE_IN_SHELL || state == KOOPAS_STATE_SPINNING)
	{
		bottom = y + KOOPAS_BBOX_SHELL_HEIGHT;
	}
	else
		bottom = y + KOOPAS_BBOX_HEIGHT;
}

void CKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	// Simple fall down
	if (!isBeingHeld)
		vy += KOOPAS_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when goomba kicked 
	if (state != KOOPAS_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	CMario* mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (!mario->isHolding)
	{
		isBeingHeld = false;
	}
	if (isBeingHeld)
	{
		y = mario->y + 8;
		
		float tmp = mario -> vx;
		if (tmp < 0)
			tmp = -1;
		else
			tmp = 1;
		DebugOut(L"vx %f\t\n", mario->vx);
		if (mario->level == MARIO_LEVEL_BIG)
		{
			x = mario->x + tmp * MARIO_BIG_BBOX_WIDTH;
		}
		else if (mario->level == MARIO_LEVEL_SMALL)
		{
			x = mario->x + tmp * MARIO_SMALL_BBOX_WIDTH;
			y = y - 10;
		}
		else if (mario->level == MARIO_LEVEL_TAIL)
		{
			x = mario->x + tmp * MARIO_TAIL_BBOX_WIDTH;
		}
		else
		{
			x = mario->x + tmp * MARIO_FIRE_BBOX_WIDTH;
		}
	}
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

		if (ny != 0) vy = 0;



		// Collision logic with the others Koopas
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CKoopas*>(e->obj)) // if e->obj is Koopas 
			{
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				if (e->nx != 0 && this->GetState() == KOOPAS_STATE_SPINNING)
				{
					if (koopas->GetState() != KOOPAS_STATE_IN_SHELL)
					{
						koopas->SetState(KOOPAS_STATE_DIE);
					}
					else if (koopas->GetState() == KOOPAS_STATE_SPINNING || koopas->GetState() == KOOPAS_STATE_IN_SHELL)
					{
						this->vx = -this->vx;
						koopas->vx = -koopas->vx;
					}
				}

			}
			else
			{
				if (nx != 0 && ny == 0)
				{
					if (dynamic_cast<CGoomba*>(e->obj))
					{
						CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
						if (goomba->GetState() != GOOMBA_STATE_DIE && (this->GetState() == KOOPAS_STATE_SPINNING || isBeingHeld))
						{
							goomba->SetState(GOOMBA_STATE_DIE_BY_KICK);
						}
					}
					else if (!dynamic_cast<CBrick*>(e->obj))
						vx = -vx;
				}
			}
		}
	}

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

}

void CKoopas::Render()
{
	int ani = -1;
	if (state == KOOPAS_STATE_DIE) {
		ani = KOOPAS_ANI_DIE;
	}
	else if (state == KOOPAS_STATE_IN_SHELL)
	{
		ani = KOOPAS_ANI_SHELL;
	}
	else if (state == KOOPAS_STATE_SPINNING)
	{
		if (vx < 0)
			ani = KOOPAS_ANI_SPIN_LEFT;
		else
			ani = KOOPAS_ANI_SPIN_RIGHT;
	}
	else
	{
		if (vx < 0)
			ani = KOOPAS_ANI_WALKING_LEFT;
		else
			ani = KOOPAS_ANI_WALKING_RIGHT;
	}

	animation_set->at(ani)->Render(x, y);

	RenderBoundingBox();
}

void CKoopas::SetState(int state)
{
	CGameObject::SetState(state);
	CMario* mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	switch (state)
	{
	case KOOPAS_STATE_DIE:
		vy = -KOOPAS_DIE_DEFLECT_SPEED;
		vx = -vx;
		break;
	case KOOPAS_STATE_WALKING:
		vx = -KOOPAS_WALKING_SPEED;
		break;
	case KOOPAS_STATE_SPINNING:
		if (nx > 0)
			vx = KOOPAS_WALKING_SPEED * 15;
		else
			vx = -KOOPAS_WALKING_SPEED * 15;
		break;
	case KOOPAS_STATE_IN_SHELL:
		vx = 0;
		break;
	}

}