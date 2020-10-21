#pragma once
#include <d3d9.h>
#include"Textures.h"
#include "Sprites.h"

#define TILE_WIDTH 16
#define TILE_HEIGHT 16
class Map
{
	int TotalRowsOfMap, TotalColumnsOfMap;
	int TotalRowsOfTileSet, TotalColumnsOfTileSet;
	int TotalTiles;	//total tiles of tile set

	LPDIRECT3DTEXTURE9 TileSet; //sceneX_bank.png
	vector<LPSPRITE> Tiles;
	int** TileMap;
public:
	Map(int TileSetID, int TotalRowsOfMap, int TotalColumnsOfMap, int TotalRowsOfTileSet, int  TotalColumnsOfTileSet, int TotalTiles);
	~Map();
	void Render();
	void SetTileMapData(int** TileMapData);
	void ExtractTileFromTileSet();
	int GetMapWidth();
	int GetMapHeight();
};

