#include "StatusBoard.h"
#include"time.h"
#include "Game.h"
#include "Sprites.h"
#include "Define.h"

#define DEFAULT_TIME_PLAY 300
#define DEFAULT_MARIO_LIFES 4
StatusBoard::StatusBoard(CMario* mario)
{
	Reset();
}

StatusBoard::~StatusBoard()
{
}

void StatusBoard::Update(DWORD dt)
{
		time += dt;
}

void StatusBoard::Render()
{
	RECT rect;
	SetRect(&rect, 0, 15, SCREEN_WIDTH, STATUS_BOARD_HEIGHT);

	if (font != NULL)
		font->DrawTextA(NULL, info.c_str(), -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 255));
}


void StatusBoard::SetFont(ID3DXFont* font)
{
	this->font = font;
}

string StatusBoard::FillStringWithZeros(string str, int totalChar)
{
	while (str.length() < totalChar) str = "0" + str;
	return str;
}

int StatusBoard::GetRemainingTime()
{
	return DEFAULT_TIME_PLAY - time / CLOCKS_PER_SEC;
}

void StatusBoard::Reset()
{
	time = 0;
}

DWORD StatusBoard::GetTime()
{
	return time;
}

void StatusBoard::SetTime(DWORD time)
{
	this->time = time;
}


