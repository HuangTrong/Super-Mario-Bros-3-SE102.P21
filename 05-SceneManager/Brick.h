#pragma once

#include "AssetIDs.h"
#include "block.h"
#include "Coin.h"
#include "Button.h"
#include "PlayScene.h"

#define ID_ANI_BRICK	10002

#define BRICK_STATE_IDLE			100
#define BRICK_STATE_HIT				200
#define BRICK_STATE_EMPTY			300
#define BRICK_STATE_TRANS_TO_COIN	400

#define BRICK_TYPE_EMPTY	1
#define BRICK_TYPE_BUTTON	2

// brick is a block that can be broken
class CBrick : public CBlock
{
	int type;
	CButton* button;
public:
	CBrick(int x, int y, CButton* button) : CBlock(x, y) {
		this->type = BRICK_TYPE_BUTTON;
		this->button = button;
		SetState(BRICK_STATE_IDLE);
	}
	CBrick(int x, int y) : CBlock(x, y) {
		this->type = BRICK_TYPE_EMPTY;
		SetState(BRICK_STATE_IDLE);
	}
	int IsBlocking() { return 1; }

	void Render() {
		if (isDeleted)
			return;

		int aniId = -1;
		if (state == BRICK_STATE_EMPTY)
			aniId = ID_ANI_EMPTY_BLOCK;
		else
			aniId = ID_ANI_BRICK;

		CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	}
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {

	}
	void SetState(int state) {

		switch (state)
		{
		case BRICK_STATE_IDLE:
			break;
		case BRICK_STATE_HIT:
			if (type == BRICK_TYPE_BUTTON)
			{
				button->SetState(BUTTON_STATE_IDLE);
				SetState(BRICK_STATE_EMPTY);
				return;
			}
			else
				Delete();
			break;
		case BRICK_STATE_EMPTY:
			break;
		case BRICK_STATE_TRANS_TO_COIN:
			Delete();

			CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
			scene->SpawnObject(new CCoin(x, y));

			scene = NULL;
			delete scene;
			break;
		}
		CGameObject::SetState(state);
	}
};