#pragma once

#include "GameObject.h"
#include "Mario.h"
#include"Goomba.h"
#include "PlayScence.h"
#include "Scence.h"
#define KOOPAS_WALKING_SPEED 0.03f
#define KOOPAS_GRAVITY 0.002f
#define KOOPAS_DIE_DEFLECT_SPEED 0.5f
#define KOOPAS_BBOX_WIDTH 16
#define KOOPAS_BBOX_HEIGHT 26
#define KOOPAS_BBOX_SHELL_HEIGHT 16

#define KOOPAS_STATE_WALKING 100
#define KOOPAS_STATE_SHELL_UP 200
#define KOOPAS_STATE_SPINNING 300
#define KOOPAS_STATE_IN_SHELL 400

#define KOOPAS_ANI_WALKING_RIGHT 0
#define KOOPAS_ANI_SPIN_RIGHT 1
#define KOOPAS_ANI_WALKING_LEFT 2
#define KOOPAS_ANI_SPIN_LEFT 3
#define KOOPAS_ANI_SHELL 4
#define KOOPAS_ANI_SHAKE 5
#define KOOPAS_ANI_SHELL_UP 6

class CKoopas : public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
public:
	bool isBeingHeld = false;
	int tag;
	CKoopas();
	void SetIsBeingHeld(bool m) { isBeingHeld = m; };
	virtual void SetState(int state);
	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);
};
