#pragma once

#include "Input/InputInclude.h"
#include "Input/InputValue.h"

class Object;
class WidgetPath;

struct InputEvent
{
public:
	virtual ~InputEvent() = default;

public:
	virtual bool IsPointEvent() const
	{
		return false;
	}
	virtual bool IsKeyEvent() const
	{
		return false;
	}

public:
	WidgetPath* mPath;
	bool mUseCtrl = false;
	bool mUseAlt = false;
	bool mUseShift = false;

	bool mIsRepeat = false;
};

struct KeyEvent : public InputEvent
{
public:
	virtual bool IsKeyEvent() const override
	{
		return true;
	}

public:
	KeyInfo mKeyInfo;
};

struct AnalogInputEvent : public KeyEvent
{
	InputValue mScaleValue;
};

struct CharEvent : public InputEvent
{
	wchar_t mChar;
};

struct PointEvent : public InputEvent
{
public:
	virtual bool IsPointEvent() const override
	{
		return true;
	}

public:
	KeyInfo mKeyInfo;
	POINT mPreMouseScreenPos;
	POINT mCurrentMouseScreenPos;
	int32 mWheelDelta;
	HWND mHWnd;
};

struct DragDropPayload
{
	GEN_STRUCT_REFLECTION(DragDropPayload)

public:
	virtual ~DragDropPayload() = default;
};

struct DragDropEvent : public PointEvent
{
public:
	DragDropEvent() = default;
	DragDropEvent(const PointEvent& event) :
		PointEvent(event)
	{
	}
	DragDropEvent(PointEvent&& event) :
		PointEvent(event)
	{
	}

public:
	std::shared_ptr<DragDropPayload> mPayload;
};
