#pragma once
#include <d3d9.h>
#include"Textures.h"
#include "Sprites.h"
#include "Utils.h"

#define TILE_WIDTH 16
#define TILE_HEIGHT 16
class CMap
{
	int TotalRowsOfMap, TotalColumnsOfMap;
	int TotalRowsOfTileSet, TotalColumnsOfTileSet;
	int TotalTiles;	//total tiles of tile set
	LPDIRECT3DTEXTURE9 TileSet; //sceneX_bank.png
	vector<LPSPRITE> Tiles;
	int** TileMap;
	float CamX, CamY;
public:
	CMap(int TileSetID, int TotalRowsOfMap, int TotalColumnsOfMap, int TotalRowsOfTileSet, int  TotalColumnsOfTileSet, int TotalTiles);
	~CMap();
	void Render();
	void SetTileMapData(int** TileMapData);
	void ExtractTileFromTileSet();
	void SetCamPos(float x, float y) { CamX = x; CamY = y; }
	//void GetCamPos() { DebugOut(L"CamX = %f,	CamY = %f\n", CamX, CamY); }
	int GetMapWidth();
	int GetMapHeight();
};

