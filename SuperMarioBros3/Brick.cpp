#include "Brick.h"

void CBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
	if (isBeingPushedUp && GetTickCount64() - pushup_start >= BRICK_PUSHED_TIME)
	{
		y -= BRICK_PUSH_HEIGHT_STACK;
		PushedStacks++;
		pushup_start = GetTickCount64();
		if (PushedStacks >= BRICK_PUSH_MAX_STACK)
		{
			PushedStacks = BRICK_PUSH_MAX_STACK;
			StopPushedUp();
		}
	}
	if (isFallingDown && GetTickCount64() - pushup_start >= BRICK_PUSHED_TIME)
	{
		y += BRICK_PUSH_HEIGHT_STACK;
		PushedStacks--;
		pushup_start = GetTickCount64();
		if (PushedStacks <= 0)
		{
			PushedStacks = 0;
			isFallingDown = false;
		}
	}
}
void CBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BRICK_BBOX_WIDTH;
	b = y + BRICK_BBOX_HEIGHT;
}
void CBrick::SetState(int state = BRICK_STATE_IDLE)
{
	switch (state)
	{
	case BRICK_STATE_QUESTION_HIT:
		if (this->state != BRICK_STATE_QUESTION_HIT)
		{
			StartPushedUp();
		}
		break;
	case BRICK_STATE_IDLE:
		break;
	}
	CGameObject::SetState(state);
}
void CBrick::Render()
{
	int ani = -1;
	if (state == BRICK_STATE_QUESTION_HIT)
		ani = BRICK_ANI_QUESTION_NOMARK;
	else
		ani = BRICK_ANI_IDLE;
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}