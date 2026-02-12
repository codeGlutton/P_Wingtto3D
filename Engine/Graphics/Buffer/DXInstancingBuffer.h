#pragma once

#include "Graphics/Resource/DXResource.h"
#include "Graphics/Buffer/DXVertexBuffer.h"
#include "Manager/DXGraphicSystem.h"

#define MAX_MESH_INSTANCING 500

template<typename T>
using InstancingBulkData = BulkWrapper<std::vector<T>>;

/**
 * 매 프레임마다 인스턴스 객체 정보를 전달할 버퍼
 */
class DXInstancingBufferBase abstract : public DXResource
{
public:
	DXInstancingBufferBase();
	~DXInstancingBufferBase();

public:
	bool IsDirty() const
	{
		return _mIsDirty;
	}

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
	void SetSlot(uint32 slot)
	{
		_mInstancingBuffer->SetSlot(slot);
	}

public:
	virtual void PushData() const = 0;
	virtual bool UpdateData() const = 0;

protected:
	std::shared_ptr<DXVertexBuffer> _mInstancingBuffer;
	mutable bool _mIsDirty = false;
};

template<typename T>
class DXInstancingBuffer : public DXInstancingBufferBase
{
public:
	uint32 GetDataCount() const
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
	void Init(std::shared_ptr<InstancingBulkData<T>> bulkDataOrigin, uint32 slot = 0, uint32 offset = 0, bool canCpuWrite = true);

public:
	void AddInstancing(const T& data, bool forcedUpdateBuffer = false);
	void RemoveInstancing(const T& data, bool forcedUpdateBuffer = false);

public:
	virtual void PushData() const override;
	virtual bool UpdateData() const override;
	void ClearData(bool forcedUpdateBuffer = false);

private:
	void Refresh();

private:
	std::shared_ptr<InstancingBulkData<T>> _mInstancingData;
};

template<typename T>
inline void DXInstancingBuffer<T>::Init(std::shared_ptr<InstancingBulkData<T>> bulkDataOrigin, uint32 slot, uint32 offset, bool canCpuWrite)
{
	_mInstancingData = bulkDataOrigin;
	_mIsDirty = false;

	_mInstancingBuffer = std::make_shared<DXVertexBuffer>();
	_mInstancingBuffer->Init(bulkDataOrigin, slot, canCpuWrite, offset);
}

template<typename T>
inline void DXInstancingBuffer<T>::AddInstancing(const T& data, bool forcedUpdateBuffer)
{
	if (IsUpdatable() == false)
	{
		return;
	}

	_mInstancingData->mValue.push_back(data);

	_mIsDirty = GetCount() < GetDataCount();
	if (forcedUpdateBuffer == true)
	{
		Refresh();
	}
}

template<typename T>
inline void DXInstancingBuffer<T>::RemoveInstancing(const T& data, bool forcedUpdateBuffer)
{
	if (IsUpdatable() == false)
	{
		return;
	}

	std::vector<T>& instancing = _mInstancingData->mValue;
	instancing.erase(std::remove(instancing.begin(), instancing.end(), data), instancing.end());

	_mIsDirty = GetCount() < GetDataCount();
	if (forcedUpdateBuffer == true)
	{
		Refresh();
	}
}

template<typename T>
inline void DXInstancingBuffer<T>::PushData() const
{
	if (IsDirty() == true)
	{
		DXInstancingBuffer<T>* mutableThis = const_cast<DXInstancingBuffer<T>*>(this);
		mutableThis->Refresh();
	}

	_mInstancingBuffer->PushData();
}

template<typename T>
inline bool DXInstancingBuffer<T>::UpdateData() const
{
	if (IsUpdatable() == false)
	{
		return false;
	}

	if (IsDirty() == true)
	{
		DXInstancingBuffer<T>* mutableThis = const_cast<DXInstancingBuffer<T>*>(this);
		mutableThis->Refresh();

		_mInstancingBuffer->PushData();
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		DX_DEVICE_CONTEXT->Map(_mInstancingBuffer->GetBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			::memcpy(subResource.pData, _mInstancingData->mValue.data(), sizeof(T) * GetCount());
		}
		DX_DEVICE_CONTEXT->Unmap(_mInstancingBuffer->GetBuffer().Get(), 0);
	}
	
	return true;
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
	_mIsDirty = false;

	_mInstancingBuffer = std::make_shared<DXVertexBuffer>();
	_mInstancingBuffer->Init(_mInstancingData, GetSlot(), IsUpdatable(), false, GetOffset());
}