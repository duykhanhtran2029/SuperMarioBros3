#pragma once
#include"GameObject.h"
#include"Sprites.h"
#include"Mario.h"
#include"Font.h"

#define FONT_BBOX_WIDTH						8
#define DEFAULT_TIME						300

#define SPRITE_HUD_ID						99999
#define SPRIT_ICONMARIO_ID					50003
#define SPRIT_ICONLUIGI_ID					50004
#define SPRIT_FILLARROW_ID					50005
#define SPRIT_P_ID							50006

class HUD: public CGameObject
{
	CMario* mario = NULL;

	LPSPRITE playerSprite;
	vector<LPSPRITE> cardSprite;
	vector<LPSPRITE> lifeSprite;
	vector<LPSPRITE> moneySprite;
	vector<LPSPRITE> scoreSprite;
	vector<LPSPRITE> remainTimeSprites;
	vector<LPSPRITE> powerMelterSprite;
	LPSPRITE PSprite;
	CAnimation* animationTakenCard;


	int nlife = 0;
	int remainTime = DEFAULT_TIME;
	int score = 0;
	int money = 0;
	int time = 0;
	int runningStacks = 0;

	int doneScenePoint;
	int indexTakenCard;
	int idTakenCard;
	int isGameDone = false;
	vector<int> cards;
public:
	HUD();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) {};
};

