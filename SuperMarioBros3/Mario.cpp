#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Portal.h"

CMario::CMario(float x, float y) : CGameObject()
{
	level = MARIO_LEVEL_SMALL;
	untouchable = 0;
	WalkingStacks = 0;
	SetState(MARIO_STATE_IDLE);

	start_x = x; 
	start_y = y; 
	this->x = x; 
	this->y = y; 

	JumpingStacks = 0;
	isReadyToJump = true;
	isTouchingGround = false;
	isFalling = false;
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy += MARIO_GRAVITY*dt;

	CGame* game = CGame::GetInstance();
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state!=MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	// reset untouchable timer if untouchable time has passed
	if ( GetTickCount() - untouchable_start > MARIO_UNTOUCHABLE_TIME) 
	{
		untouchable_start = 0;
		untouchable = 0;
	}

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
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0; 
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
		//if (rdx != 0 && rdx!=dx)
		//	x += nx*abs(rdx); 

		// block every object first!
		x += min_tx*dx + nx*0.4f;
		y += min_ty*dy + ny*0.4f;

		if (nx!=0) vx = 0;
		if (ny!=0) vy = 0;

		//reset jump
		isTouchingGround = true;
		isReadyToJump = true;
		JumpingStacks = 0;
		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGoomba *>(e->obj)) // if e->obj is Goomba 
			{
				CGoomba *goomba = dynamic_cast<CGoomba *>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (e->ny < 0)
				{
					if (goomba->GetState()!= GOOMBA_STATE_DIE)
					{
						goomba->SetState(GOOMBA_STATE_DIE);
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
				}
				else if (e->nx != 0)
				{
					if (untouchable==0)
					{
						if (goomba->GetState()!=GOOMBA_STATE_DIE)
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
			} // if Goomba
			else if (dynamic_cast<CPortal *>(e->obj))
			{
				CPortal *p = dynamic_cast<CPortal *>(e->obj);
				CGame::GetInstance()->SwitchScene(p->GetSceneId());
			}
		}
	}

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CMario::Render()
{
	//DebugOut(L"Is touching ground: %d\t Walking stacks: %d\n", isTouchingGround,WalkingStacks);
	int ani = -1;
	if (state == MARIO_STATE_DIE)
		ani = MARIO_ANI_DIE;
	else
	if (level == MARIO_LEVEL_BIG)
	{
		if (vx == 0)
		{
			if (nx>0) ani = MARIO_ANI_BIG_IDLE_RIGHT;
			else ani = MARIO_ANI_BIG_IDLE_LEFT;
		}
		else if (vx > 0) 
			ani = MARIO_ANI_BIG_WALKING_RIGHT; 
		else ani = MARIO_ANI_BIG_WALKING_LEFT;
	}
	else if (level == MARIO_LEVEL_SMALL)
	{
		if (isTouchingGround == false)
		{
			if (nx > 0)
				ani = MARIO_ANI_SMALL_JUMPING_RIGHT;
			else ani = MARIO_ANI_SMALL_JUMPING_LEFT;
		}
		else
		{
			if (vx == 0)
			{
				//if (WalkingStacks > 3)
				//{
				//	if (nx > 0) ani = MARIO_ANI_SMALL_BRAKING_LEFT;
				//	else ani = MARIO_ANI_SMALL_BRAKING_RIGHT;
				//}
				//else
				//{
					if (nx > 0) ani = MARIO_ANI_SMALL_IDLE_RIGHT;
					else ani = MARIO_ANI_SMALL_IDLE_LEFT;
				//}
			}
			else if (vx > 0)
				ani = MARIO_ANI_SMALL_WALKING_RIGHT;
			else ani = MARIO_ANI_SMALL_WALKING_LEFT;
		}
	}

	int alpha = 255;
	if (untouchable) alpha = 128;

	animation_set->at(ani)->Render(x, y, alpha);

	RenderBoundingBox();
}
void CMario::SetState(int istate)
{
	//DebugOut(L"Current State: %d\n", state);
	CGameObject::SetState(istate);
	switch (istate)
	{
	case MARIO_STATE_WALKING_RIGHT:
		if (nx == -1)
		{
			if (isTouchingGround)
			{
				WalkingStacks = 0;
				vx = MARIO_WALKING_SPEED;
			}
			else
			{
				isFalling = true;
				vx = 0;
			}
		}
		else
		{
			if (!(isFalling && vx == 0))
			{
				WalkingStacks++;
				if (WalkingStacks > MARIO_SPEED_MAX_STACKS)
					WalkingStacks = MARIO_SPEED_MAX_STACKS;

				if (WalkingStacks == MARIO_SPEED_MAX_STACKS)
					vx = MARIO_WALKING_SPEED + MARIO_ACCELERATION * MARIO_SPEED_MAX_STACKS;
				else
					vx = MARIO_WALKING_SPEED;
			}
		}
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT: 
		if (nx == 1)
		{
			if (isTouchingGround)
			{
				WalkingStacks = 0;
				vx -= MARIO_WALKING_SPEED;
			}
			else
			{
				isFalling = true;
				vx = 0;
			}
			//DebugOut(L"Right to Left\n");
		}
		else
		{
			if (!(isFalling && vx == 0))
			{
				WalkingStacks++;
				if (WalkingStacks > MARIO_SPEED_MAX_STACKS)
					WalkingStacks = MARIO_SPEED_MAX_STACKS;
				if (WalkingStacks == MARIO_SPEED_MAX_STACKS)
					vx = -(MARIO_WALKING_SPEED + MARIO_ACCELERATION * MARIO_SPEED_MAX_STACKS);
				else
					vx = -MARIO_WALKING_SPEED;
			}

		}
		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		if (isTouchingGround)
		{
			JumpingStacks++;
			vy = -MARIO_JUMP_SPEED_Y;
			isTouchingGround = false;
		}
		else
		{
			if (JumpingStacks <= MARIO_JUMP_MAX_STACKS && JumpingStacks > 0 && !isFalling)
			{
				vy = -MARIO_JUMP_SPEED_Y;
				JumpingStacks++;
			}
		}
		if (JumpingStacks == MARIO_JUMP_MAX_STACKS)
		{
			isReadyToJump = false;
			isFalling = true;
		}
		break; 
	case MARIO_STATE_IDLE: 
		vx = 0;
		WalkingStacks = 0;
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	//case MARIO_STATE_BRAKE:
	//	vx = 0;
	//	break;
	}
}

void CMario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y; 

	if (level==MARIO_LEVEL_BIG)
	{
		right = x + MARIO_BIG_BBOX_WIDTH;
		bottom = y + MARIO_BIG_BBOX_HEIGHT;
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
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
}

