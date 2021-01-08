#pragma once

#include <d3dx9.h>
#include "KeyEventHandler.h"
#include "Font.h"
#define	INTROSCENE	0
#define	WORLDSCENE	1
#define	PLAYSCENE	2

class CScene
{
protected:
	CKeyEventHandler * key_handler;
	int id;
	LPCWSTR sceneFilePath;
	CFont* fonts = NULL;
public: 
	CScene(int id, LPCWSTR filePath);
	bool isUnLoaded = false;
	CKeyEventHandler * GetKeyEventHandler() { return key_handler; }
	virtual void Load() = 0;
	virtual void Unload() = 0;
	virtual void Update(DWORD dt) = 0;
	virtual void Render() = 0; 
	CFont* GetFont() { return fonts; }
};
typedef CScene * LPSCENE;


class CSceneKeyHandler : public CKeyEventHandler
{
protected: 
	CScene * scene; 

public:
	virtual void KeyState(BYTE *states) = 0;
	virtual void OnKeyDown(int KeyCode) = 0;
	virtual void OnKeyUp(int KeyCode) = 0;
	CSceneKeyHandler(CScene *s) :CKeyEventHandler() { scene = s; }
};