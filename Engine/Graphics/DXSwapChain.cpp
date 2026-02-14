#include "pch.h"
#include "DXSwapChain.h"

#include "Manager/DXGraphicSystem.h"
#include "Graphics/Viewport/DXViewport.h"

void DXSwapChain::Init()
{
	/* 스왑 체인 생성 */
	CreateSwapChain();

	/* 해당 윈도우 창의 RTV 생성 */
	CreateRenderTargetView();
	/* 해당 윈도우 창의 DSV 생성 */
	CreateDepthStencilView();
}

void DXSwapChain::Resize(uint32 width, uint32 height)
{
	if (width == 0 || height == 0)
	{
		return;
	}

	_mWidth = width;
	_mHeight = height;

	_mRenderTargetView.Reset();
	CHECK_WIN_MSG(_mSwapChain->ResizeBuffers(
		0,							// 기존 버퍼 갯수 유지
		_mWidth,
		_mHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0
	), "ResizeBuffers failed");

	/* 해당 윈도우 창의 RTV 생성 */
	CreateRenderTargetView();
	/* 해당 윈도우 창의 DSV 생성 */
	CreateDepthStencilView();
}

void DXSwapChain::BeginRender(std::shared_ptr<DXViewport> viewport, bool clearRTV, bool clearDSV)
{
	DX_DEVICE_CONTEXT->OMSetRenderTargets(1, _mRenderTargetView.GetAddressOf(), _mDepthStencilView.Get());
	if (viewport != nullptr)
	{
		DX_DEVICE_CONTEXT->RSSetViewports(1, &viewport->GetDesc());
	}
	if (clearRTV == true)
	{
		DX_DEVICE_CONTEXT->ClearRenderTargetView(_mRenderTargetView.Get(), reinterpret_cast<const float*>(&_mClearColor));
	}
	if (clearDSV == true)
	{
		DX_DEVICE_CONTEXT->ClearDepthStencilView(_mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}
}

void DXSwapChain::EndRender()
{
	_mSwapChain->Present(0, 0);
}

void DXSwapChain::ResetRenderTargets(RenderTargetResetFlag::Type flags)
{
	ComPtr<ID3D11RenderTargetView> targetRTV;
	ComPtr<ID3D11DepthStencilView> targetDSV;
	if (flags & RenderTargetResetFlag::NullTarget)
	{
		targetRTV = nullptr;
		targetDSV = nullptr;
	}
	else
	{
		DX_DEVICE_CONTEXT->OMGetRenderTargets(1, targetRTV.ReleaseAndGetAddressOf(), targetDSV.ReleaseAndGetAddressOf());
	}

	if (flags & RenderTargetResetFlag::DefaultRTV)
	{
		targetRTV = _mRenderTargetView;
	}
	if (flags & RenderTargetResetFlag::DefaultDSV)
	{
		targetDSV = _mDepthStencilView;
	}

	DX_DEVICE_CONTEXT->OMSetRenderTargets(1, targetRTV.GetAddressOf(), targetDSV.Get());
}

void DXSwapChain::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferDesc.Width = _mWidth;
	swapDesc.BufferDesc.Height = _mHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ----------------> vsync 수직 동기화 안할 시 싱크없이 랜더링될 수 있다! 
	// ----------------> 대신 속도적으로 이득
	// -------> 앗!

	// 1 / 60 hz 주사율 모니터랑 맞춰두는게 좋다
	swapDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapDesc.BufferDesc.RefreshRate.Denominator = 60;

	// 지정 해상도와 모니터가 크기가 다를 때, 스케이링 옵션
	// DXGI_MODE_SCALING_UNSPECIFIED : OS에 맞기기
	// DXGI_MODE_SCALING_CENTERED : 가운데 배치. 나머지 검은색
	// DXGI_MODE_SCALING_STRETCHED : 쭈욱 늘리거나 줄이기
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// 화면을 그려줄 픽셀 순서 처리 옵션
	// DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED       : OS에 맞기기
	// DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE       : 순차 스캔
	// DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST : 폐기
	// DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST : 폐기
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 백버퍼 갯수
	swapDesc.BufferCount = 2;
	swapDesc.OutputWindow = _mHWnd->mData;
	// MSAA 샘플링 수와 퀄리티
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.SampleDesc.Count = DX_GRAPHICS->GetMaxSampleCount();

	swapDesc.Windowed = _mIsWindowed;
	// 스왑시 옵션
	// DXGI_SWAP_EFFECT_DISCARD			: 이전 버퍼 데이터 버림
	// DXGI_SWAP_EFFECT_SEQUENTIAL		: 큐와 같이 버퍼가 순서대로 화면에 처리.
	// DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL	: 순차 출력을 하면서 플립
	// DXGI_SWAP_EFFECT_FLIP_DISCARD	: 플립하며 백버퍼의 데이터 버림 (가장 요즘 많이씀)
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	CHECK_WIN_MSG(DX_FACTORY->CreateSwapChain(DX_DEVICE.Get(), &swapDesc, &_mSwapChain), "Swap chain creation is failed");
}

void DXSwapChain::CreateRenderTargetView()
{
	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	CHECK_WIN_MSG(_mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.ReleaseAndGetAddressOf())), "Swap chain RTV texture creation is failed");
	CHECK_WIN_MSG(DX_DEVICE->CreateRenderTargetView(backBuffer.Get(), nullptr, _mRenderTargetView.ReleaseAndGetAddressOf()), "Swap chain RTV creation is failed");
}

void DXSwapChain::CreateDepthStencilView()
{
	{
		D3D11_TEXTURE2D_DESC desc = { 0 };
		::memset(&desc, 0, sizeof(desc));
		desc.Width = _mWidth;
		desc.Height = _mHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 비트는 깊이, 8 비트는 스텐실로 사용
		desc.SampleDesc.Quality = 0;
		desc.SampleDesc.Count = DX_GRAPHICS->GetMaxSampleCount();
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // depth stencil로 쓸께!
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&desc, nullptr, _mDepthStencilTexture.ReleaseAndGetAddressOf()), "Swap chain DSV Texture creation is failed");
	}

	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		// 해당 ID3D11Texture2D를 뎁스 스텐실로 사용할 거야. 같이 DepthStencilView(핸들러 객체)를 반환해줘
		CHECK_WIN_MSG(DX_DEVICE->CreateDepthStencilView(_mDepthStencilTexture.Get(), &desc, _mDepthStencilView.ReleaseAndGetAddressOf()), "Swap chain DSV Texture creation is failed");
	}
}
