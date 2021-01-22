#include "QuestionBrick.h"
#include "PlayScene.h"
#include "MushRoom.h"
#include "Switch.h"
#include "Leaf.h"

CQuestionBrick::CQuestionBrick(int tag, int type)
{
	state = QUESTIONBRICK_STATE_IDLE;
	this->tag = tag;
	this->type = type;
}
void CQuestionBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	y += dy;
	if (state == QUESTIONBRICK_STATE_IDLE)
	{
		CMario* mario = {};
		float mLeft, mTop, mRight, mBottom;
		float oLeft, oTop, oRight, oBottom;
		if (scene != NULL)
			mario = ((CPlayScene*)scene)->GetPlayer();
		if (mario != NULL && mario->isTurningTail && mario->level == MARIO_LEVEL_TAIL)
		{
			mario->getTail()->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			GetBoundingBox(oLeft, oTop, oRight, oBottom);
			if (isColliding(floor(mLeft), mTop, ceil(mRight), mBottom) && mario->getTail()->hit_times == 0)
			{
				SetState(QUESTIONBRICK_STATE_HIT);
				mario->getTail()->hit_times = 1;
			}

		}
		//if (mario != NULL && scene->GetId() == WORLD_1_4)
		//{
		//	mario->GetBoundingBox(mLeft, mTop, mRight, mBottom);
		//	GetBoundingBox(oLeft, oTop, oRight, oBottom);
		//	if (isColliding(floor(mLeft), mTop, ceil(mRight), mBottom))
		//	{
		//		if (ceil(mRight) == oLeft)
		//			mario->isBlocked = true;
		//	}
		//}
	}
	if (isBeingPushedUp && start_y - y >= QUESTIONBRICK_PUSH_MAX_HEIGHT)
	{
		y = start_y - QUESTIONBRICK_PUSH_MAX_HEIGHT;
		StopPushedUp();
	}
	if (isFallingDown && y >= start_y)
	{
		y = start_y;
		isFallingDown = false;
		vy = 0;
		if (tag == ITEM_MUSHROOM_GREEN)
		{
			CreateItem(ITEM_MUSHROOM_GREEN);
			item->tag = MUSHROOM_TYPE_GREEN;

			CMushRoom* obj = dynamic_cast<CMushRoom*>(item);
			obj->isAppear = true;
			obj->SetPosition(x, y);
			obj->SetState(MUSHROOM_STATE_UP);
			scene->PushBack(item);
		}
		if (tag == ITEM_MUSHROOM_RED)
		{
			CreateItem(ITEM_MUSHROOM_RED);
			item->tag = MUSHROOM_TYPE_RED;

			CMushRoom* obj = dynamic_cast<CMushRoom*>(item);
			obj->isAppear = true;
			obj->SetPosition(x, y);
			obj->SetState(MUSHROOM_STATE_UP);
			scene->PushBack(item);
		}
	}
}
void CQuestionBrick::SetState(int state = BRICK_STATE_IDLE)
{
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	CMario* mario = {};
	if (scene != NULL)
		mario = ((CPlayScene*)scene)->GetPlayer();
	switch (state)
	{
	case QUESTIONBRICK_STATE_HIT:
		if (this->state != QUESTIONBRICK_STATE_HIT && mario != nullptr)
		{
			StartPushedUp();
			if (items > 0)
				items--;
			if (tag == ITEM_COIN)
			{
				CreateItem(ITEM_COIN);
				CCoin* obj = dynamic_cast<CCoin*>(item);
				obj->isAppear = true;
				obj->SetPosition(x, y - COIN_BBOX_HEIGHT - 1);
				obj->SetState(COIN_STATE_UP);
				scene->PushBack(item);
			}
			if (tag == ITEM_LEAF || (tag == ITEM_CUSTOM && mario->level == MARIO_LEVEL_BIG))
			{
				CreateItem(ITEM_LEAF);
				CLeaf* obj = dynamic_cast<CLeaf*>(item);
				obj->isAppear = true;
				obj->SetPosition(x, y);
				obj->SetState(LEAF_STATE_UP);
				scene->PushBack(item);
			}
			if (tag == ITEM_CUSTOM)
			{
				if (mario->level == MARIO_LEVEL_SMALL)
					tag = ITEM_MUSHROOM_RED;
				if (mario->level == MARIO_LEVEL_TAIL || mario->level == MARIO_LEVEL_FIRE)
					tag = ITEM_MUSHROOM_GREEN;
			}
			if (tag == ITEM_SWITCH)
			{
				CreateItem(ITEM_SWITCH);
				CSwitch* obj = dynamic_cast<CSwitch*>(item);
				obj->isAppear = true;
				obj->SetPosition(x, y);
				obj->SetState(SWITCH_STATE_UP);
				scene->PushBack(item);
			}
		}
		break;
	}
	if(items == 0)
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
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	if (itemtype == ITEM_COIN)
	{
		item = new CCoin(COIN_TYPE_INBRICK);
		item->SetType(IGNORE);
		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET tmp_ani_set = animation_sets->Get(COIN_ANI_SET_ID);

		item->SetAnimationSet(tmp_ani_set);

	}
	if (itemtype == ITEM_LEAF)
	{
		item = new CLeaf();
		item->SetType(IGNORE);
		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET tmp_ani_set = animation_sets->Get(LEAF_ANI_SET_ID);

		item->SetAnimationSet(tmp_ani_set);
	}
	if (itemtype == ITEM_MUSHROOM_RED || itemtype == ITEM_MUSHROOM_GREEN)
	{
		item = new CMushRoom();
		item->SetType(IGNORE);
		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET tmp_ani_set = animation_sets->Get(MUSHROOM_ANI_SET_ID);

		item->SetAnimationSet(tmp_ani_set);
	}
	if (itemtype == ITEM_SWITCH)
	{
		item = new CSwitch();

		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET tmp_ani_set = animation_sets->Get(SWITCH_ANI_SET_ID);

		item->SetAnimationSet(tmp_ani_set);
	}
}