#pragma once
#include "GameObject.h"
#include "State.h"
#include "SuperMushroom.h"

#define LEAF_BBOX_WIDTH 16
#define LEAF_BBOX_HEIGHT 16

#define LEAF_LEVEL_LEAF 1
#define LEAF_LEVEL_MUSHROOM 0

#define LEAF_STATE_DIE 100



class CSuperLeaf : public CGameObject {
	int dir;
	CSuperMushroom* mushroom;
public:
	CSuperLeaf(float x, float y, CSuperMushroom* mushroom);

	void OnNoCollision(DWORD dt);

	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	void GetBoundingBox(float& l, float& t, float& r, float& b);

	void SetState(int state);
	int IsBlocking() { return 0; }
	int IsCollidable();
};

