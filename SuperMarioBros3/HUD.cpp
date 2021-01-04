#include "HUD.h"
#include"Game.h"
#include"Textures.h"
#include"PlayScence.h"

void HUD::Render()
{
	animation_set->at(0)->Render(x, y);
}
void HUD::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();

	x = game->GetCamX();
	y = game->GetCamY() + SCREEN_HEIGHT;
	
}
