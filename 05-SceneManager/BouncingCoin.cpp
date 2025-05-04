#include "BouncingCoin.h"
#include "DataBase.h"
#include "ScoreDataBase.h"
#include "PlayScene.h"
#include "Effect.h"

void CBouncingCoin::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	y += vy * dt;
	if (y > alpha_y && vy != 0) {
		CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		scene->SpawnEffect(new CEffect(x, y, SCORE_BOUNCING_COIN));
		//
		y = alpha_y;
		vy = 0;
		isDeleted = true;
		return;
	}
	if (y < alpha_y) {
		vy += BOUNCING_COIN_BOUNCE_RECURE * dt;
	}
}
void CBouncingCoin::SetState(int state) {
	if (this->state == CONTAINED_STATE_ACTIVE) return;
	switch (state) {
	case CONTAINED_STATE_ACTIVE:
		vy = -BOUNCING_COIN_BOUNCE_FORCE;
		CGame::GetInstance()->GetData()->AddCoin(1);
		CGame::GetInstance()->GetData()->AddScore(SCORE_BOUNCING_COIN);
		break;
	}

	CGameObject::SetState(state);

}

void CBouncingCoin::Render() {
	if (state == CONTAINED_STATE_DEACTIVE) return;
	CCoin::Render();
}