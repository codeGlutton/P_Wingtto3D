#pragma once

#include "Graphics/Widget/Type/WidgetTypeInclude.h"

enum class SlotAlignmentType : uint8
{
	Fill = 0,
	Left,
	Middle,
	Right,

	Up = Left,
	Down = Right,
};

template<typename MinIn>
struct PaddingSlotOption
{
public:
	const Margin& GetPadding() const
	{
		return _mPadding;
	}
	void SetPadding(const Margin& padding)
	{
		_mPadding = padding;
		static_cast<MinIn*>(this)->OnResize();
	}

protected:
	Margin _mPadding;
};

template<typename MinIn>
struct AlignmentSlotOption
{
public:
	SlotAlignmentType GetHorizonAlignment() const
	{
		return _mHorizonAlignment;
	}
	SlotAlignmentType GetVerticalAlignment() const
	{
		return _mVerticalAlignment;
	}

	void SetHorizonAlignment(SlotAlignmentType type)
	{
		_mHorizonAlignment = type;
		static_cast<MinIn*>(this)->OnResize();
	}
	void SetVerticalAlignment(SlotAlignmentType type)
	{
		_mVerticalAlignment = type;
		static_cast<MinIn*>(this)->OnResize();
	}

protected:
	SlotAlignmentType _mHorizonAlignment;
	SlotAlignmentType _mVerticalAlignment;
};

/**
 * 자식에 대한 단일 축 정렬 결과
 */
struct ChildAlignmentResult
{
public:
	ChildAlignmentResult()
	{
	}
	ChildAlignmentResult(float offset, float size) :
		mOffset(offset),
		mSize(size)
	{
	}

public:
	float mOffset;
	float mSize;
};

template<typename SlotType>
ChildAlignmentResult AlignChildAxisX(float allottedSize, const SlotType& childSlot, const Margin& childPadding)
{
	const float paddingSum = childPadding.mLeft + childPadding.mRight;
	const SlotAlignmentType alignmentType = childSlot.GetHorizonAlignment();

	if (alignmentType != SlotAlignmentType::Fill)
	{
		// 채우기 말고 다른 옵션 일시, 내부 위젯의 DesiredSize과 패딩에 의존
		const float childDesiredSize = childSlot.GetWidgetRef()->GetDesiredSize().x;
		const float childSize = std::max<float>(std::min<float>(childDesiredSize, allottedSize - paddingSum), 0.f);

		// 사이즈는 동일하나, 오프셋만 변동
		switch (alignmentType)
		{
		case SlotAlignmentType::Left:
			return ChildAlignmentResult(childPadding.mLeft, childSize);
		case SlotAlignmentType::Middle:
			return ChildAlignmentResult((allottedSize - childSize) / 2.0f + childPadding.mLeft - childPadding.mRight, childSize);
		case SlotAlignmentType::Right:
			return ChildAlignmentResult(allottedSize - childSize - childPadding.mRight, childSize);
		}
	}

	// 채우기 옵션 일시, 오프셋과 크기를 패딩에 거의 의존하여 반영
	return ChildAlignmentResult(childPadding.mLeft, std::max<float>((allottedSize - paddingSum), 0.f));
}

template<typename SlotType>
ChildAlignmentResult AlignChildAxisY(float allottedSize, const SlotType& childSlot, const Margin& childPadding)
{
	const float paddingSum = childPadding.mUp + childPadding.mDown;
	const SlotAlignmentType alignmentType = childSlot.GetVerticalAlignment();

	if (alignmentType != SlotAlignmentType::Fill)
	{
		// 채우기 말고 다른 옵션 일시, 내부 위젯의 DesiredSize과 패딩에 의존
		const float childDesiredSize = childSlot.GetWidgetRef()->GetDesiredSize().y;
		const float childSize = std::max<float>(std::min<float>(childDesiredSize, allottedSize - paddingSum), 0.f);

		// 사이즈는 동일하나, 오프셋만 변동
		switch (alignmentType)
		{
		case SlotAlignmentType::Up:
			return ChildAlignmentResult(childPadding.mUp, childSize);
		case SlotAlignmentType::Middle:
			return ChildAlignmentResult((allottedSize - childSize) / 2.0f + childPadding.mUp - childPadding.mDown, childSize);
		case SlotAlignmentType::Down:
			return ChildAlignmentResult(allottedSize - childSize - childPadding.mDown, childSize);
		}
	}

	// 채우기 옵션 일시, 오프셋과 크기를 패딩에 거의 의존하여 반영
	return ChildAlignmentResult(childPadding.mUp, std::max<float>((allottedSize - paddingSum), 0.f));
}

