#include "pch.h"
#include "Splitter.h"

#include "Graphics/Render/WidgetDrawBuffer.h"
#include "Graphics/Widget/Type/ArrangedWidget.h"
#include "Graphics/Widget/Type/WidgetGeometry.h"
#include "Graphics/Widget/Type/WidgetBrush.h"

#include "Utils/MathUtils.h"

void Splitter::Slot::OnConstruct(const WidgetSlotContainer* newOwner, Splitter::Slot::Arguments&& args)
{
	WidgetSlot<Splitter::Slot>::OnConstruct(newOwner, std::move(args));

	if (args.mMinSize.IsSet() == true)
	{
		_mMinSize.Set(this, args.mMinSize);
	}
	if (args.mSize.IsSet() == true)
	{
		_mSize.Set(this, args.mSize);
	}
	if (args.mSizeRule.IsSet() == true)
	{
		_mSizeRule.Set(this, args.mSizeRule);
	}
	if (args.mOnChangeSlotSize.IsBound() == true)
	{
		_mOnChangeSlotSize = std::move(args.mOnChangeSlotSize);
	}
}

void Splitter::OnConstruct(const Arguments& args)
{
	Widget::OnConstruct(args);
	_mSlotContainer.AddChildren(std::move(const_cast<std::vector<Slot::Arguments>&>(args.mSlots)));

	ASSERT_MSG(args.mStyle != nullptr, "Splitter must need style");
	_mStyle = args.mStyle;

	SetSplitOrient(args.mSplitOrient);
	_mResizeRule = args.mResizeRule;
	_mElementMinSize = args.mElementMinSize;
	_mElementHandleSize = args.mElementHandleSize;
	_mElementHandleHitSize = args.mElementHandleHitSize;
}

Splitter::Splitter()
{
	_mVisibility.Init(VisibilityType::SelfHitTestInvisible);
}

void Splitter::PostCreate()
{
	Super::PostCreate();
	_mSlotContainer = std::static_pointer_cast<Widget>(shared_from_this());
}

void Splitter::SetSplitOrient(WidgetOrientation orient)
{
	_mSplitOrient.Set(this, orient);
}

WidgetOrientation Splitter::GetSplitOrient() const
{
	return _mSplitOrient.GetValue(this);
}

ReplyData Splitter::OnPressMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event)
{
	if (event->mKeyInfo.mType == KeyType::Mouse::LButton && _mHoveredHandleIndex != -1)
	{
		_mIsResizing = true;
		return ReplyData::Handled().HoldCapture(std::static_pointer_cast<Widget>(shared_from_this()));
	}
	return ReplyData::Unhandled();
}

ReplyData Splitter::OnReleaseMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event)
{
	if (event->mKeyInfo.mType == KeyType::Mouse::LButton && _mIsResizing == true)
	{
		_mIsResizing = false;
		return ReplyData::Handled().ReleaseCapture();
	}
	return ReplyData::Unhandled();
}

ReplyData Splitter::OnMoveMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event)
{
	Vec2 preMousePos = { static_cast<float>(event->mPreMouseScreenPos.x), static_cast<float>(event->mPreMouseScreenPos.y) };
	Vec2 curMousePos = { static_cast<float>(event->mCurrentMouseScreenPos.x), static_cast<float>(event->mCurrentMouseScreenPos.y) };
	const Vec2 curLocalMousePos = Vec2::Transform(curMousePos, Matrix(geometry.GetAccRenderMatrix2D().Invert()));

	std::vector<LayoutTransform2D> layoutTransforms = ArrangeChildLayouts(geometry);

	// 핸들 잡고 리사이징 중인 경우
	if (_mIsResizing == true)
	{
		if (IsNearlyZero(curMousePos - preMousePos) == false)
		{
			ResizeByMousePos(curLocalMousePos, layoutTransforms);
		}

		return ReplyData::Handled();
	}
	// 그 외의 경우
	else
	{
		int32 preHoverHandleIndex = _mHoveredHandleIndex;
		_mHoveredHandleIndex = FindHandleIndexUnderMousePos(curLocalMousePos, layoutTransforms);

		if (_mHoveredHandleIndex != -1)
		{
			// 고정 크기 외에 리사이징 가능한 항목이 존재하는가?
			if (FindResizableSlotIndexBeforeHandle(_mHoveredHandleIndex) <= -1 || FindResizableSlotIndexAfterHandle(_mHoveredHandleIndex) >= _mSlotContainer.Size())
			{
				_mHoveredHandleIndex = -1;
			}
		}

		return ReplyData::Unhandled();
	}
}

void Splitter::OnLeaveMouse(const std::shared_ptr<const PointEvent>& event)
{
	if (_mIsResizing == false)
	{
		_mHoveredHandleIndex = -1;
	}
}

uint32 Splitter::OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor)
{
	ArrangedChildren children(VisibilityType::VisibleFlag);
	OnArrangeChildren(allottedGeometry, children);
	int32 maxLayerId = PaintArrangedChildren(drawElements, children, accLayerId, args, allottedGeometry, currentCulling, contentInheritedColor);
	++maxLayerId;

	auto FindHandleIndex = [this, &children](int32 index)
		{
			if (index == -1 || children.Size() == _mSlotContainer.Size())
			{
				return index;
			}

			// 시야에 보이지 않는 Widget이 제외되어 인덱스가 맞지 않는 경우 직접 탐색
			const std::shared_ptr<Widget>& findWidget = _mSlotContainer.GetChild(index);
			for (int32 i = 0; i < children.mArrangedWidgets.size(); ++i)
			{
				if (children.mArrangedWidgets[i].mWidget == findWidget)
				{
					return i;
				}
			}

			return static_cast<int32>(children.mArrangedWidgets.size());
		};
	// 수집된 자식 중에 호버된 요소 인덱스 발견
	int32 hoveredHandleIndex = FindHandleIndex(_mHoveredHandleIndex);
	// 수집된 자식 중에 하이라이트 요소 인덱스 발견
	int32 highlightHandleIndex = FindHandleIndex(_mHighlightHandleIndex.GetValue());

	std::shared_ptr<const SplitterStyle> style = _mStyle.lock();
	const WidgetBrush* normalBrush = &style->mNormalBrush;

	const int32 childSize = static_cast<int32>(children.Size());
	for (int32 i = 0; i < childSize - 1; ++i)
	{
		const WidgetGeometry& childGeometry = children.mArrangedWidgets[std::clamp<int32>(i + 1, 0, childSize - 1)].mGeometry;

		Vec2 handleSize;
		Vec2 handlePos;
		if (GetSplitOrient() == WidgetOrientation::Horizon)
		{
			handleSize = Vec2(_mElementHandleSize, childGeometry.mBoxSize.y);
			handlePos = Vec2(-_mElementHandleSize, 0);
		}
		else
		{
			handleSize = Vec2(childGeometry.mBoxSize.x, _mElementHandleSize);
			handlePos = Vec2(0, -_mElementHandleSize);
		}

		if (hoveredHandleIndex == i || highlightHandleIndex == i)
		{
			drawElements.CreateBoxElement(
				maxLayerId,
				childGeometry.MakeChild(handleSize, Transform2D(handlePos, 0.f, Vec2(1.f))),
				style->mHighlightBrush,
				contentInheritedColor.mInheritedColor * style->mHighlightBrush.mTint.mSpecificColor,
				WidgetDrawOptionFlag::None
			);
		}
		else
		{
			drawElements.CreateBoxElement(
				maxLayerId,
				childGeometry.MakeChild(handleSize, Transform2D(handlePos, 0.f, Vec2(1.f))),
				style->mNormalBrush,
				contentInheritedColor.mInheritedColor * style->mNormalBrush.mTint.mSpecificColor,
				WidgetDrawOptionFlag::None
			);
		}
	}

	return maxLayerId;
}

Vec2 Splitter::ComputeDesireSize(const Vec2& layoutMultiplyValue)
{
	return ComputeDesireSizeByOrient();
}

Vec2 Splitter::ComputeDesireSizeByOrient()
{
	Vec2 resultSize(0, 0);

	/* 자식 공간 계산 */

	int32 occupiedChildCount = 0;
	for (int32 i = 0; i < _mSlotContainer.Size(); ++i)
	{
		const Splitter::Slot& slot = *static_cast<const Splitter::Slot*>(&_mSlotContainer.GetSlotRef(i));
		if (slot.GetWidget()->GetVisibility() & VisibilityType::TakesSpaceFlag)
		{
			++occupiedChildCount;

			Vec2 childDesiredSize(slot.GetWidget()->GetDesiredSize());
			if (GetSplitOrient() == WidgetOrientation::Horizon)
			{
				resultSize.x += childDesiredSize.x;
				resultSize.y = std::max<float>(childDesiredSize.y, resultSize.y);
			}
			else
			{
				resultSize.x = std::max<float>(childDesiredSize.x, resultSize.x);
				resultSize.y += childDesiredSize.y;
			}
		}
	}

	/* 핸들 크기 계산 */

	float fixedHandleSpace = std::max<int32>(0, occupiedChildCount - 1) * _mElementHandleSize;
	if (GetSplitOrient() == WidgetOrientation::Horizon)
	{
		resultSize.x += fixedHandleSpace;
	}
	else
	{
		resultSize.y += fixedHandleSpace;
	}

	return resultSize;
}

void Splitter::OnArrangeChildren(const WidgetGeometry& allottedGeometry, ArrangedChildren& children) const
{
	std::vector<LayoutTransform2D> transforms = ArrangeChildLayouts(allottedGeometry);
	
	for (std::size_t i = 0; i < _mSlotContainer.Size(); ++i)
	{
		VisibilityType::Flag childVisibility = _mSlotContainer.GetChildRef(i)->GetVisibility();
		std::shared_ptr<Widget> childWidget = std::const_pointer_cast<Widget>(_mSlotContainer.GetChildRef(i));
		children.AddWidget(
			ArrangedWidget(
				childWidget,
				allottedGeometry.MakeChild(childWidget, transforms[i].mBoxSize, transforms[i].mTransformLocalToParent)
			),
			childVisibility
		);
	}
}

std::vector<LayoutTransform2D> Splitter::ArrangeChildLayouts(const WidgetGeometry& allottedGeometry) const
{
	/* 크기 배정을 위한 수치 계산 */

	// 크기 분할 축
	const int32 vecAxisIdx = (GetSplitOrient() == WidgetOrientation::Horizon) ? 0 : 1;

	// 공간 차지하는 자식 수
	int32 occupiedChildCount = 0;

	// 고정 공간
	float fixedSpace = 0.f;

	// 동적 공간
	float totalRate = 0.f;
	float minDynamicSpace = 0.f;

	for (int32 i = 0; i < _mSlotContainer.Size(); ++i)
	{
		const Splitter::Slot& slot = *static_cast<const Splitter::Slot*>(&_mSlotContainer.GetSlotRef(i));
		if ((slot.GetWidgetRef()->GetVisibility() & VisibilityType::TakesSpaceFlag) == 0u)
		{
			continue;
		}

		++occupiedChildCount;
		if (slot.GetSizeRule() == SplitSizeRule::ContentSize)
		{
			// 콘텐츠 내부로 정적 크기 결정
			fixedSpace += slot.GetWidget()->GetDesiredSize()[vecAxisIdx];
		}
		else
		{
			// 모든 콘텐츠 크기 간의 비율로 동적 크기 결정
			minDynamicSpace += std::max<float>(_mElementMinSize, slot.GetMinSize());
			totalRate += slot.GetSize();
		}
	}

	// 핸들 공간
	const float fixedHandleSpace = std::max<int32>(0, occupiedChildCount - 1) * _mElementHandleSize;
	// 고정 공간을 제외한 여유 공간
	const float extraSpace = allottedGeometry.mBoxSize[vecAxisIdx] - fixedHandleSpace - fixedSpace;


	/* 크기 배정 */

	std::vector<float> allottedSizes(_mSlotContainer.Size());
	float accRequiredSpace = 0;
	for (int32 i = 0; i < _mSlotContainer.Size(); ++i)
	{
		const Splitter::Slot& slot = *static_cast<const Splitter::Slot*>(&_mSlotContainer.GetSlotRef(i));
		if ((slot.GetWidgetRef()->GetVisibility() & VisibilityType::TakesSpaceFlag) == 0u)
		{
			allottedSizes[i] = 0.f;
			continue;
		}

		float allottedChildSpace;
		if (slot.GetSizeRule() == SplitSizeRule::ContentSize)
		{
			// 콘텐츠 내부로 정적 크기 결정
			allottedChildSpace = slot.GetWidgetRef()->GetDesiredSize()[vecAxisIdx];
		}
		else
		{
			// 모든 콘텐츠 크기 간의 비율로 동적 크기 결정
			allottedChildSpace = extraSpace * (slot.GetSize() / totalRate) - accRequiredSpace;
			accRequiredSpace = 0;
		}

		// 배분될 동적 공간이 항목 최소 크기보다 적은 경우, 다른 동적 항목 비율에서 분배해오기
		if (extraSpace >= minDynamicSpace)
		{
			const float maxChildSpace = std::max<float>(slot.GetMinSize(), allottedChildSpace);
			float curRequiredSpace = maxChildSpace - allottedChildSpace;

			for (int32 pre = i - 1; pre >= 0 && curRequiredSpace > 0; --pre)
			{
				const Splitter::Slot& preSlot = *static_cast<const Splitter::Slot*>(&_mSlotContainer.GetSlotRef(pre));
				if (preSlot.GetSizeRule() == SplitSizeRule::AllottedRate && (slot.GetWidgetRef()->GetVisibility() & VisibilityType::TakesSpaceFlag))
				{
					const float minChildSize = std::max<float>(_mElementMinSize, preSlot.GetMinSize());
					const float extraAllottedSpace = allottedSizes[pre] - minChildSize;
					if (extraAllottedSpace > 0.f)
					{
						const float stolenSpace = std::min<float>(extraAllottedSpace, curRequiredSpace);
						allottedSizes[pre] -= stolenSpace;
						curRequiredSpace -= stolenSpace;
					}
				}
			}

			if (curRequiredSpace > 0)
			{
				// 부족한 할당 공간은 다음 항목 크기에서 빼줄 예정
				accRequiredSpace = curRequiredSpace;
			}

			allottedChildSpace = maxChildSpace;
		}

		allottedSizes[i] = allottedChildSpace;
	}


	/* 결과 저장 */

	std::vector<LayoutTransform2D> resultTransforms(_mSlotContainer.Size());
	float offset = 0;
	for (int32 i = 0; i < _mSlotContainer.Size(); ++i)
	{
		const Vec2 boxSizeVec = (GetSplitOrient() == WidgetOrientation::Horizon) ? Vec2(allottedSizes[i], allottedGeometry.mBoxSize.y) : Vec2(allottedGeometry.mBoxSize.x, allottedSizes[i]);
		const Vec2 offsetVec = (GetSplitOrient() == WidgetOrientation::Horizon) ? Vec2(offset, 0) : Vec2(0, offset);
		resultTransforms[i] = LayoutTransform2D{boxSizeVec, Transform2D(offsetVec, 0.f, Vec2(1.f))};

		const Splitter::Slot& slot = *static_cast<const Splitter::Slot*>(&_mSlotContainer.GetSlotRef(i));
		if (slot.GetWidgetRef()->GetVisibility() & VisibilityType::TakesSpaceFlag)
		{
			offset += std::round(allottedSizes[i] + _mElementHandleSize);
		}
	}

	return resultTransforms;
}

void Splitter::CollectHeaderDatas(const void* inst, OUT std::unordered_map<std::wstring, std::pair<std::string, PackageBuildScope>>& externalPackageDatas, OUT std::vector<std::shared_ptr<BulkData>>& bulkDatas) const
{
	Super::CollectHeaderDatas(inst, externalPackageDatas, bulkDatas);
	_mSlotContainer.DoForEach([&externalPackageDatas, &bulkDatas](const std::shared_ptr<const Widget>& widget) {
		if (widget != nullptr)
		{
			widget->CollectHeaderDatas(widget.get(), externalPackageDatas, bulkDatas);
		}
	});
}

void Splitter::Serialize(Archive& archive) const
{
	Super::Serialize(archive);
	_mSlotContainer.Serialize(archive);
}

void Splitter::Deserialize(Archive& archive)
{
	Super::Deserialize(archive);
	_mSlotContainer.Deserialize(archive);
}

int32 Splitter::FindResizableSlotIndexBeforeHandle(int32 handleIndex) const
{
	while (handleIndex >= 0)
	{
		const Splitter::Slot& slot = *static_cast<const Splitter::Slot*>(&_mSlotContainer.GetSlotRef(handleIndex));
		if ((slot.GetWidgetRef()->GetVisibility() & VisibilityType::TakesSpaceFlag) && slot.GetSizeRule() == SplitSizeRule::AllottedRate)
		{
			break;
		}
		--handleIndex;
	}

	return handleIndex;
}

void Splitter::FindAllResizableSlotIndexBeforeHandle(int32 handleIndex, OUT std::vector<int32>& slotIndices) const
{
	for (; handleIndex >= 0; --handleIndex)
	{
		const Splitter::Slot& slot = *static_cast<const Splitter::Slot*>(&_mSlotContainer.GetSlotRef(handleIndex));
		if ((slot.GetWidgetRef()->GetVisibility() & VisibilityType::TakesSpaceFlag) && slot.GetSizeRule() == SplitSizeRule::AllottedRate)
		{
			slotIndices.push_back(handleIndex);
		}
	}
};

int32 Splitter::FindResizableSlotIndexAfterHandle(int32 handleIndex) const
{
	const int32 size = static_cast<int32>(_mSlotContainer.Size());

	++handleIndex;
	while (handleIndex < size)
	{
		const Splitter::Slot& slot = *static_cast<const Splitter::Slot*>(&_mSlotContainer.GetSlotRef(handleIndex));
		if ((slot.GetWidgetRef()->GetVisibility() & VisibilityType::TakesSpaceFlag) && slot.GetSizeRule() == SplitSizeRule::AllottedRate)
		{
			break;
		}
		++handleIndex;
	}

	return handleIndex;
}

void Splitter::FindAllResizableSlotIndexAfterHandle(int32 handleIndex, OUT std::vector<int32>& slotIndices) const
{
	const int32 size = static_cast<int32>(_mSlotContainer.Size());

	for (++handleIndex; handleIndex < size; ++handleIndex)
	{
		const Splitter::Slot& slot = *static_cast<const Splitter::Slot*>(&_mSlotContainer.GetSlotRef(handleIndex));
		if ((slot.GetWidgetRef()->GetVisibility() & VisibilityType::TakesSpaceFlag) && slot.GetSizeRule() == SplitSizeRule::AllottedRate)
		{
			slotIndices.push_back(handleIndex);
		}
	}
};

int32 Splitter::FindHandleIndexUnderMousePos(const Vec2& localMousePos, const std::vector<LayoutTransform2D>& layoutTransforms) const
{
	const int32 vecAxisIdx = (GetSplitOrient() == WidgetOrientation::Horizon) ? 0 : 1;

	const float elementHandleHitHalfSize = (_mElementHandleHitSize / 2);
	const float elementHandleHalfSize = (_mElementHandleSize / 2);

	for (int32 i = 1; i < layoutTransforms.size(); ++i)
	{
		const LayoutTransform2D& preElementTransform = layoutTransforms[i - 1];
		const LayoutTransform2D& curHandleTransform = layoutTransforms[i];
		float handleFrontBound = preElementTransform.mTransformLocalToParent.GetLocalPosition()[vecAxisIdx] + preElementTransform.mBoxSize[vecAxisIdx] - elementHandleHitHalfSize + elementHandleHalfSize;
		float handleBackBound = curHandleTransform.mTransformLocalToParent.GetLocalPosition()[vecAxisIdx] + elementHandleHitHalfSize - elementHandleHalfSize;

		if (localMousePos[vecAxisIdx] > handleFrontBound && localMousePos[vecAxisIdx] < handleBackBound)
		{
			// [항목][핸들][항목][핸들][항목]
			// 핸들만 카운팅하기 때문에, 1 빼기
			return i - 1;
		}
	}
	return -1;
}

void Splitter::Resize(float delta, const std::vector<LayoutTransform2D>& layoutTransforms)
{
	/* 리사이징 전에 데이터 수집 */

	const int32 slotCount = static_cast<int32>(_mSlotContainer.Size());
	const int32 vecAxisIdx = (GetSplitOrient() == WidgetOrientation::Horizon) ? 0 : 1;

	// [1][2]![3]...[N] -> 3 ~ N 번에서 리사이징 가능한 슬롯 찾기
	std::vector<int32> resizableNextSlots;
	if (_mResizeRule == SplitResizeRule::ChangeNextSlot)
	{
		// 반드시 [3]이 resizable 해야됨
		int32 handleIndex = FindResizableSlotIndexAfterHandle(_mHoveredHandleIndex);

		if (slotCount > handleIndex && handleIndex >= 0)
		{
			resizableNextSlots.push_back(handleIndex);
		}
	}
	else
	{
		// ! 이후 적어도 1개 이상 resizable 해야됨
		FindAllResizableSlotIndexAfterHandle(_mHoveredHandleIndex, resizableNextSlots);
	}

	// 핸들 우측 슬롯들에서 조절가능한 핸들이 없다면 불가
	const int32 resizableNextSlotCount = static_cast<int32>(resizableNextSlots.size());
	if (resizableNextSlotCount <= 0)
	{
		// 리사이징 불가
		return;
	}

	struct ResizeSlotData
	{
		Slot* mSlot;
		const LayoutTransform2D* mTransform;
		float mNewSize;
	};

	// 리사이즈 슬롯 결과 데이터 우선 이전 데이터로 초기화
	std::vector<ResizeSlotData> resizeNextSlotDatas(resizableNextSlotCount);
	for (int32 i = 0; i < resizableNextSlotCount; ++i)
	{
		ResizeSlotData resizeSlotData;
		resizeSlotData.mSlot = static_cast<Splitter::Slot*>(&_mSlotContainer.GetSlotRef(resizableNextSlots[i]));
		resizeSlotData.mTransform = &layoutTransforms[resizableNextSlots[i]];
		resizeSlotData.mNewSize = std::max<float>(resizeSlotData.mSlot->GetMinSize(), resizeSlotData.mTransform->mBoxSize[vecAxisIdx]);

		resizeNextSlotDatas[i] = std::move(resizeSlotData);
	}

	// 핸들 좌측 슬롯들에서 조절가능한 핸들이 없다면 불가
	const int32 resizablePreSlotIndex = FindResizableSlotIndexBeforeHandle(_mHoveredHandleIndex);
	ASSERT_MSG(resizablePreSlotIndex >= 0 && resizablePreSlotIndex < slotCount, "Splitter's first element must be resizable");

	Slot& resizablePreSlot = *static_cast<Splitter::Slot*>(&_mSlotContainer.GetSlotRef(resizablePreSlotIndex));
	const LayoutTransform2D& resizablePreSlotTransform = layoutTransforms[resizablePreSlotIndex];


	/* 새로운 슬롯 크기 계산 */

	// 최근접 좌측 조절가능 슬롯의 MinSize를 고려하여 핸들 이동값인 delta값 조정
	const float resizablePreSlotSize = resizablePreSlotTransform.mBoxSize[vecAxisIdx];
	float newResizablePreSlotSize = std::max<float>(resizablePreSlot.GetMinSize(), resizablePreSlotSize + delta);
	delta = newResizablePreSlotSize - resizablePreSlotSize;

	// 이동값 delta 우측 조절가능 슬롯들에 분배
	float accDelta = delta;
	for (int32 trial = 0; trial < resizableNextSlotCount && accDelta != 0; ++trial)
	{
		float curSlotDelta = (_mResizeRule == SplitResizeRule::ChangeLastSlot) ? accDelta : accDelta / resizableNextSlotCount;
		accDelta = 0;

		// ChangeLastSlot 방식 리사이징은 마지막 슬롯만 사이즈 줄이기
		int32 targetSlotIndex = 0;
		if (_mResizeRule == SplitResizeRule::ChangeLastSlot)
		{
			targetSlotIndex = resizableNextSlotCount - 1;
		}

		// 나누어 배정한 delta가 슬롯의 MinSize에 막혀 남겨지는 경우, 다음번 trial에 재분배
		for (; targetSlotIndex < resizableNextSlotCount; ++targetSlotIndex)
		{
			ResizeSlotData& SlotInfo = resizeNextSlotDatas[targetSlotIndex];
			float curSlotSize = SlotInfo.mNewSize;
			SlotInfo.mNewSize = std::max<float>(SlotInfo.mSlot->GetMinSize(), curSlotSize - curSlotDelta);

			accDelta += SlotInfo.mNewSize - (curSlotSize - curSlotDelta);
		}
	}

	// 분배하고도, 남은 값 (MinSize에 막힌 결과)
	delta = delta - accDelta;
	// 남은 값 만큼은 리사이즈 무시
	newResizablePreSlotSize = std::max<float>(resizablePreSlot.GetMinSize(), resizablePreSlotSize + delta);


	/* 슬롯 비율 적용 */

	float totalResizableSlotSize = newResizablePreSlotSize;
	float totalResizableRates = resizablePreSlot.GetSize();

	for (int32 i = 0; i < resizableNextSlotCount; ++i)
	{
		const ResizeSlotData& resizeNextSlotData = resizeNextSlotDatas[i];

		totalResizableSlotSize += resizeNextSlotData.mNewSize;
		totalResizableRates += resizeNextSlotData.mSlot->GetSize();
	}

	auto ApplyNewSizeToSlot = [](Slot& slot, float totalResizableRates, float newSize, float totalSize)
		{
			float newFillSize = (totalSize > 0.f) ? (totalResizableRates * (newSize / totalSize)) : totalResizableRates;
			slot.SetSize(newFillSize);
			slot.OnChangeSlotSize().ExecuteIfBound(newFillSize);
		};

	ApplyNewSizeToSlot(resizablePreSlot, totalResizableRates, newResizablePreSlotSize, totalResizableSlotSize);
	for (int32 i = 0; i < resizableNextSlotCount; ++i)
	{
		const ResizeSlotData& resizeNextSlotData = resizeNextSlotDatas[i];
		ApplyNewSizeToSlot(*(resizeNextSlotData.mSlot), totalResizableRates, resizeNextSlotData.mNewSize, totalResizableSlotSize);
	}
}

void Splitter::ResizeByMousePos(const Vec2& localMousePos, const std::vector<LayoutTransform2D>& layoutTransforms)
{
	const int32 vecAxisIdx = (GetSplitOrient() == WidgetOrientation::Horizon) ? 0 : 1;
	const float handlePos = layoutTransforms[_mHoveredHandleIndex + 1].mTransformLocalToParent.GetLocalPosition()[vecAxisIdx] - _mElementHandleSize / 2;
	
	Resize(localMousePos[vecAxisIdx] - handlePos, layoutTransforms);
}

Splitter::Slot::Arguments Splitter::MakeSlot()
{
	return Splitter::Slot::Arguments(std::make_shared<Splitter::Slot>());
}

PanelSlotContainer<typename Splitter::Slot>::ConstructHelper Splitter::AddSlot(int32 index)
{
	return PanelSlotContainer<Slot>::ConstructHelper(_mSlotContainer, std::make_shared<Splitter::Slot>(), index);
}

void Splitter::RemoveSlot(int32 index)
{
	_mSlotContainer.RemoveChild(index);
}

