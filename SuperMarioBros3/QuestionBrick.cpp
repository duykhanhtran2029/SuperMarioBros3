#include "QuestionBrick.h"
#include "PlayScene.h"
#include "MushRoom.h"
#include "Switch.h"
#include "Leaf.h"
CMario* qbmario;
CPlayScene* qbscene;
CQuestionBrick::CQuestionBrick(int tag, int type)
{
	state = QUESTIONBRICK_STATE_IDLE;
	this->tag = tag;
	this->type = type;
}
void CQuestionBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
	if (state == QUESTIONBRICK_STATE_IDLE)
	{
		float mLeft, mTop, mRight, mBottom;
		float oLeft, oTop, oRight, oBottom;
		qbscene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		if (qbscene != NULL)
			qbmario = ((CPlayScene*)qbscene)->GetPlayer();
		if (qbmario != NULL)
		{
			qbmario->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			GetBoundingBox(oLeft, oTop, oRight, oBottom);
			if (isColliding(floor(mLeft), mTop, ceil(mRight), mBottom) && qbmario->level == MARIO_LEVEL_TAIL && qbmario->isTurningTail && oBottom > ceil(mTop) + MARIO_BIG_BBOX_HEIGHT - BRICK_BBOX_HEIGHT)
			{
				SetState(QUESTIONBRICK_STATE_HIT);
			}

		}

	}
	if (isBeingPushedUp && GetTickCount64() - pushup_start >= QUESTIONBRICK_PUSHED_TIME)
	{
		y -= QUESTIONBRICK_PUSH_HEIGHT_STACK;
		PushedStacks++;
		pushup_start = GetTickCount64();
		if (PushedStacks >= QUESTIONBRICK_PUSH_MAX_STACK)
		{
			PushedStacks = QUESTIONBRICK_PUSH_MAX_STACK;
			StopPushedUp();
		}
	}
	if (isFallingDown && GetTickCount64() - pushup_start >= QUESTIONBRICK_PUSHED_TIME)
	{
		y += QUESTIONBRICK_PUSH_HEIGHT_STACK;
		PushedStacks--;
		pushup_start = GetTickCount64();
		if (PushedStacks <= 0)
		{
			PushedStacks = 0;
			isFallingDown = false;
		}
	}
}
void CQuestionBrick::SetState(int state = BRICK_STATE_IDLE)
{
	switch (state)
	{
	case QUESTIONBRICK_STATE_HIT:
		if (this->state != QUESTIONBRICK_STATE_HIT)
		{
			StartPushedUp();
			if (tag == ITEM_COIN)
			{
				CreateItem(ITEM_COIN);
				CCoin* obj = dynamic_cast<CCoin*>(item);
				obj->isAppear = true;
				obj->SetPosition(x, y - COIN_BBOX_HEIGHT - 1);
				obj->SetState(COIN_STATE_UP);
			}
			if (tag == ITEM_LEAF)
			{
				CreateItem(ITEM_LEAF);
				CLeaf* obj = dynamic_cast<CLeaf*>(item);
				obj->isAppear = true;
				obj->SetPosition(x, y);
				obj->SetState(LEAF_STATE_UP);
			}
			if (tag == ITEM_MUSHROOM_GREEN)
			{
				CreateItem(ITEM_MUSHROOM_GREEN);
				item->tag = MUSHROOM_TYPE_GREEN;

				CMushRoom* obj = dynamic_cast<CMushRoom*>(item);
				obj->isAppear = true;
				obj->SetPosition(x, y);
				obj->SetState(MUSHROOM_STATE_UP);
			}
			if (tag == ITEM_MUSHROOM_RED)
			{
				CreateItem(ITEM_MUSHROOM_RED);
				item->tag = MUSHROOM_TYPE_RED;

				CMushRoom* obj = dynamic_cast<CMushRoom*>(item);
				obj->isAppear = true;
				obj->SetPosition(x, y);
				obj->SetState(MUSHROOM_STATE_UP);
			}
			if (tag == ITEM_CUSTOM)
			{
				qbscene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
				qbmario = ((CPlayScene*)qbscene)->GetPlayer();
				if (qbmario->level == MARIO_LEVEL_SMALL)
				{
					CreateItem(ITEM_MUSHROOM_RED);
					item->tag = MUSHROOM_TYPE_RED;
					CMushRoom* obj = dynamic_cast<CMushRoom*>(item);
					obj->isAppear = true;
					obj->SetPosition(x, y);
					obj->SetState(MUSHROOM_STATE_UP);
				}
				if (qbmario->level == MARIO_LEVEL_BIG)
				{
					CreateItem(ITEM_LEAF);
					CLeaf* obj = dynamic_cast<CLeaf*>(item);
					obj->isAppear = true;
					obj->SetPosition(x, y);
					obj->SetState(LEAF_STATE_UP);
				}
				if (qbmario->level == MARIO_LEVEL_TAIL || qbmario->level == MARIO_LEVEL_FIRE)
				{
					CreateItem(ITEM_MUSHROOM_GREEN);
					item->tag = MUSHROOM_TYPE_GREEN;

					CMushRoom* obj = dynamic_cast<CMushRoom*>(item);
					obj->isAppear = true;
					obj->SetPosition(x, y);
					obj->SetState(MUSHROOM_STATE_UP);
				}
				}
			if (tag == ITEM_SWITCH)
			{
				CreateItem(ITEM_SWITCH);

				CSwitch* obj = dynamic_cast<CSwitch*>(item);
				obj->isAppear = true;
				obj->SetPosition(x, y);
				obj->SetState(SWITCH_STATE_UP);
			}
		}
		break;
	}
	CGameObject::SetState(state);
}
void CQuestionBrick::Render()
{
	int ani = -1;
	if (state == QUESTIONBRICK_STATE_HIT)
		ani = QUESTIONBRICK_ANI_HIT;
	else
		if(type == QUESTIONBRICK_TYPE_FLASH)
			ani = QUESTIONBRICK_ANI_FLASH;
		else
			ani = QUESTIONBRICK_ANI_IDLE;
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}
void CQuestionBrick::CreateItem(int itemtype)
{
	if (itemtype == ITEM_COIN)
	{
		item = new CCoin(COIN_TYPE_INBRICK);

		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET tmp_ani_set = animation_sets->Get(COIN_ANI_SET_ID);

		item->SetAnimationSet(tmp_ani_set);
		item->SetPosition(-50, 100);

		qbscene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		qbscene->PushBack(item);
	}
	if (itemtype == ITEM_LEAF)
	{
		item = new CLeaf();

		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET tmp_ani_set = animation_sets->Get(LEAF_ANI_SET_ID);

		item->SetAnimationSet(tmp_ani_set);
		item->SetPosition(-50, 100);

		qbscene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		qbscene->PushBack(item);
	}
	if (itemtype == ITEM_MUSHROOM_RED || itemtype == ITEM_MUSHROOM_GREEN)
	{
		item = new CMushRoom();

		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET tmp_ani_set = animation_sets->Get(MUSHROOM_ANI_SET_ID);

		item->SetAnimationSet(tmp_ani_set);
		item->SetPosition(-50, 100);

		qbscene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		qbscene->PushBack(item);
	}
	if (itemtype == ITEM_SWITCH)
	{
		item = new CSwitch();

		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET tmp_ani_set = animation_sets->Get(SWITCH_ANI_SET_ID);

		item->SetAnimationSet(tmp_ani_set);
		item->SetPosition(-50, 100);

		qbscene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		qbscene->PushBack(item);
	}
}