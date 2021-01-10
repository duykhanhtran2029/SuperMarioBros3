#include "Tail.h"
#include "Mario.h"
#include "Game.h"
#include "PlayScene.h"
void CTail::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + TAIL_BBOX_WIDTH;
	b = t + TAIL_BBOX_HEIGHT;
}
void CTail::Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects)
{
	CMario* mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (mario->nx > 0)
	{
		if (mario->turning_state == 1 || mario->turning_state == 5)
			x = mario->x - TAIL_BBOX_WIDTH;
		else if (mario->turning_state == 3)
			x = mario->x + MARIO_BIG_BBOX_WIDTH;
		else x = mario->x + 2;
	}	
	else
	{
		if (mario->turning_state == 1 || mario->turning_state == 5)
			x = mario->x + MARIO_BIG_BBOX_WIDTH;
		else if (mario->turning_state == 3)
			x = mario->x - TAIL_BBOX_WIDTH;
		else x = mario->x + 2;
	}
	//x = mario->x - TAIL_BBOX_WIDTH;
	y = mario->y + TAIL_BBOX_WIDTH * 2 + 1;
}