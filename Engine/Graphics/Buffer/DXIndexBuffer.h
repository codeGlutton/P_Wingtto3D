#pragma once

#include "Core/Resource/BulkData.h"

using IndexBulkData = BulkWrapper<std::vector<uint32>>;

/**
 * 인덱스 버퍼 Wrapper 클래스
 */
class DXIndexBuffer : public std::enable_shared_from_this<DXIndexBuffer>
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

public:
	void Init(std::shared_ptr<IndexBulkData> bulkData, uint32 offset = 0);

public:
	void PushData();
	void PushData(uint32 offset);

private:
	ComPtr<ID3D11Buffer> _mIndexBuffer;

	uint32 _mStride = 0;
	uint32 _mOffset = 0;
	uint32 _mCount = 0;
};