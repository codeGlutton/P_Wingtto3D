#include "pch.h"
#include "DXTexture.h"

#include "Manager/DXGraphicSystem.h"

DXTextureBase::DXTextureBase()
{
	_mResourceFlags = DXResourceFlag::None;
	_mSharedResourceType = DXSharedResourceType::Texture;
}

DXTextureBase::~DXTextureBase()
{
}

void DXTextureBase::PushData() const
{
	if (_mSRVUsageFlags & DXResourceUsageFlag::Vertex)
	{
		DX_DEVICE_CONTEXT->VSSetShaderResources(_mSRVSlot, 1, _mSRV.GetAddressOf());
	}

	if (_mSRVUsageFlags & DXResourceUsageFlag::Domain)
	{
		DX_DEVICE_CONTEXT->DSSetShaderResources(_mSRVSlot, 1, _mSRV.GetAddressOf());
	}

	if (_mSRVUsageFlags & DXResourceUsageFlag::Hull)
	{
		DX_DEVICE_CONTEXT->HSSetShaderResources(_mSRVSlot, 1, _mSRV.GetAddressOf());
	}

	if (_mSRVUsageFlags & DXResourceUsageFlag::Geometry)
	{
		DX_DEVICE_CONTEXT->GSSetShaderResources(_mSRVSlot, 1, _mSRV.GetAddressOf());
	}

	if (_mSRVUsageFlags & DXResourceUsageFlag::Pixel)
	{
		DX_DEVICE_CONTEXT->PSSetShaderResources(_mSRVSlot, 1, _mSRV.GetAddressOf());
	}

	if (_mSRVUsageFlags & DXResourceUsageFlag::Compute)
	{
		DX_DEVICE_CONTEXT->CSSetShaderResources(_mSRVSlot, 1, _mSRV.GetAddressOf());
	}
}

DXTexture2DBase::DXTexture2DBase()
{
}

DXTexture2DBase::~DXTexture2DBase()
{
}

void DXTexture2DBase::InitImmutableTex(const void* initData, uint32 width, uint32 height, DXGI_FORMAT format)
{
	ASSERT_MSG(initData != nullptr, "Null init data can't be allowed to initialize read only texture2D");

	_mWidth = width;
	_mHeight = height;
	_mBindFlags = D3D11_BIND_SHADER_RESOURCE;
	_mResourceFlags = DXResourceFlag::None;

	{
		D3D11_TEXTURE2D_DESC desc;
		::memset(&desc, 0, sizeof(desc));

		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = _mBindFlags;

		std::size_t pitch, slice;
		DirectX::ComputePitch(format, width, height, pitch, slice);

		D3D11_SUBRESOURCE_DATA data;
		::memset(&data, 0, sizeof(data));

		data.pSysMem = initData;
		data.SysMemPitch = static_cast<uint32>(pitch);

		CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&desc, &data, _mTexture.GetAddressOf()), "Texture creation is failed");
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		::memset(&desc, 0, sizeof(desc));

		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		CHECK_WIN_MSG(DX_DEVICE->CreateShaderResourceView(_mTexture.Get(), &desc, _mSRV.GetAddressOf()), "Texture SRV creation is failed");
	}
}

void DXTexture2DBase::InitImmutableTex(std::shared_ptr<Texture2DBulkData> bulkData)
{
	ASSERT_MSG(bulkData != nullptr, "Null bulk data can't be allowed to initialize texture2D");

	_mBindFlags = D3D11_BIND_SHADER_RESOURCE;
	_mResourceFlags = DXResourceFlag::None;

	// ShaderResourceView는 
	// 텍스처, 밉, 배열 등을 가리키는 GPU 메모리 View
	HRESULT hr = DirectX::CreateShaderResourceViewEx(
		DX_DEVICE.Get(),				// 디바이스 생성 장치
		bulkData->mImg.GetImages(),		// 등록할 대상들
		bulkData->mImg.GetImageCount(),	// 등록할 대상 수
		bulkData->mMetaData,			// 같이 전달할 메타 데이터
		D3D11_USAGE_IMMUTABLE,
		_mBindFlags,
		0,
		0,
		false,
		_mSRV.GetAddressOf()			// OUT 생성된 SRT 객체
	);
	CHECK_WIN_MSG(hr, "Texture default SRV creation is failed");

	_mSRV->GetResource(reinterpret_cast<ID3D11Resource**>(_mTexture.GetAddressOf()));

	_mWidth = static_cast<uint32>(bulkData->mMetaData.width);
	_mHeight = static_cast<uint32>(bulkData->mMetaData.height);
}

void DXTexture2DBase::InitDefaultTex(const void* initData, uint32 width, uint32 height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlags)
{
	_mWidth = width;
	_mHeight = height;
	_mBindFlags = D3D11_BIND_SHADER_RESOURCE | bindFlags;
	_mResourceFlags = DXResourceFlag::HasOutput;

	{
		D3D11_TEXTURE2D_DESC desc;
		::memset(&desc, 0, sizeof(desc));

		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = _mBindFlags;

		if (initData == nullptr)
		{
			CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&desc, nullptr, _mTexture.GetAddressOf()), "Texture creation is failed");
		}
		else
		{
			std::size_t pitch, slice;
			DirectX::ComputePitch(format, width, height, pitch, slice);

			D3D11_SUBRESOURCE_DATA data;
			::memset(&data, 0, sizeof(data));

			data.pSysMem = initData;
			data.SysMemPitch = static_cast<uint32>(pitch);

			CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&desc, &data, _mTexture.GetAddressOf()), "Texture creation is failed");
		}
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		::memset(&desc, 0, sizeof(desc));

		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		CHECK_WIN_MSG(DX_DEVICE->CreateShaderResourceView(_mTexture.Get(), &desc, _mSRV.GetAddressOf()), "Texture SRV creation is failed");
	}
}

void DXTexture2DBase::InitDefaultTex(std::shared_ptr<Texture2DBulkData> bulkData, D3D11_BIND_FLAG bindFlags)
{
	ASSERT_MSG(bulkData != nullptr, "Null bulk data can't be allowed to initialize texture2D");

	_mBindFlags = D3D11_BIND_SHADER_RESOURCE | bindFlags;
	_mResourceFlags = DXResourceFlag::HasOutput;

	// ShaderResourceView는 
	// 텍스처, 밉, 배열 등을 가리키는 GPU 메모리 View
	HRESULT hr = DirectX::CreateShaderResourceViewEx(
		DX_DEVICE.Get(),				// 디바이스 생성 장치
		bulkData->mImg.GetImages(),		// 등록할 대상들
		bulkData->mImg.GetImageCount(),	// 등록할 대상 수
		bulkData->mMetaData,			// 같이 전달할 메타 데이터
		D3D11_USAGE_DEFAULT,
		_mBindFlags,
		0,
		0,
		false,
		_mSRV.GetAddressOf()	// OUT 생성된 SRT 객체
	);
	CHECK_WIN_MSG(hr, "Texture default SRV creation is failed");

	_mSRV->GetResource(reinterpret_cast<ID3D11Resource**>(_mTexture.GetAddressOf()));

	_mWidth = static_cast<uint32>(bulkData->mMetaData.width);
	_mHeight = static_cast<uint32>(bulkData->mMetaData.height);
}

DXConstTexture2D::DXConstTexture2D()
{
}

DXConstTexture2D::~DXConstTexture2D()
{
}

void DXConstTexture2D::Init(const void* initData, uint32 width, uint32 height, DXGI_FORMAT format, uint32 slot, DXResourceUsageFlag::Type usageFlags)
{
	_mSRVSlot = slot;
	_mSRVUsageFlags = usageFlags;
	DXTexture2DBase::InitImmutableTex(initData, width, height, format);
}

void DXConstTexture2D::Init(std::shared_ptr<Texture2DBulkData> bulkData, uint32 slot, DXResourceUsageFlag::Type usageFlags)
{
	_mSRVSlot = slot;
	_mSRVUsageFlags = usageFlags;
	DXTexture2DBase::InitImmutableTex(bulkData);
}

DXTexture2D::DXTexture2D()
{
}

DXTexture2D::~DXTexture2D()
{
}

void DXTexture2D::Init(ComPtr<ID3D11ShaderResourceView> initSRVData, D3D11_BIND_FLAG bindFlags, uint32 slot, DXResourceUsageFlag::Type usageFlags)
{
	ASSERT_MSG(initSRVData != nullptr, "Null srv data can't be allowed to initialize texture2D");
	
	ComPtr<ID3D11Texture2D> texture;
	initSRVData->GetResource((ID3D11Resource**)texture.GetAddressOf());

	ASSERT_MSG(texture != nullptr, "Empty texture resource can't be used for texture2D");

	D3D11_TEXTURE2D_DESC srcDesc;
	texture->GetDesc(&srcDesc);

	{
		_mWidth = srcDesc.Width;
		_mHeight = srcDesc.Height;

		_mSRVSlot = slot;
		_mSRVUsageFlags = usageFlags;

		_mBindFlags = D3D11_BIND_SHADER_RESOURCE | bindFlags;
		_mResourceFlags = DXResourceFlag::HasOutput;

		srcDesc.BindFlags = _mBindFlags;
		srcDesc.Usage = D3D11_USAGE_DEFAULT;

		CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&srcDesc, nullptr, _mTexture.GetAddressOf()), "Texture creation is failed");
		DX_DEVICE_CONTEXT->CopyResource(_mTexture.Get(), texture.Get());
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		::memset(&desc, 0, sizeof(desc));

		desc.Format = srcDesc.Format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		CHECK_WIN_MSG(DX_DEVICE->CreateShaderResourceView(_mTexture.Get(), &desc, _mSRV.GetAddressOf()), "Texture SRV creation is failed");
	}
}

void DXTexture2D::Init(const void* initData, uint32 width, uint32 height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlags, uint32 slot, DXResourceUsageFlag::Type usageFlags)
{
	_mSRVSlot = slot;
	_mSRVUsageFlags = usageFlags;
	DXTexture2DBase::InitDefaultTex(initData, width, height, format, bindFlags);
}

void DXTexture2D::Init(std::shared_ptr<Texture2DBulkData> bulkData, D3D11_BIND_FLAG bindFlags, uint32 slot, DXResourceUsageFlag::Type usageFlags)
{
	_mSRVSlot = slot;
	_mSRVUsageFlags = usageFlags;
	DXTexture2DBase::InitDefaultTex(bulkData, bindFlags);
}

DXTexture2DArrayBase::DXTexture2DArrayBase()
{
}

DXTexture2DArrayBase::~DXTexture2DArrayBase()
{
}

void DXTexture2DArrayBase::InitImmutableTex(const void* initData, uint32 arraySize, uint32 width, uint32 height, DXGI_FORMAT format)
{
	ASSERT_MSG(initData != nullptr, "Null init data can't be allowed to initialize read only texture2DArray");
	ASSERT_MSG(arraySize > 0, "Texture2DArray must have at least one slice");

	_mWidth = width;
	_mHeight = height;
	_mArraySize = arraySize;
	_mBindFlags = D3D11_BIND_SHADER_RESOURCE;
	_mResourceFlags = DXResourceFlag::None;

	{
		D3D11_TEXTURE2D_DESC desc;
		::memset(&desc, 0, sizeof(desc));

		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = arraySize;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = _mBindFlags;

		std::size_t pitch, slice;
		DirectX::ComputePitch(format, width, height, pitch, slice);

		std::vector<D3D11_SUBRESOURCE_DATA> datas(arraySize);
		for (uint32 i = 0; i < arraySize; ++i)
		{
			::memset(&datas[i], 0, sizeof(datas[i]));
			datas[i].pSysMem = reinterpret_cast<const BYTE*>(initData) + i * slice;
			datas[i].SysMemPitch = static_cast<uint32>(pitch);
			datas[i].SysMemSlicePitch = static_cast<uint32>(slice);
		}

		CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&desc, datas.data(), _mTexture.GetAddressOf()), "Texture creation is failed");
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		::memset(&desc, 0, sizeof(desc));

		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = arraySize;

		CHECK_WIN_MSG(DX_DEVICE->CreateShaderResourceView(_mTexture.Get(), &desc, _mSRV.GetAddressOf()), "Texture SRV creation is failed");
	}
}

void DXTexture2DArrayBase::InitImmutableTex(std::shared_ptr<Texture2DBulkData> bulkData)
{
	ASSERT_MSG(bulkData != nullptr, "Null bulk data can't be allowed to initialize texture2DArray");
	ASSERT_MSG(bulkData->IsArray() == true, "Non array bulk data can't be allowed to initialize texture2DArray");

	_mBindFlags = D3D11_BIND_SHADER_RESOURCE;
	_mResourceFlags = DXResourceFlag::None;

	// ShaderResourceView는 
	// 텍스처, 밉, 배열 등을 가리키는 GPU 메모리 View
	HRESULT hr = DirectX::CreateShaderResourceViewEx(
		DX_DEVICE.Get(),				// 디바이스 생성 장치
		bulkData->mImg.GetImages(),		// 등록할 대상들
		bulkData->mImg.GetImageCount(),	// 등록할 대상 수
		bulkData->mMetaData,			// 같이 전달할 메타 데이터
		D3D11_USAGE_IMMUTABLE,
		_mBindFlags,
		0,
		0,
		false,
		_mSRV.GetAddressOf()			// OUT 생성된 SRT 객체
	);
	CHECK_WIN_MSG(hr, "Texture default SRV creation is failed");

	_mSRV->GetResource(reinterpret_cast<ID3D11Resource**>(_mTexture.GetAddressOf()));

	_mWidth = static_cast<uint32>(bulkData->mMetaData.width);
	_mHeight = static_cast<uint32>(bulkData->mMetaData.height);
	_mArraySize = static_cast<uint32>(bulkData->mMetaData.arraySize);
}

void DXTexture2DArrayBase::InitDefaultTex(const void* initData, uint32 arraySize, uint32 width, uint32 height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlags)
{
	ASSERT_MSG(arraySize > 0, "Texture2DArray must have at least one slice");

	_mWidth = width;
	_mHeight = height;
	_mArraySize = arraySize;
	_mBindFlags = D3D11_BIND_SHADER_RESOURCE | bindFlags;
	_mResourceFlags = DXResourceFlag::HasOutput;

	{
		D3D11_TEXTURE2D_DESC desc;
		::memset(&desc, 0, sizeof(desc));

		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = arraySize;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = _mBindFlags;

		if (initData == nullptr)
		{
			CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&desc, nullptr, _mTexture.GetAddressOf()), "Texture creation is failed");
		}
		else
		{
			std::size_t pitch, slice;
			DirectX::ComputePitch(format, width, height, pitch, slice);

			std::vector<D3D11_SUBRESOURCE_DATA> datas(arraySize);
			for (uint32 i = 0; i < arraySize; ++i)
			{
				::memset(&datas[i], 0, sizeof(datas[i]));
				datas[i].pSysMem = reinterpret_cast<const BYTE*>(initData) + i * slice;
				datas[i].SysMemPitch = static_cast<uint32>(pitch);
				datas[i].SysMemSlicePitch = static_cast<uint32>(slice);
			}

			CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&desc, datas.data(), _mTexture.GetAddressOf()), "Texture creation is failed");
		}
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		::memset(&desc, 0, sizeof(desc));

		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = arraySize;

		CHECK_WIN_MSG(DX_DEVICE->CreateShaderResourceView(_mTexture.Get(), &desc, _mSRV.GetAddressOf()), "Texture SRV creation is failed");
	}
}

void DXTexture2DArrayBase::InitDefaultTex(std::shared_ptr<Texture2DBulkData> bulkData, D3D11_BIND_FLAG bindFlags)
{
	ASSERT_MSG(bulkData != nullptr, "Null bulk data can't be allowed to initialize texture2DArray");
	ASSERT_MSG(bulkData->IsArray() == true, "Non array bulk data can't be allowed to initialize texture2DArray");

	_mBindFlags = D3D11_BIND_SHADER_RESOURCE | bindFlags;
	_mResourceFlags = DXResourceFlag::HasOutput;

	// ShaderResourceView는 
	// 텍스처, 밉, 배열 등을 가리키는 GPU 메모리 View
	HRESULT hr = DirectX::CreateShaderResourceViewEx(
		DX_DEVICE.Get(),				// 디바이스 생성 장치
		bulkData->mImg.GetImages(),		// 등록할 대상들
		bulkData->mImg.GetImageCount(),	// 등록할 대상 수
		bulkData->mMetaData,			// 같이 전달할 메타 데이터
		D3D11_USAGE_DEFAULT,
		_mBindFlags,
		0,
		0,
		false,
		_mSRV.GetAddressOf()	// OUT 생성된 SRT 객체
	);
	CHECK_WIN_MSG(hr, "Texture default SRV creation is failed");

	_mSRV->GetResource(reinterpret_cast<ID3D11Resource**>(_mTexture.GetAddressOf()));

	_mWidth = static_cast<uint32>(bulkData->mMetaData.width);
	_mHeight = static_cast<uint32>(bulkData->mMetaData.height);
	_mArraySize = static_cast<uint32>(bulkData->mMetaData.arraySize);
}

DXTexture2DArray::DXTexture2DArray()
{
}

DXTexture2DArray::~DXTexture2DArray()
{
}

void DXTexture2DArray::Init(ComPtr<ID3D11ShaderResourceView> initSRVData, D3D11_BIND_FLAG bindFlags, uint32 slot, DXResourceUsageFlag::Type usageFlags)
{
	ASSERT_MSG(initSRVData != nullptr, "Null srv data can't be allowed to initialize texture2DArray");

	ComPtr<ID3D11Texture2D> texture;
	initSRVData->GetResource((ID3D11Resource**)texture.GetAddressOf());

	ASSERT_MSG(texture != nullptr, "Empty texture resource can't be used for texture2DArray");

	D3D11_TEXTURE2D_DESC srcDesc;
	texture->GetDesc(&srcDesc);

	{
		_mWidth = srcDesc.Width;
		_mHeight = srcDesc.Height;
		_mArraySize = srcDesc.ArraySize;

		_mSRVSlot = slot;
		_mSRVUsageFlags = usageFlags;

		_mBindFlags = D3D11_BIND_SHADER_RESOURCE | bindFlags;
		_mResourceFlags = DXResourceFlag::HasOutput;

		srcDesc.BindFlags = _mBindFlags;
		srcDesc.Usage = D3D11_USAGE_DEFAULT;

		CHECK_WIN_MSG(DX_DEVICE->CreateTexture2D(&srcDesc, nullptr, _mTexture.GetAddressOf()), "Texture creation is failed");
		DX_DEVICE_CONTEXT->CopyResource(_mTexture.Get(), texture.Get());
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		::memset(&desc, 0, sizeof(desc));

		desc.Format = srcDesc.Format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = _mArraySize;

		CHECK_WIN_MSG(DX_DEVICE->CreateShaderResourceView(_mTexture.Get(), &desc, _mSRV.GetAddressOf()), "Texture SRV creation is failed");
	}
}

void DXTexture2DArray::Init(const void* initData, uint32 arraySize, uint32 width, uint32 height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlags, uint32 slot, DXResourceUsageFlag::Type usageFlags)
{
	_mSRVSlot = slot;
	_mSRVUsageFlags = usageFlags;
	DXTexture2DArrayBase::InitDefaultTex(initData, arraySize, width, height, format, bindFlags);
}

void DXTexture2DArray::Init(std::shared_ptr<Texture2DBulkData> bulkData, D3D11_BIND_FLAG bindFlags, uint32 slot, DXResourceUsageFlag::Type usageFlags)
{
	_mSRVSlot = slot;
	_mSRVUsageFlags = usageFlags;
	DXTexture2DArrayBase::InitDefaultTex(bulkData, bindFlags);
}

DXConstTexture2DArray::DXConstTexture2DArray()
{
}

DXConstTexture2DArray::~DXConstTexture2DArray()
{
}

void DXConstTexture2DArray::Init(const void* initData, uint32 arraySize, uint32 width, uint32 height, DXGI_FORMAT format, uint32 slot, DXResourceUsageFlag::Type usageFlags)
{
	_mSRVSlot = slot;
	_mSRVUsageFlags = usageFlags;
	DXTexture2DArrayBase::InitImmutableTex(initData, arraySize, width, height, format);
}

void DXConstTexture2DArray::Init(std::shared_ptr<Texture2DBulkData> bulkData, uint32 slot, DXResourceUsageFlag::Type usageFlags)
{
	_mSRVSlot = slot;
	_mSRVUsageFlags = usageFlags;
	DXTexture2DArrayBase::InitImmutableTex(bulkData);
}

