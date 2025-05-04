#pragma once
#include "GameObject.h"


#define BUTTON_BBOX_WIDTH 16
#define BUTTON_BBOX_HEIGHT 16
#define BUTTON_PRESSED_BBOX_HEIGHT 8

#define BUTTON_STATE_HIDING	-100
#define BUTTON_STATE_IDLE		100
#define BUTTON_STATE_PRESSED	200

#define ID_ANI_BUTTON_IDLE		12001
#define ID_ANI_BUTTON_PRESSED	12002

class CButton : public CGameObject
{
	BOOLEAN isPressed;
	BOOLEAN areBricksHandled;
	BOOLEAN isHiding;
public:
	CButton(int x, int y): CGameObject(x, y) {
		isPressed = FALSE;
		isHiding = FALSE;
		areBricksHandled = FALSE;
		SetState(BUTTON_STATE_HIDING);
	}
	int IsBlocking() { return !isHiding; }
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	void SetState(int state);
};
