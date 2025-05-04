#include "Button.h"
#include "Brick.h"

void CButton::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	int height = (state == BUTTON_STATE_IDLE) ? BUTTON_BBOX_HEIGHT : BUTTON_PRESSED_BBOX_HEIGHT;
	l = x - BUTTON_BBOX_WIDTH / 2;
	t = y - height / 2;
	r = l + BUTTON_BBOX_WIDTH;
	b = t + height;
}
void CButton::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (areBricksHandled)
		return;
	if (isPressed)
	{
		for (int i = 0; i < coObjects->size(); i++)
			if (dynamic_cast<CBrick*>(coObjects->at(i)))
			{
				CBrick* brick = dynamic_cast<CBrick*>(coObjects->at(i));
				if (brick->GetState() != BRICK_STATE_EMPTY)
					brick->SetState(BRICK_STATE_TRANS_TO_COIN);
			}
		areBricksHandled = TRUE;
	}
}
void CButton::Render() {
	if (isHiding)
		return;

	int aniId = -1;
	if (state == BUTTON_STATE_IDLE)
		aniId = ID_ANI_BUTTON_IDLE;
	else
		aniId = ID_ANI_BUTTON_PRESSED;

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
}
void CButton::SetState(int state) {
	switch (state)
	{
	case BUTTON_STATE_HIDING:
		isHiding = TRUE;
		break;
	case BUTTON_STATE_IDLE:
		isHiding = FALSE;
		break;
	case BUTTON_STATE_PRESSED:
		y += (BUTTON_BBOX_HEIGHT - BUTTON_PRESSED_BBOX_HEIGHT) / 2;
		isPressed = TRUE;
		break;
	}
	CGameObject::SetState(state);
}