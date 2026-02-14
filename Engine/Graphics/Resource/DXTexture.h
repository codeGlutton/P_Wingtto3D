#pragma once

#include "Graphics/Resource/DXResource.h"
#include "Core/Resource/BulkData.h"
#include "Utils/MathUtils.h"

struct Texture2DBulkData : public BulkData
{
	GEN_STRUCT_REFLECTION(Texture2DBulkData)

public:
	bool IsArray() const
	{
		return mMetaData.arraySize > 1;
	}

public:
	void SerializeFromMemory()
	{
		HRESULT hr;
		hr = DirectX::LoadFromDDSMemory(
			mValue.data(),
			mValue.size(),
			DirectX::DDS_FLAGS_NONE,
			&mMetaData,
			mImg
		);
		CHECK_WIN_MSG(hr, "Loading bulk data is failed");
	}

public:
	PROPERTY(mValue)
	std::vector<char> mValue;

	DirectX::TexMetadata mMetaData;
	DirectX::ScratchImage mImg;
};

/**
 * 모든 텍스처 베이스 클래스
 */
class DXTextureBase abstract : public DXSharedResource
{
	friend class ResourceManager;

protected:
	DXTextureBase();
	~DXTextureBase();

public:
	ComPtr<ID3D11Texture2D> GetResource()
	{
		return _mTexture;
	}
	ComPtr<ID3D11ShaderResourceView> GetSRV()
	{
		return _mSRV;
	}
	D3D11_TEXTURE2D_DESC GetDesc() const
	{
		D3D11_TEXTURE2D_DESC desc;
		_mTexture->GetDesc(&desc);
		return desc;
	}

public:
	uint32 GetWidth() const
	{
		return _mWidth;
	}
	uint32 GetHeight() const
	{
		return _mHeight;
	}
	IntPoint<uint32> GetWidthHeight() const
	{
		return IntPoint<uint32>(_mWidth, _mHeight);
	}

public:
	uint32 GetSRVSlot() const
	{
		return _mSRVSlot;
	}
	DXResourceUsageFlag::Type GetSRVUsageFlags() const
	{
		return _mSRVUsageFlags;
	}

	void SetSRVSlot(uint32 slot) const
	{
		_mSRVSlot = slot;
	}
	void SetSRVUsageFlags(DXResourceUsageFlag::Type flags) const
	{
		_mSRVUsageFlags = flags;
	}

public:
	virtual void PushData() const;

protected:
	ComPtr<ID3D11Texture2D> _mTexture;
	ComPtr<ID3D11ShaderResourceView> _mSRV;

	uint32 _mWidth = 0u;
	uint32 _mHeight = 0u;
	uint32 _mBindFlags = 0u;

	/* 비고정적인 부분 */
protected:
	mutable uint32 _mSRVSlot = 0u;
	mutable DXResourceUsageFlag::Type _mSRVUsageFlags = DXResourceUsageFlag::None;
};

/**
 * 2D 텍스처 베이스 클래스
 */
class DXTexture2DBase abstract : public DXTextureBase
{
	friend class ResourceManager;

protected:
	DXTexture2DBase();
	~DXTexture2DBase();

protected:
	void InitImmutableTex(const void* initData, uint32 width, uint32 height, DXGI_FORMAT format);
	void InitImmutableTex(std::shared_ptr<Texture2DBulkData> bulkData);
	void InitDefaultTex(const void* initData, uint32 width, uint32 height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlags);
	void InitDefaultTex(std::shared_ptr<Texture2DBulkData> bulkData, D3D11_BIND_FLAG bindFlags);
};

/**
 * 읽기 전용 2D 텍스처 어댑터 클래스
 */
class DXConstTexture2D : public DXTexture2DBase
{
public:
	DXConstTexture2D();
	~DXConstTexture2D();

public:
	void Init(const void* initData, uint32 width, uint32 height, DXGI_FORMAT format, uint32 slot, DXResourceUsageFlag::Type usageFlags);
	void Init(std::shared_ptr<Texture2DBulkData> bulkData, uint32 slot, DXResourceUsageFlag::Type usageFlags);
};

/**
 * 기본 2D 텍스처 어댑터 클래스
 */
class DXTexture2D : public DXTexture2DBase
{
	friend class ResourceManager;

protected:
	DXTexture2D();
	~DXTexture2D();

public:
	void Init(ComPtr<ID3D11ShaderResourceView> initSRVData, D3D11_BIND_FLAG bindFlags, uint32 slot, DXResourceUsageFlag::Type usageFlags);
	void Init(const void* initData, uint32 width, uint32 height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlags, uint32 slot, DXResourceUsageFlag::Type usageFlags);
	void Init(std::shared_ptr<Texture2DBulkData> bulkData, D3D11_BIND_FLAG bindFlags, uint32 slot, DXResourceUsageFlag::Type usageFlags);
};

/**
 * 2D 텍스처 배열 베이스 클래스
 */
class DXTexture2DArrayBase abstract : public DXTextureBase
{
	friend class ResourceManager;

protected:
	DXTexture2DArrayBase();
	~DXTexture2DArrayBase();

public:
	uint32 GetArraySize() const
	{
		return _mArraySize;
	}

protected:
	void InitImmutableTex(const void* initData, uint32 arraySize, uint32 width, uint32 height, DXGI_FORMAT format);
	void InitImmutableTex(std::shared_ptr<Texture2DBulkData> bulkData);
	void InitDefaultTex(const void* initData, uint32 arraySize, uint32 width, uint32 height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlags);
	void InitDefaultTex(std::shared_ptr<Texture2DBulkData> bulkData, D3D11_BIND_FLAG bindFlags);

protected:
	uint32 _mArraySize;
};

/**
 * 기본 2D 텍스처 배열 어댑터 클래스
 */
class DXTexture2DArray : public DXTexture2DArrayBase
{
	friend class ResourceManager;

protected:
	DXTexture2DArray();
	~DXTexture2DArray();

public:
	void Init(ComPtr<ID3D11ShaderResourceView> initSRVData, D3D11_BIND_FLAG bindFlags, uint32 slot, DXResourceUsageFlag::Type usageFlags);
	void Init(const void* initData, uint32 arraySize, uint32 width, uint32 height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlags, uint32 slot, DXResourceUsageFlag::Type usageFlags);
	void Init(std::shared_ptr<Texture2DBulkData> bulkData, D3D11_BIND_FLAG bindFlags, uint32 slot, DXResourceUsageFlag::Type usageFlags);
};

/**
 * 읽기 전용 2D 텍스처 배열 어댑터 클래스
 */
class DXConstTexture2DArray : public DXTexture2DArrayBase
{
	friend class ResourceManager;

protected:
	DXConstTexture2DArray();
	~DXConstTexture2DArray();

public:
	void Init(const void* initData, uint32 arraySize, uint32 width, uint32 height, DXGI_FORMAT format, uint32 slot, DXResourceUsageFlag::Type usageFlags);
	void Init(std::shared_ptr<Texture2DBulkData> bulkData, uint32 slot, DXResourceUsageFlag::Type usageFlags);
};