#pragma once

#include "Graphics/Widget/Type/ArrangedWidget.h"

class Widget;
class AppWindow;

class WidgetPath
{
public:
	static WidgetPath GetPathDownTo(std::shared_ptr<Widget> widget);

public:
	std::vector<ArrangedWidget> mWidgets;
	std::weak_ptr<AppWindow> mRootWindow;
};

class WidgetWeakPath
{
public:
	bool IsValid() const
	{
		return mWidgets.empty() == false;
	}

public:
	WidgetWeakPath& operator=(const WidgetPath& path)
	{
		Clear();
		mWidgets.reserve(path.mWidgets.size());
		for (auto& arrangedWidget : path.mWidgets)
		{
			mWidgets.push_back(arrangedWidget.mWidget);
			mGeometries.push_back(arrangedWidget.mGeometry);
		}
		mRootWindow = path.mRootWindow;

		return *this;
	}
	WidgetWeakPath& operator=(WidgetPath&& path)
	{
		Clear();
		mWidgets.reserve(path.mWidgets.size());
		for (auto& arrangedWidget : path.mWidgets)
		{
			mWidgets.push_back(arrangedWidget.mWidget);
			mGeometries.push_back(arrangedWidget.mGeometry);
		}
		mRootWindow = path.mRootWindow;

		return *this;
	}

public:
	WidgetPath Lock() const
	{
		WidgetPath path;

		std::size_t size = mWidgets.size();
		for (std::size_t i = 0; i < size; ++i)
		{
			path.mWidgets.push_back(ArrangedWidget(mWidgets[i].lock(), mGeometries[i]));
		}
		path.mRootWindow = mRootWindow;

		return path;
	}

public:
	void Clear()
	{
		mWidgets.clear();
		mGeometries.clear();
		mRootWindow.reset();
	}

public:
	std::vector<std::weak_ptr<Widget>> mWidgets;
	std::vector<WidgetGeometry> mGeometries;
	std::weak_ptr<AppWindow> mRootWindow;
};