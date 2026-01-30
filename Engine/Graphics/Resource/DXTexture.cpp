#include "pch.h"
#include "DXTexture.h"

#include "Manager/DXGraphicSystem.h"

DXTexture2D::DXTexture2D()
{
}

DXTexture2D::~DXTexture2D()
{
}

void DXTexture2D::Init(ComPtr<ID3D11ShaderResourceView> srv)
{
	ASSERT_MSG(srv != nullptr, "Null SRV can't be allowed to initialize texture2D");

	_mSRV = srv;
	_mSRV->GetResource(reinterpret_cast<ID3D11Resource**>(_mTexture.GetAddressOf()));

	D3D11_TEXTURE2D_DESC desc;
	_mTexture->GetDesc(&desc);

	_mWidth = desc.Width;
	_mHeight = desc.Height;
}

void DXTexture2D::Init(std::shared_ptr<Texture2DBulkData> bulkData)
{
	ASSERT_MSG(bulkData != nullptr, "Null bulk data can't be allowed to initialize texture2D");

	DirectX::TexMetadata md;
	HRESULT hr;
	hr = DirectX::LoadFromDDSMemory(
		bulkData->mValue.data(),
		bulkData->mValue.size(),
		DirectX::DDS_FLAGS_NONE,
		&md,
		_mImg
	);
	CHECK_WIN_MSG(hr, "Loading bulk data is failed");

	// ShaderResourceView는 
	// 텍스처, 밉, 배열 등을 가리키는 GPU 메모리 View
	hr = DirectX::CreateShaderResourceView(
		DX_DEVICE.Get(),		// 디바이스 생성 장치
		_mImg.GetImages(),		// 등록할 대상들
		_mImg.GetImageCount(),	// 등록할 대상 수
		md,						// 같이 전달할 메타 데이터
		_mSRV.GetAddressOf()	// OUT 생성된 SRT 객체
	);
	CHECK_WIN_MSG(hr, "Texture default SRV creation is failed");

	_mSRV->GetResource(reinterpret_cast<ID3D11Resource**>(_mTexture.GetAddressOf()));

	_mWidth = static_cast<uint32>(md.width);
	_mHeight = static_cast<uint32>(md.height);
}
