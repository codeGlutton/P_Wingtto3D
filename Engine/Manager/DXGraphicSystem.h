#pragma once

#define DX_GRAPHICS DXGraphicSystem::GetInst()
#define DX_DEVICE DX_GRAPHICS->GetDevice()
#define DX_DEVICE_CONTEXT DX_GRAPHICS->GetDeviceContext()
#define DX_FACTORY DX_GRAPHICS->GetFactory()

/**
 * D3D11의 프레임워크 변경과 출력을 담당할 싱글톤 클래스
 */
class DXGraphicSystem
{
private:
	DXGraphicSystem();
	~DXGraphicSystem();

public:
	static DXGraphicSystem* GetInst()
	{
		static DXGraphicSystem inst;
		return &inst;
	}

	ComPtr<ID3D11Device> GetDevice()
	{
		return _mDevice;
	}
	ComPtr<ID3D11DeviceContext> GetDeviceContext()
	{
		return _mDeviceContext;
	}
	ComPtr<IDXGIFactory> GetFactory()
	{
		return _mFactory;
	}
	uint32 GetMaxSampleCount() const
	{
		return _mMaxSampleCount;
	}

public:
	void Init();
	void Destroy();

public:
	void SaveRenderTargets();
	void RollbackRenderTargets(bool releaseCache = true);

private:
	void CreateDevice();

private:
	ComPtr<ID3D11Device> _mDevice;
	ComPtr<ID3D11DeviceContext> _mDeviceContext;
	ComPtr<IDXGIFactory> _mFactory;
	uint32 _mMaxSampleCount = 4u;

private:
	ComPtr<ID3D11RenderTargetView> _mRenderTargetView = nullptr;
	ComPtr<ID3D11DepthStencilView> _mDepthStencilView = nullptr;
};
