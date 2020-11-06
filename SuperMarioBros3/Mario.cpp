#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Brick.h"
#include "Block.h"
#include "Portal.h"
bool isChangeDirection = false;
int preState = -1;
CMario::CMario(float x, float y) : CGameObject()
{
	level = MARIO_LEVEL_BIG;
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

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);

	vx += ax * dt;
	// Simple fall down
	vy += ay * dt;
	//limited the speed of mario 
	if (abs(vx) >= MARIO_WALKING_SPEED_MAX)
	{
		vx = nx * MARIO_WALKING_SPEED_MAX;
		//DebugOut(L"change %f\n");
	}
	if (vy < 0)
	{
		if (vy <= -MARIO_JUMP_SPEED_MAX)
		{
			vy = -MARIO_JUMP_SPEED_MAX;
			ay = MARIO_GRAVITY;
			isReadyToJump = false;
		}
	}


	if (state == MARIO_STATE_IDLE)//slow down
	{
		ax = -nx * MARIO_ACCELERATION;
		ay = MARIO_GRAVITY;
		if (abs(vx) <= MARIO_WALKING_SPEED_MIN)
			vx = 0;
	}

	if (state == MARIO_STATE_SIT) {
		if (vy < 0)
			vy -= MARIO_ACCELERATION_JUMP * dt;
	}
	if (isJumping&&isChangeDirection)
	{
		vx = 0;
	}
	for (int i = 0; i < coObjects->size(); i++)
	{
		LPGAMEOBJECT obj = coObjects->at(i);
		float pLeft, pTop, pRight, pBottom;
		obj->GetBoundingBox(pLeft, pTop, pRight, pBottom);
		if (dynamic_cast<CBrick*>(obj))
		{
			if (CheckBoundingBox(pLeft, pTop, pRight, pBottom))
			{
				y -= y + MARIO_BIG_BBOX_HEIGHT - pTop + 1.0f;
			}
		}
	}
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


		//reset jump
		isOnGround = true;
		isJumping = false;
		isChangeDirection = false;
		isReadyToSit = true;
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
			}
			else if (dynamic_cast<CBrick*>(e->obj)) 
			{
				x += min_tx * dx + nx * 0.8f;
				y += min_ty * dy + ny * 0.4f;
				if (e->ny != 0) vy = 0;
				//if (e->nx != 0) vx = 0;
			//	DebugOut(L"e->nx: %f\n", e->nx);
			}
			else if (dynamic_cast<CBlock*>(e->obj))
			{
				CBlock* block = dynamic_cast<CBlock*>(e->obj);
				x += dx;
				if (ny < 0)
				{
					y += min_ty * dy + ny * 0.4f;
					vy = 0;
				}
				else
				{
					y += dy;
				}
			}
			else if (dynamic_cast<CPortal *>(e->obj))
			{
				CPortal *p = dynamic_cast<CPortal *>(e->obj);
				CGame::GetInstance()->SwitchScene(p->GetSceneId());
			}
		}
	}

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	//DebugOut(L"x:	%f, y		%f:\n", x, y);
}
void CMario::BasicRenderLogicsForAllLevel(int& ani, int ani_jump_down_right, int ani_jump_down_left,
	int ani_idle_right, int ani_idle_left,
	int ani_baking_right, int ani_baking_left, int ani_walking_right, int ani_walking_left)
{
	if (vx == 0) {
		if (isOnGround && vy >= 0)
		{
			if (nx > 0) ani = ani_idle_right;
			else ani = ani_idle_left;
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
	//DebugOut(L"nx: %d\n", nx);
	if (nx > 0 && vy < 0)
		ani = ani_jump_up_right;
	else if (nx < 0 && vy < 0)
		ani = ani_jump_up_left;
	else if (nx > 0 && vy > 0)
		ani = ani_jump_down_right;
	else if (nx < 0 && vy > 0)
		ani = ani_jump_down_left;
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
			RenderJumping(ani,
				MARIO_ANI_SMALL_JUMPINGUP_RIGHT,
				MARIO_ANI_SMALL_JUMPINGUP_LEFT,
				MARIO_ANI_SMALL_JUMPINGDOWN_RIGHT,
				MARIO_ANI_SMALL_JUMPINGDOWN_LEFT);
		}
		else
		{
			BasicRenderLogicsForAllLevel(ani,
				MARIO_ANI_SMALL_JUMPINGDOWN_RIGHT, MARIO_ANI_SMALL_JUMPINGDOWN_LEFT,
				MARIO_ANI_SMALL_IDLE_RIGHT, MARIO_ANI_SMALL_IDLE_LEFT,
				MARIO_ANI_SMALL_BRAKING_RIGHT, MARIO_ANI_SMALL_BRAKING_LEFT,
				MARIO_ANI_SMALL_WALKING_RIGHT, MARIO_ANI_SMALL_WALKING_LEFT);
		}
	}
	else if (level == MARIO_LEVEL_BIG) 
	{
		if (state == MARIO_STATE_SIT) 
		{
			RenderSitting(ani,
				MARIO_ANI_BIG_SITTING_RIGHT, 
				MARIO_ANI_BIG_SITTING_LEFT);
		}
		else if (isJumping) 
		{
			RenderJumping(ani,
				MARIO_ANI_BIG_JUMPINGUP_RIGHT, 
				MARIO_ANI_BIG_JUMPINGUP_LEFT,
				MARIO_ANI_BIG_JUMPINGDOWN_RIGHT,
				MARIO_ANI_BIG_JUMPINGDOWN_LEFT);
		}
		else 
		{
				BasicRenderLogicsForAllLevel(ani,
				MARIO_ANI_BIG_JUMPINGDOWN_RIGHT, MARIO_ANI_BIG_JUMPINGDOWN_LEFT,
				MARIO_ANI_BIG_IDLE_RIGHT, MARIO_ANI_BIG_IDLE_LEFT,
				MARIO_ANI_BIG_BRAKING_RIGHT, MARIO_ANI_BIG_BRAKING_LEFT,
				MARIO_ANI_BIG_WALKING_RIGHT, MARIO_ANI_BIG_WALKING_LEFT);
		}
	}
	else if (level == MARIO_LEVEL_TAIL)
	{
		if (state == MARIO_STATE_SIT)
		{
			RenderSitting(ani,
				MARIO_ANI_TAIL_SITTING_RIGHT,
				MARIO_ANI_TAIL_SITTING_LEFT);
		}
		else if (isJumping)
		{
			RenderJumping(ani,
				MARIO_ANI_TAIL_JUMPINGUP_RIGHT,
				MARIO_ANI_TAIL_JUMPINGUP_LEFT,
				MARIO_ANI_TAIL_JUMPINGDOWN_RIGHT,
				MARIO_ANI_TAIL_JUMPINGDOWN_LEFT);
		}
		else
		{
			BasicRenderLogicsForAllLevel(ani,
				MARIO_ANI_TAIL_JUMPINGDOWN_RIGHT, MARIO_ANI_TAIL_JUMPINGDOWN_LEFT,
				MARIO_ANI_TAIL_IDLE_RIGHT, MARIO_ANI_TAIL_IDLE_LEFT,
				MARIO_ANI_TAIL_BRAKING_RIGHT, MARIO_ANI_TAIL_BRAKING_LEFT,
				MARIO_ANI_TAIL_WALKING_RIGHT, MARIO_ANI_TAIL_WALKING_LEFT);
		}
	}
	else if (level == MARIO_LEVEL_FIRE)
	{
		if (state == MARIO_STATE_SIT)
		{
			RenderSitting(ani,
				MARIO_ANI_FIRE_SITTING_RIGHT,
				MARIO_ANI_FIRE_SITTING_LEFT);
		}
		else if (isJumping)
		{
			RenderJumping(ani,
				MARIO_ANI_FIRE_JUMPINGUP_RIGHT,
				MARIO_ANI_FIRE_JUMPINGUP_LEFT,
				MARIO_ANI_FIRE_JUMPINGDOWN_RIGHT,
				MARIO_ANI_FIRE_JUMPINGDOWN_LEFT);
		}
		else
		{
			BasicRenderLogicsForAllLevel(ani,
				MARIO_ANI_FIRE_JUMPINGDOWN_RIGHT, MARIO_ANI_FIRE_JUMPINGDOWN_LEFT,
				MARIO_ANI_FIRE_IDLE_RIGHT, MARIO_ANI_FIRE_IDLE_LEFT,
				MARIO_ANI_FIRE_BRAKING_RIGHT, MARIO_ANI_FIRE_BRAKING_LEFT,
				MARIO_ANI_FIRE_WALKING_RIGHT, MARIO_ANI_FIRE_WALKING_LEFT);
		}
	}


	if (untouchable) alpha = 128;

	animation_set->at(ani)->Render(x, y, alpha);

	RenderBoundingBox(128);
}
void CMario::SetState(int state)
{
	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		if (ax < 0 && vy > 0)
		{
			isChangeDirection = true;
			//DebugOut(L"Ischange: %f\n",ay);
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
	case MARIO_STATE_JUMP:
		if (vy == 0)
			vy = -MARIO_JUMP_SPEED_MIN;
		ay = -MARIO_ACCELERATION_JUMP;
		break;
	case MARIO_STATE_IDLE:
		break;
	case MARIO_STATE_SIT:
		if (level != MARIO_LEVEL_SMALL)
		{
			if (abs(vx) <= MARIO_WALKING_SPEED_MIN) {
				vx = 0;
			}
			ay = MARIO_GRAVITY;
			ax = -nx* MARIO_ACCELERATION;
		}
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		vx = -nx * MARIO_DIE_DEFLECT_SPEED;
		break;
	}
	if (state == MARIO_STATE_SIT && this->state != MARIO_STATE_SIT)
		y += 9;
	if (state != MARIO_STATE_SIT && this->state == MARIO_STATE_SIT)
		y -= 9;
	CGameObject::SetState(state);
}

void CMario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y; 
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
		}
		else if (level == MARIO_LEVEL_FIRE)
		{
			right = left + MARIO_FIRE_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
		if (state == MARIO_STATE_SIT)
		{
			//top -= 9;
			bottom -= 9;
		}
		//DebugOut(L"%f %f %f %f\n", left, top, right, bottom);
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

