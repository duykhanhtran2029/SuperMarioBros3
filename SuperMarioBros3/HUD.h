#pragma once
#include"GameObject.h"
#include"Sprites.h"
#include"Mario.h"
#include"Font.h"

#define FONT_BBOX_WIDTH						8
#define SPRITE_HUD1_ID						50001
#define SPRITE_HUD2_ID						50002
#define SPRITE_POWERMELTER_ID				50007
#define SPRITE_POWEMELTER_ARROW_ID			50008
#define SPRITE_FILLED_POWERMELTER_ID		50005
#define SPRITE_FILLED_POWERMETER_ARROW_ID	50006
#define DEFAULT_TIME						300
#define SPRITE_MARIO_PLAYER_ID				50003
#define SPRITE_LUGI_PLAYER_ID				50004
#define HUD_BBOX_HEIGHT						58

class HUD : public CGameObject
{
	CFont* font;
	CMario* mario = NULL;



	LPSPRITE hud1;
	LPSPRITE hud2;
	vector<LPSPRITE> cardSprite;
	LPSPRITE playerSprite;
	LPSPRITE lifeSprite;
	vector<LPSPRITE> moneySprite;
	LPSPRITE worldSprite;
	vector<LPSPRITE> scoreSprite;
	vector<LPSPRITE> remainTimeSprites;
	vector<LPSPRITE> powerMelterSprite;
	vector<LPSPRITE> filledPowerMelterSprite;
	CAnimation* animationTakenCard;


	int nlife = 4;
	int remainTime = 0;
	int score = 0;
	int money = 0;
	int world = 1;
	int time = 0;
	int powerMelterStack = 0;

	int doneScenePoint;
	int indexTakenCard;
	int idTakenCard;
	int isGameDone = false;
	vector<int> card;
public:
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();
};

