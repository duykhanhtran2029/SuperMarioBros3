#pragma once

#include <Windows.h>
#include <d3dx9.h>
#include <vector>

#include "Sprites.h"
#include "Animations.h"
#include "Define.h"
#include "Game.h"

#define STATIC	0
#define MOVING	1
#define IGNORE	2
 
#define PUSHBACK 0.4f
#define VIEWPORT_PUSHBACK	16


using namespace std;

#define ID_TEX_BBOX -100		// special texture to draw object bounding box

class CGameObject; 
typedef CGameObject * LPGAMEOBJECT;

struct CCollisionEvent;
typedef CCollisionEvent * LPCOLLISIONEVENT;
struct CCollisionEvent
{
	LPGAMEOBJECT obj;
	float t, nx, ny;

	float dx, dy;		// *RELATIVE* movement distance between this object and obj

	CCollisionEvent(float t, float nx, float ny, float dx = 0, float dy = 0, LPGAMEOBJECT obj = NULL)
	{
		this->t = t;
		this->nx = nx;
		this->ny = ny;
		this->dx = dx;
		this->dy = dy;
		this->obj = obj;
	}

	static bool compare(const LPCOLLISIONEVENT& a, LPCOLLISIONEVENT& b)
	{
		return a->t < b->t;
	}
};


class CGameObject
{
public:

	float x; 
	float y;

	float dx;	// dx = vx*dt
	float dy;	// dy = vy*dt

	float vx;
	float vy;

	float ax, ay; //acceleration

	//float deltaX = 0;
	//float deltaY = 0;

	//int DebugAlpha = 0;
	//int DebugTag = 0;

	int nx;	 

	int state;

	DWORD dt; 

	LPANIMATION_SET animation_set;

public: 
	int tag = 0;
	int type = 0;
	bool isDestroyed = false;
	bool isEnable = true;

	void GetPosition(float &x, float &y) { x = this->x; y = this->y; }
	void GetSpeed(float &vx, float &vy) { vx = this->vx; vy = this->vy; }
	int GetState() { return this->state; }

	void SetIsDestroyed(bool isDestroyed) { this->isDestroyed = isDestroyed; }
	void SetPosition(float x, float y) { this->x = x, this->y = y; }
	void SetSpeed(float vx, float vy) { this->vx = vx, this->vy = vy; }
	void SetDirection(int nx) { this->nx = nx; }
	void SetTag(int tag) { this->tag = tag; }
	void SetType(int type) { this->type = type; }
	bool IsInViewPort() 
	{
		CGame* game = CGame::GetInstance();
		float camX, camY;

		camX = game->GetCamX();
		camY = game->GetCamY();

		bool xInViewPort = x >= camX - GetWidth() && x < camX + game->GetScreenWidth();
		bool yInViewPort = y >= camY - (SCREEN_HEIGHT - game->GetScreenHeight()) && y < camY + SCREEN_HEIGHT;
		
		return xInViewPort && yInViewPort;
	}
	void RenderBoundingBox(int alpha = 0);
	bool isColliding(float friend_left, float friend_top, float friend_right, float friend_bottom);
	void SetAnimationSet(LPANIMATION_SET ani_set) { animation_set = ani_set; }

	LPCOLLISIONEVENT SweptAABBEx(LPGAMEOBJECT coO);
	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);
	void FilterCollision(
		vector<LPCOLLISIONEVENT>& coEvents,
		vector<LPCOLLISIONEVENT>& coEventsResult,
		float& min_tx,
		float& min_ty,
		float& nx,
		float& ny,
		float& rdx,
		float& rdy);

	CGameObject();
	float GetWidth() 
	{
		float left, top, right, bottom;
		GetBoundingBox(left, top, right, bottom);
		return right - left;
	}
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom) = 0;
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects = NULL);
	virtual void Render() = 0;
	virtual void SetState(int state) { this->state = state; }


	~CGameObject();
};

