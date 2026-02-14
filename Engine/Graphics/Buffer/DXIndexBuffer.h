#pragma once

#include "Graphics/Resource/DXResource.h"
#include "Core/Resource/BulkData.h"

using IndexBulkData = BulkWrapper<std::vector<uint32>>;

/**
 * 인덱스 버퍼 Wrapper 클래스
 */
class DXIndexBuffer : public DXResource
{
public:
	DXIndexBuffer();
	~DXIndexBuffer();

public:
	ComPtr<ID3D11Buffer> GetBuffer() 
	{ 
		return _mIndexBuffer; 
	}
	uint32 GetStride() const 
	{ 
		return _mStride;
	}
	uint32 GetOffset() const 
	{ 
		return _mOffset;
	}
	uint32 GetCount() const
	{ 
		return _mCount; 
	}

	void SetOffset(uint32 offset) const
	{
		_mOffset = offset;
	}

public:
	void Init(std::shared_ptr<IndexBulkData> bulkData, bool canCpuWrite = false, uint32 offset = 0);
	void Init(const std::vector<uint32>& indices, bool canCpuWrite = false, uint32 offset = 0);
	void Init(uint32 count, uint32 offset = 0);

public:
	void PushData() const;
	bool UpdateData(std::shared_ptr<IndexBulkData> bulkData) const;
	bool UpdateData(const std::vector<uint32>& indices) const;

private:
	ComPtr<ID3D11Buffer> _mIndexBuffer;

private:
	uint32 _mStride = 0;
	uint32 _mCount = 0;

	mutable uint32 _mOffset = 0;
};