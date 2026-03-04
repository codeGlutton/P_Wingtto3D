#include "pch.h"
#include "BoxPanelWidget.h"

#include "Utils/MathUtils.h"

#include "Graphics/Widget/Type/ArrangedWidget.h"

BoxPanelWidget::BoxPanelWidget()
{
	_mVisibility.Init(VisibilityType::SelfHitTestInvisible);
}

void BoxPanelWidget::PostCreate()
{
	Super::PostCreate();
	_mSlotContainer = std::static_pointer_cast<Widget>(shared_from_this());
}

WidgetSlotContainer& BoxPanelWidget::GetChildren()
{
	return _mSlotContainer;
}

const WidgetSlotContainer& BoxPanelWidget::GetChildren() const
{
	return _mSlotContainer;
}

Vec2 BoxPanelWidget::ComputeDesireSize(const Vec2& layoutMultiplyValue)
{
	return ComputeDesiredSizeForBox();
}

void BoxPanelWidget::OnArrangeChildren(const WidgetGeometry& allottedGeometry, ArrangedChildren& children) const
{
	const float Offset = 0.0f;
	const bool AllowShrink = true;

	ArrangeChildLayouts(allottedGeometry, children, AllowShrink);
}

Vec2 BoxPanelWidget::ComputeDesiredSizeForBox()
{
	Vec2 result(0, 0);
	for (int32 i = 0; i < _mSlotContainer.Size(); ++i)
	{
		const Slot& childSlot = *static_cast<const Slot*>(&_mSlotContainer.GetSlotRef(i));
		if (childSlot.GetWidget()->GetVisibility() & VisibilityType::TakesSpaceFlag)
		{
			Vec2 childWidgetSize = childSlot.GetWidget()->GetDesiredSize();

			auto ClampSlotSize = [&childSlot](float desireSize) -> float
				{
					const float minSize = childSlot.GetMinSize();
					const float maxSize = childSlot.GetMaxSize();

					if (minSize > 0.f)
					{
						desireSize = std::max<float>(minSize, desireSize);
					}
					if (maxSize > 0.f)
					{
						desireSize = std::min<float>(maxSize, desireSize);
					}
					return desireSize;
				};

			if (_mPanelOrient == WidgetOrientation::Vertical)
			{
				result.x = std::max<float>(result.x, childWidgetSize.x + childSlot.GetPadding().GetDesiredSize(WidgetOrientation::Horizon));
				result.y += ClampSlotSize(childWidgetSize.y) + childSlot.GetPadding().GetDesiredSize(WidgetOrientation::Vertical);
			}
			else
			{
				result.y = std::max<float>(result.y, childWidgetSize.y + childSlot.GetPadding().GetDesiredSize(WidgetOrientation::Vertical));
				result.x += ClampSlotSize(childWidgetSize.x) + childSlot.GetPadding().GetDesiredSize(WidgetOrientation::Horizon);
			}
		}
	}
	return result;
}

void BoxPanelWidget::ArrangeChildLayouts(const WidgetGeometry& allottedGeometry, ArrangedChildren& children, bool allowShrink) const
{
	if (_mSlotContainer.Size() == 0)
	{
		return;
	}

	// 슬롯에 대한 사이즈 최소 최대 크기 제한 함수
	auto ClampArrangedSize = [](const float size, const float minSize, const float maxSize)
		{
			return std::clamp<float>(
				size,
				minSize > 0.f ? minSize : 0.f,
				maxSize > 0.f ? maxSize : FLT_MAX
			);
		};

	float totalStretchRate = 0.0f;
	float totalShrinkRate = 0.0f;
	float fixedSpace = 0.f;
	float shrinkableSpace = 0.0f;

	struct ArrangedChildData
	{
		float mSize = 0.0f;
		float mBaseSize = 0.0f;
		float mMinSize = 0.0f;
		float mMaxSize = 0.0f;
		float mStretchRate = 0.0f;
		float mShrinkRate = 0.0f;
		bool mIsFixedSize = false;
		WidgetSlotSizeRule mSizeRule = WidgetSlotSizeRule::ContentSize;
	};

	std::vector<ArrangedChildData> arrangedChildDatas(_mSlotContainer.Size());
	bool hasAnyVisibleChild = false;
	bool hasAnyStretchChild = false;
	bool hasAnyShrinkChild = false;

	/* 정렬 데이터 수집 */

	for (int32 i = 0; i < _mSlotContainer.Size(); ++i)
	{
		const Slot& childSlot = *static_cast<const Slot*>(&_mSlotContainer.GetSlotRef(i));
		if (childSlot.GetWidget()->GetVisibility() & VisibilityType::TakesSpaceFlag)
		{
			hasAnyVisibleChild = true;

			const Vec2 childDesiredSize = childSlot.GetWidget()->GetDesiredSize();
			const float slotMinSize = childSlot.GetMinSize();
			const float slotMaxSize = childSlot.GetMaxSize();

			fixedSpace += childSlot.GetPadding().GetDesiredSize(_mPanelOrient);
			
			float childSize = (_mPanelOrient == WidgetOrientation::Vertical) ? childDesiredSize.y : childDesiredSize.x;
			childSize = ClampArrangedSize(childSize, slotMinSize, slotMaxSize);

			ArrangedChildData& data = arrangedChildDatas[i];
			data.mMinSize = slotMinSize;
			data.mMaxSize = slotMaxSize;
			data.mSizeRule = childSlot.GetSizeRule();

			if (childSlot.GetSizeRule() == WidgetSlotSizeRule::AllottedRate)
			{
				hasAnyStretchChild = true;

				// 이후에 계산
				data.mSize = 0.0f;
				data.mBaseSize = 0.0f;
				data.mStretchRate = data.mShrinkRate = childSlot.GetStretchRate();

				totalStretchRate += data.mStretchRate;
				totalShrinkRate += data.mShrinkRate;
				shrinkableSpace += childSize;
			}
			else if (childSlot.GetSizeRule() == WidgetSlotSizeRule::DetailAllottedRate)
			{
				hasAnyShrinkChild = true;

				data.mSize = childSize;
				data.mBaseSize = childSize;
				data.mStretchRate = std::max<float>(0.f, childSlot.GetStretchRate());
				data.mShrinkRate = std::max<float>(0.f, childSlot.GetShrinkRate());

				totalStretchRate += data.mStretchRate;
				totalShrinkRate += data.mShrinkRate;
				shrinkableSpace += childSize;
			}
			else
			{
				data.mSize = childSize;
				data.mBaseSize = childSize;
				data.mStretchRate = 0.f;
				data.mShrinkRate = 0.f;
				data.mIsFixedSize = true;
				
				fixedSpace += childSize;
			}
		}
	}

	if (hasAnyVisibleChild == false)
	{
		return;
	}

	/* 비울 기반 슬롯의 경우 크기 배정 */

	const float minDynamicSpace = (allowShrink == true) ? 0.f : shrinkableSpace;
	const float allottedSize = (_mPanelOrient == WidgetOrientation::Vertical) ? allottedGeometry.mBoxSize.y : allottedGeometry.mBoxSize.x;
	float dynamicSpace = std::max<float>(minDynamicSpace, allottedSize - fixedSpace);

	if (hasAnyStretchChild == true && totalStretchRate > 0.f)
	{
		float usedSpace = 0.f;
		for (ArrangedChildData& data : arrangedChildDatas)
		{
			if (data.mSizeRule == WidgetSlotSizeRule::AllottedRate)
			{
				const float desiredRateSize = dynamicSpace * data.mStretchRate / totalStretchRate;
				data.mSize = ClampArrangedSize(desiredRateSize, data.mMinSize, data.mMaxSize);
				usedSpace += data.mSize;
			}
		}
		dynamicSpace -= usedSpace;
	}

	/* 증가 수축 별도 조절 슬롯의 경우, 지연 결과로 평가 */

	const bool hasFreeSpace = dynamicSpace > shrinkableSpace;
	const bool canFitToSpace = (hasFreeSpace == true) ? (totalStretchRate > 0.f) : (totalShrinkRate > 0.f);

	if (hasAnyShrinkChild == true && canFitToSpace == true)
	{
		int32 detailAllottedRateSlotCount = 0;
		for (ArrangedChildData& data : arrangedChildDatas)
		{
			if (data.mSizeRule == WidgetSlotSizeRule::DetailAllottedRate)
			{
				++detailAllottedRateSlotCount;
				dynamicSpace -= data.mSize;
				if (hasFreeSpace == true)
				{
					data.mIsFixedSize |= IsNearlyZero(data.mStretchRate);
				}
				else
				{
					data.mIsFixedSize |= IsNearlyZero(data.mShrinkRate);
				}
			}
		}

		const int32 maxTrial = std::min<int32>(detailAllottedRateSlotCount, 5);
		for (int32 trial = 0; trial < maxTrial; ++trial)
		{
			if (IsNearlyZero(dynamicSpace) == true)
			{
				break;
			}

			totalStretchRate = 0.f;
			totalShrinkRate = 0.f;

			for (const ArrangedChildData& data : arrangedChildDatas)
			{
				if (data.mSizeRule == WidgetSlotSizeRule::DetailAllottedRate && data.mIsFixedSize == false)
				{
					totalStretchRate += data.mStretchRate;
					totalShrinkRate += data.mShrinkRate;
				}
			}

			const float totalRate = (hasFreeSpace == true) ? totalStretchRate : totalShrinkRate;
			if (IsNearlyZero(totalRate) == true)
			{
				break;
			}

			float usedSpace = 0.f;
			for (ArrangedChildData& data : arrangedChildDatas)
			{
				if (data.mSizeRule == WidgetSlotSizeRule::DetailAllottedRate && data.mIsFixedSize == false)
				{
					const float sizeDelta = (hasFreeSpace == true) ? 
						(dynamicSpace * (data.mStretchRate / totalStretchRate)) : 
						(dynamicSpace * (data.mShrinkRate / totalShrinkRate));

					if (IsNearlyZero(sizeDelta) == true)
					{
						data.mIsFixedSize = true;
						continue;
					}

					const float minSize = data.mMinSize;
					const float maxSize = data.mMaxSize;
					const bool hasMaxConstraint = maxSize > 0.f;

					if ((data.mSize + sizeDelta) <= minSize)
					{
						usedSpace += minSize - data.mSize;
						data.mSize = minSize;
						data.mIsFixedSize = true;
					}
					else if (hasMaxConstraint == true && (data.mSize + sizeDelta) >= maxSize)
					{
						usedSpace += maxSize - data.mSize;
						data.mSize = maxSize;
						data.mIsFixedSize = true;
					}
					else
					{
						usedSpace += sizeDelta;
						data.mSize += sizeDelta;
					}
				}
			}
			dynamicSpace -= usedSpace;
		}
	}

	/* 결과 저장 */

	float slotPosOffset = 0.f;
	for (int32 i = 0; i < _mSlotContainer.Size(); ++i)
	{
		const Slot& childSlot = *static_cast<const Slot*>(&_mSlotContainer.GetSlotRef(i));
		const Margin& childPadding = childSlot.GetPadding();
		const VisibilityType::Flag childVisibility = childSlot.GetWidget()->GetVisibility();
		const float childSize = arrangedChildDatas[i].mSize;

		Vec2 localChildSize = (_mPanelOrient == WidgetOrientation::Vertical) ?
			Vec2(allottedGeometry.mBoxSize.x, childSize + childPadding.GetDesiredSize(WidgetOrientation::Vertical)) :
			Vec2(childSize + childPadding.GetDesiredSize(WidgetOrientation::Horizon), allottedGeometry.mBoxSize.y);

		const ChildAlignmentResult axisXAlignedResult = AlignChildAxisX(localChildSize.x, childSlot, childPadding);
		const ChildAlignmentResult axisYAlignedResult = AlignChildAxisY(localChildSize.y, childSlot, childPadding);

		const Vec2 localAlignedChildSize = Vec2(axisXAlignedResult.mSize, axisYAlignedResult.mSize);
		const Vec2 localAlignedChildPos = (_mPanelOrient == WidgetOrientation::Vertical) ?
			Vec2(axisXAlignedResult.mOffset, slotPosOffset + axisYAlignedResult.mOffset) :
			Vec2(slotPosOffset + axisXAlignedResult.mOffset, axisYAlignedResult.mOffset);

		children.AddWidget(
			ArrangedWidget(
				std::const_pointer_cast<Widget>(childSlot.GetWidgetRef()),
				allottedGeometry.MakeChild(
					childSlot.GetWidget(),
					localAlignedChildSize,
					Transform2D(localAlignedChildPos, 0.f, Vec2(1.f))
				)
			), 
			childVisibility
		);

		if (childVisibility & VisibilityType::TakesSpaceFlag)
		{
			slotPosOffset += (_mPanelOrient == WidgetOrientation::Vertical) ? localChildSize.y : localChildSize.x;
		}
	}
}

void BoxPanelWidget::CollectHeaderDatas(const void* inst, OUT std::unordered_map<std::wstring, std::pair<std::string, PackageBuildScope>>& externalPackageDatas, OUT std::vector<std::shared_ptr<BulkData>>& bulkDatas) const
{
	Super::CollectHeaderDatas(inst, externalPackageDatas, bulkDatas);
	_mSlotContainer.DoForEach([&externalPackageDatas, &bulkDatas](const std::shared_ptr<const Widget>& widget) {
		if (widget != nullptr)
		{
			widget->CollectHeaderDatas(widget.get(), externalPackageDatas, bulkDatas);
		}
		});
}

void BoxPanelWidget::Serialize(Archive& archive) const
{
	Super::Serialize(archive);
	_mSlotContainer.Serialize(archive);
}

void BoxPanelWidget::Deserialize(Archive& archive)
{
	Super::Deserialize(archive);
	_mSlotContainer.Deserialize(archive);
}

void BoxPanelWidget::RemoveSlot(int32 index)
{
	_mSlotContainer.RemoveChild(index);
}

void VerticalBox::Slot::OnConstruct(const WidgetSlotContainer* newOwner, Arguments&& args)
{
	BoxPanelWidget::SlotBase<Slot>::OnConstruct(newOwner, std::move(args));
}

void VerticalBox::OnConstruct(const Arguments& args)
{
	_mSlotContainer.Reserve(args.mSlots.size());
	for (const Slot::Arguments& args : args.mSlots)
	{
		// 다른 형식이지만, 데이터가 동일하기 때문에 형변환하여 추가
		const WidgetSlotBase::Arguments& baseSlotArgs = static_cast<const WidgetSlotBase::Arguments&>(args);
		const BoxPanelWidget::Slot::Arguments& boxSlotArgs = static_cast<const BoxPanelWidget::Slot::Arguments&>(baseSlotArgs);
		_mSlotContainer.AddChild(std::move(const_cast<BoxPanelWidget::Slot::Arguments&>(boxSlotArgs)));
	}
}

VerticalBox::Slot::Arguments VerticalBox::MakeSlot()
{
	return VerticalBox::Slot::Arguments(std::make_shared<VerticalBox::Slot>());
}

VerticalBox::ArgumentConstructHelper<VerticalBox::Slot> VerticalBox::AddSlot(int32 index)
{
	return ArgumentConstructHelper<Slot>(_mSlotContainer, std::make_shared<VerticalBox::Slot>(), index);
}

VerticalBox::Slot& VerticalBox::GetSlot(int32 index)
{
	return *static_cast<VerticalBox::Slot*>(&_mSlotContainer.GetSlotRef(index));
}

const VerticalBox::Slot& VerticalBox::GetSlot(int32 index) const
{
	return *static_cast<const VerticalBox::Slot*>(&_mSlotContainer.GetSlotRef(index));
}

void HorizonBox::Slot::OnConstruct(const WidgetSlotContainer* newOwner, Arguments&& args)
{
	BoxPanelWidget::SlotBase<Slot>::OnConstruct(newOwner, std::move(args));
}

void HorizonBox::OnConstruct(const Arguments& args)
{
	_mSlotContainer.Reserve(args.mSlots.size());
	for (const Slot::Arguments& args : args.mSlots)
	{
		// 다른 형식이지만, 데이터가 동일하기 때문에 형변환하여 추가
		const WidgetSlotBase::Arguments& baseSlotArgs = static_cast<const WidgetSlotBase::Arguments&>(args);
		const BoxPanelWidget::Slot::Arguments& boxSlotArgs = static_cast<const BoxPanelWidget::Slot::Arguments&>(baseSlotArgs);
		_mSlotContainer.AddChild(std::move(const_cast<BoxPanelWidget::Slot::Arguments&>(boxSlotArgs)));
	}
}

HorizonBox::Slot::Arguments HorizonBox::MakeSlot()
{
	return HorizonBox::Slot::Arguments(std::make_shared<HorizonBox::Slot>());
}

HorizonBox::ArgumentConstructHelper<HorizonBox::Slot> HorizonBox::AddSlot(int32 index)
{
	return ArgumentConstructHelper<Slot>(_mSlotContainer, std::make_shared<HorizonBox::Slot>(), index);
}

HorizonBox::Slot& HorizonBox::GetSlot(int32 index)
{
	return *static_cast<HorizonBox::Slot*>(&_mSlotContainer.GetSlotRef(index));
}

const HorizonBox::Slot& HorizonBox::GetSlot(int32 index) const
{
	return *static_cast<const HorizonBox::Slot*>(&_mSlotContainer.GetSlotRef(index));
}