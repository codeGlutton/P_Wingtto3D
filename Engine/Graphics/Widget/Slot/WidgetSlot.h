#pragma once

#include "Graphics/Widget/Type/WidgetTypeInclude.h"

#define DECALRE_WIDGET_SLOT_ARGS_BEGIN(slotType, parent)\
struct Arguments : public parent 						\
{														\
private:												\
	using RetArgs = slotType::Arguments;				\
														\
public:													\
	Arguments() :										\
		parent()										\
	{													\
	}													\
	Arguments(std::shared_ptr<slotType> slot) :			\
		parent(std::move(slot))							\
	{													\
	}													\
	Arguments(const Arguments&) = delete;				\
	Arguments(Arguments&&) = default;					\
														\
public:													\
	Arguments& operator=(const Arguments&) = delete;	\
	Arguments& operator=(Arguments&&) = default;
#define DECALRE_WIDGET_SLOT_ARGS_END() };

class Widget;

struct WidgetSlotContainer;

struct WidgetSlotBase abstract
{
public:
	struct Arguments abstract
	{
		virtual ~Arguments() = default;
	};

public:
	WidgetSlotBase()
	{
	}
	WidgetSlotBase(const WidgetSlotContainer* owner) :
		_mOwner(owner)
	{
	}
	virtual ~WidgetSlotBase()
	{
	}

public:
	const WidgetSlotContainer* GetOwnerContainer() const
	{
		return _mOwner;
	}
	std::shared_ptr<Widget> GetWidget()
	{
		return _mWidget;
	}
	std::shared_ptr<const Widget> GetWidget() const
	{
		return _mWidgetConst;
	}
	std::shared_ptr<Widget>& GetWidgetRef()
	{
		return _mWidget;
	}
	const std::shared_ptr<const Widget>& GetWidgetRef() const
	{
		return _mWidgetConst;
	}

protected:
	const WidgetSlotContainer* _mOwner = nullptr;
	std::shared_ptr<Widget> _mWidget;
	std::shared_ptr<const Widget> _mWidgetConst;
};

template<typename SlotType>
struct WidgetSlot abstract : public WidgetSlotBase
{
public:
	using WidgetSlotBase::WidgetSlotBase;

public:
	struct Arguments : public WidgetSlotBase::Arguments
	{
	public:
		Arguments()
		{
		}
		Arguments(std::shared_ptr<SlotType> slot) :
			_mSlot(std::move(slot))
		{
		}
		Arguments(const Arguments&) = delete;
		Arguments(Arguments&&) = default;

	public:
		Arguments& operator=(const Arguments&) = delete;
		Arguments& operator=(Arguments&&) = default;

	public:
		typename SlotType::Arguments& operator[](std::shared_ptr<Widget>&& widget)
		{
			_mWidget = std::move(widget);
			return *static_cast<typename SlotType::Arguments*>(this);
		}
		typename SlotType::Arguments& operator[](const std::shared_ptr<Widget>& widget)
		{
			_mWidget = widget;
			return *static_cast<typename SlotType::Arguments*>(this);
		}
		typename SlotType::Arguments& Assign(std::weak_ptr<SlotType>& slot)
		{
			slot = _mSlot;
			return *static_cast<typename SlotType::Arguments*>(this);
		}
		typename SlotType::Arguments& Assign(std::shared_ptr<SlotType>& slot)
		{
			slot = _mSlot;
			return *static_cast<typename SlotType::Arguments*>(this);
		}

	public:
		std::shared_ptr<SlotType> GetSlot() const
		{
			return _mSlot;
		}
		std::shared_ptr<SlotType>& GetSlotRef()
		{
			return _mSlot;
		}
		const std::shared_ptr<SlotType>& GetSlotRef() const
		{
			return _mSlot;
		}
		std::shared_ptr<Widget> GetWidget()
		{
			return _mWidget;
		}
		std::shared_ptr<const Widget> GetWidget() const
		{
			return _mWidget;
		}
		std::shared_ptr<Widget>& GetWidgetRef()
		{
			return _mWidget;
		}
		const std::shared_ptr<const Widget>& GetWidgetRef() const
		{
			return _mWidget;
		}

	private:
		std::shared_ptr<SlotType> _mSlot;
		std::shared_ptr<Widget> _mWidget;
	};

public:
	void OnConstruct(const WidgetSlotContainer* newOwner, Arguments&& args)
	{
		_mWidgetConst = _mWidget = args.GetWidget();
		if (_mOwner != newOwner)
		{
			_mOwner = newOwner;
			if (_mOwner != nullptr && _mWidget != nullptr)
			{
				std::shared_ptr<Widget> parent = _mOwner->GetOwner();
				if (parent != nullptr)
				{
					_mWidget->AssignParent(parent);
				}
			}
		}
	}
	void OnDestruct()
	{
		if (_mWidget != nullptr)
		{
			_mWidget->ResetParent();
		}
		_mOwner = nullptr;
	}

public:
	SlotType& operator[](std::shared_ptr<Widget>&& widget)
	{
		_mWidgetConst = _mWidget = std::move(widget);
		if (_mOwner != nullptr && _mWidget != nullptr)
		{
			std::shared_ptr<Widget> parent = _mOwner->GetOwner();
			if (parent != nullptr)
			{
				_mWidget->AssignParent(parent);
			}
		}
		return *static_cast<SlotType*>(this);
	}
	SlotType& operator[](const std::shared_ptr<Widget>& widget)
	{
		_mWidgetConst = _mWidget = widget;
		if (_mOwner != nullptr && _mWidget != nullptr)
		{
			std::shared_ptr<Widget> parent = _mOwner->GetOwner();
			if (parent != nullptr)
			{
				_mWidget->AssignParent(parent);
			}
		}
		return *static_cast<SlotType*>(this);
	}
};

struct WidgetSlotContainer
{
	GEN_STRUCT_REFLECTION(WidgetSlotContainer)

public:
	WidgetSlotContainer() = default;
	WidgetSlotContainer(std::shared_ptr<Widget> owner) :
		_mOwner(owner)
	{
	}
	virtual ~WidgetSlotContainer() = default;

public:
	std::shared_ptr<Widget> GetOwner() const
	{
		return _mOwner.lock();
	}

public:
	void RefreshChildren()
	{
		OnResize();
	}

public:
	virtual std::size_t Size() const = 0;
	virtual std::shared_ptr<Widget> GetChild(std::size_t index) = 0;
	virtual std::shared_ptr<const Widget> GetChild(std::size_t index) const = 0;
	virtual std::shared_ptr<Widget>& GetChildRef(std::size_t index) = 0;
	virtual const std::shared_ptr<const Widget>& GetChildRef(std::size_t index) const = 0;

	virtual std::shared_ptr<WidgetSlotBase> GetSlot(std::size_t index) = 0;
	virtual std::shared_ptr<const WidgetSlotBase> GetSlot(std::size_t index) const = 0;
	virtual WidgetSlotBase& GetSlotRef(std::size_t index) = 0;
	virtual const WidgetSlotBase& GetSlotRef(std::size_t index) const = 0;

	template<typename Predicate>
	void DoForEach(Predicate&& pred)
	{
		std::size_t size = Size();
		for (std::size_t i = 0; i < size; ++i)
		{
			std::shared_ptr<Widget>& widget = GetChildRef(i);
			pred(widget);
		}
	}
	template<typename Predicate>
	void DoForEach(Predicate&& pred) const
	{
		std::size_t size = Size();
		for (std::size_t i = 0; i < size; ++i)
		{
			const std::shared_ptr<const Widget>& widget = GetChildRef(i);
			pred(widget);
		}
	}

public:
	virtual void Serialize(Archive& archive) const = 0;
	virtual void Deserialize(Archive& archive) = 0;

protected:
	virtual bool IsContain(const std::shared_ptr<Widget>& child) = 0;

protected:
	void OnResize();

protected:
	PROPERTY(_mOwner)
	std::weak_ptr<Widget> _mOwner;
};

struct EmptySlotContainer final : public WidgetSlotContainer
{
	GEN_STRUCT_REFLECTION(EmptySlotContainer)

public:
	EmptySlotContainer() = default;
	EmptySlotContainer(std::shared_ptr<Widget> owner) :
		WidgetSlotContainer(owner)
	{
	}

public:
	static EmptySlotContainer& GetInst();

public:
	virtual std::size_t Size() const override
	{
		return 0;
	}
	virtual std::shared_ptr<Widget> GetChild(std::size_t index) override
	{
		FAIL_MSG("Is empty widget container");
		return _mNullSlot->GetWidget();
	}
	virtual std::shared_ptr<const Widget> GetChild(std::size_t index) const override
	{
		FAIL_MSG("Is empty widget container");
		return static_cast<const WidgetSlotBase*>(_mNullSlot.get())->GetWidget();
	}
	virtual std::shared_ptr<Widget>& GetChildRef(std::size_t index) override
	{
		FAIL_MSG("Is empty widget container");
		return _mNullSlot->GetWidgetRef();
	}
	virtual const std::shared_ptr<const Widget>& GetChildRef(std::size_t index) const override
	{
		FAIL_MSG("Is empty widget container");
		return static_cast<const WidgetSlotBase*>(_mNullSlot.get())->GetWidgetRef();
	}

	virtual std::shared_ptr<WidgetSlotBase> GetSlot(std::size_t index) override
	{
		FAIL_MSG("Is empty widget container");
		return _mNullSlot;
	}
	virtual std::shared_ptr<const WidgetSlotBase> GetSlot(std::size_t index) const override
	{
		FAIL_MSG("Is empty widget container");
		return std::static_pointer_cast<const WidgetSlotBase>(_mNullSlot);
	}
	virtual WidgetSlotBase& GetSlotRef(std::size_t index) override
	{
		FAIL_MSG("Is empty widget container");
		return *_mNullSlot;
	}
	virtual const WidgetSlotBase& GetSlotRef(std::size_t index) const override
	{
		FAIL_MSG("Is empty widget container");
		return *static_cast<const WidgetSlotBase*>(_mNullSlot.get());
	}

public:
	virtual void Serialize(Archive& archive) const override
	{
	}
	virtual void Deserialize(Archive& archive) override
	{
	}

protected:
	bool IsContain(const std::shared_ptr<Widget>& child) override
	{
		return false;
	}

private:
	static std::shared_ptr<WidgetSlotBase> _mNullSlot;
};

template<typename SlotType> requires std::is_base_of_v<WidgetSlotBase, SlotType>
struct SingleSlotContainer : public WidgetSlotContainer
{
	GEN_STRUCT_REFLECTION(SingleSlotContainer<SlotType>)

public:
	SingleSlotContainer() = default;
	SingleSlotContainer(std::shared_ptr<Widget> owner) :
		WidgetSlotContainer(owner)
	{
	}

public:
	std::shared_ptr<Widget> GetChild()
	{
		return _mChild->GetWidget();
	}
	std::shared_ptr<const Widget> GetChild() const
	{
		return static_cast<const SlotType*>(_mChild.get())->GetWidget();
	}
	std::shared_ptr<Widget>& GetChildRef()
	{
		return _mChild->GetWidgetRef();
	}
	const std::shared_ptr<const Widget>& GetChildRef() const
	{
		return static_cast<const SlotType*>(_mChild.get())->GetWidgetRef();
	}
	SlotType& GetSlotRef()
	{
		return *_mChild.get();
	}
	const SlotType& GetSlotRef() const
	{
		return *_mChild.get();
	}

public:
	virtual std::size_t Size() const override
	{
		return _mChild == nullptr ? 0 : 1;
	}
	virtual std::shared_ptr<Widget> GetChild(std::size_t index) override
	{
		ASSERT_MSG(index == 0, "Is single widget container");
		return GetChild();
	}
	virtual std::shared_ptr<const Widget> GetChild(std::size_t index) const override
	{
		ASSERT_MSG(index == 0, "Is single widget container");
		return GetChild();
	}
	virtual std::shared_ptr<Widget>& GetChildRef(std::size_t index) override
	{
		ASSERT_MSG(index == 0, "Is single widget container");
		return GetChildRef();
	}
	virtual const std::shared_ptr<const Widget>& GetChildRef(std::size_t index) const override
	{
		ASSERT_MSG(index == 0, "Is single widget container");
		return GetChildRef();
	}

	virtual std::shared_ptr<WidgetSlotBase> GetSlot(std::size_t index) override
	{
		ASSERT_MSG(index == 0, "Is single widget container");
		return _mChild;
	}
	virtual std::shared_ptr<const WidgetSlotBase> GetSlot(std::size_t index) const override
	{
		ASSERT_MSG(index == 0, "Is single widget container");
		return _mChild;
	}
	virtual WidgetSlotBase& GetSlotRef(std::size_t index) override
	{
		ASSERT_MSG(index == 0, "Is single widget container");
		return *static_cast<WidgetSlotBase*>(_mChild.get());
	}
	virtual const WidgetSlotBase& GetSlotRef(std::size_t index) const override
	{
		ASSERT_MSG(index == 0, "Is single widget container");
		return *static_cast<const WidgetSlotBase*>(_mChild.get());
	}

public:
	virtual void Serialize(Archive& archive) const override
	{
		// 자식 위젯 갯수 저장
		const std::size_t childCount = Size();
		TypeInfoResolver<std::size_t>::Get().Serialize(archive, &childCount);

		if (childCount != 0)
		{
			SlotType::GetStaticTypeInfo().Serialize(archive, static_cast<const void*>(_mChild.get()));
		}
	}
	virtual void Deserialize(Archive& archive) override
	{
		// 자식 위젯 갯수 로드
		std::size_t childCount;
		TypeInfoResolver<std::size_t>::Get().Deserialize(archive, &childCount);

		if (childCount != 0)
		{
			_mChild = std::make_shared<SlotType>(this);
			SlotType::GetStaticTypeInfo().Deserialize(archive, static_cast<void*>(_mChild.get()));
		}
	}

public:
	void AddChild(typename SlotType::Arguments& args)
	{
		if (IsContain(args.GetWidgetRef()) == false)
		{
			_mChild = args.GetSlotRef();
			_mChild->OnConstruct(this, std::move(args));

			OnResize();
		}
	}
	void AddChild(typename SlotType::Arguments&& args)
	{
		if (IsContain(args.GetWidgetRef()) == false)
		{
			_mChild = args.GetSlotRef();
			_mChild->OnConstruct(this, std::move(args));

			OnResize();
		}
	}
	void RemoveChild(std::shared_ptr<Widget> child)
	{
		if (IsContain(child) == true)
		{
			_mChild->OnDestruct();
			_mChild.reset();

			OnResize();
		}
	}
	void RemoveChild()
	{
		_mChild.reset();
		OnResize();
	}
	void RemoveChild(std::size_t index)
	{
		if (index == 0)
		{
			_mChild.reset();
			OnResize();
		}
	}

protected:
	bool IsContain(const std::shared_ptr<Widget>& child) override
	{
		if (_mChild != nullptr && _mChild->GetWidgetRef() == child)
		{
			return true;
		}
		return false;
	}

public:
	struct ConstructHelper : public SlotType::Arguments
	{
	public:
		ConstructHelper(SingleSlotContainer<SlotType>& container, std::shared_ptr<SlotType> slot) :
			SlotType::Arguments(slot),
			_mContainer(container)
		{
		}
		~ConstructHelper()
		{
			_mContainer.AddChild(std::move(*this));
		}

	private:
		SingleSlotContainer<SlotType>& _mContainer;
	};

protected:
	std::shared_ptr<SlotType> _mChild;
};

template<typename SlotType> requires std::is_base_of_v<WidgetSlotBase, SlotType>
struct PanelSlotContainer : public WidgetSlotContainer
{
public:
	PanelSlotContainer() = default;
	PanelSlotContainer(std::shared_ptr<Widget> owner) :
		WidgetSlotContainer(owner)
	{
	}

public:
	virtual std::size_t Size() const override
	{
		return _mChildren.size();
	}
	virtual std::shared_ptr<Widget> GetChild(std::size_t index) override
	{
		return _mChildren[index]->GetWidget();
	}
	virtual std::shared_ptr<const Widget> GetChild(std::size_t index) const override
	{
		return static_cast<const SlotType*>(_mChildren[index].get())->GetWidgetRef();
	}
	virtual std::shared_ptr<Widget>& GetChildRef(std::size_t index) override
	{
		return _mChildren[index]->GetWidgetRef();
	}
	virtual const std::shared_ptr<const Widget>& GetChildRef(std::size_t index) const override
	{
		return static_cast<const SlotType*>(_mChildren[index].get())->GetWidgetRef();
	}
	virtual std::shared_ptr<WidgetSlotBase> GetSlot(std::size_t index) override
	{
		return _mChildren[index];
	}
	virtual std::shared_ptr<const WidgetSlotBase> GetSlot(std::size_t index) const override
	{
		return _mChildren[index];
	}
	virtual WidgetSlotBase& GetSlotRef(std::size_t index) override
	{
		return *_mChildren[index];
	}
	virtual const WidgetSlotBase& GetSlotRef(std::size_t index) const override
	{
		return *_mChildren[index];
	}

public:
	virtual void Serialize(Archive& archive) const override
	{
		// 자식 위젯 갯수 저장
		const std::size_t childCount = Size();
		TypeInfoResolver<std::size_t>::Get().Serialize(archive, &childCount);

		for (std::size_t i = 0; i < childCount; ++i)
		{
			SlotType::GetStaticTypeInfo().Serialize(archive, static_cast<const void*>(_mChildren[i].get()));
		}
	}
	virtual void Deserialize(Archive& archive) override
	{
		// 자식 위젯 갯수 저장
		std::size_t childCount;
		TypeInfoResolver<std::size_t>::Get().Deserialize(archive, &childCount);

		_mChildren.resize(childCount);
		for (std::size_t i = 0; i < childCount; ++i)
		{
			_mChildren[i] = std::make_shared<SlotType>(this);
			SlotType::GetStaticTypeInfo().Deserialize(archive, static_cast<void*>(_mChildren[i].get()));
		}
	}

public:
	virtual bool IsContain(const std::shared_ptr<Widget>& child) override
	{
		for (const std::shared_ptr<SlotType>& childSlot : _mChildren)
		{
			if (child == childSlot->GetWidgetRef())
			{
				return true;
			}
		}
		return false;
	}

public:
	void Reserve(std::size_t size)
	{
		_mChildren.reserve(size);
	}

	void AddChild(typename SlotType::Arguments& args)
	{
		_mChildren.push_back(args.GetSlotRef());
		_mChildren.back()->OnConstruct(this, std::move(args));
		OnResize();
	}
	void AddChild(typename SlotType::Arguments&& args)
	{
		_mChildren.push_back(args.GetSlotRef());
		_mChildren.back()->OnConstruct(this, std::move(args));
		OnResize();
	}
	void AddChildren(typename std::vector<typename SlotType::Arguments>&& argList)
	{
		for (typename SlotType::Arguments& args : argList)
		{
			_mChildren.push_back(args.GetSlotRef());
			_mChildren.back()->OnConstruct(this, std::move(args));
		}
		OnResize();
	}
	void RemoveChild(const std::shared_ptr<Widget>& child)
	{
		for (auto iter = _mChildren.begin(); iter != _mChildren.end(); ++iter)
		{
			if (child == (*iter)->GetWidgetRef())
			{
				_mChildren.erase(iter);
				OnResize();
				return;
			}
		}
	}
	void RemoveChild(std::size_t index)
	{
		_mChildren.erase(_mChildren.begin() + index);
		OnResize();
	}
	void InsertChild(typename SlotType::Arguments&& args, std::size_t index)
	{
		_mChildren.insert(_mChildren.begin() + index, args.GetSlotRef());
		_mChildren[index]->OnConstruct(this, std::move(args));
		OnResize();
	}

	template<typename Pred>
	void Sort()
	{
		std::sort(_mChildren.begin(), _mChildren.end(), Pred());
		OnResize();
	}
	void Sort(std::function<bool(const std::shared_ptr<SlotType>&, const std::shared_ptr<SlotType>&)> compare)
	{
		std::sort(_mChildren.begin(), _mChildren.end(), compare);
		OnResize();
	}

	template<typename Pred>
	void StableSort()
	{
		std::stable_sort(_mChildren.begin(), _mChildren.end(), Pred());
		OnResize();
	}
	void StableSort(std::function<bool(const std::shared_ptr<SlotType>&, const std::shared_ptr<SlotType>&)> compare)
	{
		std::stable_sort(_mChildren.begin(), _mChildren.end(), compare);
		OnResize();
	}

	void Swap(std::size_t lhsIndex, std::size_t rhsIndex)
	{
		std::swap(_mChildren[lhsIndex], _mChildren[rhsIndex]);
		OnResize();
	}

	void Clear()
	{
		_mChildren.clear();
		OnResize();
	}

public:
	struct ConstructHelper : public SlotType::Arguments
	{
	public:
		ConstructHelper(PanelSlotContainer<SlotType>& container, std::shared_ptr<SlotType> slot, int32 slotIndex) :
			SlotType::Arguments(slot),
			_mContainer(container),
			_mSlotIndex(slotIndex)
		{
		}
		~ConstructHelper()
		{
			if (_mSlotIndex < 0)
			{
				_mContainer.AddChild(std::move(*this));
			}
			else
			{
				_mContainer.InsertChild(std::move(*this), _mSlotIndex);
			}
		}

	private:
		PanelSlotContainer<SlotType>& _mContainer;
		int32 _mSlotIndex;
	};

private:
	std::vector<std::shared_ptr<SlotType>> _mChildren;
};
