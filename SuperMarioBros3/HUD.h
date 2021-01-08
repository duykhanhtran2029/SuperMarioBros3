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

#define PLAYSCENE_HUD	0
#define WORLDSCENE_HUD	1

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

	int type_hud;
public:
	HUD(int type_hud = 0);
	void SetHUD(HUD* hud);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) {};
	void Clear(){
		for (size_t i = 0; i < cardSprite.size(); i++)
			delete cardSprite[i];
		cardSprite.clear();
		for (size_t i = 0; i < lifeSprite.size(); i++)
			delete lifeSprite[i];
		lifeSprite.clear();
		for (size_t i = 0; i < moneySprite.size(); i++)
			delete moneySprite[i];
		moneySprite.clear();
		for (size_t i = 0; i < scoreSprite.size(); i++)
			delete scoreSprite[i];
		scoreSprite.clear();
		for (size_t i = 0; i < remainTimeSprites.size(); i++)
			delete remainTimeSprites[i];
		remainTimeSprites.clear();
		for (size_t i = 0; i < powerMelterSprite.size(); i++)
			delete powerMelterSprite[i];
		powerMelterSprite.clear();
	}
};

