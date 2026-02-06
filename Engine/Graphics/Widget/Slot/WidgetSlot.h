#pragma once

#include "Graphics/Widget/Type/WidgetTypeInclude.h"

class Widget;

struct WidgetSlotContainer;

struct WidgetSlotBase
{
public:
	WidgetSlotBase(const WidgetSlotContainer* owner, std::shared_ptr<Widget> widget = nullptr) :
		_mOwner(owner),
		_mWidget(widget),
		_mWidgetConst(widget)
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
	const WidgetSlotContainer* _mOwner;
	std::shared_ptr<Widget> _mWidget;
	std::shared_ptr<const Widget> _mWidgetConst;
};

struct WidgetSlotContainer
{
	GEN_STRUCT_REFLECTION(WidgetSlotContainer)

public:
	void ResizeChildren()
	{
		OnResize();
	}
	void AddChild(std::shared_ptr<Widget> child)
	{
		if (IsContain(child) == false)
		{
			AddChild_Internal(child);
			OnResize();
		}
	}
	void RemoveChild(std::shared_ptr<Widget> child)
	{
		if (IsContain(child) == true)
		{
			RemoveChild_Internal(child);
			OnResize();
		}
	}

public:
	virtual std::size_t Size() const = 0;
	virtual std::shared_ptr<Widget> GetChild(std::size_t index) = 0;
	virtual std::shared_ptr<const Widget> GetChild(std::size_t index) const = 0;
	virtual std::shared_ptr<Widget>& GetChildRef(std::size_t index) = 0;
	virtual const std::shared_ptr<const Widget>& GetChildRef(std::size_t index) const = 0;

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
	virtual void AddChild_Internal(const std::shared_ptr<Widget>& child) = 0;
	virtual void RemoveChild_Internal(const std::shared_ptr<Widget>& child) = 0;

protected:
	virtual void OnResize() = 0;

protected:
	PROPERTY(_mOwner)
	std::weak_ptr<Widget> _mOwner;
};

template<typename SlotType> requires std::is_base_of_v<WidgetSlotBase, SlotType>
struct SingleSlotContainer : public WidgetSlotContainer
{
	GEN_STRUCT_REFLECTION(SingleSlotContainer<SlotType>)

public:
	std::shared_ptr<Widget> GetChild()
	{
		return _mChild->GetWidget();
	}
	std::shared_ptr<const Widget> GetChild() const
	{
		return std::shared_ptr<const SlotType>(_mChild)->GetWidget();
	}
	std::shared_ptr<Widget>& GetChildRef()
	{
		return _mChild->GetWidgetRef();
	}
	const std::shared_ptr<const Widget>& GetChildRef() const
	{
		return std::shared_ptr<const SlotType>(_mChild)->GetWidgetRef();
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
		std::size_t childCount = Size();
		TypeInfoResolver<std::size_t>::Get().Serialize(archive, &childCount);

		if (childCount != 0)
		{
			_mChild->GetTypeInfo().Serialize(archive, static_cast<const void*>(_mChild.get()));
		}
	}
	virtual void Deserialize(Archive& archive) override
	{
		// 자식 위젯 갯수 로드
		std::size_t childCount;
		TypeInfoResolver<std::size_t>::Get().Deserialize(archive, &childCount);

		if (childCount != 0)
		{
			_mChild = std::make_shared<SlotType>(this, nullptr);
			_mChild->GetTypeInfo().Deserialize(archive, static_cast<void*>(_mChild.get()));
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
	void AddChild_Internal(const std::shared_ptr<Widget>& child) override
	{
		_mChild = std::make_shared<SlotType>(this, child);
	}
	void RemoveChild_Internal(const std::shared_ptr<Widget>& child) override
	{
		_mChild.reset();
	}

protected:
	void OnResize() override
	{
	}

protected:
	std::shared_ptr<SlotType> _mChild;
};
