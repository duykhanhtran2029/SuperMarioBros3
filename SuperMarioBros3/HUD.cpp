#include "HUD.h"
#include"Game.h"
#include"Textures.h"
#include"PlayScene.h"
#include"WorldScene.h"
HUD::HUD(int type_hud)
{
	nlife = 0;
	remainTime = DEFAULT_TIME;
	score = 0;
	money = 0;
	time = 0;
	runningStacks = 0;

	this->type_hud = type_hud;
	if (type_hud == PLAYSCENE_HUD)
	{
		CGame* game = CGame::GetInstance();
		CSprites* sprite = CSprites::GetInstance();
		CPlayScene* scene = (CPlayScene*)game->GetCurrentScene();
		PSprite = sprite->sprites[SPRIT_P_ID];
		mario = scene->GetPlayer();
		if (mario != NULL && mario->tag == MARIO)
			playerSprite = CSprites::GetInstance()->sprites[SPRIT_ICONMARIO_ID];
		else
			playerSprite = CSprites::GetInstance()->sprites[SPRIT_ICONLUIGI_ID];

		for (unsigned int i = 0; i < MARIO_RUNNING_STACKS - 1; i++)
			powerMelterSprite.push_back((sprite->Get(SPRIT_FILLARROW_ID)));
	}
	else
	{
		CSprites* sprite = CSprites::GetInstance();
		PSprite = sprite->sprites[SPRIT_P_ID];
		playerSprite = CSprites::GetInstance()->sprites[SPRIT_ICONMARIO_ID];
	}
}
void HUD::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();
	CFont* fonts = game->GetCurrentScene()->GetFont();
	if (mario != NULL && type_hud == PLAYSCENE_HUD)
	{
		nlife = mario->life;
		runningStacks = mario->RunningStacks;
		score = mario->score;
		money = mario->money;
		time += dt;
		remainTime = DEFAULT_TIME - time / 1000;
	}
	string time_str = to_string(remainTime);
	while (time_str.length() < 3) time_str = "0" + time_str;
	remainTimeSprites = fonts->StringToSprite(time_str);

	string score_str = to_string(score);
	while (score_str.length() < 7) score_str = "0" + score_str;
	scoreSprite = fonts->StringToSprite(score_str);

	lifeSprite = fonts->StringToSprite(to_string(nlife));
	moneySprite = fonts->StringToSprite(to_string(money));
}
void HUD::Render()
{
	CSprites::GetInstance()->sprites[SPRITE_HUD_ID]->Draw(x,y);
	playerSprite->Draw(x + 11, y + 16);
	for (unsigned int i = 0; i < remainTimeSprites.size(); i++)
		remainTimeSprites[i]->Draw(x + FONT_BBOX_WIDTH * i + 132, y + 16);
	for (unsigned int i = 0; i < scoreSprite.size(); i++)
		scoreSprite[i]->Draw(x + FONT_BBOX_WIDTH * i + 60, y + 16);
	for (unsigned int i = 0; i < moneySprite.size(); i++)
		moneySprite[i]->Draw(x + FONT_BBOX_WIDTH * i + 140, y + 8);
	for (unsigned int i = 0; i < lifeSprite.size(); i++)
		lifeSprite[i]->Draw(x + FONT_BBOX_WIDTH * i + 36, y + 16);
	if (mario != NULL &&type_hud == PLAYSCENE_HUD)
	{
		for (int i = 1; i <= runningStacks; i++)
			if (i == MARIO_RUNNING_STACKS)
				PSprite->Draw(x + 107, y + 8);
			else
				powerMelterSprite[i - 1]->Draw(x + FONT_BBOX_WIDTH * (i - 1) + 60, y + 8);
	}
}
void HUD::SetHUD(HUD* hud)
{
	nlife = hud->nlife;
	remainTime = hud->remainTime;
	score = hud->score;
	money = hud->money;
	time = hud->time;
	runningStacks = hud->runningStacks;
}
