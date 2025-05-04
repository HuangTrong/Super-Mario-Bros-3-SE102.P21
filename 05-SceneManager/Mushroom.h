#pragma once
#include "State.h"
#include "SuperMushroom.h"


class CMushroom : public CSuperMushroom
{
public:
	CMushroom(float x, float y);
	virtual void Render();
};