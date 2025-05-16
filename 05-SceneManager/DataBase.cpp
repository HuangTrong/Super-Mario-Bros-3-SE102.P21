#include "DataBase.h"
#include "Mario.h"
#include "Utils.h"
#include "TestMapScene.h"


void CData::ResetData() {
	world = WORLD_DEFAULT;
	life = LIFE_DEFAULT;
	score = SCORE_DEFAULT;
	coin = COIN_DEFAULT;
	timer = TIMER_DEFAULT;
	speed = 0;

	for (int i = 0; i < CARD_QUANTITY; i++)
		cards.push_back(CARD_BLANK);

	player_level = MARIO_LEVEL_BIG;

	current_map = PORTAL_MAP_START;
	pass_map = PORTAL_MAP_START;
}