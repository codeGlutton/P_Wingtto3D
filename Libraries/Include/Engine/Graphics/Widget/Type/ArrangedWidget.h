#pragma once

#include "Graphics/Widget/Type/WidgetTypeInclude.h"
#include "Graphics/Widget/Type/WidgetGeometry.h"

class Widget;

/**
 * 레이아웃에 맞추어 정렬된 데이터 
 * (SRT를 적용해주기 전 레이아웃에 영향을 끼치는 트랜스폼 정보 소유)
 */
struct ArrangedWidget
{
public:
	ArrangedWidget()
	{
	}
	ArrangedWidget(const std::shared_ptr<Widget>& widget, const WidgetGeometry& geometry) :
		mWidget(widget),
		mGeometry(geometry)
	{
	}

public:
	std::shared_ptr<Widget> mWidget;
	WidgetGeometry mGeometry;
};

struct ArrangedChildren
{
public:
	ArrangedChildren(VisibilityType::Flag filter) :
		mFilter(filter)
	{
	}

public:
	std::size_t Size() const
	{
		return mArrangedWidgets.size();
	}

	void Reserve(std::size_t size)
	{
		mArrangedWidgets.reserve(size);
	}

	void AddWidget(const ArrangedWidget& widget, VisibilityType::Flag visibility)
	{
		if (CanAccept(visibility) == true)
		{
			mArrangedWidgets.push_back(widget);
		}
	}

	void InsertWidget(const ArrangedWidget& widget, VisibilityType::Flag visibility, std::size_t index)
	{
		if (CanAccept(visibility) == true)
		{
			mArrangedWidgets.insert(mArrangedWidgets.begin() + index, widget);
		}
	}

	bool CanAccept(VisibilityType::Flag visibility) const
	{
		return visibility & mFilter;
	}

public:
	VisibilityType::Flag mFilter;
	std::vector<ArrangedWidget> mArrangedWidgets;
};
