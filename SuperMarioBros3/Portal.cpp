#include "Portal.h"


CPortal::CPortal(int scene_id, float start_x, float start_y)
{
	this->scene_id = scene_id;
	this->start_x = start_x;
	this->start_y = start_y;
}

void CPortal::Render()
{
	RenderBoundingBox(255);
}

void CPortal::GetBoundingBox(float &l, float &t, float &r, float &b)
{
	l = x;
	t = y;
	r = x + PORTAL_BBOX_WIDTH;
	b = y + PORTAL_BBOX_HEIGHT;
}