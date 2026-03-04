#pragma once

#include "Graphics/Widget/PanelWidget.h"
#include "Graphics/Widget/Type/WidgetTypeInclude.h"
#include "Graphics/Widget/Slot/WidgetSlotOption.h"
#include "Graphics/Widget/Slot/WidgetSlotSizeParams.h"

struct ArrangedChildren;

class BoxPanelWidget abstract : public PanelWidget
{
	GEN_ABSTRACT_REFLECTION(BoxPanelWidget)

public:
	BoxPanelWidget();

	/* 슬롯 구조체 정의 */
public:
	template<typename SlotType>
	struct SlotBase : public WidgetSlot<SlotType>, public PaddingSlotOption<SlotType>, public AlignmentSlotOption<SlotType>
	{
		GEN_STRUCT_REFLECTION(BoxPanelWidget::SlotBase<SlotType>)

	public:
		using WidgetSlot<SlotType>::WidgetSlot;

		/* 생성 시, 초기화 인자 구조체 정의 */
	public:
		DECALRE_WIDGET_SLOT_ARGS_BEGIN(SlotType, WidgetSlot<SlotType>::Arguments)

			DECLARE_WIDGET_ARG_CONST(WidgetSlotSizeParams, SizeParams)
			DECLARE_INIT_WIDGET_ARG_ATTRIBUTE(float, MinSize, 0.f)
			DECLARE_INIT_WIDGET_ARG_ATTRIBUTE(float, MaxSize, FLT_MAX)

			DECLARE_WIDGET_ARG_CONST(Margin, Padding)
			DECLARE_WIDGET_ARG_CONST(SlotAlignmentType, HorizonAlignment)
			DECLARE_WIDGET_ARG_CONST(SlotAlignmentType, VerticalAlignment)

		DECALRE_WIDGET_SLOT_ARGS_END()

	public:
		void OnConstruct(const WidgetSlotContainer* newOwner, Arguments&& args)
		{
			WidgetSlot<SlotType>::OnConstruct(newOwner, std::move(args));

			_mSizeRule = args.mSizeParams.mSizeRule;

			if (args.mSizeParams.mStretchRate.IsSet() == true)
			{
				_mStretchRate.Set(this->_mWidget.get(), std::move(args.mSizeParams.mStretchRate));
			}
			if (args.mSizeParams.mShrinkRate.IsSet() == true)
			{
				_mShrinkRate.Set(this->_mWidget.get(), std::move(args.mSizeParams.mShrinkRate));
			}

			if (args.mMinSize.IsSet() == true)
			{
				_mMinSize.Set(this->_mWidget.get(), std::move(args.mMinSize));
			}
			if (args.mMaxSize.IsSet() == true)
			{
				_mMinSize.Set(this->_mWidget.get(), std::move(args.mMinSize));
			}

			this->_mPadding = args.mPadding;
			this->_mHorizonAlignment = args.mHorizonAlignment;
			this->_mVerticalAlignment = args.mVerticalAlignment;
		}

	public:
		const Margin& GetPadding() const
		{
			return this->_mPadding;
		}
		SlotAlignmentType GetHorizonAlignment() const
		{
			return this->_mHorizonAlignment;
		}
		SlotAlignmentType GetVerticalAlignment() const
		{
			return this->_mVerticalAlignment;
		}
		WidgetSlotSizeRule GetSizeRule() const
		{
			return _mSizeRule;
		}
		float GetStretchRate() const
		{
			return _mStretchRate.GetValue(this->_mWidget.get());
		}
		float GetShrinkRate() const
		{
			return _mShrinkRate.GetValue(this->_mWidget.get());
		}
		float GetMinSize() const
		{
			return _mMinSize.GetValue(this->_mWidget.get());
		}
		float GetMaxSize() const
		{
			return _mMaxSize.GetValue(this->_mWidget.get());
		}

	public:
		void SetSizeParams(WidgetSlotSizeParams sizeParams)
		{
			_mSizeRule = sizeParams.mSizeRule;
			_mStretchRate.Set(this->_mWidget.get(), std::move(sizeParams.mStretchRate));
			if (_mSizeRule == WidgetSlotSizeRule::DetailAllottedRate)
			{
				if (sizeParams.mShrinkRate.IsSet() == true)
				{
					_mShrinkRate.Set(this->_mWidget.get(), std::move(sizeParams.mShrinkRate));
				}
				else
				{
					_mShrinkRate.Set(this->_mWidget.get(), sizeParams.mShrinkRate.GetValue());
				}
			}
			else
			{
				_mShrinkRate.Set(this->_mWidget.get(), 1.f);
			}
		}

		void SetSizeUsingContentSize()
		{
			SetSizeParams(ContentSizeParams());
		}
		void SetSizeUsingAllottedRate(Attribute<float> stretchRate)
		{
			SetSizeParams(AllottedRateParams(std::move(stretchRate)));
		}
		void SetSizeUsingDetailAllottedRate(Attribute<float> stretchRate, Attribute<float> shrinkRate = Attribute<float>())
		{
			SetSizeParams(DetailAllottedRateParams(std::move(stretchRate), std::move(shrinkRate)));
		}

		void SetMinSize(Attribute<float> minSize)
		{
			_mMinSize.Set(this->_mWidget.get(), std::move(minSize));
		}
		void SetMaxSize(Attribute<float> maxSize)
		{
			_mMaxSize.Set(this->_mWidget.get(), std::move(maxSize));
		}

	protected:
		PROPERTY(_mWidget)
		PROPERTY(_mWidgetConst)

		PROPERTY(_mPadding)
		PROPERTY(_mHorizonAlignment)
		PROPERTY(_mVerticalAlignment)

		PROPERTY(_mSizeRule)
		WidgetSlotSizeRule _mSizeRule;
		PROPERTY(_mStretchRate)
		LayoutAttribute<float> _mStretchRate;
		PROPERTY(_mShrinkRate)
		LayoutAttribute<float> _mShrinkRate;
		PROPERTY(_mMinSize)
		LayoutAttribute<float> _mMinSize;
		PROPERTY(_mMaxSize)
		LayoutAttribute<float> _mMaxSize;
	};

	struct Slot : public SlotBase<Slot>
	{
		using SlotBase<Slot>::SlotBase;
	};

	/* Slot 타입 호환을 위한 별도 ConstructHelper */
public:
	template<typename SlotType>
	struct ArgumentConstructHelper : public SlotType::Arguments
	{
	public:
		ArgumentConstructHelper(PanelSlotContainer<Slot>& container, std::shared_ptr<SlotType> slot, int32 slotIndex) :
			SlotType::Arguments(slot),
			_mContainer(container),
			_mSlotIndex(slotIndex)
		{
		}
		~ArgumentConstructHelper()
		{
			Slot::Arguments* castedSlot = static_cast<Slot::Arguments*>(static_cast<WidgetSlotBase::Arguments*>(this));
			if (_mSlotIndex < 0)
			{
				_mContainer.AddChild(std::move(*castedSlot));
			}
			else
			{
				_mContainer.InsertChild(std::move(*castedSlot), _mSlotIndex);
			}
		}

	private:
		PanelSlotContainer<Slot>& _mContainer;
		int32 _mSlotIndex;
	};

protected:
	virtual void PostCreate() override;

public:
	void SetPanelOrient(WidgetOrientation orient)
	{
		if (_mPanelOrient != orient)
		{
			_mPanelOrient = orient;
			MarkLayoutDirty();
		}
	}
	WidgetOrientation GetPanelOrient() const
	{
		return _mPanelOrient;
	}

public:
	virtual WidgetSlotContainer& GetChildren() override;
	virtual const WidgetSlotContainer& GetChildren() const override;

private:
	virtual Vec2 ComputeDesireSize(const Vec2& layoutMultiplyValue) override;
	virtual void OnArrangeChildren(const WidgetGeometry& allottedGeometry, ArrangedChildren& children) const override;

private:
	Vec2 ComputeDesiredSizeForBox();
	void ArrangeChildLayouts(const WidgetGeometry& allottedGeometry, ArrangedChildren& children, bool allowShrink = true) const;

public:
	virtual void CollectHeaderDatas(const void* inst, OUT std::unordered_map<std::wstring, std::pair<std::string, PackageBuildScope>>& externalPackageDatas, OUT std::vector<std::shared_ptr<BulkData>>& bulkDatas) const override;

protected:
	virtual void Serialize(Archive& archive) const override;
	virtual void Deserialize(Archive& archive) override;

public:
	void RemoveSlot(int32 index);

protected:
	PanelSlotContainer<Slot> _mSlotContainer;

protected:
	PROPERTY(_mPanelOrient)
	WidgetOrientation _mPanelOrient;
};

class VerticalBox : public BoxPanelWidget
{
	GEN_REFLECTION(VerticalBox)

public:
	VerticalBox()
	{
		_mVisibility.Init(VisibilityType::SelfHitTestInvisible);
		_mPanelOrient = WidgetOrientation::Vertical;
	}

	/* 슬롯 구조체 정의 */
public:
	struct Slot : public BoxPanelWidget::SlotBase<Slot>
	{
		GEN_STRUCT_REFLECTION(VerticalBox::Slot)

	public:
		using BoxPanelWidget::SlotBase<Slot>::SlotBase;

		/* 생성 시, 초기화 인자 구조체 정의 */
	public:
		DECALRE_WIDGET_SLOT_ARGS_BEGIN(Slot, BoxPanelWidget::SlotBase<Slot>::Arguments)
		public:
			Arguments& ContentSize()
			{
				mSizeParams = ContentSizeParams();
				return *static_cast<Arguments*>(this);
			}

			Arguments& AllottedRate(Attribute<float> stretchRate)
			{
				mSizeParams = AllottedRateParams(std::move(stretchRate));
				return *static_cast<Arguments*>(this);
			}

			Arguments& DetailAllottedRate(Attribute<float> stretchRate, Attribute<float> shrinkRate = Attribute<float>())
			{
				mSizeParams = DetailAllottedRateParams(std::move(stretchRate), std::move(shrinkRate));
				return *static_cast<Arguments*>(this);
			}
		DECALRE_WIDGET_SLOT_ARGS_END()

	public:
		void OnConstruct(const WidgetSlotContainer* newOwner, Arguments&& args);
	};

	/* 생성 시, 초기화 인자 구조체 정의 */
public:
	DECALRE_WIDGET_ARGS_BEGIN(VerticalBox)

		DECLARE_WIDGET_ARG_SLOT(Slot, Slots)

	DECALRE_WIDGET_ARGS_END()

public:
	void OnConstruct(const Arguments& args);

public:
	static Slot::Arguments MakeSlot();
	ArgumentConstructHelper<Slot> AddSlot(int32 index = -1);

	Slot& GetSlot(int32 index);
	const Slot& GetSlot(int32 index) const;
};

class HorizonBox : public BoxPanelWidget
{
	GEN_REFLECTION(HorizonBox)

public:
	HorizonBox()
	{
		_mVisibility.Init(VisibilityType::SelfHitTestInvisible);
		_mPanelOrient = WidgetOrientation::Horizon;
	}

	/* 슬롯 구조체 정의 */
public:
	struct Slot : public BoxPanelWidget::SlotBase<Slot>
	{
		GEN_STRUCT_REFLECTION(HorizonBox::Slot)

	public:
		using BoxPanelWidget::SlotBase<Slot>::SlotBase;

		/* 생성 시, 초기화 인자 구조체 정의 */
	public:
		DECALRE_WIDGET_SLOT_ARGS_BEGIN(Slot, BoxPanelWidget::SlotBase<Slot>::Arguments)
		public:
			Arguments& ContentSize()
			{
				mSizeParams = ContentSizeParams();
				return *static_cast<Arguments*>(this);
			}

			Arguments& AllottedRate(Attribute<float> stretchRate)
			{
				mSizeParams = AllottedRateParams(std::move(stretchRate));
				return *static_cast<Arguments*>(this);
			}

			Arguments& DetailAllottedRate(Attribute<float> stretchRate, Attribute<float> shrinkRate = Attribute<float>())
			{
				mSizeParams = DetailAllottedRateParams(std::move(stretchRate), std::move(shrinkRate));
				return *static_cast<Arguments*>(this);
			}
		DECALRE_WIDGET_SLOT_ARGS_END()

	public:
		void OnConstruct(const WidgetSlotContainer* newOwner, Arguments&& args);
	};

	/* 생성 시, 초기화 인자 구조체 정의 */
public:
	DECALRE_WIDGET_ARGS_BEGIN(HorizonBox)

		DECLARE_WIDGET_ARG_SLOT(Slot, Slots)

	DECALRE_WIDGET_ARGS_END()

public:
	void OnConstruct(const Arguments& args);

public:
	static Slot::Arguments MakeSlot();
	ArgumentConstructHelper<Slot> AddSlot(int32 index = -1);

	Slot& GetSlot(int32 index);
	const Slot& GetSlot(int32 index) const;
};
