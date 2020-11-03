#pragma once
#include "GameObject.h"

#define MARIO_WALKING_SPEED		0.15f 
//0.1f
#define MARIO_JUMP_SPEED_Y		0.4f
#define MARIO_JUMP_DEFLECT_SPEED 0.2f
#define MARIO_GRAVITY			0.002f
#define MARIO_ACCELERATION			0.0025f
#define MARIO_SPEED_MAX_STACKS 7
#define MARIO_JUMP_MAX_STACKS 14
#define MARIO_DIE_DEFLECT_SPEED	 0.5f

#define MARIO_STATE_IDLE			0
#define MARIO_STATE_WALKING_RIGHT	100
#define MARIO_STATE_WALKING_LEFT	200
#define MARIO_STATE_JUMP			300
#define MARIO_STATE_BRAKE			400
#define MARIO_STATE_DIE				900

#define MARIO_ANI_BIG_IDLE_RIGHT		0
#define MARIO_ANI_BIG_IDLE_LEFT			1
#define MARIO_ANI_SMALL_IDLE_RIGHT		2
#define MARIO_ANI_SMALL_IDLE_LEFT			3

#define MARIO_ANI_BIG_WALKING_RIGHT			4
#define MARIO_ANI_BIG_WALKING_LEFT			5
#define MARIO_ANI_SMALL_WALKING_RIGHT		6
#define MARIO_ANI_SMALL_WALKING_LEFT		7
#define MARIO_ANI_SMALL_JUMPING_RIGHT		8
#define MARIO_ANI_SMALL_JUMPING_LEFT		9
#define MARIO_ANI_SMALL_BRAKING_RIGHT		10
#define MARIO_ANI_SMALL_BRAKING_LEFT		11

#define MARIO_ANI_DIE	12

#define	MARIO_LEVEL_SMALL	1
#define	MARIO_LEVEL_BIG		2

#define MARIO_BIG_BBOX_WIDTH  15
#define MARIO_BIG_BBOX_HEIGHT 27

#define MARIO_SMALL_BBOX_WIDTH  13
#define MARIO_SMALL_BBOX_HEIGHT 15

#define MARIO_UNTOUCHABLE_TIME 5000


class CMario : public CGameObject
{
	int level;
	int untouchable;
	DWORD untouchable_start;

	float start_x;			// initial position of Mario at scene
	float start_y;
public: 
	bool isTouchingGround;
	int WalkingStacks;
	int JumpingStacks;
	bool isReadyToJump;
	bool isFalling;
	CMario(float x = 0.0f, float y = 0.0f);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL);
	virtual void Render();

	void SetJumpingStacks(int l) { JumpingStacks = l; }
	void SetisReadyToJump(bool l) { isReadyToJump = l; }
	void SetisFalling(bool l) { isFalling = l; }
	void SetState(int vState);
	void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }
	void Reset();

	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
};