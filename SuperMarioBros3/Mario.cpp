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
#include "FireBullet.h"

CMario::CMario(float x, float y) : CGameObject()
{
	level = MARIO_LEVEL_FIRE;
	untouchable = 0;
	SetState(MARIO_STATE_IDLE);
	ax = MARIO_ACCELERATION;
	ay = MARIO_GRAVITY;
	nx = 1;
	start_x = x; 
	start_y = y; 
	this->x = x; 
	this->y = y; 
}
void CMario::TimingFlag()
{
	if (GetTickCount64() - running_start > MARIO_RUNNING_STACK_TIME && isRunning)
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
	vx += ax * dt + RunningStacks*ax;
	vy += ay * dt;

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
		if (x + dx >= game->GetCamX() + game->GetScreenWidth() - 16)//Right edge
			x = game->GetCamX() + game->GetScreenWidth() - 16;
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
		//
		// Collision logic with other objects
		//
		float oLeft, oTop, oRight, oBottom;
		float mLeft, mTop, mRight, mBottom;
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			x = x0 + min_tx * dx + nx * 0.4f;
			y = y0 + min_ty * dy + ny * 0.4f;
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
			if (dynamic_cast<CGoomba*>(e->obj)) // if e->obj is Goomba 
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
					// jump on top >> kill Goomba and deflect a bit 
					if (e->ny < 0)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							if (goomba->tag != GOOMBA_RED_FLY)
							{
								goomba->SetState(GOOMBA_STATE_DIE);
									vy = -MARIO_JUMP_DEFLECT_SPEED;
							}
						}
					}
					else if (e->nx != 0)
					{
						if (isTurningTail)
						{
							goomba->SetDirection(nx);
							goomba->SetState(GOOMBA_STATE_DIE_BY_KICK);
						}
						else if (untouchable == 0)
						{
							if (goomba->GetState() != GOOMBA_STATE_DIE)
							{
								if (level > MARIO_LEVEL_SMALL)
								{
									level = MARIO_LEVEL_SMALL;
									StartUntouchable();
								}
								else
									SetState(MARIO_STATE_DIE);
							}
						}
					}
			}
			else if (dynamic_cast<CKoopas*>(e->obj)) // if e->obj is Koopas 
			{
				//DebugOut(L"nx: %f\n", e->nx);
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				if (e->ny < 0)
				{
					
					if (koopas->GetState() != KOOPAS_STATE_IN_SHELL)
					{
						koopas->SetState(KOOPAS_STATE_IN_SHELL);
						vy = -1.5f * MARIO_JUMP_DEFLECT_SPEED;
					}
					else
					{
						vy = -1.5f * MARIO_JUMP_DEFLECT_SPEED;
						koopas->SetState(KOOPAS_STATE_SPINNING);
					}
				}
				else if (e->nx != 0 || e-> ny > 0)
				{
					DebugOut(L"is Turning Tail: %d\n", isTurningTail);
					if (isTurningTail)
					{
						koopas->SetState(KOOPAS_STATE_SHELL_UP);
					}
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
						{
							if (level != MARIO_LEVEL_SMALL)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);
						}

					}

				}

			}
			else if (dynamic_cast<CBrick*>(e->obj)) 
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
					if (object->tag == QUESTION && e->ny > 0)
					{
						object->SetState(BRICK_STATE_QUESTION_HIT);
					}
				}					
			}
			else if (dynamic_cast<CBlock*>(e->obj))
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
			else if (dynamic_cast<CFireBullet*>(e->obj))
			{
				x = x0 + dx;
				y = y0 + dy;
			}
			else if (dynamic_cast<CCoin*>(e->obj))
			{	
				CCoin* coin = dynamic_cast<CCoin*>(e->obj);
				coin->isDestroyed = true;
				coin->SetIsAppear(false);
				x = x0 + dx;
				y = y0 + dy;
			}
			else if (dynamic_cast<CPortal *>(e->obj))
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
	if (nx > 0 && vy < 0)
		ani = ani_jump_up_right;
	else if (nx < 0 && vy < 0)
		ani = ani_jump_up_left;
	else if (nx > 0 && vy > 0)
		ani = ani_jump_down_right;
	else if (nx < 0 && vy > 0)
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
		if (isSitting)
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
		if (isSitting)
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
		if (isSitting)
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
		//DebugOut(L"ax:	%f, vx:	%f\n", ax, vx);
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
	deltaX = 0;
	if (level!=MARIO_LEVEL_SMALL)
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
			//if (isTurningTail)
			//{
			//		right += 6;
			//		deltaX -= 6;
			//}
			//right += 7;
			//if (nx < 0)
			//	deltaX = -7;
			//DebugOut(L"%d\n", nx);
			//left += 9;
		}	
		else if (level == MARIO_LEVEL_FIRE)
		{
			right = left + MARIO_FIRE_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
		if (state == MARIO_STATE_SITTING)
		{
			//top -= 9;
			bottom -= 9;
		}
	}
	else
	{
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
}

/*
	Reset Mario status to the beginning state of a scene
*/
void CMario::Reset()
{
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_BIG);
	//SetPosition(start_x, start_y);
	SetPosition(2267.0f, 340.0f);
	SetSpeed(0, 0);
}
void CMario::SetLevel(int l)
{
	int oldlevel = this->level;
	this->level = l;
	if (level == MARIO_LEVEL_SMALL)
	{
		y += (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT);
		//x -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT);
	}
	else if (oldlevel == MARIO_LEVEL_SMALL)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT);
		//x += (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT);
	}
}

