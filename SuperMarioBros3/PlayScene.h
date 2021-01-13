#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"
#include "GameObject.h"
#include "Brick.h"
#include "Mario.h"
#include "Goomba.h"
#include "Koopas.h"
#include "Map.h"
#include "HUD.h"

#define GAMEDONE1_SPRITE_ID		50070
#define GAMEDONE2_SPRITE_ID		50071

#define GAMEDONE1_X		2630
#define GAMEDONE1_Y		240
#define GAMEDONE2_X		2620
#define GAMEDONE2_Y		270

class CPlayScene : public CScene
{
protected:
	CMario* player = NULL;					// A play scene has to have player, right? 
	CMap* current_map = NULL;
	HUD* hud = NULL;
	vector<LPGAMEOBJECT> objects;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_TILEMAP_DATA(string line);

	bool isTurnOnCamY = false;
	LPSPRITE gamedone1 = nullptr;
	LPSPRITE gamedone2 = nullptr;
	//DWORD lastDt = 0;

public:
	bool isGameDone1 = false;
	bool isGameDone2 = false;
	CPlayScene(int id, LPCWSTR filePath);
	virtual void Load();
	virtual void SetCam(float cx, float cy);
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();
	void PushBack(CGameObject* obj) { objects.push_back(obj); }
	vector<LPGAMEOBJECT> GetObjects() { return objects; }
	CMap* GetMap() { return current_map; }
	HUD* GetHUD() { return hud; }
	CMario* GetPlayer() { return player; }
	void SetPlayer(CMario* m) { player = m; }
	void PutPlayer(CMario* m)
	{
		if (dynamic_cast<CMario*> (objects[0]))
		{
			//CMario* temp = (CMario*)objects[0];
			objects[0] = m;
			//delete temp;
		}
	}
	//friend class CPlaySceneKeyHandler;
};

class CPlaySceneKeyHandler : public CSceneKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	CPlaySceneKeyHandler(CScene* s) :CSceneKeyHandler(s) {};
};