#pragma once

#include "Core/Object.h"

struct ViewportDesc
{
	GEN_STRUCT_REFLECTION(ViewportDesc)

	PROPERTY(mWidth)
	float mWidth = 800.f;
	PROPERTY(mHeight)
	float mHeight = 600.f;
};

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
	virtual void Render() = 0;

private:
	PROPERTY(_mDesc)
	ViewportDesc _mDesc;
};

