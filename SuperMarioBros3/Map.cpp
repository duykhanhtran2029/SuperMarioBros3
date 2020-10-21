#include "Map.h"
#include "Utils.h"
#include "Game.h"
Map::Map(int TileSetID, int TotalRowsOfMap, int TotalColumnsOfMap, int TotalRowsOfTileSet, int  TotalColumnsOfTileSet, int TotalTiles)
{
	TileSet = CTextures::GetInstance()->Get(TileSetID);
	this->TotalRowsOfMap = TotalRowsOfMap;
	this->TotalColumnsOfMap = TotalColumnsOfMap;
	this->TotalRowsOfTileSet = TotalRowsOfTileSet;
	this->TotalColumnsOfTileSet = TotalColumnsOfTileSet;
	this->TotalTiles = TotalTiles;
}

Map::~Map()
{
}

void Map::Render()
{
	int FirstColumn = floor(CamX / TILE_WIDTH);
	int LastColumn = ceil( (CamX + CGame::GetInstance()->GetScreenWidth()) / TILE_WIDTH);
	if (LastColumn >= TotalColumnsOfMap)
		LastColumn = TotalColumnsOfMap - 1;
	DebugOut(L"FirstColumn = %d, LastColumn = %d\n", FirstColumn, LastColumn);
	int counttiles = 0;
	for (int CurrentRow = 0; CurrentRow < TotalRowsOfMap; CurrentRow++)
		for (int CurrentColumn = FirstColumn; CurrentColumn <= LastColumn; CurrentColumn++)
		{
			Tiles.at(TileMap[CurrentRow][CurrentColumn] - 1)->Draw(CurrentColumn * TILE_WIDTH, CurrentRow * TILE_HEIGHT);
			counttiles++;
			DebugOut(L"Current Row = %d, Current Column = %d\n", CurrentRow, CurrentColumn);
		}
	DebugOut(L"Rendered Tiles = %d\n", counttiles);
			// Tiles.at(6)->Draw(CurrentColumn * TILE_WIDTH, CurrentRow * TILE_HEIGHT);
}

void Map::SetTileMapData(int** TileMapData)
{
	TileMap = TileMapData;
}


void Map::ExtractTileFromTileSet()
{
	for (int TileNum = 0; TileNum < TotalTiles; TileNum++)
	{
		int left = TileNum % TotalColumnsOfTileSet * TILE_WIDTH;
		int top = TileNum / TotalColumnsOfTileSet * TILE_HEIGHT;
		int right = left + TILE_WIDTH;
		int bottom = top + TILE_HEIGHT;
		DebugOut(L"[DETAILS]	left %d top %d right %d bottom %d\n", left, top, right, bottom);
		LPSPRITE NewTile = new CSprite(TileNum, left, top, right, bottom, TileSet); // get tile from tileset
		this->Tiles.push_back(NewTile);
	}
}

int Map::GetMapWidth()
{
	return TotalColumnsOfMap * TILE_WIDTH;
}

int Map::GetMapHeight()
{
	return TotalRowsOfMap * TILE_HEIGHT;
}
