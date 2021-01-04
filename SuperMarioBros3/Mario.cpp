#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Brick.h"
#include "Block.h"
#include "Portal.h"
#include "Define.h"
#include "Coin.h"
#include "Leaf.h"
#include "MushRoom.h"
#include "Switch.h"
#include "QuestionBrick.h"
#include "BreakableBrick.h"
#include "FireBullet.h"
#include "PlantBullet.h"
#include "PiranhaPlant.h"
#include "FirePiranhaPlant.h"

CMario::CMario(float x, float y) : CGameObject()
{
	level = MARIO_LEVEL_SMALL;
	untouchable = 0;
	SetState(MARIO_STATE_IDLE);
	ax = MARIO_ACCELERATION;
	ay = MARIO_GRAVITY;
	nx = 1;
	start_x = x; 
	start_y = y; 
	this->x = x; 
	this->y = y; 

	RunningStacks = 0;
	money = 0;
	life = 4;
	score = 0;
}
void CMario::CalcPotentialCollisions(
	vector<LPGAMEOBJECT>* coObjects,
	vector<LPCOLLISIONEVENT>& coEvents)
{
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPGAMEOBJECT object = coObjects->at(i);
		if (dynamic_cast<CKoopas*>(object) && object->state == KOOPAS_STATE_IN_SHELL && isHolding)
			continue;
		if (dynamic_cast<CCoin*>(object) || dynamic_cast<CLeaf*>(object) 
			|| dynamic_cast<CMushRoom*>(object) || dynamic_cast<CPiece*>(object)
			|| dynamic_cast<CFireBullet*>(object) || dynamic_cast<CPlantBullet*>(object)
			|| ((dynamic_cast<CPiranhaPlant*>(object) || dynamic_cast<CFirePiranhaPlant*>(object)) 
				&& object->state == PIRANHAPLANT_STATE_INACTIVE))
			continue;
		else
		{
			LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));
			if (e->t > 0 && e->t <= 1.0f)
				coEvents.push_back(e);
			else
				delete e;
		}
	}
	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}
void CMario::TimingFlag()
{
	if (GetTickCount64() - running_start > MARIO_RUNNING_STACK_TIME && isRunning && vx != 0)
	{
		running_start = GetTickCount64();
		RunningStacks++;
		if (RunningStacks > MARIO_RUNNING_STACKS)
		{
			RunningStacks = MARIO_RUNNING_STACKS;
			vx = nx * MARIO_SPEED_MAX;
		}
	}
	if (GetTickCount64() - running_stop > MARIO_SLOW_STACK_TIME && !isRunning)
	{
		running_stop = GetTickCount64();
		RunningStacks--;
		if (RunningStacks < 0)
		{
			RunningStacks = 0;
			isRunning = false;
			isFlying = false;
		}
	}
	if (GetTickCount64() - kicking_start > MARIO_KICKING_TIME && isKicking)
		isKicking = false;

	if (GetTickCount64() - shooting_start > MARIO_SHOOTING_TIME && isShooting)
	{
		isShooting = false;
		lastshoot = GetTickCount64();
	}

	if (GetTickCount64() - turning_start > MARIO_TURNING_TAIL_TIME && isTurningTail)
		isTurningTail = false;
	// reset untouchable timer if untouchable time has passed
	if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}
	if (GetTickCount64() - flapping_start > MARIO_FLAPPING_TIME && isFlapping)
		isFlapping = false;
	if (GetTickCount64() - flapping_start <= MARIO_FLAPPING_TIME && !isFlapping)
		vy -= vy / 3;
	if (GetTickCount64() - tailflying_start > MARIO_TAIL_FLYING_TIME && isTailFlying)
	{
		isTailFlying = false;
		isRunning = false;
		isFlying = false;
	}
	if (GetTickCount64() - transforming_start > MARIO_TRANSFORMING_TIME && isTransforming)
	{
		if(level == MARIO_LEVEL_SMALL && prelevel != MARIO_LEVEL_SMALL)
			y += (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT - 1);
		isTransforming = false;
	}
}
void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);
	//slow down if change direction when running
	if (vx * ax < 0 && abs(vx) > MARIO_WALKING_SPEED_MAX && (state == MARIO_STATE_WALKING_LEFT || state == MARIO_STATE_WALKING_RIGHT))
	{
		vx = -nx * MARIO_WALKING_SPEED_MAX;
		RunningStacks = MARIO_WALKING_FAST_STACKS;
		if (RunningStacks < 0)
			RunningStacks = 0;
	}

	//update speed
	if (isTransforming)
	{
		vx = vy = 0;
	}
	else
	{
		vx += ax * dt + RunningStacks * ax;
		vy += ay * dt;
	}


	//limited the speed of mario 
	if (abs(vx) >= MARIO_WALKING_SPEED_MAX)
	{
		if(!isRunning)
			vx = nx * MARIO_WALKING_SPEED_MAX;
		else
			if (abs(vx) >= MARIO_RUNNING_SPEED_MAX && vx * ax > 0)
				if(RunningStacks < MARIO_RUNNING_STACKS)
					vx = nx * MARIO_RUNNING_SPEED_MAX;
				else
					vx = nx * MARIO_SPEED_MAX;
	}

	//if (vy <= -MARIO_JUMP_SPEED_MAX )
	//{
	//	vy = -MARIO_JUMP_SPEED_MAX;
	//	if (level == MARIO_LEVEL_TAIL && isTailFlying)
	//		ay = -MARIO_GRAVITY;
	//	else
	//		ay = MARIO_GRAVITY;
	//	isReadyToJump = false;
	//}
	TimingFlag();

	if (state == MARIO_STATE_SITTING) {
		if (vy < 0)
			vy -= MARIO_ACCELERATION_JUMP * dt;
	}
	
	if (isJumping&&isChangeDirection)
		vx = 0;

	CGame* game = CGame::GetInstance();
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state!=MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	// No collision occured, proceed normally
	if (coEvents.size()==0)
	{	
		if (x + dx >= ((CPlayScene*)game->GetCurrentScene())->GetMap()->GetMapWidth() - 16)//Right edge
			x = ((CPlayScene*)game->GetCurrentScene())->GetMap()->GetMapWidth() - 16;
		else if (x + dx <= 0)//Left edge
			x = 0;
		else
			x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty;
		int nx = 0, ny = 0;
		float rdx = 0; 
		float rdy = 0;
		
		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
		//if (rdx != 0 && rdx!=dx)
		//	x += nx*abs(rdx); 
		float x0 = x, y0 = y;
		x = x0 + min_tx * dx + nx * 0.4f;
		y = y0 + min_ty * dy + ny * 0.4f;
		//
		// Collision logic with other objects
		//
		float oLeft, oTop, oRight, oBottom;
		float mLeft, mTop, mRight, mBottom;
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
			if (e->ny != 0 && !(dynamic_cast<CBlock*>(e->obj) && ny >= 0))
			{
				isOnGround = true;
				isJumping = false;
				isChangeDirection = false;
				isReadyToSit = true;
				isFlying = false;
				vy = 0;
			}
			GetBoundingBox(mLeft, mTop, mRight, mBottom);
			e->obj->GetBoundingBox(oLeft, oTop, oRight, oBottom);
			if (dynamic_cast<CSwitch*>(e->obj))
			{
				CSwitch* sw = dynamic_cast<CSwitch*>(e->obj);
				if (e->ny > 0)
				{
					//vy = -MARIO_JUMP_SPEED_MAX;
					ay = MARIO_GRAVITY;
					isReadyToJump = false;
				}
				else if (e->ny < 0)
				{
					vy = 0;
					if(sw->state != SWITCH_STATE_PRESSED)
						sw->SetState(SWITCH_STATE_PRESSED);
				}
				if (e->nx != 0)
				{
					if (ceil(mBottom) != oTop)
						vx = 0;
				}
			}
			if (dynamic_cast<CGoomba*>(e->obj)) // if e->obj is Goomba 
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
					// jump on top >> kill Goomba and deflect a bit 
					if (e->ny < 0)
					{
						AddScore();
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							if (goomba->tag != GOOMBA_RED)
							{
								goomba->SetState(GOOMBA_STATE_DIE);
									vy = -MARIO_JUMP_DEFLECT_SPEED;
							}
							else
							{
								goomba->SetTag(GOOMBA_RED_NORMAL);
								goomba->SetState(GOOMBA_STATE_WALKING);
								//goomba->SetPosition(goomba->x, goomba->y);
								vy = -MARIO_JUMP_DEFLECT_SPEED;
							}
						}
					}
					else if (e->nx != 0)
					{
						if (isTurningTail)
						{
							AddScore();
							goomba->SetDirection(nx);
							goomba->SetState(GOOMBA_STATE_DIE_BY_TAIL);
						}
						else if (untouchable == 0)
						{
							if (goomba->GetState() != GOOMBA_STATE_DIE)
							{
								x = x0;
								y = y0;
								Attacked();
							}
						}
						else
						{
							x = x0;
							y = y0;
						}
					}
			}
			if (dynamic_cast<CKoopas*>(e->obj)) // if e->obj is Koopas 
			{
				//DebugOut(L"nx: %f\n", e->nx);
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				if (e->ny < 0)
				{
					AddScore();
					vy = -1.5f * MARIO_JUMP_DEFLECT_SPEED;
					if (koopas->GetState() != KOOPAS_STATE_IN_SHELL && koopas->GetState() != KOOPAS_STATE_SHELL_UP)
						koopas->SetState(KOOPAS_STATE_IN_SHELL);
					else
						koopas->SetState(KOOPAS_STATE_SPINNING);
				}
				else if (e->nx != 0 || e-> ny > 0)
				{
					if (isTurningTail)
						koopas->SetState(KOOPAS_STATE_SHELL_UP);
					else if (koopas->GetState() == KOOPAS_STATE_IN_SHELL || koopas->GetState() == KOOPAS_STATE_SHELL_UP)
					{
						if (isReadyToHold)
						{
							koopas->SetIsBeingHeld(true);
							isHolding = true;
						}
						else
						{
							StartKicking();
							koopas->nx = this->nx;
							koopas->SetState(KOOPAS_STATE_SPINNING);
						}
					}
					else if (untouchable == 0 && isKicking == false)
					{
						if (koopas->GetState() != KOOPAS_STATE_IN_SHELL || koopas->GetState() != KOOPAS_STATE_SHELL_UP)
							Attacked();

					}

				}

			}
			if (dynamic_cast<CPiranhaPlant*>(e->obj) || dynamic_cast<CFirePiranhaPlant*>(e->obj))
			{
				if (isTurningTail && e->nx != 0)
				{
					e->obj->SetState(PIRANHAPLANT_STATE_DEATH);
					AddScore();
				}
				else
				{
					x = x0;
					y = y0;
					if (untouchable == 0)
						Attacked();
				}			
			}
			if (dynamic_cast<CBrick*>(e->obj)) 
			{
				CBrick* object = dynamic_cast<CBrick*>(e->obj);
				//object->SetDebugAlpha(255);
				if (object->tag == PLATFORM)
				{
					if (e->ny != 0)
					{
						vy = 0;
					}
					if (e->nx != 0)
					{
						if (ceil(mBottom) != oTop)
							vx = 0;
					}
				}
				else
				{
					//if (e->ny > 0) ay = MARIO_GRAVITY;
					//else if (e->ny < 0)
					//	vy = -vy;
					if (e->ny > 0)
					{
						//vy = -MARIO_JUMP_SPEED_MAX;
						ay = MARIO_GRAVITY;
						isReadyToJump = false;
					}
					else if (e->ny < 0)
						vy = 0;
					if (e->nx != 0)
					{
						if (ceil(mBottom) != oTop)
							vx = 0;
					}
				}					
			}
			if (dynamic_cast<CQuestionBrick*>(e->obj))
			{
				if (e->ny > 0)
					e->obj->SetState(QUESTIONBRICK_STATE_HIT);
			}
			if (dynamic_cast<CBreakableBrick*>(e->obj))
			{
				if (e->ny > 0)
				{
					CBreakableBrick* object = dynamic_cast<CBreakableBrick*>(e->obj);
					object->Break();
				}
			}
			if (dynamic_cast<CBlock*>(e->obj))
			{
				//DebugOut(L"Tag: %d\n", tag);
				CBlock* block = dynamic_cast<CBlock*>(e->obj);
				x = x0 + dx;
				if (ny < 0)
					vy = 0;
				else
				{
					y = y0 + dy;
				}
			}
			if (dynamic_cast<CPortal *>(e->obj))
			{
				CPortal *p = dynamic_cast<CPortal *>(e->obj);
				CGame::GetInstance()->SwitchScene(p->GetSceneId());
			}
		}
	}
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++)
	{
		//coEvents[i]->obj->SetDebugAlpha(coEvents[i]->obj->DebugAlpha - 50);
		delete coEvents[i];
	}
	if (vy < 0)
	{
		isOnGround = false;
	}
	//DebugOut(L"dy: %f\ty: %f\n",dy, y);
}
void CMario::BasicRenderLogicsForAllLevel(int& ani, int ani_jump_down_right, int ani_jump_down_left,
	int ani_idle_right, int ani_idle_left,
	int ani_baking_right, int ani_baking_left, int ani_walking_right, int ani_walking_left, int ani_kicking_right, int ani_kicking_left)
{
	if (vx == 0) 
	{
		if (isOnGround && vy >= 0)
		{
			if (nx > 0) ani = ani_idle_right;
				else ani = ani_idle_left;
			if (isKicking)
				if (nx > 0) ani = ani_kicking_right;
				else ani = ani_kicking_left;
		}
		else
		{
			if (nx > 0) ani = ani_jump_down_right;
			else ani = ani_jump_down_left;
		}
	}
	else {
		if (vx > 0 && nx < 0) {
			ani = ani_baking_right;
		}
		else if (vx < 0 && nx >0) {
			ani = ani_baking_left;
		}
		else if (vx > 0 && nx > 0) {
			ani = ani_walking_right;
		}
		else if (vx < 0 && nx < 0) {
			ani = ani_walking_left;
		}
		if (isKicking)
		{
			if (vx > 0)
				ani = ani_kicking_right;
			else if (vx < 0)
				ani = ani_kicking_left;
			//DebugOut(L"[KICK]\n");
		}	
	}
	if (isShooting)
	{
		if (nx > 0)
			ani = MARIO_ANI_SHOOTING_RIGHT;
		else if (nx < 0)
			ani = MARIO_ANI_SHOOTING_LEFT;
	}
}

void CMario::RenderSitting(int& ani, int ani_sit_right, int ani_sit_left)
{
	if (nx > 0)
		ani = ani_sit_right;
	else
		ani = ani_sit_left;
}
void CMario::RenderJumping(int& ani, int ani_jump_up_right, int ani_jump_up_left, int ani_jump_down_right, int ani_jump_down_left)
{
	if (nx > 0 && vy <= 0)
		ani = ani_jump_up_right;
	else if (nx < 0 && vy <= 0)
		ani = ani_jump_up_left;
	else if (nx > 0 && vy >= 0)
		ani = ani_jump_down_right;
	else if (nx < 0 && vy >= 0)
		ani = ani_jump_down_left;
	if (isShooting)
	{
		if (nx > 0)
			ani = MARIO_ANI_SHOOTING_JUMP_RIGHT;
		else
			ani = MARIO_ANI_SHOOTING_JUMP_LEFT;
	}
}
void CMario::Render()
{
	int ani = -1;
	int alpha = 255;
	if (isTransforming)
	{
		if (transformState)
			ani = MARIO_ANI_TRANSFORM_BANG;
		else
		{
			if (nx == 1)
				ani = MARIO_ANI_TRANSFORM_SMALL_RIGHT;
			else
				ani = MARIO_ANI_TRANSFORM_SMALL_LEFT;
		}

	}
	else
	{
		if (state == MARIO_STATE_DIE)
			ani = MARIO_ANI_DIE;
		else if (level == MARIO_LEVEL_SMALL)
		{
			if (isJumping)
			{
				if (!isHolding)
				{
					if (isFlying)
					{
						RenderJumping(ani,
							MARIO_ANI_SMALL_FLY_RIGHT,
							MARIO_ANI_SMALL_FLY_LEFT,
							MARIO_ANI_SMALL_FLY_RIGHT,
							MARIO_ANI_SMALL_FLY_LEFT);
					}
					else
						RenderJumping(ani,
							MARIO_ANI_SMALL_JUMPINGUP_RIGHT,
							MARIO_ANI_SMALL_JUMPINGUP_LEFT,
							MARIO_ANI_SMALL_JUMPINGDOWN_RIGHT,
							MARIO_ANI_SMALL_JUMPINGDOWN_LEFT);
				}
				else
					RenderJumping(ani,
						MARIO_ANI_SMALL_HOLD_JUMPINGUP_RIGHT,
						MARIO_ANI_SMALL_HOLD_JUMPINGUP_LEFT,
						MARIO_ANI_SMALL_HOLD_JUMPINGDOWN_RIGHT,
						MARIO_ANI_SMALL_HOLD_JUMPINGDOWN_LEFT);
			}
			else
			{
				if (!isHolding)
				{
					int ani_go_right = MARIO_ANI_SMALL_WALKING_RIGHT;
					int ani_go_left = MARIO_ANI_SMALL_WALKING_LEFT;
					if (isRunning)
						if (abs(vx) >= MARIO_RUNNING_SPEED_MAX)
						{
							ani_go_right = MARIO_ANI_SMALL_WALKING_FAST_RIGHT;
							ani_go_left = MARIO_ANI_SMALL_WALKING_FAST_LEFT;
						}
					if (RunningStacks == MARIO_RUNNING_STACKS)
					{
						ani_go_right = MARIO_ANI_SMALL_RUNNING_RIGHT;
						ani_go_left = MARIO_ANI_SMALL_RUNNING_LEFT;
					}
					BasicRenderLogicsForAllLevel(ani,
						MARIO_ANI_SMALL_JUMPINGDOWN_RIGHT, MARIO_ANI_SMALL_JUMPINGDOWN_LEFT,
						MARIO_ANI_SMALL_IDLE_RIGHT, MARIO_ANI_SMALL_IDLE_LEFT,
						MARIO_ANI_SMALL_BRAKING_RIGHT, MARIO_ANI_SMALL_BRAKING_LEFT,
						ani_go_right, ani_go_left, MARIO_ANI_SMALL_KICKING_RIGHT, MARIO_ANI_SMALL_KICKING_LEFT);
				}
				else
					BasicRenderLogicsForAllLevel(ani,
						MARIO_ANI_SMALL_HOLD_JUMPINGDOWN_RIGHT, MARIO_ANI_SMALL_HOLD_JUMPINGDOWN_LEFT,
						MARIO_ANI_SMALL_HOLD_IDLE_RIGHT, MARIO_ANI_SMALL_HOLD_IDLE_LEFT,
						MARIO_ANI_SMALL_HOLD_BRAKING_RIGHT, MARIO_ANI_SMALL_HOLD_BRAKING_LEFT,
						MARIO_ANI_SMALL_HOLD_WALKING_RIGHT, MARIO_ANI_SMALL_HOLD_WALKING_LEFT, MARIO_ANI_SMALL_KICKING_RIGHT, MARIO_ANI_SMALL_KICKING_LEFT);
			}
		}
		else if (level == MARIO_LEVEL_BIG)
		{
			if (state == MARIO_STATE_SITTING)
			{
				RenderSitting(ani,
					MARIO_ANI_BIG_SITTING_RIGHT,
					MARIO_ANI_BIG_SITTING_LEFT);
			}
			else if (isJumping)
			{
				if (!isHolding)
				{
					if (isFlying)
					{
						RenderJumping(ani,
							MARIO_ANI_BIG_FLY_RIGHT,
							MARIO_ANI_BIG_FLY_LEFT,
							MARIO_ANI_BIG_FLY_RIGHT,
							MARIO_ANI_BIG_FLY_LEFT);
					}
					else
						RenderJumping(ani,
							MARIO_ANI_BIG_JUMPINGUP_RIGHT,
							MARIO_ANI_BIG_JUMPINGUP_LEFT,
							MARIO_ANI_BIG_JUMPINGDOWN_RIGHT,
							MARIO_ANI_BIG_JUMPINGDOWN_LEFT);
				}
				else
					RenderJumping(ani,
						MARIO_ANI_BIG_HOLD_JUMPINGUP_RIGHT,
						MARIO_ANI_BIG_HOLD_JUMPINGUP_LEFT,
						MARIO_ANI_BIG_HOLD_JUMPINGDOWN_RIGHT,
						MARIO_ANI_BIG_HOLD_JUMPINGDOWN_LEFT);
			}
			else
			{
				if (!isHolding)
				{
					int ani_go_right = MARIO_ANI_BIG_WALKING_RIGHT;
					int ani_go_left = MARIO_ANI_BIG_WALKING_LEFT;
					if (isRunning)
						if (abs(vx) >= MARIO_RUNNING_SPEED_MAX)
						{
							ani_go_right = MARIO_ANI_BIG_WALKING_FAST_RIGHT;
							ani_go_left = MARIO_ANI_BIG_WALKING_FAST_LEFT;
						}
					if (abs(vx) >= MARIO_SPEED_MAX)
					{
						ani_go_right = MARIO_ANI_BIG_RUNNING_RIGHT;
						ani_go_left = MARIO_ANI_BIG_RUNNING_LEFT;
					}
					BasicRenderLogicsForAllLevel(ani,
						MARIO_ANI_BIG_JUMPINGDOWN_RIGHT, MARIO_ANI_BIG_JUMPINGDOWN_LEFT,
						MARIO_ANI_BIG_IDLE_RIGHT, MARIO_ANI_BIG_IDLE_LEFT,
						MARIO_ANI_BIG_BRAKING_RIGHT, MARIO_ANI_BIG_BRAKING_LEFT,
						ani_go_right, ani_go_left, MARIO_ANI_BIG_KICKING_RIGHT, MARIO_ANI_BIG_KICKING_LEFT);
				}
				else
					BasicRenderLogicsForAllLevel(ani,
						MARIO_ANI_BIG_HOLD_JUMPINGDOWN_RIGHT, MARIO_ANI_BIG_HOLD_JUMPINGDOWN_LEFT,
						MARIO_ANI_BIG_HOLD_IDLE_RIGHT, MARIO_ANI_BIG_HOLD_IDLE_LEFT,
						MARIO_ANI_BIG_HOLD_BRAKING_RIGHT, MARIO_ANI_BIG_HOLD_BRAKING_LEFT,
						MARIO_ANI_BIG_HOLD_WALKING_RIGHT, MARIO_ANI_BIG_HOLD_WALKING_LEFT, MARIO_ANI_BIG_KICKING_RIGHT, MARIO_ANI_BIG_KICKING_LEFT);
			}
		}
		else if (level == MARIO_LEVEL_TAIL)
		{
			if (state == MARIO_STATE_SITTING)
			{
				RenderSitting(ani,
					MARIO_ANI_TAIL_SITTING_RIGHT,
					MARIO_ANI_TAIL_SITTING_LEFT);
			}
			else if (isJumping)
			{
				if (!isHolding)
				{
					if (isFlying)
					{
						RenderJumping(ani,
							MARIO_ANI_TAIL_FLY_UP_RIGHT,
							MARIO_ANI_TAIL_FLY_UP_LEFT,
							MARIO_ANI_TAIL_FLY_DOWN_RIGHT,
							MARIO_ANI_TAIL_FLY_DOWN_LEFT);
					}
					else
						RenderJumping(ani,
							MARIO_ANI_TAIL_FLY_UP_RIGHT,
							MARIO_ANI_TAIL_FLY_UP_LEFT,
							MARIO_ANI_TAIL_FLY_DOWN_RIGHT,
							MARIO_ANI_TAIL_FLY_DOWN_LEFT);
				}
				else
					RenderJumping(ani,
						MARIO_ANI_TAIL_HOLD_JUMPINGUP_RIGHT,
						MARIO_ANI_TAIL_HOLD_JUMPINGUP_LEFT,
						MARIO_ANI_TAIL_HOLD_JUMPINGDOWN_RIGHT,
						MARIO_ANI_TAIL_HOLD_JUMPINGDOWN_LEFT);
				if (!isFlying)
				{
					if (nx > 0)
						ani = MARIO_ANI_TAIL_FLAPPING_RIGHT;
					if (nx < 0)
						ani = MARIO_ANI_TAIL_FLAPPING_LEFT;
				}
				else
				{
					if (nx > 0)
						ani = MARIO_ANI_TAIL_FLY_FLAPPING_RIGHT;
					if (nx < 0)
						ani = MARIO_ANI_TAIL_FLY_FLAPPING_LEFT;
				}
			}
			else
			{
				if (!isHolding)
				{
					int ani_go_right = MARIO_ANI_TAIL_WALKING_RIGHT;
					int ani_go_left = MARIO_ANI_TAIL_WALKING_LEFT;
					if (isRunning)
						if (abs(vx) >= MARIO_RUNNING_SPEED_MAX)
						{
							ani_go_right = MARIO_ANI_TAIL_WALKING_FAST_RIGHT;
							ani_go_left = MARIO_ANI_TAIL_WALKING_FAST_LEFT;
						}
					if (abs(vx) >= MARIO_SPEED_MAX)
					{
						ani_go_right = MARIO_ANI_TAIL_RUNNING_RIGHT;
						ani_go_left = MARIO_ANI_TAIL_RUNNING_LEFT;
					}
					BasicRenderLogicsForAllLevel(ani,
						MARIO_ANI_TAIL_JUMPINGDOWN_RIGHT, MARIO_ANI_TAIL_JUMPINGDOWN_LEFT,
						MARIO_ANI_TAIL_IDLE_RIGHT, MARIO_ANI_TAIL_IDLE_LEFT,
						MARIO_ANI_TAIL_BRAKING_RIGHT, MARIO_ANI_TAIL_BRAKING_LEFT,
						ani_go_right, ani_go_left, MARIO_ANI_TAIL_KICKING_RIGHT, MARIO_ANI_TAIL_KICKING_LEFT);
				}
				else
					BasicRenderLogicsForAllLevel(ani,
						MARIO_ANI_TAIL_HOLD_JUMPINGDOWN_RIGHT, MARIO_ANI_TAIL_HOLD_JUMPINGDOWN_LEFT,
						MARIO_ANI_TAIL_HOLD_IDLE_RIGHT, MARIO_ANI_TAIL_HOLD_IDLE_LEFT,
						MARIO_ANI_TAIL_HOLD_BRAKING_RIGHT, MARIO_ANI_TAIL_HOLD_BRAKING_LEFT,
						MARIO_ANI_TAIL_HOLD_WALKING_RIGHT, MARIO_ANI_TAIL_HOLD_WALKING_LEFT, MARIO_ANI_TAIL_KICKING_RIGHT, MARIO_ANI_TAIL_KICKING_LEFT);
			}
			if (isTurningTail)
			{
				if (nx > 0)
					ani = MARIO_ANI_TAIL_TURNING_RIGHT;
				else if (nx < 0)
					ani = MARIO_ANI_TAIL_TURNING_LEFT;
			}
		}
		else if (level == MARIO_LEVEL_FIRE)
		{
			if (state == MARIO_STATE_SITTING)
			{
				RenderSitting(ani,
					MARIO_ANI_FIRE_SITTING_RIGHT,
					MARIO_ANI_FIRE_SITTING_LEFT);
			}
			else if (isJumping)
			{
				if (!isHolding)
				{
					if (isFlying)
					{
						RenderJumping(ani,
							MARIO_ANI_FIRE_FLY_RIGHT,
							MARIO_ANI_FIRE_FLY_LEFT,
							MARIO_ANI_FIRE_FLY_RIGHT,
							MARIO_ANI_FIRE_FLY_LEFT);
					}
					else
						RenderJumping(ani,
							MARIO_ANI_FIRE_JUMPINGUP_RIGHT,
							MARIO_ANI_FIRE_JUMPINGUP_LEFT,
							MARIO_ANI_FIRE_JUMPINGDOWN_RIGHT,
							MARIO_ANI_FIRE_JUMPINGDOWN_LEFT);
				}
				else
					RenderJumping(ani,
						MARIO_ANI_FIRE_HOLD_JUMPINGUP_RIGHT,
						MARIO_ANI_FIRE_HOLD_JUMPINGUP_LEFT,
						MARIO_ANI_FIRE_HOLD_JUMPINGDOWN_RIGHT,
						MARIO_ANI_FIRE_HOLD_JUMPINGDOWN_LEFT);
			}
			else
			{
				if (!isHolding)
				{
					int ani_go_right = MARIO_ANI_FIRE_WALKING_RIGHT;
					int ani_go_left = MARIO_ANI_FIRE_WALKING_LEFT;
					if (isRunning)
						if (abs(vx) >= MARIO_RUNNING_SPEED_MAX)
						{
							ani_go_right = MARIO_ANI_FIRE_WALKING_FAST_RIGHT;
							ani_go_left = MARIO_ANI_FIRE_WALKING_FAST_LEFT;
						}
					if (abs(vx) >= MARIO_SPEED_MAX)
					{
						ani_go_right = MARIO_ANI_FIRE_RUNNING_RIGHT;
						ani_go_left = MARIO_ANI_FIRE_RUNNING_LEFT;
					}
					BasicRenderLogicsForAllLevel(ani,
						MARIO_ANI_FIRE_JUMPINGDOWN_RIGHT, MARIO_ANI_FIRE_JUMPINGDOWN_LEFT,
						MARIO_ANI_FIRE_IDLE_RIGHT, MARIO_ANI_FIRE_IDLE_LEFT,
						MARIO_ANI_FIRE_BRAKING_RIGHT, MARIO_ANI_FIRE_BRAKING_LEFT,
						ani_go_right, ani_go_left, MARIO_ANI_FIRE_KICKING_RIGHT, MARIO_ANI_FIRE_KICKING_LEFT);
				}
				else
					BasicRenderLogicsForAllLevel(ani,
						MARIO_ANI_FIRE_HOLD_JUMPINGDOWN_RIGHT, MARIO_ANI_FIRE_HOLD_JUMPINGDOWN_LEFT,
						MARIO_ANI_FIRE_HOLD_IDLE_RIGHT, MARIO_ANI_FIRE_HOLD_IDLE_LEFT,
						MARIO_ANI_FIRE_HOLD_BRAKING_RIGHT, MARIO_ANI_FIRE_HOLD_BRAKING_LEFT,
						MARIO_ANI_FIRE_HOLD_WALKING_RIGHT, MARIO_ANI_FIRE_HOLD_WALKING_LEFT, MARIO_ANI_FIRE_KICKING_RIGHT, MARIO_ANI_FIRE_KICKING_LEFT);
			}
		}
	}

	if (untouchable) alpha = 128;
	animation_set->at(ani)->Render(x, y, alpha);

	//DebugAlpha = 128;
	RenderBoundingBox();
}
void CMario::SetState(int state)
{
	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		if (ax < 0 && vy > 0)
		{
			isChangeDirection = true;
		}
		ax = MARIO_ACCELERATION;
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		if (ax > 0 && vy > 0)
			isChangeDirection = true;
		ax = -MARIO_ACCELERATION;
		nx = -1;
		break;
	case MARIO_STATE_JUMPING:
		if (vy == 0)
		{
			vy = -MARIO_JUMP_SPEED_MIN;
		}
		ay = -MARIO_ACCELERATION_JUMP;
		if (RunningStacks == MARIO_RUNNING_STACKS)
		{
			isFlying = true;
		}
		break;
	case MARIO_STATE_IDLE:
		if(vx > 0)
			ax = -MARIO_ACCELERATION;
		if (vx < 0)
			ax = MARIO_ACCELERATION;
		ay = MARIO_GRAVITY;
		if (abs(vx) <= MARIO_WALKING_SPEED_MIN)
		{
			vx = 0;
			ax = 0;
		}
		if (isRunning)
			StopRunning();
		break;
	case MARIO_STATE_SITTING:
		if (level != MARIO_LEVEL_SMALL)
		{
			if (abs(vx) <= MARIO_WALKING_SPEED_MIN) {
				vx = 0;
			}
			ay = MARIO_GRAVITY;
			ax = -nx* MARIO_ACCELERATION;
			isSitting = true;
		}
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		vx = -nx * MARIO_DIE_DEFLECT_SPEED;
		break;
	}
	if (state == MARIO_STATE_SITTING && this->state != MARIO_STATE_SITTING)
		y += 9;
	if (state != MARIO_STATE_SITTING && this->state == MARIO_STATE_SITTING)
		y -= 9;
	CGameObject::SetState(state);
}

void CMario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y; 

	if (level != MARIO_LEVEL_SMALL)
	{
		if (level == MARIO_LEVEL_BIG)
		{
			right = left + MARIO_BIG_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
		else if (level == MARIO_LEVEL_TAIL)
		{
			right = left + MARIO_TAIL_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
		else if (level == MARIO_LEVEL_FIRE)
		{
			right = left + MARIO_FIRE_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
		if (state == MARIO_STATE_SITTING)
		{
			bottom -= MARIO_BIG_BBOX_HEIGHT - MARIO_BIG_BBOX_SITTING_HEIGHT;
		}
	}
	else
	{
		if (isTransforming)
		{
			right = left + MARIO_BIG_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
		else
		{
			right = left + MARIO_SMALL_BBOX_WIDTH;
			bottom = top + MARIO_SMALL_BBOX_HEIGHT;
		}
	}
}

/*
	Reset Mario status to the beginning state of a scene
*/
void CMario::Reset()
{
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_BIG);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
}
void CMario::SetLevel(int l)
{
	transformState = false;

	prelevel = level;
	level = l;
	if (level != prelevel)
	{
		if (level == MARIO_LEVEL_BIG || level == MARIO_LEVEL_TAIL || level == MARIO_LEVEL_FIRE)
		{
			if (prelevel == MARIO_LEVEL_SMALL)
				y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT + 1);
			if (level == MARIO_LEVEL_BIG)
			{
				if (prelevel != MARIO_LEVEL_SMALL)
					transformState = true;
			}
			if (level == MARIO_LEVEL_TAIL || level == MARIO_LEVEL_FIRE)
				transformState = true;
		}
		StartTransforming();
	}
}

