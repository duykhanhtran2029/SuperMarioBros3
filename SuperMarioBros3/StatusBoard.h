#pragma once
#include"Mario.h"
#include <string>
#include <d3dx9.h>
#include <d3d9.h>
using namespace std;

class StatusBoard
{
	CMario* mario;

	string info;
	ID3DXFont* font;
	DWORD time;
	DWORD timeAccumulated;

	LPANIMATION_SET animation_set;
	LPANIMATION_SET item_animation_set;

public:
	StatusBoard(CMario* mario);
	~StatusBoard();
	void Update(DWORD dt);
	void Render();
	void SetFont(ID3DXFont* font);
	string FillStringWithZeros(string str, int totalChar);
	int GetRemainingTime();
	void Reset();
	DWORD GetTime();
	void SetTime(DWORD time);
	void SetSceneId(int id);
};

