#include "Portal.h"


CPortal::CPortal(float l, float t, float r, float b, int scene_id, float start_x, float start_y)
{
	this->scene_id = scene_id;
	x = l; 
	y = t;
	width = r - l + 1;
	height = b - t + 1;
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
	r = x + width;
	b = y + height;
}