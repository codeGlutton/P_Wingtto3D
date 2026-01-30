#pragma once

#include "Core/Resource/BulkData.h"
#include "Manager/DXGraphicSystem.h"

template<typename T>
using VertexBulkData = BulkWrapper<std::vector<T>>;

/**
 * 정점 버퍼 Wrapper 클래스
 */
class DXVertexBuffer : public std::enable_shared_from_this<DXVertexBuffer>
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

public:
	// 여러 형태의 Vertex 구조체를 호환해야 하기에
	// 템플릿으로 정의
	template<typename T>
	void Init(std::shared_ptr<VertexBulkData<T>> bulkData, uint32 slot = 0, bool cpuWrite = false, bool gpuWrite = false, uint32 offset = 0);

public:
	void PushData();

private:
	ComPtr<ID3D11Buffer> _mVertexBuffer;

	uint32 _mStride = 0;
	uint32 _mOffset = 0;
	uint32 _mCount = 0;

	uint32 _mSlot = 0;
	bool _mCpuWrite = false;
	bool _mGpuWrite = false;
};

template<typename T>
void DXVertexBuffer::Init(std::shared_ptr<VertexBulkData<T>> bulkData, uint32 slot, bool cpuWrite, bool gpuWrite, uint32 offset)
{
	const std::vector<T>& vertices = bulkData->mValue;

	_mStride = sizeof(T);
	_mCount = static_cast<uint32>(vertices.size());

	_mSlot = slot;
	_mCpuWrite = cpuWrite;
	_mGpuWrite = gpuWrite;
	_mOffset = offset;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;		// 정점 버퍼로 바인딩
	desc.ByteWidth = (uint32)(_mStride * _mCount); // 할당할 메모리 크기

	if (cpuWrite == false && gpuWrite == false)
	{
		desc.Usage = D3D11_USAGE_IMMUTABLE; // GPU만 읽을 수 있음
	}
	else if (cpuWrite == true && gpuWrite == false)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC; // cpu 쓰기 가능
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (cpuWrite == false && gpuWrite == true)
	{
		desc.Usage = D3D11_USAGE_DEFAULT; // GPU만 쓰기 가능
	}
	else
	{
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	}

	// 관련 CPU 데이터 연결 (GPU에 복제됨)
	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));
	data.pSysMem = vertices.data();

	// 디바이스야! GPU에 메모리 할당 공간 만들어줘~
	CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, &data, _mVertexBuffer.GetAddressOf()), "Vertex buffer creation is failed");
}
