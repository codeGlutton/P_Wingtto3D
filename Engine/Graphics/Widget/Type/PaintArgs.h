#pragma once

#include "Graphics/Widget/Type/HitTestGrid.h"
#include "Core/Transform.h"

class Widget;

struct PaintArgs
{
public:
	PaintArgs(std::shared_ptr<const Widget> widget, HitTestGrid& grid, Vec2 windowOffset, float deltaTime) :
		_mParentWidget(widget),
		_mMainHitTestGrid(grid),
		_mWindowOffset(windowOffset),
		_mDeltaTime(deltaTime),
		_mInheritedHittable(true)
	{
	}

	PaintArgs WithNewParent(std::shared_ptr<const Widget> parent) const
	{
		PaintArgs args(*this);
		args._mParentWidget = parent;

		return args;
	}

public:
	HitTestGrid& GetHitTestGrid() const
	{
		return _mMainHitTestGrid;
	}

	Transform2D GetRootToScreenTransform2D() const
	{
		return Transform2D(_mWindowOffset, 0.f, Vec2());
	}

	float GetDeltaTime() const
	{
		return _mDeltaTime;
	}

	void SetInheritedHittable(bool inheritedHittable)
	{
		_mInheritedHittable = inheritedHittable;
	}

	bool GetInheritedHittable() const
	{
		return _mInheritedHittable;
	}

private:
	std::shared_ptr<const Widget> _mParentWidget;
	HitTestGrid& _mMainHitTestGrid;
	Vec2 _mWindowOffset;
	float _mDeltaTime;
	bool _mInheritedHittable = false;
};

