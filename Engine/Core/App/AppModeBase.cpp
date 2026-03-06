#include "pch.h"
#include "AppModeBase.h"

#include "ManagerMaximum.h"

#include "Utils/Thread/Thread.h"
#include "Utils/Thread/MainThread.h"

#include "Core/Resource/Resource.h"
#include "Core/Resource/Material.h"

#include "Manager/PackageManager.h"
#include "Core/Resource/Package/Package.h"

void AppModeBase::Init()
{
	/* 싱글톤 초기화 */

	{
		// 순서 무관계

		DX_GRAPHICS->Init();
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

		RESOURCE_MANAGER->Init();
		RegisterDefaultResources();
		WIDGET_STYLE_MANAGER->Init();
		APP_WIN_MANAGER->Init();
	}

	{
		// 최후 시작 요구

		RENDER_MANAGER->Init();
		THREAD_MANAGER->Init();
		BeginThread();
	}
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
	{
		// 최후 시작 요구

		EndThread();
		THREAD_MANAGER->Destroy();
		RENDER_MANAGER->Destroy();
	}

	{
		// 패키징 로드 요구

		APP_WIN_MANAGER->Destroy();
		WIDGET_STYLE_MANAGER->Destroy();
		RESOURCE_MANAGER->Destroy();
	}

	{
		// 기반

		OBJECT_MANAGER->Destroy();
		TIME_MANAGER->Destroy();
		UnregisterDefaultResources();
		PACKAGE_MANAGER->Destroy();
	}

	{
		// 순서 무관계

		INPUT_MANAGER->Destroy();
		PATH_MANAGER->Destroy();
		DX_GRAPHICS->Destroy();
	}
}

void AppModeBase::OnPressKey(std::shared_ptr<KeyEvent>& event)
{
	ReplyData reply = ReplyData::Unhandled();

	// 버블 검사
	WidgetPath path = _mFocusWeakPath.Lock();
	event->mPath = &path;

	const auto iterEnd = path.mWidgets.end();
	for (auto iter = path.mWidgets.begin(); iter != iterEnd; ++iter)
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

	// 버블 검사
	WidgetPath path = _mFocusWeakPath.Lock();
	event->mPath = &path;

	const auto iterEnd = path.mWidgets.end();
	for (auto iter = path.mWidgets.begin(); iter != iterEnd; ++iter)
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

	// 버블 검사
	WidgetPath path = _mFocusWeakPath.Lock();
	event->mPath = &path;

	const auto iterEnd = path.mWidgets.end();
	for (auto iter = path.mWidgets.begin(); iter != iterEnd; ++iter)
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

	// 버블 검사
	WidgetPath path = _mFocusWeakPath.Lock();
	event->mPath = &path;

	const auto iterEnd = path.mWidgets.end();
	for (auto iter = path.mWidgets.begin(); iter != iterEnd; ++iter)
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
	event->mPath = &path;

	if (HasDownEvent() == false)
	{
		_mIsMouseDown = true;

		if (HasCaptureEvent() == true)
		{
			// 버블 검사
			path = _mCaptureWeakPath.Lock();
			_mMouseDownWeakPath = _mCaptureWeakPath;

			const auto iterEnd = path.mWidgets.end();
			for (auto iter = path.mWidgets.begin(); iter != iterEnd; ++iter)
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
		else
		{
			// 버블 검사
			target->GetWidgetPathUnderScreenPos(event->mCurrentMouseScreenPos, path);
			_mMouseDownWeakPath = path;

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
}

void AppModeBase::OnReleaseMouse(std::shared_ptr<PointEvent>& event)
{
	ReplyData reply = ReplyData::Unhandled();
	
	// 경로 찾기
	WidgetPath path;
	event->mPath = &path;

	if (HasDownEvent() == true)
	{
		_mIsMouseDown = false;

		if (HasDragEvent() == true)
		{
			_mIsDrag = false;

			// 경로 찾기
			std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(event->mHWnd);
			if (window == nullptr || window->IsValid() == false || window->IsContainScreenPos(event->mCurrentMouseScreenPos) == false)
			{
				HWND hWnd = ::WindowFromPoint(event->mCurrentMouseScreenPos);
				window = APP_WIN_MANAGER->GetAppWindow(hWnd);
			}

			if (window != nullptr || window->IsValid() == true)
			{
				WidgetPath dropPath;
				window->GetWidgetPathUnderScreenPos(event->mCurrentMouseScreenPos, dropPath);
				std::shared_ptr<DragDropEvent> dragDropEvent = ObjectPool<DragDropEvent>::MakeShared(*event.get());
				dragDropEvent->mPath = &dropPath;
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

		if (HasCaptureEvent() == true)
		{
			// 릴리즈 버블 검사
			path = _mCaptureWeakPath.Lock();
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
		}
		else
		{
			// 릴리즈 버블 검사
			path = _mMouseDownWeakPath.Lock();
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
		}
	}

	_mMouseDownWeakPath.Clear();
	_mDragDropWeakPath.Clear();
	_mDragDropPayload.reset();
}

void AppModeBase::OnDoubleClickMouse(std::shared_ptr<AppWindow> target, std::shared_ptr<PointEvent>& event)
{
	ReplyData reply = ReplyData::Unhandled();

	// 경로 찾기
	WidgetPath path;
	event->mPath = &path;

	if (HasDownEvent() == true)
	{
		if (HasCaptureEvent() == true)
		{
			// 버블 검사
			path = _mCaptureWeakPath.Lock();
			const auto iterEnd = path.mWidgets.end();
			for (auto iter = path.mWidgets.begin(); iter != iterEnd; ++iter)
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
		else
		{
			// 버블 검사
			path = _mMouseDownWeakPath.Lock();
			const auto iterEnd = path.mWidgets.end();
			for (auto iter = path.mWidgets.begin(); iter != iterEnd; ++iter)
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
	}
}

void AppModeBase::OnMoveMouse(std::shared_ptr<PointEvent>& event)
{
	ReplyData reply = ReplyData::Unhandled();

	// 경로 찾기
	std::shared_ptr<AppWindow> window = APP_WIN_MANAGER->GetAppWindow(event->mHWnd);
	if (window->IsContainScreenPos(event->mCurrentMouseScreenPos) == false)
	{
		HWND hWnd = ::WindowFromPoint(event->mCurrentMouseScreenPos);
		window = APP_WIN_MANAGER->GetAppWindow(hWnd);
	}

	WidgetPath hoverPath;
	event->mPath = &hoverPath;

	// 마우스 움직임
	if (window != nullptr && window->IsValid() == true)
	{
		// 버블 검사
		window->GetWidgetPathUnderScreenPos(event->mCurrentMouseScreenPos, hoverPath);

		if (HasDragEvent() == true)
		{
			std::shared_ptr<DragDropEvent> dragDropEvent = ObjectPool<DragDropEvent>::MakeShared(*event.get());
			dragDropEvent->mPayload = _mDragDropPayload;

			const auto iterEnd = dragDropEvent->mPath->mWidgets.end();
			for (auto iter = dragDropEvent->mPath->mWidgets.begin(); iter != iterEnd; ++iter)
			{
				std::shared_ptr<Widget> curWidget = iter->mWidget;
				if (curWidget != nullptr && curWidget->IsValid() == true)
				{
					reply = curWidget->OnDragMouse(iter->mGeometry, dragDropEvent);
					if (reply.IsHandle() == true)
					{
						ProcessReplyData(reply, dragDropEvent);
						break;
					}
				}
			}
		}
		else
		{
			const auto iterEnd = event->mPath->mWidgets.end();
			for (auto iter = event->mPath->mWidgets.begin(); iter != iterEnd; ++iter)
			{
				std::shared_ptr<Widget> curWidget = iter->mWidget;
				if (curWidget != nullptr && curWidget->IsValid() == true)
				{
					reply = curWidget->OnMoveMouse(iter->mGeometry, event);
					if (reply.IsHandle() == true)
					{
						ProcessReplyData(reply, event);
						break;
					}
				}
			}
		}
	}

	// 이전과 비교해서 공통 경로 탐색
	WidgetPath preHoverPath = _mPreHoverWeakPath.Lock();
	const int32 checkSize = std::min<int32>(static_cast<int32>(preHoverPath.mWidgets.size()), static_cast<int32>(hoverPath.mWidgets.size()));
	int32 changedIndexStart = 0;
	for (changedIndexStart = 0; changedIndexStart < checkSize; ++changedIndexStart)
	{
		if (preHoverPath.mWidgets[changedIndexStart].mWidget != hoverPath.mWidgets[changedIndexStart].mWidget)
		{
			break;
		}
	}

	// 마우스 탈출 이벤트
	for (int32 i = static_cast<int32>(preHoverPath.mWidgets.size()) - 1; i >= changedIndexStart; --i)
	{
		// 드래그 시
		if (HasDragEvent() == true)
		{
			std::shared_ptr<DragDropEvent> dragDropEvent = ObjectPool<DragDropEvent>::MakeShared(*event.get());
			dragDropEvent->mPayload = _mDragDropPayload;

			preHoverPath.mWidgets[i].mWidget->OnLeaveDragMouse(dragDropEvent);
		}
		// 드래그 아닐 시
		else
		{
			preHoverPath.mWidgets[i].mWidget->OnLeaveMouse(event);
		}
	}
	// 마우스 진입 이벤트
	for (int32 i = static_cast<int32>(hoverPath.mWidgets.size()) - 1; i >= changedIndexStart; --i)
	{
		// 드래그 시
		if (HasDragEvent() == true)
		{
			std::shared_ptr<DragDropEvent> dragDropEvent = ObjectPool<DragDropEvent>::MakeShared(*event.get());
			dragDropEvent->mPayload = _mDragDropPayload;

			hoverPath.mWidgets[i].mWidget->OnEnterDragMouse(hoverPath.mWidgets[i].mGeometry, dragDropEvent);
		}
		// 드래그 아닐 시
		else
		{
			hoverPath.mWidgets[i].mWidget->OnEnterMouse(hoverPath.mWidgets[i].mGeometry, event);
		}
	}

	_mPreHoverWeakPath = hoverPath;
}

void AppModeBase::OnWheelMouse(std::shared_ptr<PointEvent>& event)
{
	ReplyData reply = ReplyData::Unhandled();

	// 버블 검사
	WidgetPath path = _mFocusWeakPath.Lock();
	event->mPath = &path;

	const auto iterEnd = path.mWidgets.end();
	for (auto iter = path.mWidgets.begin(); iter != iterEnd; ++iter)
	{
		std::shared_ptr<Widget> curWidget = iter->mWidget;
		if (curWidget != nullptr && curWidget->IsValid() == true)
		{
			reply = curWidget->OnWheelMouse(iter->mGeometry, event);
			if (reply.IsHandle() == true)
			{
				ProcessReplyData(reply, event);
				break;
			}
		}
	}
}

void AppModeBase::OnChangeFocus(std::shared_ptr<Widget>& focusWidget)
{
	std::shared_ptr<AppWindow> preFocusWindow = nullptr;
	if (_mFocusWeakPath.mWidgets.empty() == false)
	{
		std::shared_ptr<Widget> preFocusWidget = _mFocusWeakPath.mWidgets.front().lock();
		preFocusWidget->OnEndFocus();
		preFocusWindow = preFocusWidget->GetRootWindow();
	}

	// 윈도우 간의 포커스 변경 알림
	std::shared_ptr<AppWindow> nextFocusWindow = nullptr;
	HWND nextFocusWindowHWnd = 0;
	if (focusWidget != nullptr)
	{
		nextFocusWindow = focusWidget->GetRootWindow();
		nextFocusWindowHWnd = focusWidget->GetRootWindow()->GetDesc().mHWndRef->mData;
	}
	if (preFocusWindow != nextFocusWindow)
	{
		APP_WIN_MANAGER->NotifyToChangeFocus(nextFocusWindowHWnd);
	}

	_mFocusWeakPath = WidgetPath::GetPathDownTo(focusWidget);
	if (focusWidget != nullptr)
	{
		focusWidget->OnBeginFocus();
	}
}

void AppModeBase::OnDropFile(const wchar_t* fileFullPath)
{
}

void AppModeBase::ProcessReplyData(const ReplyData& reply, const std::shared_ptr<KeyEvent>& event)
{
	if (HasCaptureEvent() == true && reply.mNeedToReleaseCapture == true)
	{
		_mIsCapture = false;
		_mCaptureWeakPath.Clear();
	}
	
	if (HasCaptureEvent() == false && reply.mCaptureWidget != nullptr)
	{
		_mIsCapture = true;
		_mCaptureWeakPath = WidgetPath::GetPathDownTo(reply.mCaptureWidget);
	}

	if (reply.mFocusWidget != nullptr)
	{
		std::shared_ptr<Widget> nextFocusWidget = reply.mFocusWidget;
		OnChangeFocus(nextFocusWidget);
	}
}

void AppModeBase::ProcessReplyData(const ReplyData& reply, const std::shared_ptr<PointEvent>& event)
{
	if (HasCaptureEvent() == true && reply.mNeedToReleaseCapture == true)
	{
		_mIsCapture = false;
		_mCaptureWeakPath.Clear();
	}

	if (HasDragEvent() == false)
	{
		if (reply.mNeedToDragEvent == true && HasDownEvent() == true)
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
						_mIsDrag = true;
						_mDragDropWeakPath = WidgetPath::GetPathDownTo(curWidget);
						_mDragDropPayload = reply.mDragPayload;
						break;
					}
				}
			}
		}
		else if (HasCaptureEvent() == false && reply.mCaptureWidget != nullptr)
		{
			_mIsCapture = true;
			_mCaptureWeakPath = WidgetPath::GetPathDownTo(reply.mCaptureWidget);
		}
	}

	if (reply.mFocusWidget != nullptr)
	{
		std::shared_ptr<Widget> nextFocusWidget = reply.mFocusWidget;
		OnChangeFocus(nextFocusWidget);
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
	std::array<std::shared_ptr<MainThread>, MainThreadType::Count - 1> mainThreads = {
		std::make_shared<RenderThread>()
	};
	THREAD_MANAGER->LaunchMainThreads(mainThreads);
}

void AppModeBase::EndThread()
{
	THREAD_MANAGER->Join();
	ClearUserData();
}

void AppModeBase::RegisterDefaultResources()
{
	const std::wstring packagePath = PATH_MANAGER->GetEngineResourceFolderName();

	/* 메테리얼 */

	// 기본 UI 메테리얼
	{
		const std::wstring resourcePath = packagePath + L"\\Material\\M_UI";
		std::shared_ptr<Material> matUI = RESOURCE_MANAGER->LoadOrGetResource<Material>(resourcePath);

#ifdef _EDITOR
		// 없다면 만들어주기
		if (matUI == nullptr)
		{
			DEBUG_LOG("Create UI material to %s", ConvertWStringToUtf8(resourcePath).c_str());

			std::shared_ptr<ResourcePackage> package = PACKAGE_MANAGER->LoadPackage<ResourcePackage>(resourcePath);
			matUI = RESOURCE_MANAGER->CreateResource<Material>(package);

			std::shared_ptr<MaterialBulkData> bulkData = std::make_shared<MaterialBulkData>();
			bulkData->mSamplerStateDatas.push_back({ "linearWrapSS" , L"LinearWrap" });
			bulkData->mBlendStateName = L"DefaultAlpha";
			bulkData->mRasterizerStateName = L"BackCulling";
			bulkData->mShaderName = L"UI";
			matUI->UpdateProxy(
				bulkData,
				std::vector<std::pair<std::string, std::shared_ptr<ConstantDataBase>>>(),
				std::vector<std::pair<std::string, std::shared_ptr<Texture2D>>>()
			);

			matUI->Save();
		}
#endif // _EDITOR

		matUI->GetProxy();
		_mDefaultResources.push_back(matUI);
	}
}

void AppModeBase::UnregisterDefaultResources()
{
	_mDefaultResources.clear();
}

void AppModeBase::ClearUserData()
{
	_mFocusWeakPath.Clear();
	_mPreHoverWeakPath.Clear();
	_mCaptureWeakPath.Clear();
	_mMouseDownWeakPath.Clear();
	_mDragDropWeakPath.Clear();

	_mDragDropPayload.reset();
}


