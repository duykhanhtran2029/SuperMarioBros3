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
	//ScreenWidth = CGame::GetInstance()->GetScreenWidth();
}

Map::~Map()
{
}

void Map::Render()
{
	for (int CurrentRow = 0; CurrentRow < TotalRowsOfMap; CurrentRow++)
		for (int CurrentColumn = 0; CurrentColumn < TotalColumnsOfMap; CurrentColumn++)
			Tiles[TileMap[CurrentRow][CurrentColumn] - 1]->Draw(CurrentColumn * TILE_WIDTH, CurrentRow * TILE_HEIGHT);
}

void Map::SetTileMapData(int** TileMapData)
{
	TileMap = TileMapData;
}


void Map::ExtractTileFromTileSet()
{
	for (int TileNum = 0; TileNum < 93; TileNum++)
	{
		int left = TileNum % TotalColumnsOfTileSet * TILE_WIDTH;
		int top = TileNum / TotalColumnsOfTileSet * TILE_HEIGHT;
		int right = left + TILE_WIDTH;
		int bottom = top + TILE_HEIGHT;
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
