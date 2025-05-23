#include "Bullet.h"
#include "Mario.h"
#include "debug.h"

void CBullet::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (!InBound())
	{
		isDeleted = true;
		return;
	}
	x += vx * dt;
	y += vy * dt;

}

void CBullet::Render()
{
	CAnimations* a = CAnimations::GetInstance();
	int aniID = -1;
	a->Get(ID_ANI_FIRE)->Render(x, y);

	//RenderBoundingBox();
}

void CBullet::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - FIRE_BBOX_WIDTH / 2;
	t = y - FIRE_BBOX_HEIGHT / 2;
	r = l + FIRE_BBOX_WIDTH;
	b = t + FIRE_BBOX_HEIGHT;
}