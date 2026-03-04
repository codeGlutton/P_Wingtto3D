#pragma once

#include "Graphics/Widget/CompoundWidget.h"
#include "Graphics/Widget/Splitter.h"
#include "Graphics/Widget/Type/WidgetStyle.h"

class TextBlockStyle;
class VirtualWindowSplitter;

enum class VirtualWindowType
{
	Splitter,
	Content,
};

/**
 * 가상 윈도우 창 추가 시 상관 방향
 */
enum class VirtualWindowAddDirection
{
	Left,
	Up,
	Right,
	Down,
	Center
};

class VirtualWindowStyle : public WidgetStyle
{
	GEN_REFLECTION(VirtualWindowStyle)
	friend class WidgetStyleManager;

protected:
	VirtualWindowStyle() = default;
	virtual ~VirtualWindowStyle() = default;

public:
	bool operator==(const VirtualWindowStyle& other) const = default;

public:
	PROPERTY(mTextBlockStyle)
	std::shared_ptr<const TextBlockStyle> mTextBlockStyle;
	PROPERTY(mMainBrush)
	WidgetBrush mMainBrush;
	PROPERTY(mSubBrush)
	WidgetBrush mSubBrush;
	PROPERTY(mBackgroundBrush)
	WidgetBrush mBackgroundBrush;
};

struct VirtualWindowDragDropPayload : public DragDropPayload
{
	GEN_STRUCT_REFLECTION(VirtualWindowDragDropPayload)

public:
	std::shared_ptr<Widget> mDragWidget;
};

/**
 * 가상 윈도우 창 내부 공간 영역 베이스 클래스
 */
class VirtualWindow abstract : public CompoundWidget
{
	GEN_ABSTRACT_REFLECTION(VirtualWindow)

public:
	VirtualWindowType GetVirtualWindowType() const
	{
		return _mType;
	}
	std::shared_ptr<VirtualWindowSplitter> GetParentSplitter() const
	{
		return _mParentSplitter.lock();
	}
	METHOD(GetSizeRate)
	float GetSizeRate() const
	{
		return _mSizeRate;
	}

	void SetParentSplitter(std::shared_ptr<VirtualWindowSplitter> splitter)
	{
		_mParentSplitter = splitter;
	}
	METHOD(SetSizeRate)
	void SetSizeRate(float sizeRate)
	{
		if (_mSizeRate != sizeRate)
		{
			_mSizeRate = sizeRate;
			OnResizeVirtualWindow();
		}
	}

public:
	METHOD(GetSplitSizeRule)
	virtual SplitSizeRule GetSplitSizeRule() const
	{
		return SplitSizeRule::AllottedRate;
	}

	virtual ReplyData OnAddVirtualWindow(VirtualWindowAddDirection dir, const std::shared_ptr<const DragDropEvent>& event)
	{
		return ReplyData::Unhandled();
	}

protected:
	virtual void OnResizeVirtualWindow() 
	{
		MarkLayoutDirty();
	}

protected:
	VirtualWindowType _mType;

protected:
	// 소속된 가상 윈도우 스플리터
	PROPERTY(_mParentSplitter)
	std::weak_ptr<VirtualWindowSplitter> _mParentSplitter;
	// 내부 차지 비율
	PROPERTY(_mSizeRate)
	float _mSizeRate = 1.0f;
};

