#pragma once
#include "Coin.h"
#include "Configs.h"
#include "State.h"


#define BOUNCING_COIN_BOUNCE_FORCE 0.3f
#define BOUNCING_COIN_BOUNCE_RECURE 0.0008f
class CBouncingCoin : public CCoin {
	float alpha_x, alpha_y;
public:
	CBouncingCoin(float x, float y) : CCoin(x, y) {
		this->state = CONTAINED_STATE_DEACTIVE;
		alpha_x = x;
		alpha_y = y;
	}
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	void SetState(int state);
	void Render();
	int IsCollidable() { return 0; }
};
