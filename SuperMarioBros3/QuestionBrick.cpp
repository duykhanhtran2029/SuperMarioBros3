#include "QuestionBrick.h"
#include "PlayScence.h"
#include "MushRoom.h"
#include "Leaf.h"
CMario* qbmario;
CPlayScene* qbscence;
CQuestionBrick::CQuestionBrick(int tag)
{
	state = QUESTIONBRICK_STATE_IDLE;
	if (tag == ITEM_COIN)
		CreateItem(ITEM_TYPE_COIN);
}
void CQuestionBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
	if (state == QUESTIONBRICK_STATE_IDLE)
	{
		float mLeft, mTop, mRight, mBottom;
		float oLeft, oTop, oRight, oBottom;
		qbscence = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		if (qbscence != NULL)
			qbmario = ((CPlayScene*)qbscence)->GetPlayer();
		if (qbmario != NULL)
		{
			qbmario->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			GetBoundingBox(oLeft, oTop, oRight, oBottom);
			if ((oLeft == ceil(mRight) || oRight == floor(mLeft)) && qbmario->level == MARIO_LEVEL_TAIL && qbmario->isTurningTail)
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
			if (dynamic_cast<CCoin*>(item) && tag == ITEM_COIN)
			{			
				CCoin *obj = dynamic_cast<CCoin*>(item);
				obj->isAppear = true;
				obj->SetPosition(x, y - COIN_BBOX_HEIGHT - 1);
				obj->SetState(COIN_STATE_UP);
			}
			else
			{
				CPlayScene* scence = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
				CMario* mario = ((CPlayScene*)scence)->GetPlayer();
				if (mario->level == MARIO_LEVEL_SMALL)
				{
					CreateItem(ITEM_TYPE_MUSHROOM);
					CMushRoom* obj = dynamic_cast<CMushRoom*>(item);
					obj->isAppear = true;
					obj->SetPosition(x, y);
					obj->SetState(MUSHROOM_STATE_UP);
				}
				if (mario->level == MARIO_LEVEL_BIG)
				{
					CreateItem(ITEM_TYPE_LEAF);
						CLeaf* obj = dynamic_cast<CLeaf*>(item);
						obj->isAppear = true;
						obj->SetPosition(x, y);
						obj->SetState(LEAF_STATE_UP);
				}
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
		ani = QUESTIONBRICK_ANI_IDLE;
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}
void CQuestionBrick::CreateItem(int itemtype)
{
	if (itemtype == ITEM_TYPE_COIN)
	{
		item = new CCoin(COIN_TYPE_INBRICK);
		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET tmp_ani_set = animation_sets->Get(COIN_ANI_SET_ID);
		item->SetAnimationSet(tmp_ani_set);
		item->SetPosition(-50, 100);
		CPlayScene* ps = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		ps->PushBack(item);
	}
	else if (itemtype == ITEM_TYPE_LEAF)
		{
			item = new CLeaf();
			CAnimationSets* animation_sets = CAnimationSets::GetInstance();
			LPANIMATION_SET tmp_ani_set = animation_sets->Get(LEAF_ANI_SET_ID);
			item->SetAnimationSet(tmp_ani_set);
			item->SetPosition(-50, 100);
			CPlayScene* ps = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
			ps->PushBack(item);
		}
		else if (itemtype == ITEM_TYPE_MUSHROOM)
			{
				item = new CMushRoom();
				CAnimationSets* animation_sets = CAnimationSets::GetInstance();
				LPANIMATION_SET tmp_ani_set = animation_sets->Get(MUSHROOM_ANI_SET_ID);
				item->SetAnimationSet(tmp_ani_set);
				item->SetPosition(-50, 100);
				CPlayScene* ps = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
				ps->PushBack(item);
			}
}