#pragma once
#include"Mario.h"
#include "Define.h"
class CBackUp
{
	static CBackUp* __instance;
	int life = 4;
	int money = 0;
	DWORD time = 0;
	int score = 0;
	int level = MARIO_LEVEL_SMALL;
	int state = MARIO_STATE_IDLE;
public:
	static CBackUp* GetInstance();
	void BackUpMario(CMario* mario);
	void LoadBackUp(CMario* mario);
};
