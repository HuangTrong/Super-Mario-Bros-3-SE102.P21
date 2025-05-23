#include "HeadUpDisplay.h"
#include "Configs.h"
#include "debug.h"

#include "AssetIDs.h"
#include "Sprites.h"

#include "Mario.h"

HUD* HUD::__instance = NULL;

HUD* HUD::GetInstance() {
	if (__instance == NULL) __instance = new HUD();
	return __instance;
}
HUD::HUD() {


	CGame* game = CGame::GetInstance();
	float s_width, s_height;
	float cx, cy;
	game->GetScreenSize(s_width, s_height);
	game->GetCamPos(cx, cy);

	this->x = cx;
	this->y = s_height + cy - GRID_SIZE * 3;

	background = new CBackgroundElement(x, y, 4, s_width / GRID_SIZE + 1, ID_SPRITE_BG_COLOR_BLACK);
}

void HUD::UpdateRelativePosition() {
	// get cam pos & screen size
	CGame* game = CGame::GetInstance();
	float s_width, s_height;
	float cx, cy;
	game->GetScreenSize(s_width, s_height);
	game->GetCamPos(cx, cy);

	this->x = cx;
	this->y = s_height + cy - GRID_SIZE * 3;

}
void HUD::Update() {
	UpdateRelativePosition();
	background->SetPosition(x, y);
}
void HUD::Render() {
	CSprites* sprires = CSprites::GetInstance();
	CData* data = CGame::GetInstance()->GetData();
	//render bg
	background->Render();

	//render panel
	sprires->Get(ID_SPRITE_HUD_PANEL)->Draw(
		x + (PANEL_WIDTH / 2 - GRID_SIZE / 2) + PANEL_WIDTH_BUFFER,
		y + (PANEL_HEIGHT / 2 - GRID_SIZE / 2) + PANEL_HEIGHT_BUFFER);

	//render cards
	float card_star_x = x + (PANEL_WIDTH - GRID_SIZE / 2) + PANEL_WIDTH_BUFFER + PANEL_SPACE_BETWEEN + CARD_WIDTH / 2;

	for (int i = 0; i < data->GetCards().size(); i++)
		sprires->Get(ID_SPRITE_CARD + data->GetCards()[i])->Draw(
			card_star_x + CARD_WIDTH * i,
			y + (CARD_HEIGHT / 2 - GRID_SIZE / 2) + PANEL_HEIGHT_BUFFER
		);

	//render speed panel
	float speed = data->GetSpeed();
	for (int i = 0; i < TRIANGLE_MAX_QUANTITY; i++)
	{
		if (abs(speed) >= MARIO_WALKING_SPEED + (MARIO_RUNNING_SPEED_STEP * (i + 1)))
			sprires->Get(ID_SPRITE_HUD_TRIANGLE)->Draw(
				x + PANEL_WIDTH_BUFFER + TRIANGLE_BUFFER_WIDTH - GRID_SIZE / 2 + CHARACTER_SIZE / 2 + CHARACTER_SIZE * i,
				y + PANEL_HEIGHT_BUFFER + TRIANGLE_BUFFER_HEIGHT - GRID_SIZE / 2 + CHARACTER_SIZE / 2
			);
	}
	if (abs(speed) >= MARIO_RUNNING_SPEED)
		CAnimations::GetInstance()->Get(ID_ANI_P_BAGDE_FLICKER)->Render(
			x + PANEL_WIDTH_BUFFER + P_BAGDE_BUFFER_WIDTH - GRID_SIZE / 2 + P_BAGDE_WIDTH / 2,
			y + PANEL_HEIGHT_BUFFER + P_BAGDE_BUFFER_HEIGHT - GRID_SIZE / 2 + P_BAGDE_HEIGHT / 2
		);

	//render data
	// 
	// world
	sprires->Get(ID_SPRITE_NUMBER + data->GetWorld())->Draw(
		x + PANEL_WIDTH_BUFFER + WORLD_INDEX_BUFFER_WIDTH - GRID_SIZE / 2 + CHARACTER_SIZE / 2,
		y + PANEL_HEIGHT_BUFFER + WORLD_INDEX_BUFFER_HEIGHT - GRID_SIZE / 2 + CHARACTER_SIZE / 2
	);
	// life
	{
		UINT life = data->GetLife();
		int count = 0;
		while (life > 0)
		{
			int digit = life % 10;
			sprires->Get(ID_SPRITE_NUMBER + digit)->Draw(
				x + PANEL_WIDTH_BUFFER + LIFE_BUFFER_WIDTH - GRID_SIZE / 2 + CHARACTER_SIZE / 2 + CHARACTER_SIZE * (LIFE_MAX_DIGIT - count - 1),
				y + PANEL_HEIGHT_BUFFER + LIFE_BUFFER_HEIGHT - GRID_SIZE / 2 + CHARACTER_SIZE / 2
			);
			count++;
			life /= 10;
		}
	}
	//score
	{
		UINT score = data->GetScore();
		int count = 0;
		while (score > 0)
		{
			int digit = score % 10;
			sprires->Get(ID_SPRITE_NUMBER + digit)->Draw(
				x + PANEL_WIDTH_BUFFER + SCORE_BUFFER_WIDTH - GRID_SIZE / 2 + CHARACTER_SIZE / 2 + CHARACTER_SIZE * (SCORE_MAX_DIGIT - (count + 1)),
				y + PANEL_HEIGHT_BUFFER + SCORE_BUFFER_HEIGHT - GRID_SIZE / 2 + CHARACTER_SIZE / 2
			);
			count++;
			score /= 10;
		}
		while (count < SCORE_MAX_DIGIT)
		{
			sprires->Get(ID_SPRITE_NUMBER + 0)->Draw(
				x + PANEL_WIDTH_BUFFER + SCORE_BUFFER_WIDTH - GRID_SIZE / 2 + CHARACTER_SIZE / 2 + CHARACTER_SIZE * (SCORE_MAX_DIGIT - (count + 1)),
				y + PANEL_HEIGHT_BUFFER + SCORE_BUFFER_HEIGHT - GRID_SIZE / 2 + CHARACTER_SIZE / 2
			);
			count++;
		}
	}
	//coin
	{
		UINT coin = data->GetCoin();
		int count = 0;
		while (coin > 0)
		{
			int digit = coin % 10;
			sprires->Get(ID_SPRITE_NUMBER + digit)->Draw(
				x + PANEL_WIDTH_BUFFER + COIN_BUFFER_WIDTH - GRID_SIZE / 2 + CHARACTER_SIZE / 2 + CHARACTER_SIZE * (COIN_MAX_DIGIT - (count + 1)),
				y + PANEL_HEIGHT_BUFFER + COIN_BUFFER_HEIGHT - GRID_SIZE / 2 + CHARACTER_SIZE / 2
			);
			count++;
			coin /= 10;
		}

		if (count == 0) {
			sprires->Get(ID_SPRITE_NUMBER + 0)->Draw(
				x + PANEL_WIDTH_BUFFER + COIN_BUFFER_WIDTH - GRID_SIZE / 2 + CHARACTER_SIZE / 2 + CHARACTER_SIZE * (COIN_MAX_DIGIT - 1),
				y + PANEL_HEIGHT_BUFFER + COIN_BUFFER_HEIGHT - GRID_SIZE / 2 + CHARACTER_SIZE / 2
			);
		}
	}
	//timer
	{
		UINT timer = data->GetTimer();
		int count = 0;
		while (timer > 0)
		{
			int digit = timer % 10;
			sprires->Get(ID_SPRITE_NUMBER + digit)->Draw(
				x + PANEL_WIDTH_BUFFER + TIMER_BUFFER_WIDTH - GRID_SIZE / 2 + CHARACTER_SIZE / 2 + CHARACTER_SIZE * (TIMER_MAX_DIGIT - (count + 1)),
				y + PANEL_HEIGHT_BUFFER + TIMER_BUFFER_HEIGHT - GRID_SIZE / 2 + CHARACTER_SIZE / 2
			);
			count++;
			timer /= 10;
		}
		while (count < TIMER_MAX_DIGIT)
		{
			sprires->Get(ID_SPRITE_NUMBER + 0)->Draw(
				x + PANEL_WIDTH_BUFFER + TIMER_BUFFER_WIDTH - GRID_SIZE / 2 + CHARACTER_SIZE / 2 + CHARACTER_SIZE * (TIMER_MAX_DIGIT - (count + 1)),
				y + PANEL_HEIGHT_BUFFER + TIMER_BUFFER_HEIGHT - GRID_SIZE / 2 + CHARACTER_SIZE / 2
			);
			count++;
		}
	}


}