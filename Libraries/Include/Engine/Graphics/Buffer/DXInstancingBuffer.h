#pragma once

#include "Graphics/Buffer/DXVertexBuffer.h"
#include "Manager/DXGraphicSystem.h"

#define MAX_MESH_INSTANCING 500

template<typename T>
using InstancingBulkData = BulkWrapper<std::vector<T>>;

/**
 * 매 프레임마다 인스턴스 객체 정보를 전달할 버퍼
 */
class DXInstancingBufferBase abstract : public std::enable_shared_from_this<DXInstancingBufferBase>
{
public:
	DXInstancingBufferBase();
	~DXInstancingBufferBase();

public:
	ComPtr<ID3D11Buffer> GetBuffer()
	{
		return _mInstancingBuffer->GetBuffer();
	}
	uint32 GetStride() const
	{
		return _mInstancingBuffer->GetStride();
	}
	uint32 GetOffset() const
	{
		return _mInstancingBuffer->GetOffset();
	}
	uint32 GetCount() const
	{
		return _mInstancingBuffer->GetCount();
	}
	uint32 GetSlot() const
	{
		return _mInstancingBuffer->GetSlot();
	}
	bool IsDirty() const
	{
		return _mIsDirty;
	}

public:
	virtual void PushData();

protected:
	std::shared_ptr<DXVertexBuffer> _mInstancingBuffer;
	bool _mIsDirty = false;
};

template<typename T>
class DXInstancingBuffer : public DXInstancingBufferBase
{
public:
	uint32 GetInstancingCount() const
	{
		return _mInstancingData->mValue.size();
	}

public:
	/**
	 * 인스턴싱 버퍼 초기화 함수
	 * \param bulkDataOrigin 벌크 데이터의 원형 복제본 요구 (비복제본은 에디터에서 변경될 여지 있음)
	 * \param slot 인덱싱 슬롯
	 * \param offset 시작 오프셋
	 */
	void Init(std::shared_ptr<InstancingBulkData<T>> bulkDataOrigin, uint32 slot = 0, uint32 offset = 0);

public:
	void AddInstancing(const T& data, bool forcedUpdateBuffer = false);
	void RemoveInstancing(const T& data, bool forcedUpdateBuffer = false);

public:
	virtual void PushData() override;
	void ClearData(bool forcedUpdateBuffer = false);

private:
	void Refresh();

private:
	std::shared_ptr<InstancingBulkData<T>> _mInstancingData;
};

template<typename T>
inline void DXInstancingBuffer<T>::Init(std::shared_ptr<InstancingBulkData<T>> bulkDataOrigin, uint32 slot, uint32 offset)
{
	_mInstancingData = bulkDataOrigin;
	_mIsDirty = false;

	_mInstancingBuffer = std::make_shared<DXVertexBuffer>();
	_mInstancingBuffer->Init(bulkDataOrigin, slot, true, false, offset);
}

template<typename T>
inline void DXInstancingBuffer<T>::AddInstancing(const T& data, bool forcedUpdateBuffer)
{
	_mInstancingData->mValue.push_back(data);
	if (GetCount() != GetInstancingCount())
	{
		_mIsDirty = true;
	}
	if (forcedUpdateBuffer == true)
	{
		Refresh();
	}
}

template<typename T>
inline void DXInstancingBuffer<T>::RemoveInstancing(const T& data, bool forcedUpdateBuffer)
{
	std::vector<T>& instancing = _mInstancingData->mValue;
	instancing.erase(std::remove(instancing.begin(), instancing.end(), data), instancing.end());

	if (GetCount() != GetInstancingCount())
	{
		_mIsDirty = true;
	}
	if (forcedUpdateBuffer == true)
	{
		Refresh();
	}
}

template<typename T>
inline void DXInstancingBuffer<T>::PushData()
{
	if (IsDirty() == true)
	{
		Refresh();
	}

	D3D11_MAPPED_SUBRESOURCE subResource;
	DX_DEVICE_CONTEXT->Map(_mInstancingBuffer->GetBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		::memcpy(subResource.pData, _mInstancingData->mValue.data(), sizeof(T) * GetCount());
	}
	DX_DEVICE_CONTEXT->Unmap(_mInstancingBuffer->GetBuffer().Get(), 0);
	_mInstancingBuffer->PushData();
}

template<typename T>
inline void DXInstancingBuffer<T>::ClearData(bool forcedUpdateBuffer)
{
	_mInstancingData->mValue.clear();
	if (forcedUpdateBuffer == true)
	{
		Refresh();
	}
}

template<typename T>
inline void DXInstancingBuffer<T>::Refresh()
{
	Init(_mInstancingData, GetSlot(), GetOffset());
}