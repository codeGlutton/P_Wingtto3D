#pragma once

#include "Core/Object.h"
#include "Graphics/Widget/CompoundWidget.h"

struct ViewportDesc
{
	GEN_STRUCT_REFLECTION(ViewportDesc)

	PROPERTY(mWidth)
	float mWidth = 800.f;
	PROPERTY(mHeight)
	float mHeight = 600.f;
};

/**
 * 특정 랜더 타겟 창을 표기할 viewport 베이스 클래스 (렌더 스레드에서 드로우콜)
 */
class Viewport abstract : public CompoundWidget
{
	GEN_ABSTRACT_REFLECTION(Viewport)

	friend class AppWindow;

public:
	Viewport();
	~Viewport();

public:
	const ViewportDesc& GetDesc() const
	{
		return _mDesc;
	}

protected:
	virtual void OnResize(const RECT& windowSize);

private:
	virtual void Render() = 0;

private:
	PROPERTY(_mDesc)
	ViewportDesc _mDesc;
};

