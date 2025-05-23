#pragma once
#include "GameObject.h"
#include "Configs.h"

#define VENUS_BBOX_WIDTH 12
#define VENUS_BBOX_HEIGHT_TALL 32
#define VENUS_BBOX_HEIGHT_SHORT 24

#define VENUS_FIRE_RANGE 132

#define VENUS_STATE_HIDDEN 100
#define VENUS_STATE_UP 200
#define VENUS_STATE_DOWN 300
#define VENUS_STATE_FIRE 400
#define VENUS_STATE_DIE 500
#define VENUS_STATE_AIMING 600

#define VENUS_MOVE_SPEED 0.095f
#define VENUS_FIRE_NEAR 0.05f
#define VENUS_FIRE_FAR 0.035f

#define VENUS_FIRE_RANGE 99

#define VENUS_WAIT_TIME 1300
#define VENUS_SLEEP_TIME 2900

#define ID_ANI_VENUS_TALL_LEFT_UP 6002
#define ID_ANI_VENUS_TALL_LEFT_DOWN 6001
#define ID_ANI_VENUS_TALL_RIGHT_UP 6012
#define ID_ANI_VENUS_TALL_RIGHT_DOWN 6011

#define ID_ANI_VENUS_SHORT_LEFT_UP 6102
#define ID_ANI_VENUS_SHORT_LEFT_DOWN 6101
#define ID_ANI_VENUS_SHORT_RIGHT_UP 6112
#define ID_ANI_VENUS_SHORT_RIGHT_DOWN 6111

#define ID_ANI_PIRANHA 6200

#define VENUS_TYPE_VENUS	1
#define VENUS_TYPE_PIRANHA	2

class CVenusPiranha : public CGameObject
{
	float hidden_y, grow_y;

	ULONGLONG wait_start;
	ULONGLONG sleep_start;
	int height;
	int type;

public:
	CVenusPiranha(float x, float y, int height, int type) : CGameObject(x, y) {

		this->state = VENUS_STATE_HIDDEN;
		this->hidden_y = y;
		this->grow_y = (height > 1) ? y - VENUS_BBOX_HEIGHT_TALL : y - VENUS_BBOX_HEIGHT_SHORT;
		this->vx = 0;
		this->vy = 0;
		this->height = height;
		this->type = type;
		wait_start = -1;
		sleep_start = -1;
		isActived = false;
	}
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
	int IsBlocking() { return 0; }

	void StateHandler();

	void Reset();
};