#include "BackUp.h"


CBackUp* CBackUp::__instance = NULL;
CBackUp* CBackUp::GetInstance()
{
	if (__instance == NULL) __instance = new CBackUp();
	return __instance;
}

void CBackUp::BackUpMario(CMario * mario)
{
	life = mario->life;
	score = mario->score;
	level = mario->level;
	state = mario->state;
	money = mario->money;
}

void CBackUp::LoadBackUp(CMario* mario)
{
	mario->life = life;
	mario->score = score;
	mario->level = level;
	mario->state = state;
	mario->money = money;
}
