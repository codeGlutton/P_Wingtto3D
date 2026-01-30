#pragma once

#include "Core/Object.h"
#include "Graphics/Viewport/Viewport.h"
#include "GraphicMinimum.h"

#include "Manager/TimeManager.h"
#include "Utils/Thread/RefCounting.h"

struct AppWindowDesc
{
	GEN_STRUCT_REFLECTION(AppWindowDesc)

	RefCounting<HWND> mHWndRef = nullptr;
	PROPERTY(mWidth)
	float mWidth = 1280.f;
	PROPERTY(mHeight)
	float mHeight = 720.f;
	PROPERTY(mVsync)
	bool mVsync = false;
	PROPERTY(mWindowed)
	bool mWindowed = true;

	std::wstring mName = L"";
	float mClientWidth;
	float mClientHeight;
};

/**
 * 윈도우 창 객체. 내부 viewport는 렌더 스레드가 관리
 */
class AppWindow abstract : public InterfaceReflector<Object, IUpdatable>
{
	GEN_ABSTRACT_REFLECTION(AppWindow)

	friend class AppWindowManager;

public:
	AppWindow();
	virtual ~AppWindow();

protected:
	virtual void PostLoad() override;

protected:
	bool InitWindow();
	virtual void CreateRootViewport() = 0;
	void ShowWindow();

protected:
	virtual void Update(float deltaTime) override;
	virtual void FixedUpdate() override;

protected:
	void BeginFocus();
	void EndFocus();
	virtual void OnResize(bool isWindowed, const RECT& clientSize);

public:
	const AppWindowDesc& GetDesc() const 
	{
		return _mDesc;
	}
	const std::shared_ptr<AppWindow>& GetOwner() const
	{
		return _mOwner.lock();
	}
	bool IsMainAppWindow() const
	{
		return _mOwner.lock() == nullptr;
	}

public:
	//void Picking();

private:
	PROPERTY(_mDesc)
	AppWindowDesc _mDesc;

private:
	// 부모 객체
	PROPERTY(_mOwner)
	std::weak_ptr<AppWindow> _mOwner;

protected:
	PROPERTY(_mRootViewport)
	std::shared_ptr<Viewport> _mRootViewport;

private:
	std::shared_ptr<UpdateTargetContext> _mUpdateContext;
	bool _mNeedSizeUpdate = false;
};

