#pragma once

#include "Utils/Thread/RefCounting.h"

class DXViewport;

namespace RenderTargetResetFlag
{
	enum Type : uint8
	{
		None = 0,
		NullTarget = 1 << 0,
		DefaultRTV = 1 << 1,
		DefaultDSV = 1 << 2,

		FillDefaults = DefaultRTV | DefaultDSV,
	};
}

class DXSwapChain
{
public:
	DXSwapChain(uint32 width, uint32 height, bool isWindowed) :
		_mWidth(width),
		_mHeight(height),
		_mIsWindowed(isWindowed)
	{
	}

public:
	void Init();
	void Resize(uint32 width, uint32 height);

public:
	uint32 GetWidth() const
	{
		return _mWidth;
	}
	uint32 GetHeight() const
	{
		return _mHeight;
	}

public:
	void BeginRender(std::shared_ptr<DXViewport> viewport, bool clearRTV = true, bool clearDSV = true);
	void EndRender();

public:
	void ResetRenderTargets(RenderTargetResetFlag::Type flags = RenderTargetResetFlag::FillDefaults);

private:
	void CreateSwapChain();

private:
	void CreateRenderTargetView();
	void CreateDepthStencilView();

private:
	RefCounting<HWND> _mHWnd;
	uint32 _mWidth;
	uint32 _mHeight;
	bool _mIsWindowed;

	const Color _mClearColor = Color(0.f, 0.f, 0.f);

private:
	ComPtr<IDXGISwapChain> _mSwapChain = nullptr;

private:
	ComPtr<ID3D11RenderTargetView> _mRenderTargetView = nullptr;
	ComPtr<ID3D11Texture2D> _mDepthStencilTexture = nullptr;
	ComPtr<ID3D11DepthStencilView> _mDepthStencilView = nullptr;
};

