#include "Goomba.h"
#include "Brick.h"
#include "IntroScene.h"
#include "Utils.h"
#include "Block.h"
CGoomba::CGoomba()
{
	vx = -GOOMBA_WALKING_SPEED;
	vy = GOOMBA_GRAVITY;
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
	if (state == GOOMBA_STATE_DIE_BY_TAIL)
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
	if (tag == GOOMBA_RED)
	{
		right = x + GOOMBA_RED_BBOX_WIDTH;
		bottom = y + GOOMBA_RED_BBOX_WINGS_HEIGHT;
		if(state != GOOMBA_STATE_RED_JUMPING)
			bottom = y + GOOMBA_RED_BBOX_HEIGHT;
	}
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);
	CMario* mario = {};
	if (!dynamic_cast<CIntroScene*> (CGame::GetInstance()->GetCurrentScene()))
		mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	else
		mario = ((CIntroScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (GetTickCount64() - dying_start >= GOOMBA_TIME_DIYING && isDying)
	{
		isDestroyed = true;
		if (dynamic_cast<CIntroScene*> (CGame::GetInstance()->GetCurrentScene()))
			mario->SetState(MARIO_STATE_WALKING_RIGHT);
		return;
	}
	if (tag == GOOMBA_RED)
	{
		if (GetTickCount64() - walking_start >= GOOMBA_RED_TIME_WALKING && isWalking)
		{
			isWalking = false;
			jumping_stacks = 0;
			y -= 5;
			SetState(GOOMBA_STATE_RED_JUMPING);
			return;
		}
		if (GetTickCount64() - jumping_start >= GOOMBA_RED_TIME_JUMPING && isJumping)
		{
			vy = GOOMBA_JUMP_SPEED;
			isJumping = false;
			jumping_stacks++;
			return;
		}
		if (GetTickCount64() - highjumping_start >= GOOMBA_RED_TIME_HIGHJUMPING && isHighJumping)
		{
			isHighJumping = false;
			vy = GOOMBA_GRAVITY;
			jumping_stacks = -1;
			return;
		}
	}
	//
	// TO-DO: make sure Goomba can interact with the world and to each of them too!
	// 
	// Simple fall down

	

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	// turn off collision when goomba kicked 
	if (state != GOOMBA_STATE_DIE_BY_TAIL)
		CalcPotentialCollisions(coObjects, coEvents);

	float mLeft, mTop, mRight, mBottom;
	float oLeft, oTop, oRight, oBottom;
	if (mario != NULL )
	{
		if (mario->isTurningTail && !mario->isAtIntroScene)
		{
			mario->getTail()->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			GetBoundingBox(oLeft, oTop, oRight, oBottom);
			if (isColliding(floor(mLeft), mTop, ceil(mRight), mBottom))
			{
				mario->AddScore(x, y, 100, true);
				SetDirection(mario->nx);
				SetState(GOOMBA_STATE_DIE_BY_TAIL);
			}
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

		float min_tx, min_ty, x0, y0;
		int nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// block 
		x0 = x;
		y0 = y;

		x += min_tx * dx + nx * 0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
		y += min_ty * dy + ny * 0.4f;

		if (ny != 0)
			vy = 0;			


		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if(e->obj != NULL)
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

				if (e->ny != 0)
				{
					vy = 0;
					if (e->ny < 0 && tag == GOOMBA_RED)
					{
						if (jumping_stacks == GOOMBA_RED_JUMPING_STACKS)
						{
							jumping_stacks = -1;
							StartHighJumping();
							//DebugOut(L"[JUMP]\n");
						}
						else
						{
							if (jumping_stacks == -1)
							{
								y += 5;
								SetState(GOOMBA_STATE_RED_WINGSWALKING);
							}							
							else
								SetState(GOOMBA_STATE_RED_JUMPING);
						}
						
					}
				}
				if (e->nx != 0)
				{
					if (ceil(mBottom) != oTop)
						vx = - vx;
				}
			}
			else if (dynamic_cast<CBlock*>(e->obj))
			{
				CBlock* block = dynamic_cast<CBlock*>(e->obj);
				x = x0 + dx;
				y = y0 + dy;
			}
			else if (dynamic_cast<CFireBullet*>(e->obj))
			{
				SetState(GOOMBA_STATE_DIE);
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
}

void CGoomba::Render()
{
	int ani = 0;
	switch (tag)
	{
	case GOOMBA_NORMAL:
		ani = GOOMBA_NORMAL_ANI_WALKING;
		if (state == GOOMBA_STATE_DIE)
			ani = GOOMBA_NORMAL_ANI_DIE;
		break;
	case GOOMBA_RED:
		ani = GOOMBA_RED_ANI_WINGSWALKING;
		if (state == GOOMBA_STATE_DIE)
			ani = GOOMBA_RED_ANI_DIE;
		if (state == GOOMBA_STATE_DIE_BY_TAIL)
			ani = GOOMBA_RED_ANI_DIE;
		if (vy != 0)
			ani = GOOMBA_RED_ANI_JUMPING;
		break;
	case GOOMBA_RED_NORMAL:
		ani = GOOMBA_RED_ANI_WALKING;
		if (state == GOOMBA_STATE_DIE)
			ani = GOOMBA_RED_ANI_DIE;
		break;
	}
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	switch (state)
	{
	case GOOMBA_STATE_DIE:
		y += GOOMBA_NORMAL_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE;
		vx = 0;
		vy = 0;
		StartDying();
		break;
	case GOOMBA_STATE_DIE_BY_TAIL:
		vy = -GOOMBA_DIE_DEFLECT_SPEED;
		vx = -vx;
		//StartDying();
		break;
	case GOOMBA_STATE_RED_JUMPING:
		//if (this->state == GOOMBA_STATE_RED_WINGSWALKING)
			//y -= 5;
		StartJumping();
		break;
	case GOOMBA_STATE_WALKING:
		if (tag == GOOMBA_RED)
		{
			StartWalking();
			vy = 0;
		}
		else
			vy = GOOMBA_GRAVITY;
	case GOOMBA_STATE_RED_WINGSWALKING:
		StartWalking();
		break;
	}
	CGameObject::SetState(state);
}
