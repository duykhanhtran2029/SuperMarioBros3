#include "Koopas.h"
#include "Utils.h"
#include "Block.h"
#include "QuestionBrick.h"
#include "BreakableBrick.h"
CKoopas::CKoopas()
{
	nx = -1;
	SetState(KOOPAS_STATE_WALKING);
	//SetState(KOOPAS_STATE_IN_SHELL);
	//SetState(KOOPAS_STATE_SHELL_UP);
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
	left = x;
	top = y;
	right = x + KOOPAS_BBOX_WIDTH;
	if (state == KOOPAS_STATE_IN_SHELL || state == KOOPAS_STATE_SPINNING|| state == KOOPAS_STATE_SHELL_UP)
	{
		bottom = y + KOOPAS_BBOX_SHELL_HEIGHT;
	}
	else
		bottom = y + KOOPAS_BBOX_HEIGHT;
}

void CKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);
	CMario* mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (GetTickCount64() - shell_start >= KOOPAS_SHELL_TIME && shell_start != 0 && state != KOOPAS_STATE_SPINNING)
	{
		shell_start = 0;
		StartReviving();
	}
	if (GetTickCount64() - reviving_start >= KOOPAS_REVIVE_TIME && reviving_start != 0 && state != KOOPAS_STATE_SPINNING)
	{
		reviving_start = 0;
		y -= (KOOPAS_BBOX_HEIGHT - KOOPAS_BBOX_SHELL_HEIGHT) + 1.0f;
		if (isBeingHeld)
		{
			isBeingHeld = false;
			mario->SetIsHolding(false);
		}
		SetState(KOOPAS_STATE_WALKING);
	}
	// Simple fall down
	if (!isBeingHeld)
		vy += KOOPAS_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	bool isNoCollision = false;
	// turn off collision when goomba kicked 
	//if (state != KOOPAS_STATE_SHELL_UP)
		CalcPotentialCollisions(coObjects, coEvents);
		
	float mLeft, mTop, mRight, mBottom;
	float oLeft, oTop, oRight, oBottom;
	if (mario != NULL)
	{
		if (mario->isTurningTail)
		{
			mario->getTail()->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			GetBoundingBox(oLeft, oTop, oRight, oBottom);
			if (isColliding(floor(mLeft), mTop, ceil(mRight), mBottom))
				SetState(KOOPAS_STATE_SHELL_UP);
		}
		if (!mario->isHolding && !mario->isTurningTail)
		{
			float mLeft, mTop, mRight, mBottom;
			mario->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			isBeingHeld = false;
			if (isColliding(mLeft, mTop, mRight, mBottom))
			{
				if (state == KOOPAS_STATE_IN_SHELL)
				{
					this->nx = mario->nx;
					this->SetState(KOOPAS_STATE_SPINNING);
					mario->StartKicking();
				}
			}
		}
	}
	if (isBeingHeld)
	{
		y = mario->y + KOOPAS_BBOX_SHELL_HEIGHT/2;
		float tmp = mario -> vx;
		if (tmp < 0)
			tmp = -1;
		if (tmp > 0)
			tmp = 1;
		if (tmp == 0)
			tmp = mario->nx;

		x = mario->x + tmp * (MARIO_BIG_BBOX_WIDTH);
		if (mario->level == MARIO_LEVEL_SMALL)
		{
			if (tmp > 0)
				x = mario->x + tmp * (MARIO_SMALL_BBOX_WIDTH);
			else
				x = mario->x + tmp * (KOOPAS_BBOX_WIDTH);
			y -=(MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT);
		}
	}
	// No collision occured, proceed normally
	if (coEvents.size() == 0||isBeingHeld)
	{
		x += dx;
		y += dy;
		if (!isBeingHeld && state == KOOPAS_STATE_WALKING && 
			(tag == KOOPAS_RED || tag == KOOPAS_RED_PARA) && CanPullBack && (y - lastStanding_Y >= 1.0f))
		{
			//DebugOut(L"[SHELL] x %f y %f newx %f newy %f\n", x, y,x - nx * KOOPAS_BBOX_WIDTH, lastStanding_Y);
			y = lastStanding_Y;
			x -= nx * KOOPAS_BBOX_WIDTH;
			nx = -nx;
			vx = -vx;			
		}
	}
	else
	{

		float min_tx, min_ty;
		int nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// block 
		float x0 = x, y0 = y;
		x = x0 + min_tx * dx + nx * 0.4f;
		y = y0 + min_ty * dy + ny * 0.4f;

		if (ny != 0) vy = 0;
		//if (nx != 0) vx = 0;

		if (state == KOOPAS_STATE_SHELL_UP)
			vx = 0;
		// Collision logic with others
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (e->obj != NULL)
				if (e->obj->isDestroyed == true)
					continue;
			GetBoundingBox(mLeft, mTop, mRight, mBottom);
			if (dynamic_cast<CKoopas*>(e->obj)) // if e->obj is Koopas 
			{
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				if (e->nx != 0 && this->GetState() == KOOPAS_STATE_SPINNING)
				{
					if (koopas->GetState() != KOOPAS_STATE_IN_SHELL)
					{
						koopas->SetState(KOOPAS_STATE_SHELL_UP);
					}
					else if (koopas->GetState() == KOOPAS_STATE_SPINNING || koopas->GetState() == KOOPAS_STATE_IN_SHELL)
					{
						this->vx = -this->vx;
						koopas->vx = -koopas->vx;
					}
				}

			}
			if (dynamic_cast<CGoomba*>(e->obj))
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				if (goomba->GetState() != GOOMBA_STATE_DIE && (this->GetState() == KOOPAS_STATE_SPINNING || isBeingHeld))
				{
					goomba->SetState(GOOMBA_STATE_DIE_BY_TAIL);
				}
			}
			if (dynamic_cast<CBrick*>(e->obj))
			{
				CBrick* object = dynamic_cast<CBrick*>(e->obj);
				//object->SetDebugAlpha(255);
				object->GetBoundingBox(oLeft, oTop, oRight, oBottom);
				CanPullBack = true;
				lastStanding_Y = y;
				if (e->ny != 0) vy = 0;
				if (e->ny < 0 && (tag == KOOPAS_GREEN_PARA || tag == KOOPAS_RED_PARA))
				{
					vy = -KOOPAS_JUMP_SPEED;
				}
				if (e->nx != 0)
				{
					if (ceil(mBottom) != oTop)
						vx = -vx;
				}
			}
			if (dynamic_cast<CQuestionBrick*>(e->obj) && state == KOOPAS_STATE_SPINNING)
			{
				CQuestionBrick* tmp = dynamic_cast<CQuestionBrick*>(e->obj);
				if (tmp->state != QUESTIONBRICK_STATE_HIT)
					tmp->SetState(QUESTIONBRICK_STATE_HIT);
			}
			if (dynamic_cast<CBreakableBrick*>(e->obj) && state == KOOPAS_STATE_SPINNING)
			{
				CBreakableBrick* tmp = dynamic_cast<CBreakableBrick*>(e->obj);
				tmp->Break();
			}
			if (dynamic_cast<CBlock*>(e->obj))
			{
				CBlock* block = dynamic_cast<CBlock*>(e->obj);
				CanPullBack = true;
				lastStanding_Y = y;
				if (e->ny < 0)
				{
					vy = 0;
				}
				else
					x = x0 + dx;
				if (e->ny < 0 && (tag == KOOPAS_GREEN_PARA || tag == KOOPAS_RED_PARA))
				{
					vy = -KOOPAS_JUMP_SPEED;
				}
			}
			if (dynamic_cast<CFireBullet*>(e->obj))
				SetState(KOOPAS_STATE_IN_SHELL);
		}
	}

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

}

void CKoopas::Render()
{
	int ani = -1;
	if (state == KOOPAS_STATE_SHELL_UP) {
		ani = KOOPAS_ANI_SHELL_UP;
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
	if(tag == KOOPAS_GREEN_PARA || tag == KOOPAS_RED_PARA)
		if (vx < 0)
			ani = KOOPAS_ANI_PARA_LEFT;
		else
			ani = KOOPAS_ANI_PARA_RIGHT;
	if (reviving_start != 0)
	{
		if(state == KOOPAS_STATE_IN_SHELL)
			ani = KOOPAS_ANI_SHAKE;
		if (state == KOOPAS_STATE_SHELL_UP)
			ani = KOOPAS_ANI_SHAKE_UP;
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
	case KOOPAS_STATE_SHELL_UP:
		vy = -KOOPAS_DIE_DEFLECT_SPEED;
		vx = -vx;
		nx = -nx;
		StartShell();
		break;
	case KOOPAS_STATE_WALKING:
		vx = nx*KOOPAS_WALKING_SPEED;
		break;
	case KOOPAS_STATE_SPINNING:
		if (nx > 0)
			vx = KOOPAS_WALKING_SPEED * 5;
		else
			vx = -KOOPAS_WALKING_SPEED * 5;
		break;
	case KOOPAS_STATE_IN_SHELL:
		vx = 0;
		StartShell();
		break;
	case KOOPAS_STATE_PARA:
		vy = -KOOPAS_JUMP_SPEED;
		break;
	}

}