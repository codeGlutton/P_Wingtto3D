#include "pch.h"
#include "AppModeBase.h"

#include "ManagerMaximum.h"

#include "Utils/Thread/Thread.h"
#include "Utils/Thread/MainThread.h"

#include "Core/Resource/Resource.h"
#include "Core/Resource/Material.h"

void AppModeBase::Init()
{
	/* 싱글톤 초기화 */

	{
		// 순서 무관계

		PATH_MANAGER->Init();
		INPUT_MANAGER->Init();
	}

	{
		// 기반

		PACKAGE_MANAGER->Init();
		TIME_MANAGER->Init();
		OBJECT_MANAGER->Init();
	}

	{
		// 패키징 로드 요구

		APP_WIN_MANAGER->Init();
		RESOURCE_MANAGER->Init();
	}

	{
		// 최후 시작 요구

		DX_GRAPHICS->Init();
		RENDER_MANAGER->Init();
		THREAD_MANAGER->Init();
	}

	BeginThread();
}

void AppModeBase::Update()
{
	INPUT_MANAGER->Update();
	THREAD_MANAGER->DoGameJob();
	TIME_MANAGER->Update();

	// 위젯 프리패스, 페이팅, 업데이트
	// 랜더링 명령 송신
	APP_WIN_MANAGER->Update();
}

void AppModeBase::End()
{
	ClearUserData();
	EndThread();

	{
		// 최후 시작 요구

		THREAD_MANAGER->Destroy();
		RENDER_MANAGER->Destroy();
		DX_GRAPHICS->Destroy();
	}

	{
		// 패키징 로드 요구

		RESOURCE_MANAGER->Destroy();
		APP_WIN_MANAGER->Destroy();
	}

	{
		// 기반

		OBJECT_MANAGER->Destroy();
		TIME_MANAGER->Destroy();
		PACKAGE_MANAGER->Destroy();
	}

	{
		// 순서 무관계

		INPUT_MANAGER->Destroy();
		PATH_MANAGER->Destroy();
	}
}

void AppModeBase::OnPressKey(std::shared_ptr<KeyEvent>& event)
{
	ReplyData reply = ReplyData::Unhandled();
	if (event->mPath == nullptr)
	{
		return;
	}

	// 버블 검사
	const auto iterEnd = event->mPath->mWidgets.rend();
	for (auto iter = event->mPath->mWidgets.rbegin(); iter != iterEnd; ++iter)
	{
		std::shared_ptr<Widget> curWidget = iter->mWidget;
		if (curWidget != nullptr && curWidget->IsValid() == true)
		{
			reply = curWidget->OnPressKey(iter->mGeometry, event);
			if (reply.IsHandle() == true)
			{
				ProcessReplyData(reply, event);
				break;
			}
		}
	}
}

void AppModeBase::OnReleaseKey(std::shared_ptr<KeyEvent>& event)
{
	ReplyData reply = ReplyData::Unhandled();
	if (event->mPath == nullptr)
	{
		return;
	}

	// 버블 검사
	const auto iterEnd = event->mPath->mWidgets.end();
	for (auto iter = event->mPath->mWidgets.begin(); iter != iterEnd; ++iter)
	{
		std::shared_ptr<Widget> curWidget = iter->mWidget;
		if (curWidget != nullptr && curWidget->IsValid() == true)
		{
			reply = curWidget->OnReleaseKey(iter->mGeometry, event);
			if (reply.IsHandle() == true)
			{
				ProcessReplyData(reply, event);
				break;
			}
		}
	}
}

void AppModeBase::OnChangeAnalogValue(std::shared_ptr<AnalogInputEvent>& event)
{
	ReplyData reply = ReplyData::Unhandled();
	if (event->mPath == nullptr)
	{
		return;
	}

	// 버블 검사
	const auto iterEnd = event->mPath->mWidgets.end();
	for (auto iter = event->mPath->mWidgets.begin(); iter != iterEnd; ++iter)
	{
		std::shared_ptr<Widget> curWidget = iter->mWidget;
		if (curWidget != nullptr && curWidget->IsValid() == true)
		{
			reply = curWidget->OnChangeAnalogValue(iter->mGeometry, event);
			if (reply.IsHandle() == true)
			{
				ProcessReplyData(reply, event);
				break;
			}
		}
	}
}

void AppModeBase::OnPressChar(std::shared_ptr<CharEvent>& event)
{
	ReplyData reply = ReplyData::Unhandled();
	if (event->mPath == nullptr)
	{
		return;
	}

	// 버블 검사
	const auto iterEnd = event->mPath->mWidgets.end();
	for (auto iter = event->mPath->mWidgets.begin(); iter != iterEnd; ++iter)
	{
		std::shared_ptr<Widget> curWidget = iter->mWidget;
		if (curWidget != nullptr && curWidget->IsValid() == true)
		{
			reply = curWidget->OnPressChar(iter->mGeometry, event);
			if (reply.IsHandle() == true)
			{
				break;
			}
		}
	}
}

void AppModeBase::OnPressMouse(std::shared_ptr<AppWindow> target, std::shared_ptr<PointEvent>& event)
{
	ReplyData reply = ReplyData::Unhandled();

	// 경로 찾기
	WidgetPath path;
	target->GetWidgetPathUnderScreenPos(event->mCurrentMouseScreenPos, path);

	event->mPath = &path;

	if (HasDownEvent() == false)
	{
		_mMouseDownWeakPath = path;

		// 버블 검사
		const auto iterEnd = event->mPath->mWidgets.end();
		for (auto iter = event->mPath->mWidgets.begin(); iter != iterEnd; ++iter)
		{
			std::shared_ptr<Widget> curWidget = iter->mWidget;
			if (curWidget != nullptr && curWidget->IsValid() == true)
			{
				reply = curWidget->OnPressMouse(iter->mGeometry, event);
				if (reply.IsHandle() == true)
				{
					ProcessReplyData(reply, event);
					break;
				}
			}
		}
	}
}

void AppModeBase::OnReleaseMouse(std::shared_ptr<PointEvent>& event)
{
	ReplyData reply = ReplyData::Unhandled();

	if (HasDownEvent() == false)
	{
		_mMouseDownWeakPath.Clear();
		_mDragDropWeakPath.Clear();

		return;
	}

	// 릴리즈 버블 검사
	WidgetPath path = _mMouseDownWeakPath.Lock();
	const auto iterEnd = path.mWidgets.end();
	for (auto iter = path.mWidgets.begin(); iter != iterEnd; ++iter)
	{
		std::shared_ptr<Widget> curWidget = iter->mWidget;
		if (curWidget != nullptr && curWidget->IsValid() == true)
		{
			reply = curWidget->OnReleaseMouse(iter->mGeometry, event);
			if (reply.IsHandle() == true)
			{
				ProcessReplyData(reply, event);
				break;
			}
		}
	}

	if (HasDragEvent() == true)
	{
		// 경로 찾기
		std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(event->mHWnd);
		if (window == nullptr || window->IsValid() == false || window->IsContainScreenPos(event->mCurrentMouseScreenPos) == false)
		{
			HWND hWnd = ::WindowFromPoint(event->mCurrentMouseScreenPos);
			window = APP_WIN_MANAGER->GetAppWindow(hWnd);
		}

		if (window != nullptr || window->IsValid() == true)
		{
			WidgetPath path;
			window->GetWidgetPathUnderScreenPos(event->mCurrentMouseScreenPos, path);
			std::shared_ptr<DragDropEvent> dragDropEvent = ObjectPool<DragDropEvent>::MakeShared(*event.get());
			dragDropEvent->mPath = &path;
			dragDropEvent->mPayload = _mDragDropPayload;

			// 드랍 버블 검사
			const auto iterEnd = dragDropEvent->mPath->mWidgets.end();
			for (auto iter = dragDropEvent->mPath->mWidgets.begin(); iter != iterEnd; ++iter)
			{
				std::shared_ptr<Widget> curWidget = iter->mWidget;
				if (curWidget != nullptr && curWidget->IsValid() == true)
				{
					reply = curWidget->OnDrop(iter->mGeometry, dragDropEvent);
					if (reply.IsHandle() == true)
					{
						ProcessReplyData(reply, dragDropEvent);
						break;
					}
				}
			}
		}
	}

	_mMouseDownWeakPath.Clear();
	_mDragDropWeakPath.Clear();
}

void AppModeBase::OnDoubleClickMouse(std::shared_ptr<AppWindow> target, std::shared_ptr<PointEvent>& event)
{
	ReplyData reply = ReplyData::Unhandled();

	// 경로 찾기
	WidgetPath path;
	target->GetWidgetPathUnderScreenPos(event->mCurrentMouseScreenPos, path);

	event->mPath = &path;

	// 버블 검사
	const auto iterEnd = event->mPath->mWidgets.end();
	for (auto iter = event->mPath->mWidgets.begin(); iter != iterEnd; ++iter)
	{
		std::shared_ptr<Widget> curWidget = iter->mWidget;
		if (curWidget != nullptr && curWidget->IsValid() == true)
		{
			reply = curWidget->OnDoubleClickMouse(iter->mGeometry, event);
			if (reply.IsHandle() == true)
			{
				ProcessReplyData(reply, event);
				break;
			}
		}
	}
}

void AppModeBase::OnMoveMouse(std::shared_ptr<PointEvent>& event)
{
	// 경로 찾기
	std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(event->mHWnd);
	if (window == nullptr || window->IsValid() == false || window->IsContainScreenPos(event->mCurrentMouseScreenPos) == false)
	{
		HWND hWnd = ::WindowFromPoint(event->mCurrentMouseScreenPos);
		window = APP_WIN_MANAGER->GetAppWindow(hWnd);
	}

	if (window != nullptr && window->IsValid() == true)
	{
		std::shared_ptr<Widget> preWidget = _mPreHoverWidget.lock();
		ArrangedWidget curArrangedWidget;
		window->GetWidgetUnderScreenPos(event->mCurrentMouseScreenPos, curArrangedWidget);

		if (preWidget != curArrangedWidget.mWidget)
		{
			// 드래그 시
			if (HasDragEvent() == true)
			{
				std::shared_ptr<DragDropEvent> dragDropEvent = ObjectPool<DragDropEvent>::MakeShared(*event.get());
				dragDropEvent->mPayload = _mDragDropPayload;

				if (preWidget != nullptr)
				{
					preWidget->OnLeaveDragMouse(dragDropEvent);
				}
				_mPreHoverWidget = curArrangedWidget.mWidget;
				if (curArrangedWidget.mWidget != nullptr)
				{
					curArrangedWidget.mWidget->OnEnterDragMouse(curArrangedWidget.mGeometry, dragDropEvent);
				}
			}
			// 드래그 아닐 시
			else
			{
				if (preWidget != nullptr)
				{
					preWidget->OnLeaveMouse(event);
				}
				_mPreHoverWidget = curArrangedWidget.mWidget;
				if (curArrangedWidget.mWidget != nullptr)
				{
					curArrangedWidget.mWidget->OnEnterMouse(curArrangedWidget.mGeometry, event);
				}
			}
		}
	}
	else
	{
		_mPreHoverWidget.reset();
	}
}

void AppModeBase::ProcessReplyData(const ReplyData& reply, const std::shared_ptr<KeyEvent>& event)
{
	if (reply.mFocusWidget != nullptr)
	{
		APP_WIN_MANAGER->NotifyToChangeFocus(reply.mFocusWidget);
	}
}

void AppModeBase::ProcessReplyData(const ReplyData& reply, const std::shared_ptr<PointEvent>& event)
{
	if (reply.mNeedToDragEvent == true && HasDragEvent() == false && HasDownEvent() == true)
	{
		ReplyData reply = ReplyData::Unhandled();

		std::shared_ptr<DragDropEvent> dragDropEvent = ObjectPool<DragDropEvent>::MakeShared(*event.get());
		dragDropEvent->mPayload = reply.mDragPayload;

		// 버블 검사
		const auto iterEnd = event->mPath->mWidgets.end();
		for (auto iter = event->mPath->mWidgets.begin(); iter != iterEnd; ++iter)
		{
			std::shared_ptr<Widget> curWidget = iter->mWidget;
			if (curWidget != nullptr && curWidget->IsValid() == true)
			{
				reply = curWidget->OnDetectDrag(iter->mGeometry, dragDropEvent);
				if (reply.IsHandle() == true)
				{
					break;
				}
			}
		}

		_mDragDropWeakPath = *event->mPath;
		_mDragDropPayload = reply.mDragPayload;
	}

	if (reply.mFocusWidget != nullptr)
	{
		APP_WIN_MANAGER->NotifyToChangeFocus(reply.mFocusWidget);
	}
}

void AppModeBase::BeginThread()
{
	/* 스레드 생성 */

	const uint32 coreCount = std::thread::hardware_concurrency();
	const uint32 reservedCount = MainThreadType::Count;

	// 글로벌 워커 스레드 실행
	const uint32 workerThreadCount = (coreCount > reservedCount) ? (coreCount - reservedCount) : 1u;
	for (uint32 i = 0; i < workerThreadCount; ++i)
	{
		THREAD_MANAGER->Launch(std::make_shared<GlobalWorkerThread>());
	}

	// 메인 스레드 실행
	RegisterDefaultResources();
	std::array<std::shared_ptr<MainThread>, MainThreadType::Count - 1> mainThreads = {
		std::make_shared<RenderThread>()
	};
	THREAD_MANAGER->LaunchMainThreads(mainThreads);
}

void AppModeBase::EndThread()
{
	THREAD_MANAGER->Join();
	UnregisterDefaultResources();
}

void AppModeBase::RegisterDefaultResources()
{
	std::wstring packagePath = PATH_MANAGER->GetEngineResourceFolderName();

	/* 메테리얼 */

	// UI
	{
		std::shared_ptr<Material> matUI = RESOURCE_MANAGER->LoadOrGetResource<Material>(packagePath + L"\\M_UI");
		matUI->GetProxy();
		_mDefaultResources.push_back(matUI);

		// 게임 빌드 옵션에서는 엔진 리소스가 없는 경우, 에러 발생
		// 에디터 빌드 옵션에서는 Editor 참고
	}
}

void AppModeBase::UnregisterDefaultResources()
{
	_mDefaultResources.clear();
}

void AppModeBase::ClearUserData()
{
	_mPreHoverWidget.reset();
	_mDragDropPayload.reset();
}


