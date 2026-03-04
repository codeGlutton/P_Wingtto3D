#pragma once

#include "Core/Object.h"
#include "Input/InputInclude.h"
#include "Input/InputValue.h"
#include "Input/InputEvent.h"

#include "Graphics/Widget/Type/Attribute.h"

#include "Graphics/Widget/Type/WidgetTypeInclude.h"
#include "Graphics/Widget/Slot/WidgetSlot.h"
#include "Graphics/Widget/Type/WidgetGeometry.h"

#define DECALRE_WIDGET_ARGS_BEGIN(type)					\
struct Arguments : WidgetArguments<type> {				\
private:												\
	using RetArgs = type::Arguments;					\
														\
public:

#define DECALRE_WIDGET_ARGS_END() };

#define WIDGET_ARG_CONST_PROPERTY(type, name) type m##name
#define WIDGET_ARG_CONST_FUNCTION(type, name)												\
RetArgs& name(type value)																	\
{																							\
m##name = std::move(value);																	\
return *static_cast<RetArgs*>(this);														\
}

#define DECLARE_WIDGET_ARG_CONST(type, name)												\
WIDGET_ARG_CONST_PROPERTY(type, name);														\
WIDGET_ARG_CONST_FUNCTION(type, name)

#define DECLARE_INIT_WIDGET_ARG_CONST(type, name, value)									\
WIDGET_ARG_CONST_PROPERTY(type, name) = value;												\
WIDGET_ARG_CONST_FUNCTION(type, name)

#define WIDGET_ARG_ATTRIBUTE_PROPERTY(type, name) Attribute<type> m##name
#define WIDGET_ARG_ATTRIBUTE_FUNCTION(type, name)											\
RetArgs& name(Attribute<type> attribute)													\
{																							\
	m##name = std::move(attribute);															\
	return *static_cast<RetArgs*>(this);													\
}																							\
																							\
RetArgs& name##_Bind(std::shared_ptr<Object> object, const std::string& methodName)			\
{																							\
	m##name.Bind(object, methodName);														\
	return *static_cast<RetArgs*>(this);													\
}																							\
RetArgs& name##_Bind(std::shared_ptr<Object> object, std::string&& methodName)				\
{																							\
	m##name.Bind(object, std::move(methodName));											\
	return *static_cast<RetArgs*>(this);													\
}																							\
RetArgs& name##_Bind(Attribute<type>::OnGet::Binder binder)									\
{																							\
	m##name.Bind(binder);																	\
	return *static_cast<RetArgs*>(this);													\
}

#define DECLARE_WIDGET_ARG_ATTRIBUTE(type, name)											\
WIDGET_ARG_ATTRIBUTE_PROPERTY(type, name);													\
WIDGET_ARG_ATTRIBUTE_FUNCTION(type, name)

#define DECLARE_INIT_WIDGET_ARG_ATTRIBUTE(type, name, value)								\
WIDGET_ARG_ATTRIBUTE_PROPERTY(type, name) = value;											\
WIDGET_ARG_ATTRIBUTE_FUNCTION(type, name)

#define DECLARE_WIDGET_ARG_EVENT( type, name )												\
RetArgs& name(const type& delegate)															\
{																							\
	m##name = delegate;																		\
	return *static_cast<RetArgs*>(this);													\
}																							\
RetArgs& name(type&& delegate)																\
{																							\
	m##name = std::move(delegate);															\
	return *static_cast<RetArgs*>(this);													\
}																							\
RetArgs& name(type::Binder binder)															\
{																							\
	m##name.Bind(binder);																	\
	return *static_cast<RetArgs*>(this);													\
}																							\
RetArgs& name##_Bind(std::shared_ptr<Object> object, const std::string& methodName)			\
{																							\
	m##name.BindMethod(object, methodName.c_str());											\
	return *static_cast<RetArgs*>(this);													\
}																							\
RetArgs& name##_Bind(std::shared_ptr<Object> object, std::string&& methodName)				\
{																							\
	m##name.BindMethod(object, std::move(methodName.c_str()));								\
	return *static_cast<RetArgs*>(this);													\
}																							\
RetArgs& name##_Bind(std::shared_ptr<Object> object, const char* methodName)				\
{																							\
	m##name.BindMethod(object, methodName);													\
	return *static_cast<RetArgs*>(this);													\
}																							\
type m##name;

#define DECLARE_WIDGET_ARG_SLOT(type, name)													\
RetArgs& operator+(typename type::Arguments& slotArgs)										\
{																							\
	m##name.push_back(std::move(slotArgs));													\
	return *static_cast<RetArgs*>(this);													\
}																							\
RetArgs& operator+(typename type::Arguments&& slotArgs)										\
{																							\
	m##name.push_back(std::move(slotArgs));													\
	return *static_cast<RetArgs*>(this);													\
}																							\
std::vector<typename type::Arguments> m##name;

class AppWindow;
struct PaintArgs;
struct WindowRenderElementContainer;
struct ArrangedWidget;
struct ArrangedChildren;
struct WidgetInheritedColor;

class Widget;

enum class ReplyType : uint8
{
	Unhandled = 0,
	Handled,
};

struct ReplyData
{
public:
	ReplyData()
	{
	}
	ReplyData(ReplyType type) :
		mType(type)
	{
	}

public:
	static ReplyData Handled();
	static ReplyData Unhandled();

public:
	bool IsHandle() const
	{
		return mType == ReplyType::Handled;
	}

public:
	ReplyData& HoldCapture(std::shared_ptr<Widget> widget)
	{
		mCaptureWidget = widget;
		return *this;
	}
	ReplyData& ReleaseCapture()
	{
		mNeedToReleaseCapture = true;
		mCaptureWidget.reset();
		return *this;
	}
	ReplyData& ChangeFocus(std::shared_ptr<Widget> widget)
	{
		mFocusWidget = widget;
		return *this;
	}
	ReplyData& DetectDrag()
	{
		mNeedToDragEvent = true;
		return *this;
	}
	ReplyData& SetDragPayload(std::shared_ptr<DragDropPayload> payload)
	{
		mDragPayload = payload;
		return *this;
	}

public:
	std::shared_ptr<Widget> mCaptureWidget = nullptr;
	std::shared_ptr<Widget> mFocusWidget = nullptr;
	std::shared_ptr<DragDropPayload> mDragPayload = nullptr;
	bool mNeedToReleaseCapture = false;
	bool mNeedToDragEvent = false;
	ReplyType mType;
};

struct WidgetRenderCache
{
	std::weak_ptr<AppWindow> mRootWindow;
	WidgetGeometry mRenderGeometry;
	WidgetGeometry mScreenGeometry;
	uint32 mLayerId;
};

DECLARE_DYNAMIC_DELEGATE_RET_2_PARAMS(OnCallKeyEvent, ReplyData, const WidgetGeometry*, const std::shared_ptr<const KeyEvent>*);
DECLARE_DYNAMIC_DELEGATE_RET_2_PARAMS(OnCallAnalogInputEvent, ReplyData, const WidgetGeometry*, const std::shared_ptr<const AnalogInputEvent>*);
DECLARE_DYNAMIC_DELEGATE_RET_2_PARAMS(OnCallPointEvent, ReplyData, const WidgetGeometry*, const std::shared_ptr<const PointEvent>*);
DECLARE_DYNAMIC_DELEGATE_2_PARAMS(OnCallEnterPointEvent, const WidgetGeometry*, const std::shared_ptr<const PointEvent>*);
DECLARE_DYNAMIC_DELEGATE_1_PARAM(OnCallLeavePointEvent, const std::shared_ptr<const PointEvent>*);

struct WidgetArgumentsBase abstract
{
	WIDGET_ARG_ATTRIBUTE_PROPERTY(VisibilityType::Flag, Visibility);
	WIDGET_ARG_ATTRIBUTE_PROPERTY(Transform2D, RenderTransform) = Transform2D();
	WIDGET_ARG_ATTRIBUTE_PROPERTY(Vec2, RenderTransformPivot) = Vec2(0.5f);

	WIDGET_ARG_CONST_PROPERTY(ClippingType, Clipping);
	WIDGET_ARG_CONST_PROPERTY(float, RenderOpacity) = 1.f;
};

template<typename T>
struct WidgetArguments abstract : public WidgetArgumentsBase
{
private:
	using Arguments = T::Arguments;
	using RetArgs = T::Arguments;

public:
	WIDGET_ARG_ATTRIBUTE_FUNCTION(VisibilityType::Flag, Visibility);
	WIDGET_ARG_ATTRIBUTE_FUNCTION(Transform2D, RenderTransform);
	WIDGET_ARG_ATTRIBUTE_FUNCTION(Vec2, RenderTransformPivot);

	WIDGET_ARG_CONST_FUNCTION(ClippingType, Clipping);
	WIDGET_ARG_CONST_FUNCTION(float, RenderOpacity);
};

class Widget : public Object
{
	GEN_ABSTRACT_REFLECTION(Widget)

	friend class AppWindowManager;

public:
	void OnConstruct(const WidgetArgumentsBase& args);

public:
	void Prepass(const Vec2& layoutMultiplyValue);
	uint32 Paint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& parentCulling, const WidgetInheritedColor& inheritedColor);

protected:
	virtual void Update(const WidgetGeometry& screenGeometry, float deltaTime)
	{
	}

public:
	void AssignParent(std::shared_ptr<Widget> parent)
	{
		ASSERT_MSG(parent != nullptr, "Null parent isn't allowed without root widget");

		_mParent = parent;
		_mRenderCache.mRootWindow = parent->_mRenderCache.mRootWindow;
	}
	void ResetParent()
	{
		_mParent.reset();
		_mRenderCache.mRootWindow.reset();
	}

	bool IsLayoutDirty() const
	{
		return _mNeedToPrepass;
	}
	void MarkLayoutDirty(bool isWindowDirty = true) const;

	VisibilityType::Flag GetVisibility() const
	{
		return _mVisibility.GetValue(this);
	}
	void SetVisibility(const Attribute<VisibilityType::Flag>& visibility);

	const Vec2& GetRenderTransformPivot() const
	{
		// TODO : 정렬 방향에 따라서 뒤바뀔 수 있음
		return _mRenderTransformPivot.GetValue(this);
	}
	void SetRenderTransformPivot(const Attribute<Vec2>& renderTransform);

	const Transform2D& GetRenderTransform() const
	{
		// TODO : 정렬 방향에 따라서 뒤바뀔 수 있음
		return _mRenderTransform.GetValue(this);
	}
	void SetRenderTransform(const Attribute<Transform2D>& renderTransform);

	const WidgetGeometry& GetRenderGeometry() const
	{
		return _mRenderCache.mRenderGeometry;
	}
	const WidgetGeometry& GetScreenGeometry() const
	{
		return _mRenderCache.mScreenGeometry;
	}
	std::shared_ptr<AppWindow> GetRootWindow() const
	{
		return _mRenderCache.mRootWindow.lock();
	}

public:
	virtual WidgetSlotContainer& GetChildren() = 0;
	virtual const WidgetSlotContainer& GetChildren() const = 0;
	virtual Vec2 GetChildRelativeScale(int32 index, const Vec2& layoutMultiplyValue) const
	{
		return Vec2(1.f, 1.f);
	}

public:
	const std::shared_ptr<Widget> GetParent() const
	{
		return _mParent.lock();
	}
	const Vec2& GetDesiredSize() const
	{
		return _mDesiredSize;
	}

public:
	virtual const std::wstring& GetDisplayName() const
	{
		return GetName();
	}

public:
	virtual void OnBeginFocus()
	{
	}
	virtual void OnEndFocus()
	{
	}

	/* Focus 연관성 있음 */
public:
	// 마우스 제외 모든 키

	virtual ReplyData OnPressChar(const WidgetGeometry& geometry, const std::shared_ptr<const CharEvent>& event)
	{
		return ReplyData::Unhandled();
	}

	virtual ReplyData OnPressKey(const WidgetGeometry& geometry, const std::shared_ptr<const KeyEvent>& event);
	virtual ReplyData OnReleaseKey(const WidgetGeometry& geometry, const std::shared_ptr<const KeyEvent>& event)
	{
		return ReplyData::Unhandled();
	}
	virtual ReplyData OnChangeAnalogValue(const WidgetGeometry& geometry, const std::shared_ptr<const AnalogInputEvent>& event);

	// 마우스

	virtual ReplyData OnPressMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event);
	virtual ReplyData OnReleaseMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event);
	virtual ReplyData OnDoubleClickMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event);
	virtual ReplyData OnMoveMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event);

	/* Focus 연관성 없음 */
public:
	virtual void OnEnterMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event);
	virtual void OnLeaveMouse(const std::shared_ptr<const PointEvent>& event);
	virtual ReplyData OnDragMouse(const WidgetGeometry& geometry, const std::shared_ptr<const DragDropEvent>& event)
	{
		return ReplyData::Unhandled();
	}
	virtual void OnEnterDragMouse(const WidgetGeometry& geometry, const std::shared_ptr<const DragDropEvent>& event)
	{
	}
	virtual void OnLeaveDragMouse(const std::shared_ptr<const DragDropEvent>& event)
	{
	}

	virtual ReplyData OnDetectDrag(const WidgetGeometry& geometry, const std::shared_ptr<const DragDropEvent>& event)
	{
		return ReplyData::Unhandled();
	}
	virtual ReplyData OnDrop(const WidgetGeometry& geometry, const std::shared_ptr<const DragDropEvent>& event)
	{
		return ReplyData::Unhandled();
	}

protected:
	bool IsChildWidgetCulling(const BoundingAABB2D& cullingRect, const ArrangedWidget& child) const;
	virtual void OnArrangeChildren(const WidgetGeometry& allottedGeometry, ArrangedChildren& children) const = 0;

private:
	// 조건 없이 즉시 프리패스 검사
	void Prepass_Internal(const Vec2& layoutMultiplyValue);
	// 자식 위젯들에	프리패스 검사
	void Prepass_Children(const Vec2& layoutMultiplyValue);

private:
	BoundingAABB2D CalculateClipping(const WidgetGeometry& allottedGeometry, const BoundingAABB2D& cullingRect) const;
	virtual Vec2 ComputeDesireSize(const Vec2& layoutMultiplyValue) = 0;
	virtual uint32 OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor) = 0;

protected:
	PROPERTY(_mParent)
	std::weak_ptr<Widget> _mParent;

	/* Prepass 연관 데이터 */
protected:
	Vec2 _mDesiredSize;

	/* Paint 연관 데이터 */
protected:
	PROPERTY(_mRenderTransformPivot)
	LayoutAttribute<Vec2> _mRenderTransformPivot;
	PROPERTY(_mRenderTransform)
	LayoutAttribute<Transform2D> _mRenderTransform;
	PROPERTY(_mRenderOpacity)
	float _mRenderOpacity = 1.f;

	PROPERTY(_mClipping)
	ClippingType _mClipping;
	PROPERTY(_mCullingExtension)
	Margin _mCullingExtension;

	WidgetRenderCache _mRenderCache;

	/* 입력 연관 대리자 */
public:
	OnCallKeyEvent mOnPressKey;
	OnCallAnalogInputEvent mOnChangeAnalogValue;

	OnCallEnterPointEvent mOnEnterMouse;
	OnCallLeavePointEvent mOnLeaveMouse;

	OnCallPointEvent mOnPressMouse;
	OnCallPointEvent mOnReleaseMouse;
	OnCallPointEvent mOnMoveMouse;
	OnCallPointEvent mOnDoubleClickMouse;

	/* 가상 함수 호출을 줄이기 위한 데이터 */
protected:
	bool _mCanHaveChild = false;
	bool _mHasRelativeChildScale = false;

protected:
	bool _mIsUpdatable = false;
	bool _mIsFocusable = false;

	PROPERTY(_mVisibility)
	LayoutAttribute<VisibilityType::Flag> _mVisibility;

private:
	mutable bool _mNeedToPrepass = true;
};

class NullWidget : public Widget
{
	GEN_REFLECTION(NullWidget)

	/* 생성 시, 초기화 인자 구조체 정의 */
public:
	DECALRE_WIDGET_ARGS_BEGIN(NullWidget)
	DECALRE_WIDGET_ARGS_END()

public:
	void OnConstruct(const Arguments& args);

public:
	virtual uint32 OnPaint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& currentCulling, const WidgetInheritedColor& contentInheritedColor) override
	{
		return accLayerId;
	}

	virtual WidgetSlotContainer& GetChildren()
	{
		return EmptySlotContainer::GetInst();
	}
	virtual const WidgetSlotContainer& GetChildren() const
	{
		return EmptySlotContainer::GetInst();
	}

	virtual void OnArrangeChildren(const WidgetGeometry& allottedGeometry, ArrangedChildren& children) const override
	{
	}

protected:
	virtual Vec2 ComputeDesireSize(const Vec2& layoutMultiplyValue) override
	{
		return Vec2(0.f);
	}
};

