#pragma once

#include "Graphics/Widget/VirtualWindow.h"
#include "Graphics/Widget/BoxPanelWidget.h"

#include "Manager/AppWindowManager.h"

class Border;

/**
 *  _________________________
 * [_HL_|___TitleBar______|_HR_]
 * 가상 윈도우 상단 구성 데이터
 */
struct VirtualContentHeaderData
{
	VirtualContentHeaderData() :
		mHeaderLeft(APP_WIN_MANAGER->NullWidgetInst()),
		mHeaderRight(APP_WIN_MANAGER->NullWidgetInst())
	{
	}

	std::shared_ptr<Widget> mHeaderLeft;
	std::shared_ptr<Widget> mHeaderRight;
};

/**
 * 가상 윈도우 창을 이루는 메인 콘텐츠 요소 베이스 클래스
 */
class VirtualWindowContent : public VirtualWindow
{
	GEN_REFLECTION(VirtualWindowContent)

public:
	VirtualWindowContent();

protected:
	virtual void PostLoad() override;

	/* 생성 시, 초기화 인자 구조체 정의 */
public:
	DECALRE_WIDGET_ARGS_BEGIN(VirtualWindowContent)

		DECLARE_INIT_WIDGET_ARG_CONST(bool, ShowHeader, true)
		DECLARE_INIT_WIDGET_ARG_CONST(bool, ShouldAutosize, true)
		DECLARE_INIT_WIDGET_ARG_CONST(float, HeaderScale, 1.f)

		DECLARE_INIT_WIDGET_ARG_CONST(Margin, ContentPadding, Margin(0.f))

	DECALRE_WIDGET_ARGS_END()

public:
	void OnConstruct(const Arguments& args);

protected:
	virtual void Serialize(Archive& archive) const override;
	virtual void Deserialize(Archive& archive) override;

public:
	virtual SplitSizeRule GetSplitSizeRule() const override;
	virtual ReplyData OnAddVirtualWindow(VirtualWindowAddDirection dir, const std::shared_ptr<const DragDropEvent>& event) override;

public:
	bool IsShowingHeader() const
	{
		return _mShowHeader;
	}
	bool ShouldAutoSize() const
	{
		return _mShouldAutosize;
	}
	float GetHeaderScale() const
	{
		return _mHeaderScale;
	}

	const WidgetBrush& GetMainBrush() const
	{
		return _mMainBrush;
	}
	const WidgetBrush& GetSubBrush() const
	{
		return _mSubBrush;
	}
	const WidgetBrush& GetBackgroundBrush() const
	{
		return _mBackgroundBrush;
	}

	const Margin& GetContentPadding() const
	{
		return _mContentPadding;
	}

public:
	bool HasContent() const
	{
		return _mHasContent;
	}

public:
	void SetContent(const std::shared_ptr<Widget>& content);
	void SetContentHeader(const VirtualContentHeaderData& header);

private:
	METHOD(GetContentName)
	std::wstring GetContentName() const;

private:
	std::shared_ptr<VirtualWindowContent> CreateNewSiblingContent(VirtualWindowAddDirection dir);

	/* 캐싱 데이터 */
private:
	std::weak_ptr<HorizonBox::Slot> _mHeaderLeft;
	std::weak_ptr<HorizonBox::Slot> _mHeaderRight;
	PROPERTY(_mTitleBarWidget)
	std::shared_ptr<Widget> _mTitleBarWidget;

	PROPERTY(_mContentBorder)
	std::shared_ptr<Border> _mContentBorder;

private:
	PROPERTY(_mShowHeader)
	bool _mShowHeader;
	PROPERTY(_mShouldAutosize)
	bool _mShouldAutosize;
	PROPERTY(_mHeaderScale)
	float _mHeaderScale;

	PROPERTY(_mContentPadding)
	Margin _mContentPadding;

	PROPERTY(_mMainBrush)
	WidgetBrush _mMainBrush;
	PROPERTY(_mSubBrush)
	WidgetBrush _mSubBrush;
	PROPERTY(_mBackgroundBrush)
	WidgetBrush _mBackgroundBrush;

	PROPERTY(_mStyle)
	std::weak_ptr<const VirtualWindowStyle> _mStyle;
	PROPERTY(_mTextStyle)
	std::weak_ptr<const TextBlockStyle> _mTextStyle;

private:
	bool _mHasContent = false;
};

