#include "BackUp.h"
#include "PlayScene.h"


CBackUp* CBackUp::__instance = NULL;
CBackUp* CBackUp::GetInstance()
{
	if (__instance == NULL) __instance = new CBackUp();
	return __instance;
}

void CBackUp::BackUpMario(CMario* mario)
{
	life = mario->life;
	score = mario->score;
	level = mario->level;
	money = mario->money;
	scene = CGame::GetInstance()->GetCurrentScene()->GetId();
	cards = mario->cards;
}

void CBackUp::LoadBackUp(CMario* mario)
{
	mario->life = life;
	mario->score = score;
	mario->level = level;
	mario->money = money;
	mario->cards = cards;
}
