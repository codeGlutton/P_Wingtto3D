#pragma once

#include "Utils/MathUtils.h"

class Widget;
class WidgetPath;
struct ArrangedWidget;

/**
 * 해당 그리드 공간에 어떤 위젯들이 걸치고 있는지를 의미하는 객체
 */
class HitTestGrid
{
private:
	struct WidgetData
	{
	public:
		std::shared_ptr<Widget> GetWidget()
		{
			return mWidget.lock();
		}

	public:
		std::weak_ptr<Widget> mWidget;
		IntPoint<uint32> mLeftTopCell;
		IntPoint<uint32> mRightBottomCell;
		// 형제끼리는 중복될 여지 있음
		uint32 mLayerId;
	};

	struct Cell
	{
		// 드로잉 순서대로 나열
		std::vector<uint32> mWidgetIndexes;
	};

public:
	bool IsValid() const
	{
		return _mCellCount.mX != 0 && _mCellCount.mY != 0;
	}

public:
	IntPoint<uint32> GetCellIndex(const Vec2& position) const;
	IntPoint<uint32> GetCellIndex(const POINT& position) const;

private:
	Cell& GetCell(IntPoint<uint32> pos);
	Cell& GetCell(uint32 x, uint32 y);
	const Cell& GetCell(IntPoint<uint32> pos) const;
	const Cell& GetCell(uint32 x, uint32 y) const;

public:
	bool SetHitTestArea(const Vec2& screenPos, const Vec2& clientSize);
	void AddWidget(std::shared_ptr<Widget> widget, uint32 layerId);
	void RemoveWidget(std::shared_ptr<Widget> widget);

	void GetWidgetUnderScreenPos(POINT pos, OUT ArrangedWidget& arrangedWidget) const;
	void GetWidgetPathUnderScreenPos(POINT pos, OUT WidgetPath& path) const;

public:
	void Clear();

private:
	void Clear_Internal(uint32 resize = 0);

private:
	// 모든 위젯
	std::unordered_map<Widget*, uint32> _mWidgetMap;
	std::vector<WidgetData> _mWidgets;

	// 특정 크기로 나누어 히트 판정
	std::vector<Cell> _mCells;
	IntPoint<uint32> _mCellCount;
	const Vec2 _mCellSize = { 128.f, 128.f };

	// 화면 상의 윈도우 위치
	Vec2 _mScreenPos;
	// 윈도우 사이즈
	Vec2 _mClientSize;
};

