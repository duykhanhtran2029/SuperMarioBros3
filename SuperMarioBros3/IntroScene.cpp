#include <iostream>
#include <fstream>
#include "IntroScene.h"
#include "IntroObject.h"
#include "Mario.h"
#include "IntroGround.h"
#include "Goomba.h"
#include "Koopas.h"
#include "Leaf.h"
#include "MushRoom.h"


#define OBJECT_TYPE_MARIO		0
#define OBJECT_TYPE_GOOMBA		1
#define OBJECT_TYPE_MUSHROOM	2
#define OBJECT_TYPE_KOOPAS		3
#define OBJECT_TYPE_GROUND		4
#define OBJECT_TYPE_LEAF		5
#define OBJECT_TYPE_INTRO		6


#define ANISET_BACKGROUND_ID   50
#define ANI_THREE_ID		   800

#define MAX_SCENE_LINE 1024

using namespace std;
CIntroScene::CIntroScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	key_handler = new IntroSceneKeyHandler(this);
	srolling_start = GetTickCount64();
}


void CIntroScene::_ParseSection_TEXTURES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 5) return; // skip invalid lines

	int texID = atoi(tokens[0].c_str());
	wstring path = ToWSTR(tokens[1]);
	int R = atoi(tokens[2].c_str());
	int G = atoi(tokens[3].c_str());
	int B = atoi(tokens[4].c_str());

	CTextures::GetInstance()->Add(texID, path.c_str(), D3DCOLOR_XRGB(R, G, B));
}

void CIntroScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPDIRECT3DTEXTURE9 tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return;
	}

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CIntroScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < tokens.size(); i += 2)	// why i+=2 ?  sprite_id | frame_time  
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i + 1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
	if (ani_id == ANI_THREE_ID)
		THREE = ani;
}

void CIntroScene::_ParseSection_ANIMATION_SETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return; // skip invalid lines - an animation set must at least id and one animation id
	//DebugOut(L"--> %s\n", ToWSTR(line).c_str());
	int ani_set_id = atoi(tokens[0].c_str());
	LPANIMATION_SET s;
	if (CAnimationSets::GetInstance()->animation_sets[ani_set_id] != NULL)
		s = CAnimationSets::GetInstance()->animation_sets[ani_set_id];
	else
		s = new CAnimationSet();
	CAnimations* animations = CAnimations::GetInstance();

	for (int i = 1; i < tokens.size(); i++)
	{
		int ani_id = atoi(tokens[i].c_str());

		LPANIMATION ani = animations->Get(ani_id);
		s->push_back(ani);
	}
	CAnimationSets::GetInstance()->Add(ani_set_id, s);
	if (ani_set_id == ANISET_BACKGROUND_ID)
		BackGround = s;
	if (ani_set_id == ANISET_ARROW_ID)
		Arrow = s;
}

void CIntroScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	if (tokens.size() < 3) return; // skip invalid lines - an object set must have at least id, x, y
	int tag = 0, option_tag_1 = 0, option_tag_2 = 0;
	int object_type = atoi(tokens[0].c_str());
	float x = atof(tokens[1].c_str());
	float y = atof(tokens[2].c_str());

	int ani_set_id = atoi(tokens[3].c_str());
	if (tokens.size() >= 5)
		tag = atof(tokens[4].c_str());
	if (tokens.size() >= 6)
		option_tag_1 = atof(tokens[5].c_str());
	if (tokens.size() >= 7)
		option_tag_2 = atof(tokens[6].c_str());


	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	CGameObject* obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_GROUND:
		obj = new CIntroGround();
		break;
	case OBJECT_TYPE_MARIO:
		obj = new CMario(x,y,true);
		if (luigi == NULL)
		{
			luigi = (CMario*)obj;
			luigi->SetState(MARIO_STATE_IDLE);
			luigi->SetLevel(MARIO_LEVEL_BIG,false);
		}
		else
		{
			mario = (CMario*)obj;
			mario->SetDirection(-1);
			mario->SetState(MARIO_STATE_IDLE);
			mario->SetLevel(MARIO_LEVEL_BIG, false);
			player = mario;
		}
		break;
	case OBJECT_TYPE_GOOMBA:
		obj = new CGoomba();
		obj->SetSpeed(0,0);
		fallingObjects.push_back(obj);
		goomba = dynamic_cast<CGoomba*>(obj);
		break;
	case OBJECT_TYPE_KOOPAS:
		obj = new CKoopas();
		obj->SetSpeed(0, 0);
		obj->SetState(KOOPAS_STATE_IN_SHELL);
		if (shell == nullptr)
		{
			fallingObjects.push_back(obj);
			shell = dynamic_cast<CKoopas*> (obj);
		}
		else
			koopas.push_back(obj);
		break;
	case OBJECT_TYPE_LEAF:
		obj = new CLeaf();
		((CLeaf*)obj)->SetIsAppear(true);
		obj->SetState(LEAF_STATE_IDLE);
		fallingObjects.push_back(obj);
		break;
	case OBJECT_TYPE_MUSHROOM:
		obj = new CMushRoom();
		((CMushRoom*)obj)->SetIsAppear(true);
		obj->SetState(MUSHROOM_STATE_IDLE);
		fallingObjects.push_back(obj);
		break;
	case OBJECT_TYPE_INTRO:
		obj = new CIntroObject();
		obj->SetTag(tag);
		fallingObjects.push_back(obj);
		break;
	default:
		DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
		return;
	}

	// General object setup
	obj->SetPosition(x, y);

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
	obj->SetAnimationSet(ani_set);

	objects.push_back(obj);
}

void CIntroScene::Load()
{
	DebugOut(L"[INFO] Start loading scene resources from : %s \n", sceneFilePath);

	ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[TEXTURES]") { section = SCENE_SECTION_TEXTURES; continue; }
		if (line == "[SPRITES]") {
			section = SCENE_SECTION_SPRITES; continue;
		}
		if (line == "[TILEMAP DATA]") {
			section = SCENE_SECTION_TILEMAP_DATA; continue;
		}
		if (line == "[ANIMATIONS]") {
			section = SCENE_SECTION_ANIMATIONS; continue;
		}
		if (line == "[ANIMATION_SETS]") {
			section = SCENE_SECTION_ANIMATION_SETS; continue;
		}
		if (line == "[OBJECTS]") {
			section = SCENE_SECTION_OBJECTS; continue;
		}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }
		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
		case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		}
	}

	f.close();
	CTextures::GetInstance()->Add(ID_TEX_BBOX, L"Resources\\Textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));

	fonts = new CFont();
	DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);
}
void CIntroScene::CalColliableObjects(LPGAMEOBJECT curObj, vector<LPGAMEOBJECT>* coObjects)
{
	if (dynamic_cast<CMario*>(curObj))
		for (UINT i = 0; i < coObjects->size(); i++)
		{
			LPGAMEOBJECT object = coObjects->at(i);
			
			if (object!= nullptr && (dynamic_cast<CLeaf*>(object)|| object->type == IGNORE
				||(dynamic_cast<CKoopas*>(object) && object->state == KOOPAS_STATE_IN_SHELL && ((CMario*)curObj)->isHolding)))
			{
				coObjects->erase(coObjects->begin() + i);
				i--;
			}
		}
	else
		for (UINT i = 0; i < coObjects->size(); i++)
		{
			LPGAMEOBJECT object = coObjects->at(i);
			if (coObjects->at(i)->type == IGNORE || (dynamic_cast<CMario*>(coObjects->at(i))&& ((CMario*)coObjects->at(i))->level == MARIO_LEVEL_SMALL))
			{
				coObjects->erase(coObjects->begin() + i);
				i--;
			}
		}
}
void CIntroScene::Update(DWORD dt)
{
	vector<LPGAMEOBJECT> coObjects;
	if (mario->isTransforming)
	{
		mario->Update(0, &coObjects);
		return;
	}
	//SECTION 0
	if (section == 0)
	{
		//show mario and luigi when a half of curtain is scrolled
		mario->SetPosition(mario->start_x, mario->y);
		luigi->SetPosition(luigi->start_x, luigi->y);

		if (GetTickCount64() - srolling_start >= SCROLLING_TIME/2)
		{
			mario->SetState(MARIO_STATE_WALKING_LEFT);
			luigi->SetState(MARIO_STATE_WALKING_RIGHT);
		}
		//scrolling curtain
		point = -(float)(GetTickCount64() - srolling_start) / SCROLLING_STACK_TIME;
		if (GetTickCount64() - srolling_start >= SCROLLING_TIME)
			StartFirstSection();
	}
	//SECTION 1
	if (section == 1)
	{
		//luigi jump
		if (luigi->x >= GROUND_WIDTH / 6 && luigi->state != MARIO_STATE_JUMPING)
		{
			luigi->SetState(MARIO_STATE_JUMPING);
			luigi->SetIsJumping(true);
			luigi->SetSpeed(0, -MARIO_JUMP_SPEED_MIN);
		}
		if (luigi->y <= MARIO_BIG_BBOX_HEIGHT)
		{
			luigi->y = MARIO_BIG_BBOX_HEIGHT;
			luigi->isDeflect = false;

			mario->SetState(MARIO_STATE_IDLE);
			slidedown_start = GetTickCount64();
			point = -BANNER_HEIGHT;
		}
		if (slidedown_start != 0)
		{
			if (point < 0)
				point += BANNER_STACK_HEIGHT;
			else
				point = 0;
		}
		else 
		{
			point = -300;
		}
		if (luigi->x > GROUND_WIDTH)
		{
			slidedown_start = 0;
			StartSecondSection();
			luigi->SetState(MARIO_STATE_IDLE);
			luigi->SetDirection(-1);
			luigi->SetPosition(luigi->x + KOOPAS_BBOX_WIDTH, luigi->y);
			luigi->SetSpeed(0,0);
		}
	}
	//SECTION 2
	if (section == 2)
	{
		if (ani_section == 2 && GetTickCount64() - changeAniTime >= ANI_TIME)
		{
			ani_section = 3;
			mario->SetState(MARIO_STATE_JUMPING);
			mario->SetIsJumping(true);
			mario->SetSpeed(0, -MARIO_JUMP_SPEED_MIN);
			isCustomSpeed = true;
		}
		if (ani_section == 1 && GetTickCount64() - changeAniTime >= ANI_TIME / 2)
		{
			ani_section = 2;
			changeAniTime = GetTickCount64();
		}
		if (shell != nullptr)
		{
			if (shell->x >= GROUND_WIDTH - KOOPAS_BBOX_WIDTH && !luigi->isHolding && mini_section == 0)
			{
				shell->SetState(KOOPAS_STATE_IN_SHELL);
				shell->SetSpeed(0, 0);
				luigi->isHolding = true;
				holder = luigi;
				shell->isBeingHeld = true;

				mario->SetState(MARIO_STATE_IDLE);
				mario->SetSpeed(0, 0);

				luigi->SetSpeed(-MARIO_WALKING_SPEED_MIN, 0);
				mini_section = 1;
			}
			if (luigi->x <= GROUND_WIDTH && luigi->isHolding && mini_section ==1)
			{
				mario->SetState(MARIO_STATE_WALKING_LEFT);
				mario->SetSpeed(-MARIO_WALKING_SPEED_MIN, 0);
				mini_section = 2;
			}
			if (luigi->x <= GROUND_WIDTH - KOOPAS_BBOX_WIDTH * 3 && luigi->isHolding && mini_section == 2)
			{
				luigi->SetSpeed(0, 0);
				luigi->isHolding = false;
				luigi->StartKicking();
				shell->vy = MARIO_JUMP_SPEED_MIN;
				mini_section = 3;
			}
			if (shell->x <= GROUND_WIDTH / 2 + KOOPAS_BBOX_WIDTH && mini_section == 3)
			{
				mario->SetState(MARIO_STATE_JUMPING);
				mario->SetIsJumping(true);
				mario->SetSpeed(-MARIO_WALKING_SPEED_MIN*2, -0.15);
				StartJump();
				mini_section = 4;
			}
			if (GetTickCount64() - jump_start >= JUMP_TIME && jump_start != 0 && mini_section == 4)
			{
				mario->SetSpeed(-MARIO_WALKING_SPEED_MIN, MARIO_JUMP_SPEED_MIN);
				mini_section = 5;
			}
			if (shell->x < mario->x && jump_start != 0 && mini_section == 5)
			{
				shell->SetState(KOOPAS_STATE_IN_SHELL);
				shell->SetSpeed(0, 0);

				mario->SetState(MARIO_STATE_JUMPING);
				mario->SetIsJumping(true);
				mario->SetSpeed(-MARIO_WALKING_SPEED_MIN*2, -MARIO_JUMP_SPEED_MIN);
				jump_start = 0;
				StartJump();
				mini_section = 6;
			}

			if (GetTickCount64() - jump_start >= JUMP_TIME && jump_start != 0 && mini_section == 6)
			{
				mario->SetSpeed(-MARIO_WALKING_SPEED_MIN, MARIO_JUMP_SPEED_MIN);
				Revenge = true;
				mini_section = 7;
			}
			if (mario->isHolding && shell->x >= GROUND_WIDTH /2 && mini_section == 7)
			{
				mario->SetSpeed(0, 0);
				mario->isHolding = false;
				mario->isReadyToHold = false;
				mario->StartKicking();
				shell->vy = MARIO_JUMP_SPEED_MIN;

				luigi->SetState(MARIO_STATE_WALKING_RIGHT);
				luigi->SetSpeed(MARIO_WALKING_SPEED_MIN * 2, 0);
				mini_section = 8;
			}
			if (shell->x > GROUND_WIDTH && mini_section == 8)
			{
				shell->SetSpeed(0, 0);
			}
			if (luigi->x > GROUND_WIDTH * 2 - KOOPAS_BBOX_WIDTH * 3 && mini_section == 8)
			{
				luigi->SetSpeed(0, 0);
				shell->SetPosition(luigi->start_x, luigi->start_y + (MARIO_BIG_BBOX_HEIGHT - KOOPAS_BBOX_HEIGHT));
				shell->SetSpeed(KOOPAS_WALKING_SPEED*5,MARIO_JUMP_SPEED_MIN);
				mini_section = 9;
			}
			if (mario->level == MARIO_LEVEL_SMALL && mini_section == 9 && shell->x >= GROUND_WIDTH)
			{
				mario->SetState(MARIO_STATE_WALKING_RIGHT);
				mario->SetSpeed(MARIO_WALKING_SPEED_MIN*2, 0);
				mini_section = 10;
			}
			if (mini_section == 10 && mario->x >= GROUND_WIDTH - KOOPAS_BBOX_WIDTH * 3)
			{
				mario->SetState(MARIO_STATE_WALKING_LEFT);
				mario->SetSpeed(MARIO_WALKING_SPEED_MIN * 2, 0);
				mini_section = 11;
			}
			if (mini_section == 11 && mario->x >= GROUND_WIDTH - KOOPAS_BBOX_WIDTH * 2)
			{
				mario->SetSpeed(-MARIO_WALKING_SPEED_MIN * 2, 0);
				mini_section = 12;
			}
			if (mini_section == 12 && mario->x <= GROUND_WIDTH/2 + KOOPAS_BBOX_WIDTH)
			{
				mario->SetState(MARIO_STATE_WALKING_RIGHT);
				mario->SetSpeed(MARIO_WALKING_SPEED_MIN, 0);
				mini_section = 13;
			}
			if (mini_section == 13 && mario->x >= GROUND_WIDTH)
			{
				StartThirdSection();
			}
		}

	}
	//SECTION 3
	if (section == 3)
	{
		if (GetTickCount64() - switch_start >= SWITCH_TIME && switch_start != 0)
		{
			switch_start = 0;
			CGame::GetInstance()->SwitchScene(0);
		}
		for (size_t i = 0; i < koopas.size(); i++)
		{
			if (koopas[i]->x > SCREEN_WIDTH && mini_section == 0)
					koopas[i]->SetIsDestroyed(true);
			if (koopas[i]->x >= (GROUND_BBOX_WIDTH - KOOPAS_BBOX_WIDTH) * 2)
			{
				koopas[i]->x = -KOOPAS_BBOX_WIDTH + 1;
				if (i == 0)
					mini_section = 0;
				koopas[i]->y = mario->start_y + (MARIO_BIG_BBOX_HEIGHT - KOOPAS_BBOX_HEIGHT);
				if (i == 1)
				{
					koopas[0]->nx = 1;
					koopas[0]->SetState(KOOPAS_STATE_WALKING);
					koopas[0]->vx = KOOPAS_WALKING_SPEED * 2;
				}
			}
		}
	}
	if (mario == nullptr)
		return;
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (!objects[i]->isDestroyed)
			coObjects.push_back(objects[i]);
		else
		{
			LPGAMEOBJECT tmp = objects[i];
			objects.erase(objects.begin() + i);
			delete tmp;
			i--;
		}

	}
	for (size_t i = 0; i < objects.size(); i++)
	{
		CalColliableObjects(objects[i], &coObjects);
		objects[i]->Update(dt, &coObjects);
	}
}
void CIntroScene::Render()
{
	int index = 0;
	BackGround->at(section)->Render(0, point);
	if (section == 3)
	{
		if(switch_start != 0)
			Arrow->at(0)->Render(ARROW_X, ARROW_Y);
		else
			Arrow->at(1)->Render(ARROW_X, ARROW_Y);
	}
	if (section >= 2)
		THREE->Render(THREE_X, THREE_Y);

	//only show mario and luigi after a half of curtain is scrolled
	if (!(section == 0 && GetTickCount64() - srolling_start <= SCROLLING_TIME / 3))
	{
		if (ani_section == 1)
			CSprites::GetInstance()->sprites[HIT_ID]->Draw(mario->x, mario->y);
		else if (ani_section == 2)
			CSprites::GetInstance()->sprites[LOOKUP_ID]->Draw(mario->x, mario->y);
		else
			mario->Render();
		luigi->Render();
	}
	for (int i = 2; i < objects.size(); i++)
		objects[i]->Render();
	if (mini_section == 13 && section == 2)
		CSprites::GetInstance()->sprites[BUSH_ID]->Draw(BUSH_X, BUSH_Y);

}

/*
	Unload current scene
*/
void CIntroScene::Unload()
{
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];
	objects.clear();

	delete fonts;

	mario = nullptr;
	luigi = nullptr;
	fonts = nullptr;
	shell = nullptr;
	holder = nullptr;
	goomba = nullptr;
	BackGround = nullptr;
	THREE = nullptr;
	Arrow = nullptr;

	fallingObjects.clear();
	koopas.clear();
	section = 0;

	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}
void CIntroScene::StartSecondSection()
{
	section = 2;
	point = 0;
	for (size_t i = 0; i < fallingObjects.size(); i++)
	{
		fallingObjects[i]->SetSpeed(0, MARIO_JUMP_SPEED_MAX);
		if (dynamic_cast<CMushRoom*>(fallingObjects[i]))
			fallingObjects[i]->SetState(MUSHROOM_STATE_WALK);
		if (dynamic_cast<CLeaf*>(fallingObjects[i]))
		{
			fallingObjects[i]->SetState(LEAF_STATE_FALLING);
			fallingObjects[i]->SetSpeed(LEAF_SPEED,LEAF_GRAVITY*2);
		}
	}
}
void CIntroScene::StartThirdSection()
{
	section = 3;
	point = 0;
	for (size_t i = 0; i < fallingObjects.size(); i++)
			fallingObjects[i]->isDestroyed = true;
	shell = nullptr;
	holder = mario;
	mario->SetPosition(-SCREEN_WIDTH, 0);
	mario->SetSpeed(0, 0);
	luigi->SetPosition(-SCREEN_WIDTH, 0);
	luigi->SetSpeed(0, 0);
	isCustomSpeed = true;

	for (size_t i = 1; i < koopas.size(); i++)
	{
		koopas[i]->nx = 1;
		koopas[i]->SetState(KOOPAS_STATE_WALKING);
	}
	mini_section = 13;
}
void IntroSceneKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	CIntroScene* scene = ((CIntroScene*)CGame::GetInstance()->GetCurrentScene());
	switch (KeyCode)
	{
	case DIK_RETURN:
		if (scene->section != 3)
			scene->StartThirdSection();
		else
			scene->StartSwitch();
		break;
	}
}