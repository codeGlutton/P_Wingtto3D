#pragma once

class Widget;

template<typename T>
struct LayoutAttribute;

struct WidgetSlotBase;

/**
 * 바인딩 가능 속성
 */
template<typename T>
struct Attribute
{
	GEN_MINIMUM_STRUCT_REFLECTION(Attribute<T>)

public:
	friend struct LayoutAttribute<T>;

public:
	DECLARE_DYNAMIC_DELEGATE_RET(OnGet, T);

public:
	Attribute() :
		_mValue(),
		_mIsSet(false),
		_mGetter()
	{
	}

	template<typename InputType>
	Attribute(const InputType& value) :
		_mValue(static_cast<T>(value)),
		_mIsSet(true),
		_mGetter()
	{
	}

	Attribute(T&& value) :
		_mValue(std::move(value)),
		_mIsSet(true),
		_mGetter()
	{
	}

	Attribute(const OnGet& getter) :
		_mValue(),
		_mIsSet(true),
		_mGetter(getter)
	{
	}

	Attribute(OnGet&& getter) :
		_mValue(),
		_mIsSet(true),
		_mGetter(std::move(getter))
	{
	}

	Attribute(OnGet::Binder binder) :
		_mValue(),
		_mIsSet(true),
		_mGetter()
	{
		_mGetter.Bind(binder);
	}

	Attribute(std::shared_ptr<Object> object, const std::string& methodName) :
		_mValue(),
		_mIsSet(true),
		_mGetter()
	{
		_mGetter.BindMethod(std::move(object), methodName.c_str());
	}

	Attribute(std::shared_ptr<Object> object, std::string&& methodName) :
		_mValue(),
		_mIsSet(true),
		_mGetter()
	{
		_mGetter.BindMethod(std::move(object), methodName.c_str());
	}

	~Attribute()
	{
		_mGetter.Clear();
	}

public:
	bool IsSet() const
	{
		return _mIsSet;
	}

	const T& GetValue() const
	{
		if (_mGetter.IsBound() == true)
		{
			_mValue = _mGetter.Execute();
			return _mValue;
		}

		return _mValue;
	}

	const T& GetValue(const T& defaultValue) const
	{
		return _mIsSet == true ? GetValue() : defaultValue;
	}

public:
	template<typename InputType> requires std::is_convertible_v<InputType, T>
	void Set(const InputType& value)
	{
		_mValue = static_cast<T>(value);
		_mIsSet = true;
	}

	void Set(const T& value)
	{
		_mValue = value;
		_mIsSet = true;
	}
	void Set(T&& value)
	{
		_mValue = std::move(value);
		_mIsSet = true;
	}

	void Set(const Attribute<T>& attribute)
	{
		_mValue = attribute._mValue;
		_mGetter = attribute._mGetter;
		_mIsSet = attribute._mIsSet;
	}

	void Bind(const OnGet& getter)
	{
		_mGetter = getter;
		_mIsSet = true;
	}

	void Bind(OnGet&& getter)
	{
		_mGetter = std::move(getter);
		_mIsSet = true;
	}

	void Bind(OnGet::Binder binder)
	{
		_mGetter.Bind(binder);
		_mIsSet = true;
	}

	void Bind(std::shared_ptr<Object> object, const std::string& methodName)
	{
		_mGetter.BindMethod(object, methodName.c_str());
		_mIsSet = true;
	}

	void Bind(std::shared_ptr<Object> object, std::string&& methodName)
	{
		_mGetter.BindMethod(object, methodName.c_str());
		_mIsSet = true;
	}

private:
	PROPERTY(_mValue)
	mutable T _mValue;
	PROPERTY(_mIsSet)
	bool _mIsSet;
	PROPERTY(_mGetter)
	OnGet _mGetter;
};

struct LayoutAttributeBase
{
protected:
	void OnChangeLayout(const Widget* widget) const;
	void OnChangeLayout(const WidgetSlotBase* slot) const;
};

/**
 * 값 변경 시점에 레이아웃 더티를 표기해주는 바인딩 가능 속성
 */
template<typename T>
struct LayoutAttribute : public LayoutAttributeBase
{
	GEN_MINIMUM_STRUCT_REFLECTION(LayoutAttribute<T>)

public:
	DECLARE_DYNAMIC_DELEGATE_RET(OnGet, T);

public:
	LayoutAttribute() :
		_mValue(),
		_mIsSet(false),
		_mGetter()
	{
	}
	LayoutAttribute(const T& value) :
		_mValue(value),
		_mIsSet(true),
		_mGetter()
	{
	}
	LayoutAttribute(T&& value) :
		_mValue(value),
		_mIsSet(true),
		_mGetter()
	{
	}
	LayoutAttribute(const Widget* widget, const Attribute<T>& attribute) :
		_mValue(attribute._mValue),
		_mIsSet(attribute._mIsSet),
		_mGetter(attribute._mGetter)
	{
		GetValue(widget);
	}
	LayoutAttribute(const WidgetSlotBase* slot, const Attribute<T>& attribute) :
		_mValue(attribute._mValue),
		_mIsSet(attribute._mIsSet),
		_mGetter(attribute._mGetter)
	{
		GetValue(slot);
	}
	LayoutAttribute(const Widget* widget, Attribute<T>&& attribute) :
		_mValue(std::move(attribute._mValue)),
		_mIsSet(std::move(attribute._mIsSet)),
		_mGetter(std::move(attribute._mGetter))
	{
		GetValue(widget);
	}
	LayoutAttribute(const WidgetSlotBase* slot, Attribute<T>&& attribute) :
		_mValue(std::move(attribute._mValue)),
		_mIsSet(std::move(attribute._mIsSet)),
		_mGetter(std::move(attribute._mGetter))
	{
		GetValue(slot);
	}

	template<typename InputType> requires std::is_convertible_v<InputType, T>
	LayoutAttribute(const Widget* widget, const InputType& value)
	{
		Set(widget, static_cast<T>(value));
	}
	template<typename InputType> requires std::is_convertible_v<InputType, T>
	LayoutAttribute(const WidgetSlotBase* slot, const InputType& value)
	{
		Set(slot, static_cast<T>(value));
	}

	LayoutAttribute(const Widget* widget, T&& value)
	{
		Set(widget, std::move(value));
	}
	LayoutAttribute(const WidgetSlotBase* slot, T&& value)
	{
		Set(slot, std::move(value));
	}

	LayoutAttribute(const OnGet& getter) :
		_mValue(),
		_mIsSet(true),
		_mGetter(getter)
	{
	}

	LayoutAttribute(OnGet&& getter) :
		_mValue(),
		_mIsSet(true),
		_mGetter(std::move(getter))
	{
	}

	LayoutAttribute(OnGet::Binder binder) :
		_mValue(),
		_mIsSet(true),
		_mGetter()
	{
		_mGetter.Bind(binder);
	}

	LayoutAttribute(std::shared_ptr<Object> object, const std::string& methodName) :
		_mValue(),
		_mIsSet(true),
		_mGetter()
	{
		_mGetter.BindMethod(std::move(object), methodName.c_str());
	}

	LayoutAttribute(std::shared_ptr<Object> object, std::string&& methodName) :
		_mValue(),
		_mIsSet(true),
		_mGetter()
	{
		_mGetter.BindMethod(std::move(object), methodName.c_str());
	}

	~LayoutAttribute()
	{
		_mGetter.Clear();
	}

public:
	LayoutAttribute& operator==(const LayoutAttribute& other) = delete;
	LayoutAttribute& operator==(LayoutAttribute&& other) = delete;

public:
	bool IsSet() const
	{
		return _mIsSet;
	}

	const T& GetValue(const Widget* widget) const
	{
		if (_mGetter.IsBound() == true)
		{
			T newValue = _mGetter.Execute();
			if (_mValue != newValue)
			{
				_mValue = std::move(newValue);
				OnChangeLayout(widget);
			}
			return _mValue;
		}

		return _mValue;
	}
	const T& GetValue(const WidgetSlotBase* slot) const
	{
		if (_mGetter.IsBound() == true)
		{
			T newValue = _mGetter.Execute();
			if (_mValue != newValue)
			{
				_mValue = std::move(newValue);
				OnChangeLayout(slot);
			}
			return _mValue;
		}

		return _mValue;
	}

	const T& GetValue(const Widget* widget, const T& defaultValue) const
	{
		return _mIsSet == true ? GetValue(widget) : defaultValue;
	}
	const T& GetValue(const WidgetSlotBase* slot, const T& defaultValue) const
	{
		return _mIsSet == true ? GetValue(slot) : defaultValue;
	}

public:
	void Init(const T& value)
	{
		_mValue = value;
		_mIsSet = true;
	}
	void Init(T&& value)
	{
		_mValue = value;
		_mIsSet = true;
	}

	template<typename InputType>
	void Set(const Widget* widget, const InputType& value)
	{
		T newValue = static_cast<T>(value);
		Set(widget, std::move(newValue));
	}
	template<typename InputType>
	void Set(const WidgetSlotBase* slot, const InputType& value)
	{
		T newValue = static_cast<T>(value);
		Set(slot, std::move(newValue));
	}

	void Set(const Widget* widget, const T& value)
	{
		if (_mValue != value)
		{
			_mValue = value;
			OnChangeLayout(widget);
		}
		_mIsSet = true;
	}
	void Set(const WidgetSlotBase* slot, const T& value)
	{
		if (_mValue != value)
		{
			_mValue = value;
			OnChangeLayout(slot);
		}
		_mIsSet = true;
	}
	void Set(const Widget* widget, T&& value)
	{
		if (_mValue != value)
		{
			_mValue = std::move(value);
			OnChangeLayout(widget);
		}
		_mIsSet = true;
	}
	void Set(const WidgetSlotBase* slot, T&& value)
	{
		if (_mValue != value)
		{
			_mValue = std::move(value);
			OnChangeLayout(slot);
		}
		_mIsSet = true;
	}

	void Set(const Widget* widget, Attribute<T>&& attribute)
	{
		_mValue = std::move(attribute._mValue);
		_mIsSet = std::move(attribute._mIsSet);
		_mGetter = std::move(attribute._mGetter);

		GetValue(widget);
	}
	void Set(const WidgetSlotBase* slot, Attribute<T>&& attribute)
	{
		_mValue = std::move(attribute._mValue);
		_mIsSet = std::move(attribute._mIsSet);
		_mGetter = std::move(attribute._mGetter);

		GetValue(slot);
	}
	void Set(const Widget* widget, const Attribute<T>& attribute)
	{
		_mValue = attribute._mValue;
		_mIsSet = attribute._mIsSet;
		_mGetter = attribute._mGetter;

		GetValue(widget);
	}
	void Set(const WidgetSlotBase* slot, const Attribute<T>& attribute)
	{
		_mValue = attribute._mValue;
		_mIsSet = attribute._mIsSet;
		_mGetter = attribute._mGetter;

		GetValue(slot);
	}
	void Set(const Widget* widget, const LayoutAttribute<T>& attribute)
	{
		_mValue = attribute._mValue;
		_mIsSet = attribute._mIsSet;
		_mGetter = attribute._mGetter;

		GetValue(widget);
	}
	void Set(const WidgetSlotBase* slot, const LayoutAttribute<T>& attribute)
	{
		_mValue = attribute._mValue;
		_mIsSet = attribute._mIsSet;
		_mGetter = attribute._mGetter;

		GetValue(slot);
	}

	void Bind(const OnGet& InGetter)
	{
		_mGetter = InGetter;
		_mIsSet = true;
	}

	void Bind(OnGet&& InGetter)
	{
		_mGetter = std::move(InGetter);
		_mIsSet = true;
	}

	void Bind(OnGet::Binder binder)
	{
		_mGetter.Bind(binder);
		_mIsSet = true;
	}

	void Bind(std::shared_ptr<Object> object, const std::string& methodName)
	{
		_mGetter.BindMethod(object, methodName.c_str());
		_mIsSet = true;
	}

	void Bind(std::shared_ptr<Object> object, std::string&& methodName)
	{
		_mGetter.BindMethod(object, methodName.c_str());
		_mIsSet = true;
	}

private:
	PROPERTY(_mValue)
	mutable T _mValue;
	PROPERTY(_mIsSet)
	bool _mIsSet;
	PROPERTY(_mGetter)
	OnGet _mGetter;
};
