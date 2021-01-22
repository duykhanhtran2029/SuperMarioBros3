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
#include "Grid.h"

#define GAMEDONE1_SPRITE_ID		50070
#define GAMEDONE2_SPRITE_ID		50071


#define WORLD_1			0
#define WORLD_INTRO		6
#define WORLD_1_1		1
#define WORLD_1_4		4

#define CAM_CHANGE_TIME		30

class CPlayScene : public CScene
{
protected:
	CMario* player = NULL;					// A play scene has to have player, right? 
	CMap* current_map = NULL;
	HUD* hud = NULL;

	vector<LPGAMEOBJECT> objects;
	vector<Unit*> units;
	vector<LPGAMEOBJECT> objectsRenderFirst; //enemies // items pushed up
	vector<LPGAMEOBJECT> objectsRenderSecond; // statics
	vector<LPGAMEOBJECT> objectsRenderThird; // items fall down

	Grid* grid;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_TILEMAP_DATA(string line);
	void ParseObjFromFile(LPCWSTR path);

	bool isTurnOnCamY = false;
	LPSPRITE gamedone1 = nullptr;
	LPSPRITE gamedone2 = nullptr;
	//DWORD lastDt = 0;


public:
	bool isGameDone1 = false;
	bool isGameDone2 = false;
	bool isGameDone3 = false;
	DWORD sum_dt = 0;
	CPlayScene(int id, LPCWSTR filePath);
	virtual void Load();
	virtual void SetCam(float cx, float cy, DWORD dt = 0);
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();
	void LoadBackUp();
	void BackUpPlayer();
	void PushBack(CGameObject* obj) 
	{
		Unit* unit = new Unit(grid, obj, obj->x, obj->y);
	}
	vector<LPGAMEOBJECT> GetObjects() { return objects; }
	CMap* GetMap() { return current_map; }
	HUD* GetHUD() { return hud; }
	CMario* GetPlayer() { return player; }
	Grid* GetGrid() { return grid; }
	void SetPlayer(CMario* m) { player = m; }
	void PutPlayer(CMario* m)
	{
		if (dynamic_cast<CMario*> (objects[0]))
			objects[0] = m;
	}
	int CalScore();
	void SetActivation();
	void SetInactivation();
	//grid
	void GetObjectFromGrid();
	void UpdateGrid();
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