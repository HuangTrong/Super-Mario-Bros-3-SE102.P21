#pragma once
#include "Scene.h"

class CTestMapKeyHandler : public CSceneKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	CTestMapKeyHandler(LPSCENE s) :CSceneKeyHandler(s) {};
};