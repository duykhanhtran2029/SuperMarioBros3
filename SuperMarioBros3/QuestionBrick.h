#pragma once
#include "Brick.h"
#include "Coin.h"
#include "Utils.h"

#define ITEM_COIN				0
#define ITEM_CUSTOM				1
#define ITEM_LEAF				2
#define ITEM_MUSHROOM_RED		3
#define ITEM_MUSHROOM_GREEN		4
#define ITEM_SWITCH				6

#define QUESTIONBRICK_TYPE_MARK		0
#define QUESTIONBRICK_TYPE_FLASH	1

#define QUESTIONBRICK_STATE_HIT		142
#define QUESTIONBRICK_STATE_IDLE	100

#define QUESTIONBRICK_PUSH_MAX_STACK 4
#define QUESTIONBRICK_PUSH_HEIGHT_STACK 2
#define QUESTIONBRICK_PUSHED_TIME	30

#define QUESTIONBRICK_ANI_IDLE	0
#define QUESTIONBRICK_ANI_HIT	1
#define QUESTIONBRICK_ANI_FLASH 2

class CQuestionBrick :
    public CBrick
{
	DWORD pushup_start = 0;
	bool isBeingPushedUp = false;
	bool isFallingDown = false;
	int PushedStacks = 0;
	int type = 0;
public:
	CQuestionBrick(int tag = ITEM_COIN, int type = QUESTIONBRICK_TYPE_MARK);
	CGameObject* item = NULL;
	void SetState(int vState);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	void StartPushedUp() { pushup_start = GetTickCount64(); isBeingPushedUp = true; }
	void CreateItem(int itemtype);
	void StopPushedUp() { pushup_start = GetTickCount64(); isBeingPushedUp = false; isFallingDown = true; }
};

