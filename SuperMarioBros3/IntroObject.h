#pragma once
#include "GameObject.h"
#define INTROOBJECT_BBOX_WIDTH 16

#define DEFLECT_SPEED   0.28f
#define GRAVITY	        0.002f
#define BLACK_SHELL 1
#define STAR        2
class CIntroObject :
    public CGameObject
{
    int collision_times = 0;
public:
    CIntroObject();
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
    void Render();
    void GetBoundingBox(float& l, float& t, float& r, float& b);
};

