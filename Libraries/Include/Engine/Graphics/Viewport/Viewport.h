#pragma once

#include "Core/Object.h"

/**
 * 창을 표기할 viewport 베이스 클래스 (렌더 스레드에서 드로우콜)
 */
class Viewport abstract : public Object
{
	GEN_ABSTRACT_REFLECTION(Viewport)

public:
	Viewport();
	~Viewport();

public:
	void SetSize();
	void GetSize();

private:
	virtual void ReDraw() = 0;

private:
	PROPERTY(_mWidth)
	float _mWidth;
	PROPERTY(_mHeight)
	float _mHeight;
};

