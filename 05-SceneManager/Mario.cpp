#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "Game.h"
#include "DataBase.h"
#include "ScoreDataBase.h"

#include "Goomba.h"
#include "Coin.h"
#include "Block.h"
#include "SuperMushroom.h"
#include "SuperLeaf.h"
#include "Bullet.h"
#include "VenusPiranha.h"
#include "Koopa.h"
#include "ParaGoomba.h"
#include "ParaKoopa.h"
#include "Brick.h"
#include "Button.h"
#include "Pipe.h"
#include "Card.h"
#include "Mushroom.h"
#include "Portal.h"

#include "Collision.h"

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	//// all timer
// reset untouchable timer if untouchable time has passed
	if (untouchable && GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}
	// reset kick timer 
	if (isKicking && GetTickCount64() - kick_start > MARIO_KICK_ANI_TIME)
	{
		kick_start = 0;
		isKicking = FALSE;
	}
	// reset tail attack timer
	if (isTailAttacking && GetTickCount64() - tail_attack_start > MARIO_TAIL_ATTACK_TIME)
	{
		tail_attack_start = 0;
		isTailAttacking = FALSE;
	}

	// reset transform timer
	if (isTransform && GetTickCount64() - transform_start > MARIO_TRANSFORM_TIME) {
		transform_start = 0;
		isTransform = FALSE;

		CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		scene->UnPause();
	}

	// reset pipe timer
	if (isPipe && GetTickCount64() - pipe_start > MARIO_PIPE_TIME) {
		if (state == MARIO_STATE_ENTER_PIPE)
			SetState(MARIO_STATE_OUTER_PIPE);
		else
		{
			pipe_start = 0;
			isPipe = FALSE;
		}

	}

	if (isTransform) return; // skip the rest if transforming

	// update
	if (isOnPlatform)
	{
		ay = MARIO_GRAVITY;
		isFlying = FALSE;
	}
	else
		ay = MARIO_ON_AIR_DECLERATION;

	if (!isPipe)
		vy += ay * dt;
	vx += ax * dt;
	CGame::GetInstance()->GetData()->SetSpeed(vx);

	// deceleration to prevent slipping
	if (ax == 0 && vx != 0) {
		float deccel = (vx > 0) ? -MARIO_DECEL_WALK : MARIO_DECEL_WALK;
		if (deccel > 0)
			vx = min(vx + deccel * dt, 0);
		else
			vx = max(vx + deccel * dt, 0);
	}

	// switch state: walk -> run
	if (abs(vx) > abs(maxVx)) {
		vx = maxVx;
		if (state == MARIO_STATE_ACCEL_TO_RUN_RIGHT)
			SetState(MARIO_STATE_RUNNING_RIGHT);
		else if (state == MARIO_STATE_ACCEL_TO_RUN_LEFT)
			SetState(MARIO_STATE_RUNNING_LEFT);
	}

	// if mario fall off the platform
	CPlayScene* scene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
	if (scene->IsFallOff(y) && scene->GetState() != PLAY_STATE_LOSE && !isPipe)
	{
		scene->SetState(PLAY_STATE_LOSE);
		return;
	}

	isOnPlatform = false;
	readyToPipe = FALSE;

	CCollision::GetInstance()->Process(this, dt, coObjects);

	if (isHolding && koopaShell != nullptr)
	{
		// Update koopa position to match Mario's hands
		dynamic_cast<CKoopa*>(koopaShell)->UpdatePositionFollowMario();

		// If run button is released, throw the shell
		if (!CGame::GetInstance()->IsKeyDown(DIK_A))
		{
			CKoopa* koopa = dynamic_cast<CKoopa*>(koopaShell);
			koopa->KickedByMario(this->nx);
			isHolding = false;
			koopaShell = nullptr;
		}
	}
}

void CMario::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
	isOnPlatform = false;
}

void CMario::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->ny != 0 && e->obj->IsBlocking())
	{
		vy = 0;
		if (e->ny < 0) isOnPlatform = true;
	}
	else if (e->nx != 0 && e->obj->IsBlocking())
	{
		vx = 0;
	}


	if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<CCoin*>(e->obj))
		OnCollisionWithCoin(e);
	else if (dynamic_cast<CBrick*>(e->obj))
		OnCollisionWithBrick(e);
	else if (dynamic_cast<CBlock*>(e->obj))
		OnCollisionWithQBlock(e);
	else if (dynamic_cast<CMushroom*>(e->obj))
		OnCollisionWithMushroom(e);
	else if (dynamic_cast<CSuperMushroom*>(e->obj))
		OnCollisionWithSuperMushroom(e);
	else if (dynamic_cast<CSuperLeaf*>(e->obj))
		OnCollisionWithSuperLeaf(e);
	else if (dynamic_cast<CBullet*>(e->obj))
		OnCollisionWithFire(e);
	else if (dynamic_cast<CVenusPiranha*>(e->obj))
		OnCollisionWithPiranha(e);
	else if (dynamic_cast<CKoopa*>(e->obj))
		HandleKoopaInteraction(dynamic_cast<CKoopa*>(e->obj), e);
	else if (dynamic_cast<CButton*>(e->obj))
		OnCollisionWithButton(e);
	else if (dynamic_cast<CPipe*>(e->obj))
		OnCollisionWithPipe(e);
	else if (dynamic_cast<CCard*>(e->obj))
		OnCollisionWithCard(e);
	//else if (dynamic_cast<CPortal*>(e->obj))
	//	OnCollisionWithPortal(e);

}


void CMario::HandleKoopaInteraction(CKoopa* koopa, LPCOLLISIONEVENT e)
{
	if (e->ny < 0) // Jump on top of Koopa
	{
		if (koopa->GetState() == KOOPA_STATE_WALKING)
		{
			// Turn koopa into shell
			koopa->SetState(KOOPA_STATE_SHELL_IDLE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
		else if (koopa->GetState() == KOOPA_STATE_SHELL_MOVE)
		{
			// Stop moving shell
			koopa->SetState(KOOPA_STATE_SHELL_IDLE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
		else if (koopa->GetState() == KOOPA_STATE_SHELL_IDLE ||
			koopa->GetState() == KOOPA_STATE_SHELL_IDLE_VIBRATE ||
			koopa->GetState() == KOOPA_STATE_SHELL_IDLE_COMING_OUT)
		{
			// Kick the shell
			koopa->KickedByMario(this->nx);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
	}
	else // Side collision
	{
		if (koopa->GetState() == KOOPA_STATE_SHELL_IDLE ||
			koopa->GetState() == KOOPA_STATE_SHELL_IDLE_VIBRATE ||
			koopa->GetState() == KOOPA_STATE_SHELL_IDLE_COMING_OUT)
		{
			// If run button is pressed, hold the shell
			if (CGame::GetInstance()->IsKeyDown(DIK_A))
			{
				koopa->HoldByMario(&x, &y, &nx);
				isHolding = true;
				koopaShell = koopa;
			}
			else
			{
				// Otherwise kick the shell
				koopa->KickedByMario(this->nx);
			}
		}
		else if (koopa->GetState() == KOOPA_STATE_SHELL_MOVE)
		{
			// Get hit by moving shell
			if (untouchable == 0)
			{
				if (level > MARIO_LEVEL_SMALL)
				{
					level = MARIO_LEVEL_SMALL;
					StartUntouchable();
				}
				else
				{
					SetState(MARIO_STATE_DIE);
				}
			}
		}
		else // Normal koopa walking
		{
			if (untouchable == 0)
			{
				if (level > MARIO_LEVEL_SMALL)
				{
					level = MARIO_LEVEL_SMALL;
					StartUntouchable();
				}
				else
				{
					SetState(MARIO_STATE_DIE);
				}
			}
		}
	}
}

void CMario::OnCollisionWithMushroom(LPCOLLISIONEVENT e) {
	CMushroom* mushroom = dynamic_cast<CMushroom*>(e->obj);
	if (mushroom->IsCollidable()) {
		mushroom->SetState(MUSHROOM_STATE_DIE);
		CGame::GetInstance()->GetData()->AddLife(1);
	}
}

void CMario::OnCollisionWithCard(LPCOLLISIONEVENT e) {
	CCard* card = dynamic_cast<CCard*>(e->obj);
	card->SetState(ROULETTE_CARD_STATE_WAS_COLLECTED);

	//win the scene
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	scene->SetState(PLAY_STATE_WIN);
}

void CMario::OnCollisionWithPipe(LPCOLLISIONEVENT e)
{
	CPipe* pipe = dynamic_cast<CPipe*>(e->obj);
	if ((pipe->GetMap() == MAP_MAIN && e->ny < 0) ||
		(pipe->GetMap() > MAP_MAIN && e->ny > 0))
	{
		if (pipe->GetType() == GATE_TYPE_ENTRANCE)
		{
			readyToPipe = TRUE;
			pipeDestination = pipe->GetIndex();

			float des_y;
			pipe->GetPosition(pipeDestination_x, des_y);

			if (pipe->GetMap() > MAP_MAIN)
				pipeDestination_ny = -1;
			else
				pipeDestination_ny = 1;

			CPipe* exit_pipe = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->GetExitPipeWithIndex(pipeDestination);
			mapIndex = exit_pipe->GetMap();
		}
	}
}

void CMario::OnCollisionWithBrick(LPCOLLISIONEVENT e)
{
	CBrick* brick = dynamic_cast<CBrick*>(e->obj);
	if (e->ny > 0 || (e->nx != 0 && isTailAttacking))
	{
		if (brick->GetState() == BRICK_STATE_IDLE)
			brick->SetState(BRICK_STATE_HIT);
	}
}

void CMario::OnCollisionWithButton(LPCOLLISIONEVENT e)
{
	CButton* button = dynamic_cast<CButton*>(e->obj);
	if (e->ny < 0)
	{
		if (button->GetState() == BUTTON_STATE_IDLE)
			button->SetState(BUTTON_STATE_PRESSED);
	}
}

void CMario::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
{
	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);

	if (koopa->GetState() == KOOPA_STATE_HELD) return;

	// jump on top >> 
	if (e->ny < 0)
	{
		if (dynamic_cast<CParaKoopa*>(e->obj))
		{
			if (koopa->GetState() == PARAKOOPA_STATE_FLY)
			{
				koopa->SetState(KOOPA_STATE_WALKING);
				vy = -MARIO_JUMP_DEFLECT_SPEED;
				return;
			}
		}

		if (koopa->GetState() == KOOPA_STATE_WALKING)
		{
			koopa->SetState(KOOPA_STATE_SHELL_IDLE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
		else
			if (koopa->GetState() == KOOPA_STATE_SHELL_IDLE)
			{
				koopa->SetState(KOOPA_STATE_SHELL_MOVE);
				vy = -MARIO_JUMP_DEFLECT_SPEED;
			}
	}
	else
	{
		if (untouchable == 0 && koopa->IsCollidable())
		{
			if (koopa->GetState() != KOOPA_STATE_SHELL_IDLE)
			{
				if (isTailAttacking)
				{
					koopa->GetHit(nx);
				}
				else
					HitByEnemy();
			}
			else
			{
				if (isTailAttacking)
				{
					koopa->GetHit(nx);
				}
				else
					if (readyToHold)
					{
						koopa->HoldByMario(&x, &y, &nx);
						koopaShell = koopa;
						SetState(MARIO_STATE_HOLDING_KOOPA);
					}
					else
					{
						SetState(MARIO_STATE_KICK);
						koopa->KickedByMario(nx);
					}

			}
		}
	}
}

void CMario::OnCollisionWithFire(LPCOLLISIONEVENT e) {

	HitByEnemy();

}
void CMario::OnCollisionWithPiranha(LPCOLLISIONEVENT e) {

	HitByEnemy();

}
void CMario::OnCollisionWithSuperLeaf(LPCOLLISIONEVENT e) {
	CSuperLeaf* leaf = dynamic_cast<CSuperLeaf*>(e->obj);

	if (leaf->IsCollidable()) {
		leaf->SetState(LEAF_STATE_DIE);
		SetLevel(MARIO_LEVEL_RACCOON);
	}

}
void CMario::OnCollisionWithSuperMushroom(LPCOLLISIONEVENT e) {
	CSuperMushroom* mushroom = dynamic_cast<CSuperMushroom*>(e->obj);

	if (mushroom->IsCollidable()) {
		mushroom->SetState(MUSHROOM_STATE_DIE);
		SetLevel(MARIO_LEVEL_BIG);
	}

}

void CMario::OnCollisionWithQBlock(LPCOLLISIONEVENT e) {
	CBlock* block = dynamic_cast<CBlock*>(e->obj);
	//jump from below the block
	if (e->ny > 0)
	{
		if (block->GetState() == BLOCK_STATE_EMPTY)
			return;
		if (block->GetState() == BLOCK_STATE_IDLE) {
			block->SetState(BLOCK_STATE_COLLIDED_BELOW);
		}
	}
}

void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

	// jump on top >> kill Goomba and deflect a bit 
	if (e->ny < 0)
	{
		if (goomba->GetState() != GOOMBA_STATE_DIE && goomba->IsCollidable())
		{
			if (dynamic_cast<CParaGoomba*>(e->obj))
			{
				if (goomba->GetState() == PARAGOOMBA_STATE_NO_WING_WALKING)
					goomba->SetState(GOOMBA_STATE_DIE);
				else
					goomba->SetState(PARAGOOMBA_STATE_NO_WING_WALKING);
			}
			else
				goomba->SetState(GOOMBA_STATE_DIE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
	}
	else // hit by Goomba or hit Goomba by tail
	{
		if (untouchable == 0)
		{
			if (goomba->GetState() != GOOMBA_STATE_DIE && goomba->IsCollidable())
			{
				if (isTailAttacking)
				{
					goomba->SetState(GOOMBA_STATE_HIT);
				}
				else
					HitByEnemy();
			}
		}
	}
}


void CMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsCollidable()) return;

	CGame::GetInstance()->GetData()->AddCoin(1);
	CGame::GetInstance()->GetData()->AddScore(SCORE_COIN);

	e->obj->Delete();
}

void CMario::HitByEnemy()
{
	if (untouchable == 0)
	{
		if (level == MARIO_LEVEL_RACCOON)
		{
			SetLevel(MARIO_LEVEL_BIG);
			StartUntouchable();
		}
		else
			if (level == MARIO_LEVEL_BIG)
			{
				SetLevel(MARIO_LEVEL_SMALL);
				StartUntouchable();
			}
			else
			{
				DebugOut(L">>> Mario DIE >>> \n");
				SetState(MARIO_STATE_DIE);
			}
	}
}

//void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
//{
//	CPortal* p = (CPortal*)e->obj;
//	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
//}

//
// Get animation ID for small Mario
//
int CMario::GetAniIdSmall()
{
	int aniId = -1;

	if (isPipe)
	{
		aniId = ID_ANI_MARIO_SMALL_PIPE;
	}
	else
		if (isTransform)
		{
			aniId = (nx < 0) ? ID_ANI_MARIO_TURN_INTO_SMALL_LEFT : ID_ANI_MARIO_TURN_INTO_SMALL_RIGHT;
		}
		else
			if (!isOnPlatform)
			{
				if (isHolding)
				{
					aniId = (nx > 0) ? ID_ANI_MARIO_SMALL_HOLD_RIGHT_JUMP : ID_ANI_MARIO_SMALL_HOLD_LEFT_JUMP;
				}
				else
					if (abs(vx) == MARIO_RUNNING_SPEED)
					{
						if (nx >= 0)
							aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT;
						else
							aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT;
					}
					else
					{
						if (nx >= 0)
							aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT;
						else
							aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT;
					}
			}
			else
				if (isSitting)
				{
					if (nx > 0)
						aniId = ID_ANI_MARIO_SIT_RIGHT;
					else
						aniId = ID_ANI_MARIO_SIT_LEFT;
				}
				else
					if (vx == 0)
					{
						if (isHolding)
						{
							aniId = (nx > 0) ? ID_ANI_MARIO_SMALL_HOLD_RIGHT_IDLE : ID_ANI_MARIO_SMALL_HOLD_LEFT_IDLE;
						}
						else
							if (isKicking)
							{
								aniId = (nx > 0) ? ID_ANI_MARIO_SMALL_KICK_RIGHT : ID_ANI_MARIO_SMALL_KICK_LEFT;
							}
							else
								if (nx > 0) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;
								else aniId = ID_ANI_MARIO_SMALL_IDLE_LEFT;
					}
					else if (vx > 0)
					{
						if (isHolding)
						{
							aniId = ID_ANI_MARIO_SMALL_HOLD_RIGHT_WALKING;
						}
						else
							if (isKicking)
							{
								aniId = ID_ANI_MARIO_SMALL_KICK_RIGHT;
							}
							else
								if (ax < 0 || nx < 0)
									aniId = ID_ANI_MARIO_SMALL_BRACE_RIGHT;
								else if (ax == MARIO_ACCEL_RUN_X)
									aniId = ID_ANI_MARIO_SMALL_RUNNING_RIGHT;
								else if (ax == MARIO_ACCEL_WALK_X || ax == MARIO_ACCEL_WALK_TO_RUN || ax == 0)
									aniId = ID_ANI_MARIO_SMALL_WALKING_RIGHT;
					}
					else // vx < 0
					{
						if (isHolding)
						{
							aniId = ID_ANI_MARIO_SMALL_HOLD_LEFT_WALKING;
						}
						else
							if (isKicking)
							{
								aniId = ID_ANI_MARIO_SMALL_KICK_LEFT;
							}
							else
								if (ax > 0)
									aniId = ID_ANI_MARIO_SMALL_BRACE_LEFT;
								else if (ax == -MARIO_ACCEL_RUN_X)
									aniId = ID_ANI_MARIO_SMALL_RUNNING_LEFT;
								else if (ax == -MARIO_ACCEL_WALK_X || ax == -MARIO_ACCEL_WALK_TO_RUN || ax == 0)
									aniId = ID_ANI_MARIO_SMALL_WALKING_LEFT;
					}

	if (aniId == -1) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;

	return aniId;
}


//
// Get animdation ID for big Mario
//
int CMario::GetAniIdBig()
{
	int aniId = -1;

	if (isPipe)
	{
		aniId = ID_ANI_MARIO_PIPE;
	}
	else
		if (isTransform)
		{
			if (previous_level > level)
				aniId = ID_ANI_MARIO_TURN_INTO_RACCOON;
			else
				aniId = (nx < 0) ? ID_ANI_MARIO_SMALL_TURN_INTO_BIG_LEFT : ID_ANI_MARIO_SMALL_TURN_INTO_BIG_RIGHT;
		}
		else
			if (!isOnPlatform)
			{
				if (isHolding)
				{
					aniId = (nx > 0) ? ID_ANI_MARIO_HOLD_RIGHT_JUMP : ID_ANI_MARIO_HOLD_LEFT_JUMP;
				}
				else
					if (abs(vx) == MARIO_RUNNING_SPEED)
					{
						if (nx >= 0)
							aniId = ID_ANI_MARIO_JUMP_RUN_RIGHT;
						else
							aniId = ID_ANI_MARIO_JUMP_RUN_LEFT;
					}
					else
					{
						if (nx >= 0)
							aniId = ID_ANI_MARIO_JUMP_WALK_RIGHT;
						else
							aniId = ID_ANI_MARIO_JUMP_WALK_LEFT;
					}
			}
			else
				if (isSitting)
				{
					if (nx > 0)
						aniId = ID_ANI_MARIO_SIT_RIGHT;
					else
						aniId = ID_ANI_MARIO_SIT_LEFT;
				}
				else
					if (vx == 0)
					{
						if (isHolding)
						{
							aniId = (nx > 0) ? ID_ANI_MARIO_HOLD_RIGHT_IDLE : ID_ANI_MARIO_HOLD_LEFT_IDLE;
						}
						else
							if (isKicking)
							{
								aniId = (nx > 0) ? ID_ANI_MARIO_KICK_RIGHT : ID_ANI_MARIO_KICK_LEFT;
							}
							else
								if (nx > 0) aniId = ID_ANI_MARIO_IDLE_RIGHT;
								else aniId = ID_ANI_MARIO_IDLE_LEFT;
					}
					else if (vx > 0)
					{
						if (isHolding)
						{
							aniId = ID_ANI_MARIO_HOLD_RIGHT_WALKING;
						}
						else
							if (isKicking)
							{
								aniId = ID_ANI_MARIO_KICK_RIGHT;
							}
							else
								if (ax < 0)
									aniId = ID_ANI_MARIO_BRACE_RIGHT;
								else if (ax == MARIO_ACCEL_RUN_X)
									aniId = ID_ANI_MARIO_RUNNING_RIGHT;
								else if (ax == MARIO_ACCEL_WALK_X || ax == MARIO_ACCEL_WALK_TO_RUN || ax == 0)
									aniId = ID_ANI_MARIO_WALKING_RIGHT;
					}
					else // vx < 0
					{
						if (isHolding)
						{
							aniId = ID_ANI_MARIO_HOLD_LEFT_WALKING;
						}
						else
							if (isKicking)
							{
								aniId = ID_ANI_MARIO_KICK_LEFT;
							}
							else
								if (ax > 0)
									aniId = ID_ANI_MARIO_BRACE_LEFT;
								else if (ax == -MARIO_ACCEL_RUN_X)
									aniId = ID_ANI_MARIO_RUNNING_LEFT;
								else if (ax == -MARIO_ACCEL_WALK_X || ax == -MARIO_ACCEL_WALK_TO_RUN || ax == 0)
									aniId = ID_ANI_MARIO_WALKING_LEFT;
					}

	if (aniId == -1) aniId = ID_ANI_MARIO_IDLE_RIGHT;

	return aniId;
}

//
// Get animdation ID for raccoon Mario
//
int CMario::GetAniIdRaccoon()
{
	int aniId = -1;

	if (isPipe)
	{
		aniId = ID_ANI_MARIO_RACCOON_PIPE;
	}
	else
		if (isTransform)
			aniId = ID_ANI_MARIO_TURN_INTO_RACCOON;
		else
			if (isTailAttacking)
			{
				aniId = (nx > 0) ? ID_ANI_MARIO_RACCOON_TAIL_ATTACK_RIGHT : ID_ANI_MARIO_RACCOON_TAIL_ATTACK_LEFT;
			}
			else
				if (!isOnPlatform)
				{
					if (isHolding)
					{
						aniId = (nx > 0) ? ID_ANI_MARIO_RACCOON_HOLD_RIGHT_JUMP : ID_ANI_MARIO_RACCOON_HOLD_LEFT_JUMP;
					}
					else
						if (isWagging)
						{
							if (nx > 0)
								aniId = ID_ANI_MARIO_RACCOON_TAIL_WAGGING_RIGHT;
							else
								aniId = ID_ANI_MARIO_RACCOON_TAIL_WAGGING_LEFT;
						}
						else
							if (isFlying)
							{
								if (nx > 0)
									aniId = ID_ANI_MARIO_RACCOON_FLY_TAIL_WAGGING_RIGHT;
								else
									aniId = ID_ANI_MARIO_RACCOON_FLY_TAIL_WAGGING_LEFT;
							}
							else
								if (abs(vx) == MARIO_RUNNING_SPEED)
								{
									if (nx > 0)
										aniId = ID_ANI_MARIO_RACCOON_JUMP_RUN_RIGHT;
									else
										aniId = ID_ANI_MARIO_RACCOON_JUMP_RUN_LEFT;
								}
								else
								{
									if (vy < 0)
										aniId = (nx >= 0) ? ID_ANI_MARIO_RACCOON_JUMP_WALK_RIGHT : ID_ANI_MARIO_RACCOON_JUMP_WALK_LEFT;
									else
										aniId = (nx >= 0) ? ID_ANI_MARIO_RACCOON_JUMP_FALLING_RIGHT : ID_ANI_MARIO_RACCOON_JUMP_FALLING_LEFT;
								}
				}
				else
					if (isSitting)
					{
						if (nx > 0)
							aniId = ID_ANI_MARIO_RACCOON_SIT_RIGHT;
						else
							aniId = ID_ANI_MARIO_RACCOON_SIT_LEFT;
					}
					else
						if (vx == 0)
						{
							if (isHolding)
							{
								aniId = (nx > 0) ? ID_ANI_MARIO_RACCOON_HOLD_RIGHT_IDLE : ID_ANI_MARIO_RACCOON_HOLD_LEFT_IDLE;
							}
							else
								if (isKicking)
								{
									aniId = (nx > 0) ? ID_ANI_MARIO_RACCOON_KICK_RIGHT : ID_ANI_MARIO_RACCOON_KICK_LEFT;
								}
								else
									if (nx > 0) aniId = ID_ANI_MARIO_RACCOON_IDLE_RIGHT;
									else aniId = ID_ANI_MARIO_RACCOON_IDLE_LEFT;
						}
						else if (vx > 0)
						{
							if (isHolding)
							{
								aniId = ID_ANI_MARIO_RACCOON_HOLD_RIGHT_WALKING;
							}
							else
								if (isKicking)
								{
									aniId = ID_ANI_MARIO_RACCOON_KICK_RIGHT;
								}
								else
									if (ax < 0)
										aniId = ID_ANI_MARIO_RACCOON_BRACE_RIGHT;
									else if (ax == MARIO_ACCEL_RUN_X)
										aniId = ID_ANI_MARIO_RACCOON_RUNNING_RIGHT;
									else if (ax == MARIO_ACCEL_WALK_X || ax == MARIO_ACCEL_WALK_TO_RUN || ax == 0)
										aniId = ID_ANI_MARIO_RACCOON_WALKING_RIGHT;
						}
						else // vx < 0
						{
							if (isHolding)
							{
								aniId = ID_ANI_MARIO_RACCOON_HOLD_LEFT_WALKING;
							}
							else
								if (isKicking)
								{
									aniId = ID_ANI_MARIO_RACCOON_KICK_LEFT;
								}
								else
									if (ax > 0)
										aniId = ID_ANI_MARIO_RACCOON_BRACE_LEFT;
									else if (ax == -MARIO_ACCEL_RUN_X)
										aniId = ID_ANI_MARIO_RACCOON_RUNNING_LEFT;
									else if (ax == -MARIO_ACCEL_WALK_X || ax == -MARIO_ACCEL_WALK_TO_RUN || ax == 0)
										aniId = ID_ANI_MARIO_RACCOON_WALKING_LEFT;
						}

	if (aniId == -1) aniId = ID_ANI_MARIO_RACCOON_IDLE_RIGHT;

	return aniId;
}


void CMario::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	if (state == MARIO_STATE_DIE)
		aniId = ID_ANI_MARIO_DIE;
	else if (level == MARIO_LEVEL_RACCOON)
		aniId = GetAniIdRaccoon();
	else if (level == MARIO_LEVEL_BIG)
		aniId = GetAniIdBig();
	else if (level == MARIO_LEVEL_SMALL)
		aniId = GetAniIdSmall();

	animations->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
	
	//DebugOutTitle(L"Coins: %d", coin);
}

void CMario::SetState(int state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == MARIO_STATE_DIE) return;
	if (isPipe && state != MARIO_STATE_OUTER_PIPE) return;

	switch (state)
	{
	case MARIO_STATE_RUNNING_RIGHT:
		if (isSitting) return;
		maxVx = MARIO_RUNNING_SPEED;
		ax = MARIO_ACCEL_RUN_X;
		nx = 1;
		break;
	case MARIO_STATE_RUNNING_LEFT:
		if (isSitting) return;
		maxVx = -MARIO_RUNNING_SPEED;
		ax = -MARIO_ACCEL_RUN_X;
		nx = -1;
		break;
	case MARIO_STATE_WALKING_RIGHT:
		if (isSitting) return;
		if (vx < 0 && -vx > MARIO_WALKING_SPEED)
		{
			maxVx = MARIO_RUNNING_SPEED;
			ax = MARIO_DECEL_RUN;
		}
		else
		{
			maxVx = MARIO_WALKING_SPEED;
			ax = MARIO_ACCEL_WALK_X;
		}
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		if (isSitting) return;
		if (vx > 0 && -vx < -MARIO_WALKING_SPEED)
		{
			maxVx = -MARIO_RUNNING_SPEED;
			ax = -MARIO_DECEL_RUN;
		}
		else
		{
			maxVx = -MARIO_WALKING_SPEED;
			ax = -MARIO_ACCEL_WALK_X;
		}

		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		if (isSitting) return;
		if (isOnPlatform)
		{
			if (abs(this->vx) == MARIO_RUNNING_SPEED)
				vy = -MARIO_JUMP_RUN_SPEED_Y;
			else
				vy = -MARIO_JUMP_SPEED_Y;
		}
		else if (abs(this->vx) == MARIO_RUNNING_SPEED && level == MARIO_LEVEL_RACCOON)
		{
			isFlying = TRUE;
			vy = -MARIO_FLYING_SPEED;
		}
		else if (level == MARIO_LEVEL_RACCOON)
		{
			isWagging = TRUE;
			vy = -MARIO_FLY_FALLING_SPEED;
		}
		break;

	case MARIO_STATE_RELEASE_JUMP:
		if (vy < 0) vy += MARIO_JUMP_SPEED_Y / 3;
		isFlying = FALSE;
		isWagging = FALSE;
		break;
	case MARIO_STATE_UP:
		if (readyToPipe)
		{
			SetState(MARIO_STATE_ENTER_PIPE);
			return;
		}
		return;
		break;
	case MARIO_STATE_SIT:
		if (readyToPipe)
		{
			SetState(MARIO_STATE_ENTER_PIPE);
			return;
		}
		if (vx != 0) return;
		if (isOnPlatform && level != MARIO_LEVEL_SMALL)
		{
			state = MARIO_STATE_IDLE;
			isSitting = true;
			vx = 0; vy = 0.0f;
			y += MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MARIO_STATE_SIT_RELEASE:
		if (isSitting)
		{
			isSitting = false;
			state = MARIO_STATE_IDLE;
			y -= MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MARIO_STATE_KICK:
		if (isSitting) return;
		isKicking = TRUE;
		kick_start = GetTickCount64();
		break;
	case MARIO_STATE_READY_TO_HOLD:
		readyToHold = TRUE;
		break;
	case MARIO_STATE_HOLDING_KOOPA:
		readyToHold = FALSE;
		isHolding = TRUE;
		break;

	case MARIO_STATE_HOLDING_KOOPA_RELEASE:
		readyToHold = FALSE;
		if (isHolding)
		{
			isHolding = FALSE;
			CKoopa* koopa = dynamic_cast<CKoopa*>(koopaShell);
			SetState(MARIO_STATE_KICK);
			koopa->KickedByMario(nx);
			koopaShell = nullptr;
		}
		break;

	case MARIO_STATE_ACCEL_TO_RUN_LEFT:
		if (isSitting) return;
		maxVx = -MARIO_RUNNING_SPEED;
		if (vx <= 0)
			ax = -MARIO_ACCEL_WALK_TO_RUN;
		else
			ax = -MARIO_DECEL_RUN;
		nx = -1;
		break;

	case MARIO_STATE_ACCEL_TO_RUN_RIGHT:
		if (isSitting) return;
		maxVx = MARIO_RUNNING_SPEED;
		if (vx >= 0)
			ax = MARIO_ACCEL_WALK_TO_RUN;
		else
			ax = MARIO_DECEL_RUN;
		nx = 1;
		break;

	case MARIO_STATE_TAIL_ATTACK:
		isTailAttacking = TRUE;
		tail_attack_start = GetTickCount64();

		break;

	case MARIO_STATE_IDLE:
		ax = 0.0f;
		//vx = 0.0f;
		break;

	case MARIO_STATE_DIE:
		vy = -MARIO_JUMP_DEFLECT_SPEED;
		vx = 0;
		ax = 0;
		break;

	case MARIO_STATE_ENTER_PIPE:
		readyToPipe = FALSE;
		isPipe = TRUE;
		pipe_start = GetTickCount64();
		vy = pipeDestination_ny * MARIO_ENTER_PIPE_SPEED;
		x = pipeDestination_x + GRID_SIZE / 2;
		break;

	case MARIO_STATE_OUTER_PIPE:
		isPipe = TRUE;
		pipe_start = GetTickCount64();

		float exit_x, exit_y;

		CPipe* exit_pipe = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->GetExitPipeWithIndex(pipeDestination);
		exit_pipe->GetPosition(exit_x, exit_y);

		x = exit_x + GRID_SIZE / 2;

		if (exit_pipe->GetMap() == MAP_MAIN)
			y = exit_y;
		else
			y = exit_y;
		vy = pipeDestination_ny * MARIO_ENTER_PIPE_SPEED;
		break;
	}


	CGameObject::SetState(state);
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (level == MARIO_LEVEL_BIG || level == MARIO_LEVEL_RACCOON)
	{
		if (isSitting)
		{
			left = x - MARIO_BIG_SITTING_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_SITTING_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_SITTING_BBOX_WIDTH;
			bottom = top + MARIO_BIG_SITTING_BBOX_HEIGHT;
		}
		else
		{
			float width = (!isTailAttacking) ? MARIO_BIG_BBOX_WIDTH : MARIO_RACCOON_TAIL_ATTACK_WIDTH;
			left = x - width / 2;
			top = y - MARIO_BIG_BBOX_HEIGHT / 2;
			right = left + width;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}

	}
	else
	{
		left = x - MARIO_SMALL_BBOX_WIDTH / 2;
		top = y - MARIO_SMALL_BBOX_HEIGHT / 2;
		right = left + MARIO_SMALL_BBOX_WIDTH;
		bottom = top + MARIO_SMALL_BBOX_HEIGHT;
	}
}

void CMario::SetLevel(int l)
{
	// Adjust position to avoid falling off platform
	if (this->level == MARIO_LEVEL_SMALL)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
	}

	//
	if (this->level != l)
	{
		isTransform = TRUE;
		transform_start = GetTickCount64();

		CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		scene->Pause();
	}

	previous_level = this->level;
	level = l;

}

int CMario::GetLevel()
{
	return this->level;
}


