#include "pch.h"
#include "DXGraphicSystem.h"

DXGraphicSystem::DXGraphicSystem()
{

}

DXGraphicSystem::~DXGraphicSystem()
{

}

void DXGraphicSystem::Init()
{
	CreateDevice();
}

void DXGraphicSystem::Destroy()
{
	_mRenderTargetView.Reset();
	_mDepthStencilView.Reset();
	_mFactory.Reset();

	_mDeviceContext->ClearState();
	_mDeviceContext->Flush();

	_mDeviceContext.Reset();

#ifdef _DEBUG
	ComPtr<ID3D11Debug> debug;
	_mDevice.As(&debug);

	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif // _DEBUG

	_mDevice.Reset();
}

void DXGraphicSystem::SaveRenderTargets()
{
	_mDeviceContext->OMGetRenderTargets(1, _mRenderTargetView.ReleaseAndGetAddressOf(), _mDepthStencilView.ReleaseAndGetAddressOf());
}

void DXGraphicSystem::RollbackRenderTargets(bool releaseCache)
{
	_mDeviceContext->OMSetRenderTargets(1, _mRenderTargetView.GetAddressOf(), _mDepthStencilView.Get());
	if (releaseCache == true)
	{
		_mRenderTargetView.Reset();
		_mDepthStencilView.Reset();
	}
}

void DXGraphicSystem::CreateDevice()
{
	/* 디바이스 생성 */
	{
		int32 flags = 0;
#ifdef _DEBUG
		flags = D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

		D3D_FEATURE_LEVEL featureLevelList = D3D_FEATURE_LEVEL_11_0;
		D3D_FEATURE_LEVEL resFeatureLevel;

		CHECK_WIN_MSG(D3D11CreateDevice(
			nullptr,					// 현재 장착 그래픽 
			D3D_DRIVER_TYPE_HARDWARE,	// 하드웨어 드라이버 사용
			0,							// 스프트웨어로 실행할 때 모듈
			flags,						// 생성 모드
			&featureLevelList,			// DX 버전 지원 여부 배열 순회해 파악
			1,							// 지원 여부 배열의 갯수
			D3D11_SDK_VERSION,
			&_mDevice,					// (출력) 디바이스
			&resFeatureLevel,			// (출력) 지원 가능 버전
			&_mDeviceContext			// (출력) Immediate 컨텍스트 <-> 반대로는 멀티스레딩용 컨텍스트가 따로 있다
		), "Device creation is failed");
	}


	/* 내 컴 MASS 어느정도 쓸 수 있니? */
	{
		uint32 result = FALSE;
		_mDevice->CheckMultisampleQualityLevels(
			DXGI_FORMAT_R8G8B8A8_UNORM,		// 픽셀 수준
			4,								// 샘플링 수
			&result							// MSAA 지원여부 결과
		);
		if (result != TRUE)
		{
			_mMaxSampleCount = 1;
		}

		// TODO : 임시
		_mMaxSampleCount = 1;
	}


	/* Swap Chain 생성 용 팩토리 캐싱 */
	{
		ComPtr<IDXGIDevice> dxgiDevice = nullptr;
		_mDevice->QueryInterface(
			__uuidof(IDXGIDevice),	// 타입의 id 가져옴
			reinterpret_cast<void**>(dxgiDevice.ReleaseAndGetAddressOf())
		);

		ComPtr<IDXGIAdapter> adapter = nullptr;
		dxgiDevice->GetParent(
			__uuidof(IDXGIAdapter), 
			reinterpret_cast<void**>(adapter.ReleaseAndGetAddressOf())
		);

		adapter->GetParent(
			__uuidof(IDXGIFactory), 
			reinterpret_cast<void**>(_mFactory.ReleaseAndGetAddressOf())
		);
	}
}
