#pragma once

#include "Graphics/Viewport/Viewport.h"

class Viewport;

enum class SplitDirection
{
	Vertical,
	Horizon
};

/**
 * Viewport 컴포짓
 */
class ViewportComposite : public Viewport
{
	GEN_REFLECTION(ViewportComposite)

public:
	ViewportComposite();

public:
	void SetDir(SplitDirection dir)
	{
		_mDir = dir;
		UpdateViewports();
	}

	void AddChild(std::shared_ptr<Viewport> child)
	{
		if (child == nullptr)
		{
			return;
		}
		_mChildren.push_back(child);
		UpdateViewports();
	}
	void RemoveChild(std::shared_ptr<Viewport> child)
	{
		if (child == nullptr)
		{
			return;
		}
		_mChildren.remove(child);
		UpdateViewports();
	}

private:
	void UpdateViewports();
	virtual void Render() override;

private:
	SplitDirection _mDir;
	std::list<std::shared_ptr<Viewport>> _mChildren;
};

