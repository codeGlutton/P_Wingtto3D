#include "pch.h"
#include "HitTestGrid.h"

#include "Graphics/Widget/Widget.h"
#include "Graphics/Widget/Type/WidgetPath.h"

IntPoint<uint32> HitTestGrid::GetCellIndex(const Vec2& position) const
{
	return IntPoint<uint32>(
		std::min<int32>(
			std::max<int32>(static_cast<int32>(std::ceilf(position.x / _mCellSize.x)), 0),
			static_cast<int32>(_mCellCount.mX) - 1
		),
		std::min<int32>(
			std::max<int32>(static_cast<int32>(std::ceilf(position.y / _mCellSize.y)), 0),
			static_cast<int32>(_mCellCount.mY) - 1
		)
	);
}

IntPoint<uint32> HitTestGrid::GetCellIndex(const POINT& position) const
{
	return IntPoint<uint32>(
		std::min<int32>(
			std::max<int32>(static_cast<int32>(std::ceilf(position.x / _mCellSize.x)), 0),
			static_cast<int32>(_mCellCount.mX) - 1
		),
		std::min<int32>(
			std::max<int32>(static_cast<int32>(std::ceilf(position.y / _mCellSize.y)), 0),
			static_cast<int32>(_mCellCount.mY) - 1
		)
	);
}

HitTestGrid::Cell& HitTestGrid::GetCell(IntPoint<uint32> pos)
{
	return GetCell(pos.mX, pos.mY);
}

HitTestGrid::Cell& HitTestGrid::GetCell(uint32 x, uint32 y)
{
	return _mCells[_mCellCount.mX * y + x];
}

const HitTestGrid::Cell& HitTestGrid::GetCell(IntPoint<uint32> pos) const
{
	return GetCell(pos.mX, pos.mY);
}

const HitTestGrid::Cell & HitTestGrid::GetCell(uint32 x, uint32 y) const
{
	return _mCells[_mCellCount.mX * y + x];
}

bool HitTestGrid::SetHitTestArea(const Vec2& screenPos, const Vec2& clientSize)
{
	bool needToClearCells = false;

	// 리사이즈 된 경우
	if (clientSize != _mClientSize)
	{
		needToClearCells = true;

		_mClientSize = clientSize;
		_mCellCount = IntPoint<uint32>(
			static_cast<uint32>(std::ceilf(clientSize.x / _mCellSize.x)),
			static_cast<uint32>(std::ceilf(clientSize.y / _mCellSize.y))
		);

		const uint32 cellTotalCount = _mCellCount.mX * _mCellCount.mY;
		if (cellTotalCount <= 0)
		{
			_mClientSize = Vec2::Zero;
			_mCellCount = 0;
			Clear_Internal();
		}
		else
		{
			Clear_Internal(cellTotalCount);
		}
	}

	_mScreenPos = screenPos;

	return needToClearCells;
}

void HitTestGrid::AddWidget(std::shared_ptr<Widget> widget, uint32 layerId)
{
	if (widget == nullptr || (widget->GetVisibility() & VisibilityType::HittableFlag) == false)
	{
		return;
	}

	WidgetGeometry copyGeometry = widget->GetScreenGeometry();

	BoundingAABB2D boundingBox = copyGeometry.GetRenderBoundingBox();
	const IntPoint<uint32> leftTopCell = GetCellIndex(Vec2(static_cast<float>(boundingBox.x), static_cast<float>(boundingBox.y)));
	const IntPoint<uint32> rightBottomCell = GetCellIndex(Vec2(static_cast<float>(boundingBox.x + boundingBox.width), static_cast<float>(boundingBox.y + boundingBox.height)));
	
	auto iter = _mWidgetMap.find(widget.get());
	if (iter != _mWidgetMap.end())
	{
		const uint32 widgetIndex = (*iter).second;
		WidgetData& widgetData = _mWidgets[widgetIndex];

		if (widgetData.mRightBottomCell != rightBottomCell || widgetData.mLeftTopCell != leftTopCell)
		{
			RemoveWidget(widget);
		}
		else
		{
			widgetData.mLayerId = layerId;
			return;
		}
	}

	const uint32 widgetIndex = static_cast<uint32>(_mWidgets.size());
	_mWidgetMap[widget.get()] = widgetIndex;
	_mWidgets.push_back(WidgetData{ widget, leftTopCell, rightBottomCell, layerId });
	for (uint32 x = leftTopCell.mX; x <= leftTopCell.mX; ++x)
	{
		for (uint32 y = rightBottomCell.mY; y <= rightBottomCell.mY; ++y)
		{
			GetCell(x, y).mWidgetIndexes.push_back(widgetIndex);
		}
	}
}

void HitTestGrid::RemoveWidget(std::shared_ptr<Widget> widget)
{
	auto iter = _mWidgetMap.find(widget.get());
	if (iter == _mWidgetMap.end())
	{
		return;
	}

	const uint32 widgetIndex = (*iter).second;
	WidgetData& widgetData = _mWidgets[widgetIndex];

	if (_mCellCount.mX * _mCellCount.mY != 0)
	{
		const IntPoint<uint32> leftTopCell(
			std::max<uint32>(widgetData.mLeftTopCell.mX, 0), 
			std::max<uint32>(widgetData.mLeftTopCell.mY, 0)
		);
		const IntPoint<uint32> rightBottomCell(
			std::max<uint32>(widgetData.mRightBottomCell.mX, static_cast<uint32>(_mClientSize.x) - 1),
			std::max<uint32>(widgetData.mRightBottomCell.mY, static_cast<uint32>(_mClientSize.y) - 1)
		);

		for (uint32 x = leftTopCell.mX; x <= leftTopCell.mX; ++x)
		{
			for (uint32 y = rightBottomCell.mY; y <= rightBottomCell.mY; ++y)
			{
				SwapAndRemove(GetCell(x, y).mWidgetIndexes, widgetIndex);
			}
		}
	}

	_mWidgetMap.erase(widget.get());
	SwapAndRemove(_mWidgets, widgetIndex);
	FAIL_MSG("Invalid function call");

	// Dirty 되어 내부 인덱싱이 꼬임. 
	// (단 매 업데이트에서 HitTestGrid가 비워지기 떄문에 애초에 호출되지 않는 부분)
	// 
	// + 참고.
	// 언리얼의 Fast Path의 경우는 조금 다른 방식으로 접근
	// Grid를 내부 위젯이 소유하여 붙이는 형태로 사용 
	// (WidgetArray를 채우지 않고 별도의 Grid Array를 소유)
	// 레이아웃에 영향을 주지 않는 경우, 그대로 사용
	// 레이아웃에 영향을 주는 경우, 자식 위젯을 포함한 모든 소속 Grid는 제거되며
	// Slow Path로 처리되고 다음 틱 이전에 다시 Grid 채워짐
}

void HitTestGrid::GetWidgetUnderScreenPos(POINT pos, OUT ArrangedWidget& arrangedWidget) const
{
	if (IsValid() == false)
	{
		return;
	}

	IntPoint<uint32> cellIndex = GetCellIndex(pos);
	const Cell& cell = GetCell(cellIndex);

	auto iterEnd = cell.mWidgetIndexes.rend();
	for (auto iter = cell.mWidgetIndexes.rbegin(); iter != iterEnd; --iter)
	{
		uint32 widgetIndex = cell.mWidgetIndexes[*iter];
		if (_mWidgets.size() <= widgetIndex)
		{
			continue;
		}

		std::shared_ptr<Widget> widget = _mWidgets[widgetIndex].mWidget.lock();
		if (widget == nullptr || widget->IsValid() == false)
		{
			continue;
		}

		if (widget->GetScreenGeometry().GetRenderBoundingBox().Contains(pos.x, pos.y) == true)
		{
			arrangedWidget = ArrangedWidget(widget, widget->GetScreenGeometry());
		}
	}
}

void HitTestGrid::GetWidgetPathUnderScreenPos(POINT pos, OUT WidgetPath& path) const
{
	if (IsValid() == false)
	{
		return;
	}

	IntPoint<uint32> cellIndex = GetCellIndex(pos);
	const Cell& cell = GetCell(cellIndex);
	
	auto iterEnd = cell.mWidgetIndexes.rend();
	for (auto iter = cell.mWidgetIndexes.rbegin(); iter != iterEnd; --iter)
	{
		uint32 widgetIndex = cell.mWidgetIndexes[*iter];
		if (_mWidgets.size() <= widgetIndex)
		{
			continue;
		}

		std::shared_ptr<Widget> widget = _mWidgets[widgetIndex].mWidget.lock();
		if (widget == nullptr || widget->IsValid() == false)
		{
			continue;
		}

		if (widget->GetScreenGeometry().GetRenderBoundingBox().Contains(pos.x, pos.y) == true)
		{
			while (widget != nullptr && widget->IsValid() == false)
			{
				path.mWidgets.push_back(ArrangedWidget(widget, widget->GetScreenGeometry()));
				widget = widget->GetParent();
			}
			break;
		}
	}
}

void HitTestGrid::Clear()
{
	Clear_Internal(static_cast<uint32>(_mCells.size()));
}

void HitTestGrid::Clear_Internal(uint32 resize)
{
	if (resize == 0)
	{
		_mCells.clear();
	}
	else
	{
		_mCells.resize(resize);
		for (Cell& Cell : _mCells)
		{
			Cell.mWidgetIndexes.clear();
		}
	}

	_mWidgetMap.clear();
	_mWidgets.clear();
}
