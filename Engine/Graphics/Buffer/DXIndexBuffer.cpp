#include "pch.h"
#include "DXIndexBuffer.h"

#include "Manager/DXGraphicSystem.h"

DXIndexBuffer::DXIndexBuffer()
{
}

DXIndexBuffer::~DXIndexBuffer()
{
}

void DXIndexBuffer::Init(std::shared_ptr<IndexBulkData> bulkData, bool canCpuWrite, uint32 offset)
{
	const std::vector<uint32>& indices = bulkData->mValue;

	Init(indices, canCpuWrite, offset);
}

void DXIndexBuffer::Init(std::vector<uint32> indices, bool canCpuWrite, uint32 offset)
{
	_mStride = sizeof(uint32);
	_mCount = static_cast<uint32>(indices.size());

	_mResourceFlags = (canCpuWrite == true ? DXResourceFlag::Updatable : DXResourceFlag::None);
	_mOffset = offset;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	//ZeroMemory(&desc, size(desc));

	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;		// 입력 어셈블러(사용자 데이터 정리)에서 인덱스 버퍼
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
	data.pSysMem = indices.data();

	// 디바이스야! GPU에 메모리 할당 공간 만들어줘~
	CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, &data, _mIndexBuffer.GetAddressOf()), "Index buffer creation is failed");
}

void DXIndexBuffer::PushData() const
{
	DX_DEVICE_CONTEXT->IASetIndexBuffer(_mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, _mOffset);
}

bool DXIndexBuffer::UpdateData(std::shared_ptr<IndexBulkData> bulkData) const
{
	if (IsUpdatable() == false)
	{
		return false;
	}

	const std::vector<uint32>& indices = bulkData->mValue;
	std::size_t indicesSize = indices.size() * sizeof(uint32);

	D3D11_MAPPED_SUBRESOURCE subResource;
	memset(&subResource, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

	DX_DEVICE_CONTEXT->Map(
		_mIndexBuffer.Get(),
		0,							// 하위 리소스 인덱스 번호
		D3D11_MAP_WRITE_DISCARD,	// CPU 읽기 쓰기 권한에 대한 타입
		0,							// GPU가 사용 중일 때, CPU의 작업 권한 플래그
		&subResource				// 하위 리소스 데이터들 (버퍼를 통해 전달될 데이터)
	);

	// 전달 데이터를 할당
	::memcpy(
		subResource.pData,
		indices.data(),
		indicesSize
	);

	DX_DEVICE_CONTEXT->Unmap(
		_mIndexBuffer.Get(),
		0							// 하위 리소스 인덱스 번호
	);

	return true;
}

