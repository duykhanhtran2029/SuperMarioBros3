#include "HUD.h"
#include"Game.h"
#include"Textures.h"
#include"PlayScence.h"
HUD::HUD()
{
	CGame* game = CGame::GetInstance();
	CSprites* sprite = CSprites::GetInstance();
	CPlayScene* scence = (CPlayScene*)game->GetCurrentScene();
	PSprite = sprite->sprites[SPRIT_P_ID];
	mario = scence->GetPlayer();
	if(mario->tag == MARIO)
		playerSprite = CSprites::GetInstance()->sprites[SPRIT_ICONMARIO_ID];
	else
		playerSprite = CSprites::GetInstance()->sprites[SPRIT_ICONLUIGI_ID];

	for (unsigned int i = 0; i < MARIO_RUNNING_STACKS - 1; i++)
		powerMelterSprite.push_back((sprite->Get(SPRIT_FILLARROW_ID)));
}
void HUD::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();
	CPlayScene* scence = (CPlayScene*)game->GetCurrentScene();
	CFont* fonts = scence->GetFont();

	nlife = mario->life;
	runningStacks = mario->RunningStacks;
	score = mario->score;
	money = mario->money;
	time += dt;
	remainTime = DEFAULT_TIME - time/1000;

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
	for (int i = 1; i <= runningStacks; i++)
		if(i == MARIO_RUNNING_STACKS)
			PSprite->Draw(x + 107, y + 8);
		else
			powerMelterSprite[i-1]->Draw(x + FONT_BBOX_WIDTH * (i-1) + 60, y + 8);
}
