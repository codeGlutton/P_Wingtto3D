#pragma once

#include "Graphics/Resource/DXResource.h"
#include "Core/Resource/BulkData.h"
#include "Manager/DXGraphicSystem.h"

// CBUFFER_GLOBAL_PREFIX = "G_";
// CBUFFER_FRAME_PREFIX = "F_";
// CBUFFER_MAT_PREFIX = "M_";
// CBUFFER_OBJ_PREFIX = "O_";

struct ConstantBufferBulkData : public BulkData
{
	GEN_STRUCT_REFLECTION(ConstantBufferBulkData)

public:
	PROPERTY(mSize)
	std::size_t mSize;
	PROPERTY(mRawBytes)
	std::vector<uint8> mRawBytes;
};

class DXConstantBuffer : public DXResource
{
public:
	DXConstantBuffer()
	{
	}
	~DXConstantBuffer()
	{
	}

public:
	ComPtr<ID3D11Buffer> GetBuffer()
	{
		return _mConstantBuffer;
	}
	uint32 GetSlot() const
	{
		return _mSlot;
	}
	DXResourceUsageFlag::Type GetUsageFlags() const
	{
		return _mUsageFlags;
	}

	void SetSlot(uint32 slot) const
	{
		_mSlot = slot;
	}
	void SetUsageFlags(DXResourceUsageFlag::Type flags) const
	{
		_mUsageFlags = flags;
	}

public:
	bool CanBeUsedFor(DXResourceUsageFlag::Type flag) const
	{
		return _mUsageFlags & flag;
	}

public:
	void Init(std::shared_ptr<ConstantBufferBulkData> bulkData, uint32 slot = 0, DXResourceUsageFlag::Type usageFlags = DXResourceUsageFlag::None, bool canCpuWrite = false);
	void Init(std::size_t stride, uint32 slot = 0, DXResourceUsageFlag::Type usageFlags = DXResourceUsageFlag::None, bool canCpuWrite = false);
	void Init(const void* initData, std::size_t stride, uint32 slot = 0, DXResourceUsageFlag::Type usageFlags = DXResourceUsageFlag::None, bool canCpuWrite = false);
	bool UpdateData(const void* data, std::size_t stride) const;
	void PushData() const;

protected:
	ComPtr<ID3D11Buffer> _mConstantBuffer;

protected:
	mutable uint32 _mSlot;
	mutable DXResourceUsageFlag::Type _mUsageFlags;
};

/**
 * 상수 버퍼 Wrapper 클래스
 */
template<typename T>
class DXConstantBufferTemplate : public DXConstantBuffer
{
public:
	DXConstantBufferTemplate()
	{
	}
	~DXConstantBufferTemplate() 
	{
	}

public:
	uint32 GetStride() const
	{
		return sizeof(T);
	}

public:
	void Init(std::shared_ptr<ConstantBufferBulkData> bulkData, uint32 slot = 0, DXResourceUsageFlag::Type usageFlags = DXResourceUsageFlag::None, bool canCpuWrite = false);
	void Init(uint32 slot = 0, DXResourceUsageFlag::Type usageFlags = DXResourceUsageFlag::None, bool canCpuWrite = false);
	void Init(const T& initData, uint32 slot = 0, DXResourceUsageFlag::Type usageFlags = DXResourceUsageFlag::None, bool canCpuWrite = false);
	bool UpdateData(const T& data) const;
};

template<typename T>
inline void DXConstantBufferTemplate<T>::Init(std::shared_ptr<ConstantBufferBulkData> bulkData, uint32 slot, DXResourceUsageFlag::Type usageFlags, bool canCpuWrite)
{
	ASSERT_MSG(bulkData->mSize == sizeof(T), "Bulk data has other type data");
	DXConstantBuffer::Init(bulkData, slot, usageFlags, canCpuWrite);
}

template<typename T>
inline void DXConstantBufferTemplate<T>::Init(uint32 slot, DXResourceUsageFlag::Type usageFlags, bool canCpuWrite)
{
	DXConstantBuffer::Init(GetStride(), slot, usageFlags, canCpuWrite);
}

template<typename T>
inline void DXConstantBufferTemplate<T>::Init(const T& initData, uint32 slot, DXResourceUsageFlag::Type usageFlags, bool canCpuWrite)
{
	DXConstantBuffer::Init(&initData, GetStride(), slot, usageFlags, canCpuWrite);
}

template<typename T>
inline bool DXConstantBufferTemplate<T>::UpdateData(const T& data) const
{
	return DXConstantBuffer::UpdateData(&data, GetStride());
}