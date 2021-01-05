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
#include "Font.h"
#include "HUD.h"

class CPlayScene : public CScene
{
protected:
	CMario* player = NULL;					// A play scene has to have player, right? 
	CMap* current_map = NULL;
	CFont* fonts = NULL;
	HUD* hud = NULL;
	vector<LPGAMEOBJECT> objects;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_TILEMAP_DATA(string line);


public:
	CPlayScene(int id, LPCWSTR filePath);
	virtual void Load();
	virtual void SetCam(float cx, float cy);
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();
	void PushBack(CGameObject* obj) { objects.push_back(obj); }
	vector<LPGAMEOBJECT> GetObjects() { return objects; }

	CMario* GetPlayer() { return player; }
	CMap* GetMap() { return current_map; }
	CFont* GetFont() { return fonts; }
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