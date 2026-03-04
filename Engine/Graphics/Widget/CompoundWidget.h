#pragma once

#include "Graphics/Widget/Widget.h"
#include "Graphics/Widget/Slot/WidgetSlotOption.h"

/**
 * 자식을 하나 가질 수 있는 베이스 위젯 클래스
 */
class CompoundWidget abstract : public Widget
{
	GEN_ABSTRACT_REFLECTION(CompoundWidget)

	/* 슬롯 구조체 정의 */
public:
	struct Slot : public WidgetSlot<Slot>, public PaddingSlotOption<Slot>, public AlignmentSlotOption<Slot>
	{
		GEN_STRUCT_REFLECTION(CompoundWidget::Slot)

	public:
		using WidgetSlot<Slot>::WidgetSlot;

		/* 생성 시, 초기화 인자 구조체 정의 */
	public:
		DECALRE_WIDGET_SLOT_ARGS_BEGIN(Slot, WidgetSlot<Slot>::Arguments)

			DECLARE_WIDGET_ARG_CONST(Margin, Padding)
			DECLARE_WIDGET_ARG_CONST(SlotAlignmentType, HorizonAlignment)
			DECLARE_WIDGET_ARG_CONST(SlotAlignmentType, VerticalAlignment)

		DECALRE_WIDGET_SLOT_ARGS_END()

	public:
		void OnConstruct(const WidgetSlotContainer* newOwner, Arguments&& args);

	protected:
		PROPERTY(_mWidget)
		PROPERTY(_mWidgetConst)

		PROPERTY(_mPadding)
		PROPERTY(_mHorizonAlignment)
		PROPERTY(_mVerticalAlignment)
	};

public:
	CompoundWidget();

protected:
	virtual void PostCreate() override;

public:
	virtual WidgetSlotContainer& GetChildren() override;
	virtual const WidgetSlotContainer& GetChildren() const override;

private:
	virtual void OnArrangeChildren(const WidgetGeometry& allottedGeometry, ArrangedChildren& children) const override;

protected:
	virtual Vec2 ComputeDesireSize(const Vec2& layoutMultiplyValue) override;
	virtual uint32 OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor) override;

public:
	const Vec2& GetContentSize() const
	{
		return _mContentSize.GetValue(this);
	}
	const Color& GetChildInheritedColor() const
	{
		return _mChildInheritedColor.GetValue();
	}
	const WidgetColor& GetChildForegroundColor() const
	{
		return _mChildForegroundColor.GetValue();
	}

	void SetContentSize(const Attribute<Vec2>& size)
	{
		if (size.IsSet() == true)
		{
			_mContentSize.Set(this, size);
		}
	}
	void SetChildInheritedColor(const Attribute<Color>& color)
	{
		if (color.IsSet() == true)
		{
			_mChildInheritedColor.Set(color);
		}
	}
	void SetChildForegroundColor(const Attribute<WidgetColor>& color)
	{
		if (color.IsSet() == true)
		{
			_mChildForegroundColor.Set(color);
		}
	}

public:
	virtual void CollectHeaderDatas(const void* inst, OUT std::unordered_map<std::wstring, std::pair<std::string, PackageBuildScope>>& externalPackageDatas, OUT std::vector<std::shared_ptr<BulkData>>& bulkDatas) const override;

protected:
	virtual void Serialize(Archive& archive) const override;
	virtual void Deserialize(Archive& archive) override;

public:
	static Slot::Arguments MakeSlot();
	SingleSlotContainer<Slot>::ConstructHelper AddSlot();

protected:
	SingleSlotContainer<Slot> _mSlotContainer;

private:
	PROPERTY(_mContentSize)
	LayoutAttribute<Vec2> _mContentSize;
	// 본인 제외, 자식에게 영향을 주는 색상
	PROPERTY(_mChildInheritedColor)
	Attribute<Color> _mChildInheritedColor;
	// 본인 제외, 자식에게 영향을 주는 별도 요소 색상 (ex. 텍스트)
	PROPERTY(_mChildForegroundColor)
	Attribute<WidgetColor> _mChildForegroundColor;
};

