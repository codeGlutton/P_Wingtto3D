#pragma once

#include "Core/Resource/BulkData.h"
#include "Graphics/Resource/DXResource.h"

#include "Manager/DXGraphicSystem.h"

template<typename T>
using VertexBulkData = BulkWrapper<std::vector<T>>;

/**
 * 정점 버퍼 Wrapper 클래스
 */
class DXVertexBuffer : public DXResource
{
public:
	DXVertexBuffer();
	~DXVertexBuffer();

public:
	ComPtr<ID3D11Buffer> GetBuffer() 
	{ 
		return _mVertexBuffer;
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
	uint32 GetSlot() const 
	{ 
		return _mSlot;
	}

	void SetOffset(uint32 offset) const
	{
		_mOffset = offset;
	}
	void SetSlot(uint32 slot) const
	{
		_mSlot = slot;
	}

public:
	// 여러 형태의 Vertex 구조체를 호환해야 하기에
	// 템플릿으로 정의
	template<typename T>
	void Init(std::shared_ptr<VertexBulkData<T>> bulkData, uint32 slot = 0, bool canCpuWrite = false, uint32 offset = 0);
	template<typename T>
	void Init(const std::vector<T>& vertices, uint32 slot = 0, bool canCpuWrite = false, uint32 offset = 0);
	void Init(uint32 stride, uint32 count, uint32 slot = 0, uint32 offset = 0);

public:
	void PushData() const;

	template<typename T>
	bool UpdateData(std::shared_ptr<VertexBulkData<T>> bulkData) const;
	template<typename T>
	bool UpdateData(const std::vector<T>& vertices) const;

private:
	ComPtr<ID3D11Buffer> _mVertexBuffer;

private:
	uint32 _mStride = 0;
	uint32 _mCount = 0;

	mutable uint32 _mOffset = 0;
	mutable uint32 _mSlot = 0;
};

template<typename T>
void DXVertexBuffer::Init(std::shared_ptr<VertexBulkData<T>> bulkData, uint32 slot, bool canCpuWrite, uint32 offset)
{
	const std::vector<T>& vertices = bulkData->mValue;
	Init(vertices, slot, canCpuWrite, offset);
}

template<typename T>
inline void DXVertexBuffer::Init(const std::vector<T>& vertices, uint32 slot, bool canCpuWrite, uint32 offset)
{
	_mStride = sizeof(T);
	_mCount = static_cast<uint32>(vertices.size());

	_mSlot = slot;
	_mResourceFlags = (canCpuWrite == true ? DXResourceFlag::Updatable : DXResourceFlag::None);
	_mOffset = offset;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;		// 정점 버퍼로 바인딩
	desc.ByteWidth = (uint32)(_mStride * _mCount);	// 할당할 메모리 크기

	if (canCpuWrite == true)
	{
		// GPU는 읽고, CPU가 중간에 Map으로 고침

		desc.Usage = D3D11_USAGE_DYNAMIC; // cpu 쓰기 가능
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		// Create 당시 데이터로 그대로 GPU 사용

		desc.Usage = D3D11_USAGE_IMMUTABLE; // GPU만 읽을 수 있음
	}

	// 관련 CPU 데이터 연결 (GPU에 복제됨)
	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));
	data.pSysMem = vertices.data();

	// 디바이스야! GPU에 메모리 할당 공간 만들어줘~
	CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, &data, _mVertexBuffer.ReleaseAndGetAddressOf()), "Vertex buffer creation is failed");
}

template<typename T>
inline bool DXVertexBuffer::UpdateData(std::shared_ptr<VertexBulkData<T>> bulkData) const
{
	const std::vector<T>& vertices = bulkData->mValue;
	return UpdateData(vertices);
}

template<typename T>
inline bool DXVertexBuffer::UpdateData(const std::vector<T>& vertices) const
{
	if (IsUpdatable() == false)
	{
		return false;
	}

	std::size_t verticesSize = vertices.size() * sizeof(T);

	D3D11_MAPPED_SUBRESOURCE subResource;
	memset(&subResource, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

	DX_DEVICE_CONTEXT->Map(
		_mVertexBuffer.Get(),
		0,							// 하위 리소스 인덱스 번호
		D3D11_MAP_WRITE_DISCARD,	// CPU 읽기 쓰기 권한에 대한 타입
		0,							// GPU가 사용 중일 때, CPU의 작업 권한 플래그
		&subResource				// 하위 리소스 데이터들 (버퍼를 통해 전달될 데이터)
	);

	// 전달 데이터를 할당
	::memcpy(
		subResource.pData,
		vertices.data(),
		verticesSize
	);

	DX_DEVICE_CONTEXT->Unmap(
		_mVertexBuffer.Get(),
		0							// 하위 리소스 인덱스 번호
	);

	return true;
}
