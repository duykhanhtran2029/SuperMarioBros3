#pragma once
#include "Brick.h"
#include "Coin.h"
#include "Utils.h"

#define ITEM_COIN 0
#define ITEM_LEAF_OR_MUSHROOM	1

#define ITEM_TYPE_COIN 0
#define ITEM_TYPE_LEAF	1
#define ITEM_TYPE_MUSHROOM	2

#define QUESTIONBRICK_STATE_HIT	142
#define QUESTIONBRICK_STATE_IDLE	100

#define QUESTIONBRICK_PUSH_MAX_STACK 4
#define QUESTIONBRICK_PUSH_HEIGHT_STACK 2
#define QUESTIONBRICK_PUSHED_TIME	30

#define QUESTIONBRICK_ANI_IDLE 0
#define QUESTIONBRICK_ANI_HIT 1

class CQuestionBrick :
    public CBrick
{
	DWORD pushup_start;
	bool isBeingPushedUp = false;
	bool isFallingDown = false;
	int PushedStacks = 0;
public:
	CQuestionBrick(int tag = ITEM_COIN);
	CGameObject* item;
	void SetState(int vState);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	void StartPushedUp() { pushup_start = GetTickCount64(); isBeingPushedUp = true; }
	void CreateItem(int itemtype);
	void StopPushedUp() { pushup_start = GetTickCount64(); isBeingPushedUp = false; isFallingDown = true; }
};

