#include"conio.h"
#include"stdio.h"
#include"string.h"
#include"iostream"
#include <fstream>
#include<vector>
#include "../SuperMarioBros3/Utils.cpp"
using namespace std;

#define MAX_SCENE_LINE 1024
#define TILESET_WIDTH	16
#define TILESET_HEIGHT	16
#define CELL_WIDTH		136
#define CELL_HEIGHT		128


void CreateGridFile(int sceneId);
void ReadMapInformation(int sceneID, int& mapWidth, int& mapHeight);
void WriteGridFile(int sceneID, int gridCols, int gridRows);


int main()
{
	int sceneID;
	cout << "Insert ID of Scene you want to create grid: ";
	cin >> sceneID;
	CreateGridFile(sceneID);
	cout << "\n";
	system("Pause");
	return 0;
}



void CreateGridFile(int sceneID)
{
	int mapWidth = -1, mapHeight = -1;

	ReadMapInformation(sceneID, mapWidth, mapHeight);


	if (mapWidth == -1 || mapHeight == -1)
		return;


	int gridCols = ceil((float)mapWidth / CELL_WIDTH);
	int gridRows = ceil((float)mapHeight / CELL_HEIGHT);

	cout << "\nColumns of Grid:\t" << gridCols << "\nRows of Grid:\t" << gridRows;

	WriteGridFile(sceneID, gridCols, gridRows);

};


void ReadMapInformation(int sceneID, int& mapWidth, int& mapHeight)
{
	char fileMapName[100];
	int extrasceneid = sceneID;
	if (sceneID == 5 || sceneID == 2)
	{
		extrasceneid--;
		sprintf_s(fileMapName, "..\\SuperMarioBros3\\Resources\\Tilemap\\world1-%d-1.txt", extrasceneid);
	}
	else
		sprintf_s(fileMapName, "..\\SuperMarioBros3\\Resources\\Tilemap\\world1-%d.txt", sceneID);
	ifstream mapFile(fileMapName);
	if (!mapFile)
		cout << "Could not open the map file!";
	else
	{
		cout << "Open the map file sucessfully!" << endl;
		char str[MAX_SCENE_LINE];
		mapFile.getline(str, MAX_SCENE_LINE);
		string line(str);

		vector<string> tokens = split(line);
		if (tokens.size() < 3) return; // skip invalid lines

		int tolRowMap = atoi(tokens[1].c_str());
		int tolColMap = atoi(tokens[2].c_str());


		mapWidth = tolColMap * TILESET_WIDTH;
		mapHeight = tolRowMap * TILESET_HEIGHT;

		cout << "\ntolRowMap:\t" << tolRowMap << "\ntolColMap:\t" << tolColMap;
		cout << "\nMapWidth:\t" << mapWidth << "\nMapHeight:\t" << mapHeight;
	}
	mapFile.close();
};

void WriteGridFile(int sceneID, int gridCols, int gridRows)
{
	char gridFileName[100];
	int worldId = sceneID;
	if (sceneID == 5 || sceneID == 2)
	{
		worldId--;
		sprintf_s(gridFileName, "..\\SuperMarioBros3\\Resources\\Scene\\World1-%d\\world1-%d-1_Grid.txt", worldId, worldId);
	}
	else
		sprintf_s(gridFileName, "..\\SuperMarioBros3\\Resources\\Scene\\World1-%d\\world1-%d_Grid.txt", sceneID, sceneID);
	ofstream gridFile;
	gridFile.open(gridFileName);
	gridFile << gridCols << "\t" << gridRows;

	char objectsFileName[100];
	if (sceneID == 5 || sceneID == 2)
		sprintf_s(objectsFileName, "..\\SuperMarioBros3\\Resources\\Scene\\World1-%d\\world1-%d-1_Obj.txt", worldId, worldId);
	else
		sprintf_s(objectsFileName, "..\\SuperMarioBros3\\Resources\\Scene\\World1-%d\\world1-%d_Obj.txt", sceneID, sceneID);
	ifstream objFile(objectsFileName);
	char str[1024];

	while (objFile.getline(str, 1024))
	{
		string line(str);
		vector<string> tokens = split(line);


		if (line[0] != '#' && line[0] != '0')
		{
			if (tokens.size() < 3)
				continue;

			float x = (float)atof(tokens[1].c_str());
			float y = (float)atof(tokens[2].c_str());

			int row = (int)(y / CELL_HEIGHT);
			int col = (int)(x / CELL_WIDTH);


			if (row == gridRows)
				row = gridRows - 1;
			if (col == gridCols)
				col = gridCols - 1;

			gridFile << "\n" << line << "\t" << row << "\t" << col;
		}
		else
		{
			gridFile << "\n" << line;
		}
		cout << "\n" << line;
	}
	objFile.close();
	gridFile.close();

}