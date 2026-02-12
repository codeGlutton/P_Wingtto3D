#pragma once

#include "Core/App/App.h"
#include "Core/App/Execute.h"

#include "Graphics/Widget/Type/WidgetPath.h"

class AppModeBase : public IExecute
{
public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void End() override;

public:
	virtual void OnPressKey(std::shared_ptr<KeyEvent>& event) override;
	virtual void OnReleaseKey(std::shared_ptr<KeyEvent>& event) override;
	virtual void OnChangeAnalogValue(std::shared_ptr<AnalogInputEvent>& event) override;

	virtual void OnPressChar(std::shared_ptr<CharEvent>& event) override;

	virtual void OnPressMouse(std::shared_ptr<AppWindow> target, std::shared_ptr<PointEvent>& event) override;
	virtual void OnReleaseMouse(std::shared_ptr<PointEvent>& event) override;
	virtual void OnDoubleClickMouse(std::shared_ptr<AppWindow> target, std::shared_ptr<PointEvent>& event) override;

	virtual void OnMoveMouse(std::shared_ptr<PointEvent>& event) override;
	
protected:
	void ProcessReplyData(const ReplyData& reply, const std::shared_ptr<KeyEvent>& event);
	void ProcessReplyData(const ReplyData& reply, const std::shared_ptr<PointEvent>& event);

protected:
	virtual void BeginThread();
	virtual void EndThread();

protected:
	void ClearUserData();

public:
	bool HasDragEvent() const
	{
		return _mMouseDownWeakPath.IsValid();
	}
	bool HasDownEvent() const
	{
		return _mDragDropWeakPath.IsValid();
	}

protected:
	std::weak_ptr<Widget> _mPreHoverWidget;

	WidgetWeakPath _mMouseDownWeakPath;
	WidgetWeakPath _mDragDropWeakPath;
	std::shared_ptr<Object> _mDragDropPayload;
};
