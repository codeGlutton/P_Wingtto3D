#pragma once

#include "Graphics/Widget/PanelWidget.h"
#include "Graphics/Widget/Type/WidgetStyle.h"
#include "Graphics/Widget/Type/WidgetTypeInclude.h"

DECLARE_DYNAMIC_DELEGATE_1_PARAM(OnChangeSlotSizeEvent, float);

/**
 * 분할 사이즈 기준
 */
enum class SplitSizeRule
{
	ContentSize, // 콘텐츠 크기 맞춤
	AllottedRate // 부모의 할당 비율 맞춤
};

/**
 * 핸들 재조정 시, 슬롯 변형 규칙
 */
enum class SplitResizeRule
{
	// 핸들 이전과 다음 슬롯의 크기를 조절
	// [][][+++]![---][][]
	// [+++][][]![---][][]
	ChangeNextSlot,
	// 핸들 이전과 리사이징 가능한 마지막 슬롯의 크기를 조절
	// [][][+++]![][---][]
	// [+++][][]![][---][]
	ChangeLastSlot,
	// 크기 조절 후, 재분배
	DevideNextSlots,
};

struct SplitterStyle : public WidgetStyle
{
	GEN_REFLECTION(SplitterStyle)
	friend class WidgetStyleManager;

protected:
	SplitterStyle() = default;
	virtual ~SplitterStyle() = default;

public:
	bool operator==(const SplitterStyle& other) const = default;

public:
	PROPERTY(mNormalBrush)
	WidgetBrush mNormalBrush;
	PROPERTY(mHighlightBrush)
	WidgetBrush mHighlightBrush;
};

class Splitter : public PanelWidget
{
	GEN_REFLECTION(Splitter)

public:
	Splitter();
	~Splitter()
	{
	}

	/* 슬롯 구조체 정의 */
public:
	struct Slot : public WidgetSlot<Slot>
	{
		GEN_STRUCT_REFLECTION(Splitter::Slot)

	public:
		using WidgetSlot<Slot>::WidgetSlot;

		/* 생성 시, 초기화 인자 구조체 정의 */
	public:
		DECALRE_WIDGET_SLOT_ARGS_BEGIN(Slot, WidgetSlot<Slot>::Arguments)

			DECLARE_WIDGET_ARG_ATTRIBUTE(SplitSizeRule, SizeRule)
			DECLARE_WIDGET_ARG_ATTRIBUTE(float, Size)
			DECLARE_WIDGET_ARG_ATTRIBUTE(float, MinSize)
			DECLARE_WIDGET_ARG_EVENT(OnChangeSlotSizeEvent, OnChangeSlotSize)

		DECALRE_WIDGET_SLOT_ARGS_END()

	public:
		SplitSizeRule GetSizeRule() const
		{
			return _mSizeRule.GetValue(this);
		}
		float GetSize() const
		{
			return _mSize.GetValue(this);
		}
		float GetMinSize() const
		{
			return _mMinSize.GetValue(this);
		}

		void SetSizeRule(Attribute<SplitSizeRule> sizeRule)
		{
			_mSizeRule.Set(this, sizeRule);
		}
		void SetSize(Attribute<float> size)
		{
			_mSize.Set(this, size);
		}
		void SetMinSize(Attribute<float> minSize)
		{
			_mMinSize.Set(this, minSize);
		}

	public:
		OnChangeSlotSizeEvent& OnChangeSlotSize()
		{
			return _mOnChangeSlotSize;
		}
		const OnChangeSlotSizeEvent& OnChangeSlotSize() const
		{
			return _mOnChangeSlotSize;
		}

	public:
		void OnConstruct(const WidgetSlotContainer* newOwner, Arguments&& args);

	protected:
		PROPERTY(_mWidget)
		PROPERTY(_mWidgetConst)

		PROPERTY(_mSizeRule)
		LayoutAttribute<SplitSizeRule> _mSizeRule;
		PROPERTY(_mSize)
		LayoutAttribute<float> _mSize;
		PROPERTY(_mMinSize)
		LayoutAttribute<float> _mMinSize;

		PROPERTY(_mOnChangeSlotSize)
		OnChangeSlotSizeEvent _mOnChangeSlotSize;
	};

protected:
	virtual void PostCreate() override;

	/* 생성 시, 초기화 인자 구조체 정의 */
public:
	DECALRE_WIDGET_ARGS_BEGIN(Splitter)

		DECLARE_WIDGET_ARG_CONST(std::shared_ptr<const SplitterStyle>, Style)

		DECLARE_WIDGET_ARG_SLOT(Slot, Slots)
		DECLARE_INIT_WIDGET_ARG_CONST(WidgetOrientation, SplitOrient, WidgetOrientation::Horizon)
		DECLARE_INIT_WIDGET_ARG_CONST(SplitResizeRule, ResizeRule, SplitResizeRule::ChangeNextSlot)
		DECLARE_INIT_WIDGET_ARG_CONST(float, ElementMinSize, 20.f)
		DECLARE_INIT_WIDGET_ARG_CONST(float, ElementHandleSize, 5.f)
		DECLARE_INIT_WIDGET_ARG_CONST(float, ElementHandleHitSize, 5.f)

	DECALRE_WIDGET_ARGS_END()

public:
	void OnConstruct(const Arguments& args);

public:
	virtual WidgetSlotContainer& GetChildren() override
	{
		return _mSlotContainer;
	}
	virtual const WidgetSlotContainer& GetChildren() const override
	{
		return _mSlotContainer;
	}

public:
	void SetSplitOrient(WidgetOrientation orient);
	WidgetOrientation GetSplitOrient() const;

public:
	virtual ReplyData OnPressMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event) override;
	virtual ReplyData OnReleaseMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event) override;

public:
	virtual ReplyData OnMoveMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event) override;
	virtual void OnLeaveMouse(const std::shared_ptr<const PointEvent>& event) override;

private:
	uint32 OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor) override;

private:
	virtual Vec2 ComputeDesireSize(const Vec2& layoutMultiplyValue) override;
	Vec2 ComputeDesireSizeByOrient();
	virtual void OnArrangeChildren(const WidgetGeometry& allottedGeometry, ArrangedChildren& children) const override;
	std::vector<LayoutTransform2D> ArrangeChildLayouts(const WidgetGeometry& allottedGeometry) const;

public:
	virtual void CollectHeaderDatas(const void* inst, OUT std::unordered_map<std::wstring, std::pair<std::string, PackageBuildScope>>& externalPackageDatas, OUT std::vector<std::shared_ptr<BulkData>>& bulkDatas) const override;

protected:
	virtual void Serialize(Archive& archive) const override;
	virtual void Deserialize(Archive& archive) override;

private:
	/**
	 * 특정 핸들 이전에 리사이징 가능한 가장 가까운 슬롯 인덱스 찾기
	 * \param handleIndex 기준이될 핸들 인덱스
	 * \return 탐색된 슬롯 인덱스
	 */
	int32 FindResizableSlotIndexBeforeHandle(int32 handleIndex) const;
	void FindAllResizableSlotIndexBeforeHandle(int32 handleIndex, OUT std::vector<int32>& slotIndices) const;
	/**
	 * 특정 핸들 이후에 리사이징 가능한 가장 가까운 슬롯 인덱스 찾기
	 * \param handleIndex 기준이될 핸들 인덱스
	 * \return 탐색된 슬롯 인덱스
	 */
	int32 FindResizableSlotIndexAfterHandle(int32 handleIndex) const;
	void FindAllResizableSlotIndexAfterHandle(int32 handleIndex, OUT std::vector<int32>& slotIndices) const;

	/**
	 * 로컬 좌표계의 마우스 아래에 어떤 핸들이 호버되는가?
	 * \param localMousePos 로컬 좌표계의 마우스 위치
	 * \param layoutTransforms 로컬 좌표계로 계산된 레이아웃 트랜스폼들
	 * \return 호버 핸들 인덱스
	 */
	int32 FindHandleIndexUnderMousePos(const Vec2& localMousePos, const std::vector<LayoutTransform2D>& layoutTransforms) const;
	/**
	 * 핸들을 통해 리사이징 시 호출되는 함수
	 * \param delta 로컬 좌표계에서의 이동량
	 * \param layoutTransforms 로컬 좌표계로 계산된 레이아웃 트랜스폼들
	 */
	void Resize(float delta, const std::vector<LayoutTransform2D>& layoutTransforms);
	/**
	 * 핸들을 통해 리사이징 시 호출되는 함수
	 * \param localMousePos 로컬 좌표계의 마우스 위치
	 * \param layoutTransforms 로컬 좌표계로 계산된 레이아웃 트랜스폼들
	 */
	void ResizeByMousePos(const Vec2& localMousePos, const std::vector<LayoutTransform2D>& layoutTransforms);

public:
	static Slot::Arguments MakeSlot();
	PanelSlotContainer<Slot>::ConstructHelper AddSlot(int32 index = -1);
	void RemoveSlot(int32 index);

protected:
	PanelSlotContainer<Slot> _mSlotContainer;

private:
	PROPERTY(_mSplitOrient)
	LayoutAttribute<WidgetOrientation> _mSplitOrient;
	PROPERTY(_mResizeRule)
	SplitResizeRule _mResizeRule;

	// 슬롯 요소의 최저 크기 제한
	PROPERTY(_mElementMinSize)
	float _mElementMinSize;
	// 슬롯 핸들 크기
	PROPERTY(_mElementHandleSize)
	float _mElementHandleSize;
	// 슬롯 핸들의 히트 판정 크기 
	// (핸들 크기보다 조금 더 크게 하여 사용자 경험 향상)
	PROPERTY(_mElementHandleHitSize)
	float _mElementHandleHitSize;

	PROPERTY(_mStyle)
	std::weak_ptr<const SplitterStyle> _mStyle;

private:
	// 현재 호버된 핸들 인덱스 (Slot 인덱스와 별개로 핸들만 카운팅)
	int32 _mHoveredHandleIndex = -1;
	// 현재 하이라이트된 핸들 인덱스 (Slot 인덱스와 별개로 핸들만 카운팅)
	Attribute<int32> _mHighlightHandleIndex = -1;
	bool _mIsResizing = false;
};

