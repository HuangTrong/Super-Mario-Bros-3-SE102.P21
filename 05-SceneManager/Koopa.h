#pragma once
#include "GameObject.h"
#include "PlatformCheck.h"

#define KOOPA_GRAVITY 0.0004f
#define KOOPA_WALKING_SPEED 0.025f
#define KOOPA_SHELL_MOVE 0.16f
#define KOOMBA_HIT_DEFLECT_SPEED 0.15f

#define KOOPA_BBOX_WIDTH 16
#define KOOPA_BBOX_HEIGHT 26
#define KOOPA_SHELL_BBOX_HEIGHT 14

#define KOOPA_STATE_WALKING 100
#define KOOPA_STATE_SHELL_IDLE 200
#define KOOPA_STATE_SHELL_IDLE_HIT 201
#define KOOPA_STATE_SHELL_IDLE_VIBRATE 202
#define KOOPA_STATE_SHELL_IDLE_COMING_OUT 203
#define KOOPA_STATE_SHELL_MOVE 300
#define KOOPA_STATE_HELD 400
#define KOOPA_STATE_RELEASE_HELD 400
#define KOOPA_STATE_HIT	500

#define PARAKOOPA_STATE_FLY		600
#define PARAKOOPA_STATE_NO_WING_WALKING	700

#define ID_ANI_KOOPA_GREEN_WALKING_LEFT 8001
#define ID_ANI_KOOPA_GREEN_WALKING_RIGHT 8002
#define ID_ANI_KOOPA_GREEN_SHELL_IDLE 8011
#define ID_ANI_KOOPA_GREEN_SHELL_MOVE 8012
#define ID_ANI_KOOPA_GREEN_SHELL_IDLE_VIBRATE 8020
#define ID_ANI_KOOPA_GREEN_SHELL_COMING_OUT 8021

#define ID_ANI_KOOPA_RED_WALKING_LEFT 8101
#define ID_ANI_KOOPA_RED_WALKING_RIGHT 8102
#define ID_ANI_KOOPA_RED_SHELL_IDLE 8111
#define ID_ANI_KOOPA_RED_SHELL_MOVE 8112
#define ID_ANI_KOOPA_RED_SHELL_IDLE_VIBRATE 8120
#define ID_ANI_KOOPA_RED_SHELL_COMING_OUT 8121

#define KOOPA_SHELL_TIME	5000
#define KOOPA_SHELL_VIBRATE_TIME	2000
#define KOOPA_SHELL_COMING_OUT_TIME	1000

class CKoopa : public CGameObject
{
protected:
	float ax;
	float ay;

	int nx;

	float* m_x;
	float* m_y;
	int* m_nx;

	BOOLEAN isBlockByPlatform;
	BOOLEAN isOnPlatform;

	BOOLEAN is_mario_holding;

	BOOLEAN is_mario_kicked;

	int trigger;

	CPlatformCheck* platformChecker;

	ULONGLONG shell_start;
	ULONGLONG shell_vibrate_start;
	ULONGLONG shell_coming_out_start;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();


	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

	virtual void OnCollisionWithPlatform(LPCOLLISIONEVENT e);
	virtual void OnCollisionWithBlock(LPCOLLISIONEVENT e);
	virtual void OnCollisionWithOtherUnit(LPCOLLISIONEVENT e);

public:
	CKoopa(float x, float y, BOOLEAN block);
	virtual void SetState(int state);
	void KickedByMario(int nx);
	void GetHit(int nx);
	void HoldByMario(float* x, float* y, int* nx);
	void UpdatePositionFollowMario();
	virtual int IsCollidable() { return isActived; };
	virtual int IsBlocking() { return 0; }
};
