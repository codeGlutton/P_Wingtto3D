#include "pch.h"
#include "VirtualWindowContent.h"
#include "Graphics/Widget/VirtualWindowSplitter.h"

#include "Utils/WidgetUtils.h"

#include "Graphics/Widget/Border.h"
#include "Graphics/Widget/Image.h"
#include "Graphics/Widget/TextBlock.h"

#include "Manager/WidgetStyleManager.h"

VirtualWindowContent::VirtualWindowContent() :
	_mHasContent(false)
{
	_mType = VirtualWindowType::Content;
	_mVisibility.Init(VisibilityType::SelfHitTestInvisible);
}

void VirtualWindowContent::PostLoad()
{
	Super::PostLoad();

	if (_mTitleBarWidget != nullptr)
	{
		_mHeaderLeft = std::static_pointer_cast<HorizonBox::Slot>(_mTitleBarWidget->GetChildren().GetSlot(0ull));
		_mHeaderRight = std::static_pointer_cast<HorizonBox::Slot>(_mTitleBarWidget->GetChildren().GetSlot(2ull));
	}
	if (_mContentBorder != nullptr && _mContentBorder->GetContent() != nullptr)
	{
		_mHasContent = true;
	}
}

void VirtualWindowContent::OnConstruct(const Arguments& args)
{
	Widget::OnConstruct(args);

	std::shared_ptr<const VirtualWindowStyle> style = WIDGET_STYLE_MANAGER->GetStyle<VirtualWindowStyle>(L"WindowStyle");
	_mStyle = style;
	_mTextStyle = style->mTextBlockStyle;
	_mMainBrush = style->mMainBrush;
	_mSubBrush = style->mSubBrush;
	_mBackgroundBrush = style->mBackgroundBrush;

	_mShowHeader = args.mShowHeader;
	_mShouldAutosize = args.mShouldAutosize;
	_mHeaderScale = args.mHeaderScale;

	_mContentPadding = args.mContentPadding;

	_mHeaderLeft.reset();
	_mHeaderRight.reset();
	_mHasContent = true;

	std::shared_ptr<VerticalBox::Slot> titleBarSlot = nullptr;


	const VisibilityType::Flag titleBarVisibility = _mShowHeader == true ? VisibilityType::SelfHitTestInvisible : VisibilityType::Collapsed;

	// 전체 부분
	_mSlotContainer.AddChild(
		MakeSlot()
		[
			NEW_EDIT_WIDGET(VerticalBox)
				.Visibility(VisibilityType::SelfHitTestInvisible)
					+ VerticalBox::MakeSlot() // 상단바
						.ContentSize()
						[
							NEW_EDIT_WIDGET(Border)
								.Visibility(titleBarVisibility)
								.DesiredSizeScale(GetHeaderScale())
								.Brush(GetMainBrush())
								//.OnPressMouse(OnCallPointEvent::Binder::BIND_DYNAMIC(shared_from_this(), OnRightClicked)) // TODO
									+ Border::MakeSlot()
										.VerticalAlignment(SlotAlignmentType::Down)
										.Padding(Margin(0.f))
										[
											NEW_EDIT_WIDGET(VerticalBox)
												.Visibility(VisibilityType::SelfHitTestInvisible)

												+ VerticalBox::MakeSlot() // 상단바 디자인 부분
													.Assign(titleBarSlot)
													.ContentSize()
													.MinSize(30.f)
													.MaxSize(40.f)

												+ VerticalBox::MakeSlot() // ----------라인
													.ContentSize()
													.MinSize(5.f)
													.MaxSize(10.f)
													[
														NEW_EDIT_WIDGET(Image)
															.Brush(GetSubBrush())
															.Visibility(VisibilityType::HitTestInvisible)
													]
										]
						]

					+ VerticalBox::MakeSlot() // 콘텐츠
						.AllottedRate(1.0f)
						[
							ASSIGN_EDIT_WIDGET(Border, _mContentBorder)
								.Brush(GetBackgroundBrush())
								.Clipping(ClippingType::Inherit)
								.Visibility(VisibilityType::SelfHitTestInvisible)
									+ Border::MakeSlot()
										.Padding(GetContentPadding())
										.VerticalAlignment(SlotAlignmentType::Middle)
										[
											// 임시 콘텐츠
											NEW_EDIT_WIDGET(TextBlock)
												.Style(_mTextStyle.lock())
												.Text(L"Empty content")
												.AutoWrap(true)
												.JustifyPolicy(TextJustifyPolicy::Center)
												.Visibility(VisibilityType::HitTestInvisible)
										]
						]
		]
	);

	//  _________________________
	// [_HL_|___TitleBar______|_HR_]
	// 상단 헤더 부분

	_mTitleBarWidget = NEW_EDIT_WIDGET(HorizonBox)
		.Visibility(VisibilityType::SelfHitTestInvisible)
			+ HorizonBox::MakeSlot() // Header Left
				.ContentSize()
				.Assign(_mHeaderLeft)
				[
					APP_WIN_MANAGER->NullWidgetInst()
				]
			+ HorizonBox::MakeSlot() // 타이틀
				.AllottedRate(1.f)
				.VerticalAlignment(SlotAlignmentType::Down)
				.Padding(Margin(2.f, 0.f))
				[
					NEW_EDIT_WIDGET(TextBlock)
						.Style(_mTextStyle.lock())
						.PixelSize(18)
						.Text_Bind(Attribute<std::wstring>::OnGet::Binder::BIND_DYNAMIC(shared_from_this(), GetContentName))
						.Visibility(VisibilityType::SelfHitTestInvisible)
						.JustifyPolicy(TextJustifyPolicy::Left)
				]
			+ HorizonBox::MakeSlot() // Header Right
				.ContentSize()
				.Assign(_mHeaderRight)
				.Padding(Margin(5.f, 0.f, 0.f, 0.f))
				.VerticalAlignment(SlotAlignmentType::Middle)
				[
					APP_WIN_MANAGER->NullWidgetInst()
				];

	(*titleBarSlot)[_mTitleBarWidget];
}

void VirtualWindowContent::Serialize(Archive& archive) const
{
	Super::Serialize(archive);
}

void VirtualWindowContent::Deserialize(Archive& archive)
{
	Super::Deserialize(archive);
}

SplitSizeRule VirtualWindowContent::GetSplitSizeRule() const
{
	if (HasContent() == true && ShouldAutoSize() == false)
	{
		return SplitSizeRule::ContentSize;
	}
	else
	{
		return SplitSizeRule::AllottedRate;
	}
}

ReplyData VirtualWindowContent::OnAddVirtualWindow(VirtualWindowAddDirection dir, const std::shared_ptr<const DragDropEvent>& event)
{
	std::shared_ptr<VirtualWindowDragDropPayload> payload = CastSharedPointer<VirtualWindowDragDropPayload>(event->mPayload);
	if (payload != nullptr && payload->mDragWidget != nullptr)
	{
		CreateNewSiblingContent(dir)->SetContent(payload->mDragWidget);
		//HideCross();
		return ReplyData::Handled();
	}
	else
	{
		return ReplyData::Unhandled();
	}
}

void VirtualWindowContent::SetContent(const std::shared_ptr<Widget>& content)
{
	if (content != nullptr)
	{
		_mHasContent = true;
		_mContentBorder->SetContent(content);
	}
	else
	{
		_mHasContent = false;
		_mContentBorder->SetContent(nullptr);
	}
}

void VirtualWindowContent::SetContentHeader(const VirtualContentHeaderData& header)
{
	std::shared_ptr<HorizonBox::Slot> leftSlot = _mHeaderLeft.lock();
	if (leftSlot != nullptr && leftSlot->GetWidget() != header.mHeaderLeft)
	{
		(*leftSlot)[header.mHeaderLeft];
		MarkLayoutDirty();
	}
	std::shared_ptr<HorizonBox::Slot> rightSlot = _mHeaderRight.lock();
	if (rightSlot != nullptr && rightSlot->GetWidget() != header.mHeaderLeft)
	{
		(*rightSlot)[header.mHeaderRight];
		MarkLayoutDirty();
	}
}

std::wstring VirtualWindowContent::GetContentName() const
{
	if (HasContent() == true)
	{
		return _mContentBorder->GetContent()->GetDisplayName();
	}
	return L"";
}

std::shared_ptr<VirtualWindowContent> VirtualWindowContent::CreateNewSiblingContent(VirtualWindowAddDirection dir)
{
	std::shared_ptr<VirtualWindowSplitter> parentSplitter = GetParentSplitter();
	ASSERT_MSG(parentSplitter != nullptr, "Virtual window's parent splitter is invalid");

	std::shared_ptr<VirtualWindowContent> newWindow = NEW_EDIT_WIDGET(VirtualWindowContent)
		.ShowHeader(IsShowingHeader())
		.ShouldAutosize(ShouldAutoSize())
		.HeaderScale(GetHeaderScale())
		.ContentPadding(GetContentPadding());

	parentSplitter->AddChildVirtualWindow(newWindow, std::static_pointer_cast<VirtualWindowContent>(shared_from_this()), dir);
	newWindow->SetSizeRate(GetSizeRate());
	return newWindow;
}
