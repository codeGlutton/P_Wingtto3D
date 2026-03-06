#pragma once

#include "Core/App/App.h"
#include "Core/App/Execute.h"

#include "Graphics/Widget/Type/WidgetPath.h"

class Resource;
struct DragDropPayload;

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
	virtual void OnWheelMouse(std::shared_ptr<PointEvent>& event) override;

	virtual void OnChangeFocus(std::shared_ptr<Widget>& focusWidget) override;

	virtual void OnDropFile(const wchar_t* fileFullPath) override;
	
protected:
	void ProcessReplyData(const ReplyData& reply, const std::shared_ptr<KeyEvent>& event);
	void ProcessReplyData(const ReplyData& reply, const std::shared_ptr<PointEvent>& event);

protected:
	virtual void BeginThread();
	virtual void EndThread();

protected:
	virtual void RegisterDefaultResources();
	virtual void UnregisterDefaultResources();

protected:
	void ClearUserData();

public:
	bool HasCaptureEvent() const
	{
		return _mIsCapture;
	}
	bool HasDownEvent() const
	{
		return _mIsMouseDown;
	}
	bool HasDragEvent() const
	{
		return _mIsDrag;
	}

	/* 유저 정보 (엔진이 커지면 분리 필요) */
protected:
	// 이전에 호버 중인 위젯 경로
	WidgetWeakPath _mPreHoverWeakPath;
	// 포커스 중인 위젯 경로
	WidgetWeakPath _mFocusWeakPath;
	// 캡처 중인 위젯 경로 (마우스 버튼 이벤트 독점)
	WidgetWeakPath _mCaptureWeakPath;
	bool _mIsCapture = false;
	// 이전에 클릭한 위젯 경로
	WidgetWeakPath _mMouseDownWeakPath;
	bool _mIsMouseDown = false;
	// 드래그 중인 위젯 경로
	WidgetWeakPath _mDragDropWeakPath;
	bool _mIsDrag = false;

	std::shared_ptr<DragDropPayload> _mDragDropPayload;

	/* 게임 스레드 기본 엔진 리소스 */
protected:
	std::vector<std::shared_ptr<Resource>> _mDefaultResources;
};
