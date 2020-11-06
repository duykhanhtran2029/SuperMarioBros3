#pragma once
#include "GameObject.h"

#define MARIO_WALKING_SPEED_START	0.001f 
#define MARIO_WALKING_SPEED_MAX		0.15f
#define MARIO_ACCELERATION			0.0003f
#define MARIO_WALKING_SPEED_MIN		0.01f


#define MARIO_ACCELERATION_JUMP		0.0005f
#define MARIO_JUMP_SPEED_MAX		0.28f
#define MARIO_JUMP_SPEED_MIN		0.18f
#define MARIO_JUMP_DEFLECT_SPEED	0.28f
#define MARIO_GRAVITY				0.002f
#define MARIO_DIE_DEFLECT_SPEED		0.5f

#define MARIO_RUN_SPEED_MAX			0.3f

#define MARIO_UNTOUCHABLE_TIME		5000
#define MARIO_USING_TAIL_TIME		300
#define MARIO_SHOOTING_TIME			200

#define MARIO_STATE_IDLE			0
#define MARIO_STATE_WALKING_RIGHT	100
#define MARIO_STATE_WALKING_LEFT	101
#define MARIO_STATE_JUMP			200
#define MARIO_STATE_SIT				201
#define MARIO_STATE_INERTIA			400
#define MARIO_STATE_DIE				900

// SMALL
#define MARIO_ANI_SMALL_IDLE_RIGHT			0
#define MARIO_ANI_SMALL_WALKING_RIGHT		1
#define MARIO_ANI_SMALL_WALKING_FAST_RIGHT	2
#define MARIO_ANI_SMALL_RUNNING_RIGHT		3
#define MARIO_ANI_SMALL_JUMPINGUP_RIGHT		4
#define MARIO_ANI_SMALL_JUMPINGDOWN_RIGHT	5
#define MARIO_ANI_SMALL_BRAKING_RIGHT		6

#define MARIO_ANI_SMALL_IDLE_LEFT			7
#define MARIO_ANI_SMALL_WALKING_LEFT		8
#define MARIO_ANI_SMALL_WALKING_FAST_LEFT	9
#define MARIO_ANI_SMALL_RUNNING_LEFT		10
#define MARIO_ANI_SMALL_JUMPINGUP_LEFT		11
#define MARIO_ANI_SMALL_JUMPINGDOWN_LEFT	12
#define MARIO_ANI_SMALL_BRAKING_LEFT		13

//BIG
#define MARIO_ANI_BIG_IDLE_RIGHT			14
#define MARIO_ANI_BIG_WALKING_RIGHT			15
#define MARIO_ANI_BIG_WALKING_FAST_RIGHT	16
#define MARIO_ANI_BIG_RUNNING_RIGHT			17
#define MARIO_ANI_BIG_JUMPINGUP_RIGHT		18
#define MARIO_ANI_BIG_JUMPINGDOWN_RIGHT		19
#define MARIO_ANI_BIG_BRAKING_RIGHT			20
#define MARIO_ANI_BIG_SITTING_RIGHT			21

#define MARIO_ANI_BIG_IDLE_LEFT				22
#define MARIO_ANI_BIG_WALKING_LEFT			23
#define MARIO_ANI_BIG_WALKING_FAST_LEFT		24
#define MARIO_ANI_BIG_RUNNING_LEFT			25
#define MARIO_ANI_BIG_JUMPINGUP_LEFT		26
#define MARIO_ANI_BIG_JUMPINGDOWN_LEFT		27
#define MARIO_ANI_BIG_BRAKING_LEFT			28
#define MARIO_ANI_BIG_SITTING_LEFT			29

//TAIL
#define MARIO_ANI_TAIL_IDLE_RIGHT			30
#define MARIO_ANI_TAIL_WALKING_RIGHT		31
#define MARIO_ANI_TAIL_WALKING_FAST_RIGHT	32
#define MARIO_ANI_TAIL_RUNNING_RIGHT		33
#define MARIO_ANI_TAIL_JUMPINGUP_RIGHT		34
#define MARIO_ANI_TAIL_JUMPINGDOWN_RIGHT	35
#define MARIO_ANI_TAIL_BRAKING_RIGHT		36
#define MARIO_ANI_TAIL_SITTING_RIGHT		37

#define MARIO_ANI_TAIL_IDLE_LEFT			38
#define MARIO_ANI_TAIL_WALKING_LEFT			39
#define MARIO_ANI_TAIL_WALKING_FAST_LEFT	40
#define MARIO_ANI_TAIL_RUNNING_LEFT			41
#define MARIO_ANI_TAIL_JUMPINGUP_LEFT		42
#define MARIO_ANI_TAIL_JUMPINGDOWN_LEFT		43
#define MARIO_ANI_TAIL_BRAKING_LEFT			44
#define MARIO_ANI_TAIL_SITTING_LEFT			45

//FIRE
#define MARIO_ANI_FIRE_IDLE_RIGHT			46
#define MARIO_ANI_FIRE_WALKING_RIGHT		47
#define MARIO_ANI_FIRE_WALKING_FAST_RIGHT	48
#define MARIO_ANI_FIRE_RUNNING_RIGHT		49
#define MARIO_ANI_FIRE_JUMPINGUP_RIGHT		50
#define MARIO_ANI_FIRE_JUMPINGDOWN_RIGHT	51
#define MARIO_ANI_FIRE_BRAKING_RIGHT		52
#define MARIO_ANI_FIRE_SITTING_RIGHT		53

#define MARIO_ANI_FIRE_IDLE_LEFT			54
#define MARIO_ANI_FIRE_WALKING_LEFT			55
#define MARIO_ANI_FIRE_WALKING_FAST_LEFT	56
#define MARIO_ANI_FIRE_RUNNING_LEFT			57
#define MARIO_ANI_FIRE_JUMPINGUP_LEFT		58
#define MARIO_ANI_FIRE_JUMPINGDOWN_LEFT		59
#define MARIO_ANI_FIRE_BRAKING_LEFT			60
#define MARIO_ANI_FIRE_SITTING_LEFT			61


#define MARIO_ANI_DIE	62

#define	MARIO_LEVEL_SMALL	1
#define	MARIO_LEVEL_BIG		2
#define	MARIO_LEVEL_TAIL	3
#define	MARIO_LEVEL_FIRE	4

#define MARIO_BIG_BBOX_WIDTH  15
#define MARIO_BIG_BBOX_HEIGHT 27

#define MARIO_SMALL_BBOX_WIDTH  13
#define MARIO_SMALL_BBOX_HEIGHT 15

#define MARIO_TAIL_BBOX_WIDTH  21

#define MARIO_FIRE_BBOX_WIDTH  14

#define MARIO_UNTOUCHABLE_TIME 5000


class CMario : public CGameObject
{
	DWORD untouchable_start;

	float start_x;			// initial position of Mario at scene
	float start_y;
public: 
	int level;
	int untouchable;
	//state
	bool isOnGround = false;

	//sit
	bool isReadyToSit = true;
	bool isSitting = false;

	//hight-jump
	bool isReadyToJump = true;
	bool isFalling = false;
	bool isJumping = false;

	//using tail
	bool isUsingTail = false;

	//shoot
	bool isShootingFireBall = false;
	bool isForFireBallAppear = true;

	//hold
	bool isHolding = false;
	bool isReadyToHold = false;
	//kick
	bool isKicking = false;
	bool isReadyToKick = true;

	//run
	bool isReadyToRun = false;
	bool isRunning = false;

	CMario(float x = 0.0f, float y = 0.0f);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL);
	virtual void Render();

	//set
	void SetIsOnGround(bool On) { this->isReadyToHold = On; }
	void SetIsReadyToHold(bool hold) { this->isReadyToHold = hold; }
	void SetIsHolding(bool m) { this->isHolding = m; }

	void SetIsReadyToJump(bool jump) { isReadyToJump = jump; }
	void SetIsJumping(bool jump) { this->isJumping = jump; }
	void SetIsFalling(bool fall) { this->isFalling = fall; }

	void SetIsReadyToRun(bool run) { this->isReadyToRun = run; }
	void SetIsRunning(bool run) { this->isRunning = run; }

	void SetIsReadyToSit(bool sit) { this->isReadyToSit = sit; }
	void SetIsSitting(bool sit) { this->isSitting= sit; }

	void SetIsShootingFireBall(bool shoot) { this->isShootingFireBall = shoot; }

	void SetIsReadyToKick(bool m) { this->isReadyToKick = m; }
	void SetIsKicking(bool m) { this->isKicking = m; }

	//render for all
	void BasicRenderLogicsForAllLevel(int& ani,
		int ani_idle_right, int ani_idle_left, int ani_jump_down_right, int ani_jump_down_left,
		int ani_baking_right, int ani_baking_left, int ani_walking_right, int ani_walking_left);
	void RenderSitting(int& ani, int ani_sit_right, int ani_sit_left);

	void RenderJumping(int& ani, int ani_jump_up_right, int ani_jump_up_left, int ani_jump_down_right, int ani_jump_down_left);

	//void RenderRunning(int& ani, int ani_run_up_right, int ani_run_up_left, int ani_run_down);
	void SetState(int vState);
	void SetLevel(int l);
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }
	void Reset();

	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
};