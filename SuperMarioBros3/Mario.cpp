#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "BoomerangBrother.h"
#include "FloatingWood.h"
#include "Game.h"
#include "PlayScene.h"
#include "IntroScene.h"
#include "Score.h"
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
#include "IntroObject.h"
#include "Card.h"
CMario::CMario(float x, float y, bool isatintroscene) : CGameObject()
{
	//level = MARIO_LEVEL_SMALL;
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
	isAtIntroScene = isatintroscene;

	RunningStacks = 0;
	money = 0;
	life = MARIO_DEFAULT_LIFES;
	score = 0;
	tail = new CTail(x,y);
}
void CMario::TimingFlag()
{
	if (GetTickCount64() - gamedone > MARIO_GAMEDONE_TIME && gamedone != 0)
	{
		gamedone = 0;
		CGame::GetInstance()->SwitchScene(0);
		return;
	}
	if (GetTickCount64() - slow_start > MARIO_SLOW_TIME && slow_start != 0)
	{
		slow_start = 0;
		isReadyToRun = true;
	}
	if (GetTickCount64() - running_start > MARIO_RUNNING_STACK_TIME && isRunning && vx != 0 && isReadyToRun)
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
	if (GetTickCount64() - kicking_start > MARIO_KICKING_TIME && isKicking)	isKicking = false;
	if (GetTickCount64() - shooting_start > MARIO_SHOOTING_TIME && isShooting) isShooting = false;
	if (GetTickCount64() - turning_state_start > MARIO_TURNING_STATE_TIME && isTurningTail)
	{
		turning_state_start = GetTickCount64();
		turning_state++;
	}
	if (GetTickCount64() - turning_start > MARIO_TURNING_TAIL_TIME && isTurningTail)
	{
		isTurningTail = false;
		turning_state = 0;
		turning_state_start = 0;
		tail->hit_times = 0;
	}
	if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}
	if (GetTickCount64() - flapping_start > MARIO_FLAPPING_TIME && isFlapping)	isFlapping = false;
	if (GetTickCount64() - flapping_start <= MARIO_FLAPPING_TIME && isFlapping)
	{
		if (vy >= MARIO_SLOW_FALLING_SPEED)
			vy = MARIO_SLOW_FALLING_SPEED;
	}
	if (GetTickCount64() - tailflying_start > MARIO_TAIL_FLYING_TIME && isTailFlying)
	{
		isTailFlying = false;
		isRunning = false;
		isFlying = false;
	}
	if (GetTickCount64() - fly_start > MARIO_FLYING_TIME && fly_start != 0 && isFlying)
	{
		fly_start = 0;
		isTailFlying = false;
		isRunning = false;
		isFlying = false;
		StartSlowDown();
	}
	if (GetTickCount64() - pipedown_start > MARIO_PIPE_TIME && pipedown_start > 0)
	{
		pipedown_start = 0;
		lostControl = false;
		isSitting = false;
		if(!isInPipe)
			CGame::GetInstance()->SwitchExtraScene(portal->GetSceneId(), portal->start_x, portal->start_y,portal->pipeUp);
		else
		{
			lostControl = false;
			vx = vy = 0;
			ay = MARIO_GRAVITY;
		}
	}
	if (GetTickCount64() - pipeup_start > MARIO_PIPE_TIME && pipeup_start > 0)
	{
		pipeup_start = 0;
		lostControl = false;
		isSitting = false;
		if(!isInPipe)
			CGame::GetInstance()->SwitchBackScene(portal->GetSceneId(), portal->start_x, portal->start_y);
		else
		{
			lostControl = false;
			vx = vy = 0;
			ay = MARIO_GRAVITY;
		}
	}
}
void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	CGame* game = CGame::GetInstance();
	//stop update when transform
	if (GetTickCount64() - transforming_start > MARIO_TRANSFORMING_TIME && isTransforming)
	{
		if (level == MARIO_LEVEL_SMALL && prelevel != MARIO_LEVEL_SMALL)
			y += (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT - 1);
		isTransforming = false;
	}
	if (GetTickCount64() - gamedone > MARIO_GAMEDONE_TIME && gamedone != 0)
	{
		gamedone = 0;
		((CPlayScene*)game->GetCurrentScene())->isGameDone3 = true;
		return;
	}
	if (isTransforming || ((CPlayScene*)game->GetCurrentScene())->isGameDone2 == true)
		return;

	// Calculate dx, dy 
	CGameObject::Update(dt);

	//slow down if change direction when running
	if (vx * ax < 0 && abs(vx) > MARIO_WALKING_SPEED_MAX
		&& (state == MARIO_STATE_WALKING_LEFT || state == MARIO_STATE_WALKING_RIGHT))
	{
		vx = -nx * MARIO_WALKING_SPEED_MAX;
		if (RunningStacks < 0)
			RunningStacks = 0;
	}

	//update and limit speed
	if (isAtIntroScene)
	{
		if (state != MARIO_STATE_IDLE && state != MARIO_STATE_SITTING
			&& !((CIntroScene*)game->GetCurrentScene())->isCustomSpeed)
			vx = nx * MARIO_WALKING_SPEED_MIN * 2;
		if (!((CIntroScene*)game->GetCurrentScene())->isCustomSpeed)
			vy += ay * dt;
		if (vy <= -MARIO_JUMP_DEFLECT_INTRO && !lostControl && !isDeflect)
		{
			vy = -MARIO_JUMP_SPEED_MAX;
			ay = MARIO_GRAVITY;
			isReadyToJump = false;
		}
		if (isDeflect)
			vy = -MARIO_JUMP_SPEED_MAX * 2;
	}
	else
	{
		vx += ax * dt + RunningStacks * ax;
		vy += ay * dt;
		//limit the speed of mario 
		if (abs(vx) >= MARIO_WALKING_SPEED_MAX)
		{
			if (!isRunning)
				vx = nx * MARIO_WALKING_SPEED_MAX;
			else
				if (abs(vx) >= MARIO_RUNNING_SPEED_MAX && vx * ax > 0)
					if (RunningStacks < MARIO_RUNNING_STACKS)
						vx = nx * MARIO_RUNNING_SPEED_MAX;
					else
						vx = nx * MARIO_SPEED_MAX;
		}
		if (vy > MARIO_JUMP_SPEED_MAX)
		{
			vy = MARIO_JUMP_SPEED_MAX;
			ay = MARIO_GRAVITY;
		}
		if (vy <= -MARIO_JUMP_SPEED_MAX && !lostControl)
		{
			vy = -MARIO_JUMP_SPEED_MAX;
			ay = MARIO_GRAVITY;
			isReadyToJump = false;
		}
		if (isFlying)
		{
			vy = -MARIO_FLY_SPEED;
			ay = -MARIO_GRAVITY;
			isReadyToJump = false;
		}

	}


	//timing for stop/start
	TimingFlag();

	//handle for sitting when jump
	if (state == MARIO_STATE_SITTING && vy < 0)
			vy -= MARIO_ACCELERATION_JUMP * dt;
	//handle for changing direction when jump
	if (isJumping&&isChangeDirection)
		vx = 0;
	//cant jump again until touch the ground
	if (vy < 0)
		isOnGround = false;
	//handle for collision
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);
	else
	{
		x += dx;
		y += dy;
		return;
	}

	if (lostControl)
	{
		if (pipedown_start > 0)
			y += 0.5f;
		else if (pipeup_start > 0)
			y += -0.5f;
	}
	else
	{
		// No collision occured, proceed normally
		if (coEvents.size() == 0)
		{
			x += dx;
			y += dy;
			if (y - lastStandingY > 1.0f)
				isOnGround = false;
		}
		else
		{
			float min_tx, min_ty, nx = 0, ny;
			float rdx = 0;
			float rdy = 0;

			// TODO: This is a very ugly designed function!!!!
			FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);
			float x0 = x, y0 = y;
			x = x0 + min_tx * dx + nx * PUSHBACK;
			y = y0 + min_ty * dy + ny * PUSHBACK;
			//
			// Collision logic with other objects
			//
			float oLeft, oTop, oRight, oBottom;
			float mLeft, mTop, mRight, mBottom;
			for (UINT i = 0; i < coEventsResult.size(); i++)
			{				
				LPCOLLISIONEVENT e = coEventsResult[i];
				if (e->ny != 0 && !(dynamic_cast<CBlock*>(e->obj) && ny > 0))
				{
					if (!(dynamic_cast<CBrick*>(e->obj) && e->obj->tag == PIPE && isFlying))
					{
						isOnGround = true;
						isJumping = false;
						isChangeDirection = false;
						isReadyToSit = true;
						isFlying = false;
					}

				}
				GetBoundingBox(mLeft, mTop, mRight, mBottom);
				e->obj->GetBoundingBox(oLeft, oTop, oRight, oBottom);
				//platforms
				if (dynamic_cast<CBrick*>(e->obj))
				{
					if (isGameDone)
					{
						SetState(MARIO_STATE_WALKING_RIGHT);
						vx = MARIO_WALKING_SPEED_MAX;
						this->nx = 1;
					}
					CBrick* object = dynamic_cast<CBrick*>(e->obj);
					//object->SetDebugAlpha(255);
					if (isAtIntroScene && ((CIntroScene*)game->GetCurrentScene())->Revenge)
					{
						lastStandingY = y;
						SetState(MARIO_STATE_WALKING_RIGHT);
						isReadyToHold = true;
						((CIntroScene*)game->GetCurrentScene())->holder = this;
						vx = MARIO_WALKING_SPEED_MIN * 2;
					}
					else
					{
						if (!(tag == PIPE && isFlying))
						{
							if (e->ny < 0)
							{
								vy = 0;
								lastStandingY = y;
							}
							if (e->ny > 0)
							{
								vy = 0;
								ay = MARIO_GRAVITY;
								isReadyToJump = false;
							}
							if (e->nx != 0)
							{
								if (ceil(mBottom) != oTop)
								{
									vx = 0;
									if (isRunning && tag != PIPE)
										StopRunning();
									if (x < game->GetCamX())
									{
										SetState(MARIO_STATE_DIE);
										return;
									}
								}
							}
						}
					}
				}
				if (dynamic_cast<CQuestionBrick*>(e->obj) && e->ny > 0)
					e->obj->SetState(QUESTIONBRICK_STATE_HIT);
				if (dynamic_cast<CBreakableBrick*>(e->obj) && e->ny > 0)
				{
					AddScore(e->obj->x, e->obj->y, 10, false, false);
					((CBreakableBrick*)e->obj)->Break();
				}
				else if (dynamic_cast<CFloatingWood*>(e->obj))
				{
					CFloatingWood* floatingwood = dynamic_cast<CFloatingWood*>(e->obj);
					if (e->nx != 0 && x > game->GetCamX() && abs(mRight - oLeft) <= 1.0f)
					{
						x--;
						SetState(MARIO_STATE_WALKING_RIGHT);
						vx = MARIO_WALKING_SPEED_MAX;
						nx = 1;
						if (x <= game->GetCamX())
						{
							SetState(MARIO_STATE_DIE);
							return;
						}
					}
					if (e->ny < 0)
					{
						vy = 0;
						e->obj->SetState(FLOATING_WOOD_STATE_FALLING);
						isTouchingWood = true;
						floatingwood->isBeingTouched = true;
					}
					if (e->ny > 0)
					{
						vy = 0;
						ay = MARIO_GRAVITY;
						isReadyToJump = false;
					}

					
				}
				else if (dynamic_cast<CBlock*>(e->obj))
				{
					if (e->nx != 0 && ceil(mBottom) != oTop)
						x = x0 + dx;
					if (e->ny < 0)
					{
						lastStandingY = y;
						vy = 0;
					}
					if (e->ny > 0 && vy < 0)
						y = y0 + dy;	
				}
				//enemies
				else if (dynamic_cast<CGoomba*>(e->obj)) // if e->obj is Goomba 
				{
					CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
					// jump on top >> kill Goomba and deflect a bit 
					if (e->ny < 0)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							if (!isAtIntroScene)
								AddScore(goomba->x, goomba->y, 100, true);
							if (goomba->tag != GOOMBA_RED)
							{
								goomba->SetState(GOOMBA_STATE_DIE);
								if (!isAtIntroScene)
									vy = -MARIO_JUMP_DEFLECT_SPEED;
								else
								{
									((CIntroScene*)game->GetCurrentScene())->isCustomSpeed = false;
									vy = -MARIO_JUMP_DEFLECT_INTRO;
								}
							}
							else
							{
								goomba->SetTag(GOOMBA_RED_NORMAL);
								goomba->SetState(GOOMBA_STATE_WALKING);
								vy = -MARIO_JUMP_DEFLECT_SPEED;
							}
						}
						else
							vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
					else if(goomba->GetState() != GOOMBA_STATE_DIE)
					{
						y = y0;
						if (untouchable == 0)
							Attacked();
						else
						{
							x = x0 + dx;
							y = y0;
							if (e->ny > 0 && vy < 0)
								y = y0 + dy;
							if (e->nx != 0 && isTurningTail)
							{
								AddScore(goomba->x, goomba->y, 100, true);
								goomba->SetState(GOOMBA_STATE_DIE_BY_TAIL);
							}
						}
					}
				}
				else if (dynamic_cast<CBoomerangBrother*>(e->obj)) // if e->obj is Goomba 
				{
					CBoomerangBrother* tmp = dynamic_cast<CBoomerangBrother*>(e->obj);
					// jump on top >> kill Goomba and deflect a bit 
					if (e->ny < 0)
					{
						if (tmp->GetState() != BOOMERANG_BROTHER_STATE_DIE)
						{
							AddScore(tmp->x, tmp->y, 1000, true);
							tmp->SetState(BOOMERANG_BROTHER_STATE_DIE);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
					}
					else
					{
						if (untouchable != 0)
						{
							x = x0 + dx;
							y = y0;
						}
						else
							y = y0;
						if (untouchable == 0 && tmp->GetState() != GOOMBA_STATE_DIE && !isTurningTail)
							Attacked();
					}
				}
				else if (dynamic_cast<CKoopas*>(e->obj)) // if e->obj is Koopas 
				{
					CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
					if (isAtIntroScene)
					{
						if (level == MARIO_LEVEL_TAIL)
						{
							if (e->ny > 0)
								vy = 0;
							if (e->ny < 0)
							{
								koopas->SetState(KOOPAS_STATE_IN_SHELL);
								vy = -MARIO_JUMP_DEFLECT_SPEED;
							}
							else
								if (e->nx != 0)
								{
									if (isReadyToHold)
									{
										koopas->SetIsBeingHeld(true);
										isHolding = true;
									}
									else
									{
										if (((CIntroScene*)CGame::GetInstance()->GetCurrentScene())->mini_section == 9)
										{
											SetLevel(MARIO_LEVEL_SMALL);
											//koopas->x++;
										}
										else
										{
											StartKicking();
											koopas->nx = this->nx;
											koopas->SetState(KOOPAS_STATE_SPINNING);
											((CIntroScene*)CGame::GetInstance()->GetCurrentScene())->isCustomSpeed = true;
										}

									}

								}
						}
						else
						{
							if (level != MARIO_LEVEL_SMALL)
							{
								x = x0;
								y = y0;
								koopas->nx = this->nx;
								koopas->SetSpeed(-MARIO_WALKING_SPEED_MIN * 2, MARIO_JUMP_SPEED_MIN);
								((CIntroScene*)CGame::GetInstance()->GetCurrentScene())->SetShell(koopas);
								((CIntroScene*)CGame::GetInstance()->GetCurrentScene())->StartChangeAni();
							}
						}

					}
					else
					{
						if (e->ny < 0)
						{
							AddScore(koopas->x, koopas->y, 100, true);
							vy = -1.5f * MARIO_JUMP_DEFLECT_SPEED;
							if (this->nx > 0)
							{
								if (vx < MARIO_WALKING_SPEED_MIN * 2)
									vx = MARIO_WALKING_SPEED_MIN * 2;
							}
							else
							{
								if (vx > -MARIO_WALKING_SPEED_MIN * 2)
									vx = -MARIO_WALKING_SPEED_MIN * 2;
							}
							if (koopas->tag == KOOPAS_GREEN_PARA)
								koopas->tag = KOOPAS_GREEN;
							else if (koopas->tag == KOOPAS_RED_PARA)
								koopas->tag = KOOPAS_RED;
							else if (koopas->GetState() != KOOPAS_STATE_IN_SHELL && koopas->GetState() != KOOPAS_STATE_SHELL_UP)
								koopas->SetState(KOOPAS_STATE_IN_SHELL);
							else
								koopas->SetState(KOOPAS_STATE_SPINNING);
						}
						else
						{
							if (e->ny > 0)	
								y = y0;
							if (untouchable != 0)
							{
								x = x0 + dx;
								if (e->ny > 0 && vy < 0)
									y = y0 + dy;
							}
							if (koopas->GetState() == KOOPAS_STATE_IN_SHELL|| koopas->GetState() == KOOPAS_STATE_SHELL_UP)
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
							else if (untouchable == 0 && (koopas->GetState() == KOOPAS_STATE_SPINNING || koopas->GetState() == KOOPAS_STATE_WALKING))
									Attacked();
						}
					}
				}
				else if (dynamic_cast<CPiranhaPlant*>(e->obj) || dynamic_cast<CFirePiranhaPlant*>(e->obj))
				{
					if (isTurningTail && e->nx != 0)
					{
						e->obj->SetState(PIRANHAPLANT_STATE_DEATH);
						AddScore(e->obj->x, e->obj->y, 100, true);
					}
					else
					{
						if (untouchable == 0)
							Attacked();
						else
						{
							x = x0 + dx;
							if(e->ny < 0)
								y = y0 + dy;
						}
					}
				}
				//others
				else if (dynamic_cast<CSwitch*>(e->obj))
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
						if (sw->state != SWITCH_STATE_PRESSED)
							sw->SetState(SWITCH_STATE_PRESSED);
					}
					if (e->nx != 0)
					{
						if (ceil(mBottom) != oTop)
							vx = 0;
					}
				}
				else if (dynamic_cast<CMario*>(e->obj)) // if e->obj is Mario 
				{
					CMario* tmp = dynamic_cast<CMario*>(e->obj);
					x = x0;
					y = y0;
					if (e->ny < 0)
					{
						vy = -MARIO_JUMP_DEFLECT_SPEED;
						isDeflect = true;
					}
					else
					{
						SetState(MARIO_STATE_SITTING);
						vx = vy = 0;
					}
				}
				else if (dynamic_cast<CCard*>(e->obj))
				{
					srand(time(NULL));
					int id = rand() % 3 + 1;
					e->obj->vy = -CARD_SPEED;
					e->obj->SetState(id);
					isGameDone = true;
					cards.push_back(id);
					vy = 0;
					ay = MARIO_GRAVITY;
					isReadyToJump = false;
				}
			}
		}
	}
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++)
		delete coEvents[i];
	
	if (!isAtIntroScene)
	{
		//limit X, Y
		float mw = ((CPlayScene*)game->GetCurrentScene())->GetMap()->GetMapWidth();
		float mh = ((CPlayScene*)game->GetCurrentScene())->GetMap()->GetMapHeight();
		if (!dynamic_cast<CIntroScene*>(game->GetCurrentScene()) && !isGameDone)
		{
			//limit X
			if (x >= mw - MARIO_BIG_BBOX_WIDTH)//Right edge
				x = mw - MARIO_BIG_BBOX_WIDTH;
			else if (x <= 0)//Left edge
				x = 0;
			//limit Y
			if (y >= mh)
				y = mh;
			if (y <= -HUD_HEIGHT)
				y = -HUD_HEIGHT;

			if (game->GetCurrentScene()->GetId() == WORLD_1_4)
			{
				if ( x - 1 <= game->GetCamX())
				{
					//if(!isBlocked)
					//vx
					SetState(MARIO_STATE_WALKING_RIGHT);
					vx = 0.1f;
					nx = 1;
				}
				if (x >= game->GetCamX() + game->GetScreenWidth() - GetWidth())
					x = game->GetCamX() + game->GetScreenWidth() - GetWidth();
			}
		}
		if (x >= mw)
		{
			((CPlayScene*)game->GetCurrentScene())->isGameDone1 = true;
		}
		if (y >= mh + MARIO_BIG_BBOX_WIDTH * 2)
		{
			((CPlayScene*)game->GetCurrentScene())->isGameDone2 = true;
			score += ((CPlayScene*)game->GetCurrentScene())->CalScore();
			y = mh + MARIO_BIG_BBOX_WIDTH * 2;
			if (gamedone == 0)
				StartGameDone();
		}
		//update tail
		tail->Update(dt);
	}
	//DebugOut(L"[MARIO] state %d \n", state);
}
void CMario::BasicRenderLogicsForAllLevel(int& ani, int ani_jump_down_right, int ani_jump_down_left,
	int ani_idle_right, int ani_idle_left,
	int ani_baking_right, int ani_baking_left, int ani_walking_right, int ani_walking_left, int ani_kicking_right, int ani_kicking_left)
{
	if (vx == 0) 
	{
		if (isOnGround)
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
			if (level == MARIO_LEVEL_SMALL && isAtIntroScene)
			{
				if (nx > 0) ani = ani_idle_right;
				else ani = ani_idle_left;
			}
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
	if (vx != 0 && vy > 0 && !isOnGround && !isAtIntroScene)
	{
		if (nx > 0) ani = ani_jump_down_right;
		else ani = ani_jump_down_left;
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
	int alpha = MARIO_RENDER_ALPHA;
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

	if (untouchable) 
		alpha = MARIO_RENDER_ALPHA/2;
	if (pipedown_start > 0 || pipeup_start > 0)
	{
		int sprite_id = MARIO_SPRITE_PIPE_SMALL;
		if(level == MARIO_LEVEL_BIG)
			sprite_id = MARIO_SPRITE_PIPE_BIG;
		if (level == MARIO_LEVEL_TAIL)
			sprite_id = MARIO_SPRITE_PIPE_TAIL;
		if (level == MARIO_LEVEL_FIRE)
			sprite_id = MARIO_SPRITE_PIPE_FIRE;
		CSprites::GetInstance()->sprites[sprite_id]->Draw(x, y - HUD_HEIGHT, alpha);
	}
	else if (level == MARIO_LEVEL_TAIL && nx > 0)
	{
		if (isTurningTail)
		{
			if (turning_state == MARIO_WHACK_STATE_1 || turning_state == MARIO_WHACK_STATE_5)
				CSprites::GetInstance()->sprites[MARIO_SPRITE_WHACK_RIGHT_1_ID]->Draw(x - MARIO_WHACK_RENDER_DIFF_7, y - HUD_HEIGHT, alpha);
			if (turning_state == MARIO_WHACK_STATE_2)
				CSprites::GetInstance()->sprites[MARIO_SPRITE_WHACK_RIGHT_2_ID]->Draw(x, y - HUD_HEIGHT, alpha);
			if (turning_state == MARIO_WHACK_STATE_3)
				CSprites::GetInstance()->sprites[MARIO_SPRITE_WHACK_RIGHT_3_ID]->Draw(x, y - HUD_HEIGHT, alpha);
			if (turning_state == MARIO_WHACK_STATE_4)
				CSprites::GetInstance()->sprites[MARIO_SPRITE_WHACK_RIGHT_4_ID]->Draw(x, y - HUD_HEIGHT, alpha);
		}
		else
		{
			if(isSitting || vx >= 0)
				animation_set->at(ani)->Render(x - MARIO_SITTING_RENDER_DIFF_5, y, alpha);
			else
				animation_set->at(ani)->Render(x, y, alpha);
		}
	}	
	else if (level == MARIO_LEVEL_TAIL && nx < 0)
	{
		if (isTurningTail)
		{
			if (turning_state == MARIO_WHACK_STATE_1 || turning_state == MARIO_WHACK_STATE_5)
				CSprites::GetInstance()->sprites[MARIO_SPRITE_WHACK_LEFT_1_ID]->Draw(x + MARIO_WHACK_RENDER_DIFF_2, y - HUD_HEIGHT, alpha);
			if (turning_state == MARIO_WHACK_STATE_2)
				CSprites::GetInstance()->sprites[MARIO_SPRITE_WHACK_LEFT_2_ID]->Draw(x, y - HUD_HEIGHT, alpha);
			if (turning_state == MARIO_WHACK_STATE_3)
				CSprites::GetInstance()->sprites[MARIO_SPRITE_WHACK_LEFT_3_ID]->Draw(x - MARIO_WHACK_RENDER_DIFF_7, y - HUD_HEIGHT, alpha);
			if (turning_state == MARIO_WHACK_STATE_4)
				CSprites::GetInstance()->sprites[MARIO_SPRITE_WHACK_LEFT_4_ID]->Draw(x, y - HUD_HEIGHT, alpha);
		}
		else
			animation_set->at(ani)->Render(x, y, alpha);
	}
	else
		animation_set->at(ani)->Render(x, y, alpha);

	RenderBoundingBox();
	if(isTurningTail)
		tail->Render();
}
void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;

	if (level != MARIO_LEVEL_SMALL)
	{
		right = left + MARIO_BIG_BBOX_WIDTH;
		bottom = top + MARIO_BIG_BBOX_HEIGHT;
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
void CMario::SetState(int state)
{
	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		if (ax < 0 && vy > 0)
		{
			isChangeDirection = true;
		}
		if (ax < 0 && RunningStacks != 0)
		{
			StopRunning();
			StartSlowDown();
		}
		ax = MARIO_ACCELERATION;
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		if (ax > 0 && vy > 0)
			isChangeDirection = true;
		if (ax > 0 && RunningStacks != 0)
		{
			StopRunning();
			StartSlowDown();
		}
		ax = -MARIO_ACCELERATION;
		nx = -1;
		break;
	case MARIO_STATE_JUMPING:
		if (vy > -MARIO_JUMP_SPEED_MIN)
			vy = -MARIO_JUMP_SPEED_MIN;
		if (RunningStacks == MARIO_RUNNING_STACKS)
		{
			isFlying = true;
			StartFlying();
		}
		ay = -MARIO_ACCELERATION_JUMP;
		if (isTouchingWood)
			isTouchingWood = false;
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
/*
	Reset Mario status to the beginning state of a scene
*/
void CMario::SetLevel(int l,bool transform)
{
	prelevel = level;
	level = l;
	transformState = false;
	if (level != prelevel && transform)
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
void CMario::AddScore(int ox, int oy, int s, bool isEnemy, bool showscore)
{
	if (isEnemy)
	{
		if (GetTickCount64() - last_kill <= MARIO_KILLSTREAK_TIME)
		{
			kill_streak++;
			if (kill_streak > 3)
				kill_streak = 3;
		}
		else
			kill_streak = 0;
		last_kill = GetTickCount64();
	}
	else
		kill_streak = 0;
	s = pow(2, kill_streak) * s;
	this->score += s;
	if (showscore)
	{
		CScore* cscore = new CScore(s);
		cscore->SetPosition(ox, oy);
		((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->PushBack(cscore);
	}
}
void CMario::Reset()
{
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_BIG);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
}
void CMario::Attacked()
{
	StartUntouchable();
	if (level == MARIO_LEVEL_SMALL)
	{
		SetState(MARIO_STATE_DIE);
		return;
	}
	if (level == MARIO_LEVEL_TAIL || level == MARIO_LEVEL_FIRE)
		SetLevel(MARIO_LEVEL_BIG);
	else if (level == MARIO_LEVEL_BIG)
		SetLevel(MARIO_LEVEL_SMALL);
}
void CMario::StartShooting(float bx, float by)
{
	if (ShootTimes < MARIO_FIRE_BULLETS)
	{
		shooting_start = GetTickCount64();
		isShooting = true;
		CFireBullet* bullet = new CFireBullet(bx + nx * FIRE_BULLET_BBOX_WIDTH, by + FIRE_BULLET_SHOOT_DIFF * (MARIO_LEVEL_SMALL ? MARIO_SMALL_BBOX_HEIGHT : MARIO_BIG_BBOX_HEIGHT));
		bullet->SetSpeed(nx * FIRE_BULLET_SPEED_X, FIRE_BULLET_SPEED_Y);
		bullet->SetTemHeight(0);
		((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->PushBack(bullet);
		ShootTimes++;
	}
}
void CMario::Tele()
{
	SetPosition(x + 500, 50);
	SetSpeed(0, 0);
}
