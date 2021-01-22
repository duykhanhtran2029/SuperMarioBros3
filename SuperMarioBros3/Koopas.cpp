#include "Koopas.h"
#include "Utils.h"
#include "Block.h"
#include "QuestionBrick.h"
#include "BreakableBrick.h"
#include "IntroScene.h"
#include "IntroObject.h"
#include "PiranhaPlant.h"
#include "FirePiranhaPlant.h"
CKoopas::CKoopas()
{
	nx = -1;
	SetState(KOOPAS_STATE_WALKING);
	//SetState(KOOPAS_STATE_IN_SHELL);
	//SetState(KOOPAS_STATE_SHELL_UP);
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
int CKoopas::CalRevivable()
{
	CGame* game = CGame::GetInstance();
	float camX, camY;

	camX = game->GetCamX();
	camY = game->GetCamY();

	bool IsStartInViewPort = (start_x > camX - GetWidth() - VIEWPORT_PUSHBACK 
		&& start_x < camX + game->GetScreenWidth() + VIEWPORT_PUSHBACK
		&& start_y > camY - (SCREEN_HEIGHT - game->GetScreenHeight()) 
		&& start_y < camY + SCREEN_HEIGHT);
	//if (!IsStartInViewPort && !IsInViewPort())
	//	return 1;
	//if (IsStartInViewPort && !IsInViewPort())
	//	return -1;
	return IsStartInViewPort;
}
bool CKoopas::CalKillable(vector<LPGAMEOBJECT>* coObjects)
{
	for(UINT i = 0; i < coObjects->size(); i++)
		if (dynamic_cast<CBrick*>(coObjects->at(i)))
		{
			float mLeft, mTop, mRight, mBottom;
			coObjects->at(i)->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			if (isColliding(floor(mLeft), floor(mTop), ceil(mRight), ceil(mBottom)))
				return true;
		}
	return false;
}
bool CKoopas::CalTurnable(LPGAMEOBJECT object, vector<LPGAMEOBJECT>* coObjects)
{
	if (!IsInViewPort())
		return false;
	for (UINT i = 0; i < coObjects->size(); i++)
		if (dynamic_cast<CBrick*>(coObjects->at(i)) || dynamic_cast<CBlock*>(coObjects->at(i)))
			if (abs(coObjects->at(i)->y == object->y))
			{
				if (nx > 0)
					if (coObjects->at(i)->x > object->x && coObjects->at(i)->x - BRICK_BBOX_WIDTH < object->x + BRICK_BBOX_WIDTH)
						return false;
				if(nx < 0)
					if (coObjects->at(i)->x + BRICK_BBOX_WIDTH > object->x - BRICK_BBOX_WIDTH && coObjects->at(i)->x < object->x)
						return false;
			}
	return true;
}
void CKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (!isEnable)
		return;
	if (GetTickCount64() - dying_start >= KOOPAS_DIYING_TIME && dying_start != 0)
		isDestroyed = true;
	CGameObject::Update(dt);
	CMario* mario = {};
	if (!dynamic_cast<CIntroScene*> (CGame::GetInstance()->GetCurrentScene()))
		mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	else
	{
		if (((CIntroScene*)CGame::GetInstance()->GetCurrentScene())->holder == NULL)
			mario = ((CIntroScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		else
			mario = ((CIntroScene*)CGame::GetInstance()->GetCurrentScene())->holder;
	}
	if (!mario->isAtIntroScene && state != KOOPAS_STATE_DEATH)
	{
		if (GetTickCount64() - shell_start >= KOOPAS_SHELL_TIME && shell_start != 0 && state != KOOPAS_STATE_SPINNING)
		{
			shell_start = 0;
			StartReviving();
		}
		if (GetTickCount64() - reviving_start >= KOOPAS_REVIVE_TIME && reviving_start != 0 && state != KOOPAS_STATE_SPINNING && shell_start == 0)
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
	}
	// Simple fall down
	if (!isBeingHeld && !mario->isAtIntroScene)
	{
		if(tag == KOOPAS_GREEN_PARA)
			vy += KOOPAS_PARA_GRAVITY * dt;
		if (tag == KOOPAS_RED || tag == KOOPAS_GREEN)
			vy += KOOPAS_GRAVITY * dt;
	}
	if (tag == KOOPAS_RED_PARA)
	{
		vx = 0;
		if (y <= start_y)
			vy = KOOPAS_RED_SPEED;
		if (y + KOOPAS_BBOX_HEIGHT 
			>= ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetMap()->GetMapHeight())
			vy = -KOOPAS_RED_SPEED;
		//DebugOut(L"[KOOPAS] nx %d\n",this->nx);
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	if (state != KOOPAS_STATE_DEATH)
		CalcPotentialCollisions(coObjects, coEvents);
	
	float mLeft, mTop, mRight, mBottom;
	float oLeft, oTop, oRight, oBottom;
	if (mario != NULL)
	{
		GetBoundingBox(oLeft, oTop, oRight, oBottom);
		if (mario->isTurningTail && mario->level == MARIO_LEVEL_TAIL)
		{
			mario->getTail()->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			if (isColliding(floor(mLeft), floor(mTop), ceil(mRight), ceil(mBottom)))
			{
				SetState(KOOPAS_STATE_SHELL_UP);
				mario->getTail()->ShowHitEffect();
				if (tag == KOOPAS_GREEN_PARA)
					tag = KOOPAS_GREEN;
			}
		}
		else if (!mario->isTurningTail)
		{
			mario->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			if (isColliding(floor(mLeft), floor(mTop), ceil(mRight), ceil(mBottom)))
			{
				if (!mario->isHolding && isBeingHeld)
				{
					isBeingHeld = false;
						if ((state == KOOPAS_STATE_IN_SHELL || state == KOOPAS_STATE_SHELL_UP) && !isBeingHeld)
						{
							this->nx = mario->nx;
							this->SetState(KOOPAS_STATE_SPINNING);
							mario->StartKicking();
							if (isKillable)
							{
								this->SetState(KOOPAS_STATE_DEATH);
								mario->AddScore(x, y, 100, true);
							}
						}
				}
				else if (!isBeingHeld)
				{
					if(abs(oBottom - mTop) <= 1.0f && mario->y > y &&
						mario->untouchable == 0 
						&& (state == KOOPAS_STATE_WALKING || state == KOOPAS_STATE_SPINNING))
						mario->Attacked();
				}
			}
		}
		if (isBeingHeld)
		{
			y = mario->y + KOOPAS_BBOX_SHELL_HEIGHT / 2;
			float tmp = mario->vx;
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
				y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT);
			}
			isKillable = CalKillable(coObjects);
		}
	}
	// No collision occured, proceed normally
	if (coEvents.size() == 0||isBeingHeld)
	{
		x += dx;
		y += dy;
	}
	else
	{

		float min_tx, min_ty;
		float nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// block 
		float x0 = x, y0 = y;
		x = x0 + min_tx * dx + nx * PUSHBACK;
		y = y0 + min_ty * dy + ny * PUSHBACK;

		//if (nx != 0) vx = 0;
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
				if (e->ny > 0)
					y = y0;
				if (koopas->state == KOOPAS_STATE_SPINNING)
				{
					if (tag == KOOPAS_GREEN_PARA)
						tag = KOOPAS_GREEN;
					if (tag == KOOPAS_RED_PARA)
						tag = KOOPAS_RED;
					this->SetState(KOOPAS_STATE_DEATH);
					mario->AddScore(x, y, 100, true);
				}
				else
				{
					x = x0;
					y = y0;
					if ((koopas->state == KOOPAS_STATE_SHELL_UP || koopas->state == KOOPAS_STATE_IN_SHELL)
						&& state == KOOPAS_STATE_WALKING)
					{
						x = x0 + dx;
						y = y0 + dy;
					}
					if (state == KOOPAS_STATE_WALKING && koopas->state == KOOPAS_STATE_WALKING)
					{
						x = x0 + dx;
						if(e->ny < 0)
							y = y0 + dy;
						this->vx = -this->vx;
						this->nx = -this->nx;
						koopas->vx = -koopas->vx;
						koopas->nx = -koopas->nx;
					}
				}
			}
			if (dynamic_cast<CGoomba*>(e->obj))
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				if (goomba->GetState() != GOOMBA_STATE_DIE && (this->GetState() == KOOPAS_STATE_SPINNING || isBeingHeld))
				{
					mario->AddScore(x, y, 100, true);
					goomba->SetState(GOOMBA_STATE_DIE_BY_TAIL);
				}
				else
				{
					goomba->vx = -goomba->vx;
					goomba->nx = -goomba->nx;
					this->vx = -this->vx;
					this->nx = -this->nx;
				}
			}
			if (dynamic_cast<CPiranhaPlant*>(e->obj) || dynamic_cast<CFirePiranhaPlant*>(e->obj))
			{
				mario->AddScore(x, y, 100, true);
				SetState(PIRANHAPLANT_STATE_DEATH);
			}
			if (dynamic_cast<CBrick*>(e->obj))
			{
				if (mario->isAtIntroScene)
				{
					if(!((CIntroScene*)CGame::GetInstance()->GetCurrentScene())->isCustomSpeed)
						SetState(KOOPAS_STATE_IN_SHELL);
				}
				else
				{
					CBrick* object = dynamic_cast<CBrick*>(e->obj);
					//object->SetDebugAlpha(255);
					object->GetBoundingBox(oLeft, oTop, oRight, oBottom);
					if (e->ny != 0)
					{
						vy = 0;
						if (state == KOOPAS_STATE_SHELL_UP)
							vx = 0;
					}
					if (e->ny < 0)
					{
						if (tag == KOOPAS_RED && state == KOOPAS_STATE_WALKING)
						{
							//DebugOut(L"[KOOPAS] bx %f x %f nx %d vx %f\n", e->obj->x, x, this->nx, vx);
							if (this->nx > 0 && x >= e->obj->x + KOOPAS_TURN_DIFF)
							{
								if (CalTurnable(e->obj, coObjects))
								{
									this->nx = -1;
									vx = -KOOPAS_WALKING_SPEED;
								}
							}
							if (this->nx < 0 && x <= e->obj->x - KOOPAS_TURN_DIFF)
								if (CalTurnable(e->obj, coObjects))
								{
									this->nx = 1;
									vx = KOOPAS_WALKING_SPEED;
								}
						}
						if (tag == KOOPAS_GREEN_PARA)
							vy = -KOOPAS_JUMP_SPEED;
						if (dynamic_cast<CQuestionBrick*>(e->obj))
						{
							if (dynamic_cast<CQuestionBrick*>(e->obj)->isBeingPushedUp)
							{
								SetState(KOOPAS_STATE_SHELL_UP);
								this->nx = 1;
								this->vx = KOOPAS_WALKING_SPEED;
								//vy = -KOOPAS_DIE_DEFLECT_SPEED;
							}

						}
					}
					if (e->nx != 0)
					{
						if (ceil(mBottom) != oTop)
						{
							vx = -vx;
							this->nx = -this->nx;
						}
					}
				}
			}
			if (dynamic_cast<CQuestionBrick*>(e->obj) && state == KOOPAS_STATE_SPINNING && e->nx != 0 && ceil(mBottom) != oTop)
			{
				CQuestionBrick* tmp = dynamic_cast<CQuestionBrick*>(e->obj);
				if (tmp->state != QUESTIONBRICK_STATE_HIT)
					tmp->SetState(QUESTIONBRICK_STATE_HIT);
			}
			if (dynamic_cast<CBreakableBrick*>(e->obj) && state == KOOPAS_STATE_SPINNING && e->nx != 0 && ceil(mBottom) != oTop)
			{
				CBreakableBrick* tmp = dynamic_cast<CBreakableBrick*>(e->obj);
				tmp->Break();
			}
			if (dynamic_cast<CBlock*>(e->obj))
			{
				if (e->ny < 0)
				{
					vy = 0;
					if (state == KOOPAS_STATE_SHELL_UP)
						vx = 0;
					if (tag == KOOPAS_RED && state == KOOPAS_STATE_WALKING)
					{
						if (this->nx > 0 && x >= e->obj->x + KOOPAS_TURN_DIFF)
							if (CalTurnable(e->obj, coObjects))
							{
								this->nx = -1;
								vx = this->nx * KOOPAS_WALKING_SPEED;
							}
						if (this->nx < 0 && x <= e->obj->x - KOOPAS_TURN_DIFF)
							if (CalTurnable(e->obj, coObjects))
							{
								this->nx = 1;
								vx = this->nx * KOOPAS_WALKING_SPEED;
							}
					}
					if (tag == KOOPAS_GREEN_PARA)
					{
						y = e->obj->y - KOOPAS_BBOX_HEIGHT;
						vy = -KOOPAS_JUMP_SPEED;
					}

				}
				else
				{
					if(e->nx != 0)
						x = x0 + dx;
					if(state == KOOPAS_STATE_SHELL_UP && e->ny > 0)
						y = y0 + dy;
				}
				//DebugOut(L"[KOOPAS] kx: %f ky: %f bx: %f by: %f\n",x,y, e->obj->x, e->obj->y);
			}
		}
	}
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) 
		delete coEvents[i];
}
void CKoopas::Render()
{
	if (!isEnable)
		return;
	int ani = -1;
	if (state == KOOPAS_STATE_SHELL_UP || state == KOOPAS_STATE_DEATH) 
		ani = KOOPAS_ANI_SHELL_UP;
	else if (state == KOOPAS_STATE_IN_SHELL)
		ani = KOOPAS_ANI_SHELL;
	else if (state == KOOPAS_STATE_SPINNING)
	{
		if (vx < 0)
			ani = KOOPAS_ANI_SPIN_LEFT;
		else
			ani = KOOPAS_ANI_SPIN_RIGHT;
	}
	else
	{
		if (nx < 0)
			ani = KOOPAS_ANI_WALKING_LEFT;
		else
			ani = KOOPAS_ANI_WALKING_RIGHT;
	}
	if(tag == KOOPAS_GREEN_PARA || tag == KOOPAS_RED_PARA)
		if (nx < 0)
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
	switch (state)
	{
	case KOOPAS_STATE_SHELL_UP:
	{
		CMario* mario = {};
		if (!dynamic_cast<CIntroScene*> (CGame::GetInstance()->GetCurrentScene()))
			mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		else
		{
			if (((CIntroScene*)CGame::GetInstance()->GetCurrentScene())->holder == NULL)
				mario = ((CIntroScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
			else
				mario = ((CIntroScene*)CGame::GetInstance()->GetCurrentScene())->holder;
		}
		vy = -KOOPAS_SHELL_DEFLECT_SPEED;
		if (x <= mario->x)
			nx = -1;
		else
			nx = 1;
		vx = nx * KOOPAS_WALKING_SPEED;
		StartShell();
		break;
	}
	case KOOPAS_STATE_WALKING:
		if (tag == KOOPAS_GREEN_PARA)
			vx = nx * KOOPAS_PARA_WALKING_SPEED;
		if (tag == KOOPAS_RED_PARA)
		{
			vx = 0;
			vy = KOOPAS_RED_SPEED;
		}
		if(tag == KOOPAS_RED || tag == KOOPAS_GREEN)
			vx = nx * KOOPAS_WALKING_SPEED;
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
	case KOOPAS_STATE_DEATH:
		vy = -KOOPAS_DIE_DEFLECT_SPEED;
		vx = 0;
		StartDying();
		SetType(IGNORE);
		break;
	}

}
void CKoopas::Reset()
{
	x = start_x;
	y = start_y;
	nx = -1;
	SetState(KOOPAS_STATE_WALKING); 
	tag = start_tag;
	type = MOVING;
	isEnable = false;
}