#pragma once

#include "Core/Object.h"
#include "Input/InputInclude.h"
#include "Input/InputValue.h"
#include "Input/InputEvent.h"

#include "Graphics/Widget/Type/WidgetTypeInclude.h"
#include "Graphics/Widget/Slot/WidgetSlot.h"
#include "Graphics/Widget/Type/WidgetGeometry.h"

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
	ReplyData& SetDragPayload(std::shared_ptr<Object> payload)
	{
		mDragPayload = payload;
		return *this;
	}

public:
	std::shared_ptr<Widget> mFocusWidget = nullptr;
	std::shared_ptr<Object> mDragPayload = nullptr;
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

class Widget : public Object
{
	GEN_ABSTRACT_REFLECTION(Widget)

	friend class AppWindowManager;

public:
	void Prepass(const Vec2& layoutMultiplyValue);
	uint32 Paint(OUT WindowRenderElementContainer& drawElements, uint32 accLayerId, const PaintArgs& args, const WidgetGeometry& allottedGeometry, const BoundingAABB2D& parentCulling, const WidgetInheritedColor& inheritedColor);

protected:
	virtual void Update(const WidgetGeometry& screenGeometry, float deltaTime)
	{
	}

public:
	VisibilityType::Flag GetVisibility() const
	{
		return _mVisibility;
	}
	const Transform2D& GetRenderTransform() const
	{
		// TODO : 정렬 방향에 따라서 뒤바뀔 수 있음
		return _mRenderTransform;
	}
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
	virtual void OnBeginFocus()
	{
	}
	virtual void OnEndFocus()
	{
	}

	/* Focus 연관성 있음 */
public:
	// 마우스 제외 모든 키

	virtual ReplyData OnPressChar(const WidgetGeometry& geometry, const std::shared_ptr<const CharEvent>& state)
	{
		return ReplyData::Unhandled();
	}

	virtual ReplyData OnPressKey(const WidgetGeometry& geometry, const std::shared_ptr<const KeyEvent>& state)
	{
		return ReplyData::Unhandled();
	}
	virtual ReplyData OnReleaseKey(const WidgetGeometry& geometry, const std::shared_ptr<const KeyEvent>& state)
	{
		return ReplyData::Unhandled();
	}
	virtual ReplyData OnChangeAnalogValue(const WidgetGeometry& geometry, const std::shared_ptr<const AnalogInputEvent>& state)
	{
		return ReplyData::Unhandled();
	}

	// 마우스

	virtual ReplyData OnPressMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event)
	{
		return ReplyData::Unhandled();
	}
	virtual ReplyData OnReleaseMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event)
	{
		return ReplyData::Unhandled();
	}
	virtual ReplyData OnDoubleClickMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event)
	{
		return ReplyData::Unhandled();
	}

	/* Focus 연관성 없음 */
public:
	virtual void OnEnterMouse(const WidgetGeometry& geometry, const std::shared_ptr<const PointEvent>& event)
	{
	}
	virtual void OnLeaveMouse(const std::shared_ptr<const PointEvent>& event)
	{
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
	PROPERTY(_mRenderTransform)
	Transform2D _mRenderTransform;
	PROPERTY(_mRenderOpacity)
	float _mRenderOpacity = 1.f;

	PROPERTY(_mClipping)
	ClippingType _mClipping;
	PROPERTY(_mCullingExtension)
	Margin _mCullingExtension;

	WidgetRenderCache _mRenderCache;

	/* 가상 함수 호출을 줄이기 위한 데이터 */
protected:
	bool _mNeedToPrepass = true;
	bool _mCanHaveChild = false;
	bool _mHasRelativeChildScale = false;

protected:
	bool _mIsUpdatable = false;
	bool _mIsFocusable = false;

protected:
	VisibilityType::Flag _mVisibility = VisibilityType::None;
};

