#pragma once
#include"Scene.h"
#include"Game.h"
#include"Goomba.h"

#define SCROLLING_STACK_TIME	10
#define SCROLLING_TIME			2000
#define ANI_TIME				500
#define SWITCH_TIME				1000
#define JUMP_TIME				300
#define GROUND_WIDTH			256

#define BANNER_HEIGHT			187
#define BANNER_STACK_HEIGHT		5
#define MENU_STACK_HEIGHT		16

#define THREE_X		112
#define THREE_Y		96

#define BUSH_X		193
#define BUSH_Y		88

#define ARROW_X		70
#define ARROW_Y		144

#define HIT_ID			14023
#define LOOKUP_ID		14024
#define BUSH_ID			50009
#define ANISET_ARROW_ID	51


class CIntroScene :public CScene
{
	DWORD changeAniTime = 0;
	DWORD jump_start = 0;
	DWORD switch_start = 0;
	CGoomba* goomba;
	vector<LPGAMEOBJECT> fallingObjects;
	vector<LPGAMEOBJECT> koopas;
	LPANIMATION_SET BackGround;
	LPANIMATION_SET Arrow;
	LPANIMATION THREE;
	vector<LPGAMEOBJECT> objects;

	CMario* player = NULL;

	int ani_section = 0;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);

	DWORD srolling_start = 0;
	DWORD slidedown_start = 0;
	float point = 0;
public:
	CIntroScene(int id, LPCWSTR filePath);
	CMario* mario = nullptr;
	CMario* luigi = nullptr;
	CMario* holder = nullptr;
	CKoopas* shell = nullptr;
	int section = 0; // 1 2 3
	int mini_section = 0;
	bool isCustomSpeed = false;
	bool isReadyToJump = false;
	bool Revenge = false; //:"))))))
	void StartFirstSection()
	{
		section = 1;
		point = 0;
	}
	void StartSecondSection();
	void StartThirdSection();
	//void ShowKoopas();
	//void ShowMenu();
	//void SwitchMenu();


	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();
	CMario* GetPlayer() { return player; }
	void SetShell(CKoopas* k) { shell = k; }
	void StartChangeAni() 
	{ 
		if (ani_section == 0)
		{
			changeAniTime = GetTickCount64(); 
			ani_section++;
		}
	}
	void StartJump()
	{
		jump_start = GetTickCount64();
	}
	void ReleaseGoomba()
	{
		goomba->vx = -GOOMBA_WALKING_SPEED/3;
	}
	void StartSwitch()
	{
		switch_start = GetTickCount64();
	}
	void CalColliableObjects(LPGAMEOBJECT curobj, vector<LPGAMEOBJECT>* coObjects = NULL);
	friend class IntroSceneHandler;
};
class IntroSceneKeyHandler : public CSceneKeyHandler
{
public:
	virtual void KeyState(BYTE* states) {};
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode) {};

	IntroSceneKeyHandler(CScene* s) :CSceneKeyHandler(s) {};
};
