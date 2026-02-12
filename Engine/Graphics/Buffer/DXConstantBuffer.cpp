#include "pch.h"
#include "DXConstantBuffer.h"

void DXConstantBuffer::Init(std::shared_ptr<ConstantBufferBulkData> bulkData, uint32 slot, DXResourceUsageFlag::Type usageFlags, bool canCpuWrite)
{
	ASSERT_MSG(bulkData != nullptr, "Null bulk data can't be allowed to initialize constant buffer");

	_mSlot = slot;
	_mUsageFlags = usageFlags;
	_mResourceFlags = (canCpuWrite == true ? DXResourceFlag::Updatable : DXResourceFlag::None);

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	//ZeroMemory(&desc, sizeof(desc));

	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;				// 셰이더에서 상수 버퍼 용도
	desc.ByteWidth = static_cast<uint32>(bulkData->mSize);		// T 구초제로 할당될 상수 버퍼

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
	data.pSysMem = bulkData->mRawBytes.data();

	CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, &data, _mConstantBuffer.GetAddressOf()), "Constant buffer creation is failed");
}

void DXConstantBuffer::Init(std::size_t stride, uint32 slot, DXResourceUsageFlag::Type usageFlags, bool canCpuWrite)
{
	_mSlot = slot;
	_mUsageFlags = usageFlags;
	_mResourceFlags = (canCpuWrite == true ? DXResourceFlag::Updatable : DXResourceFlag::None);

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	//ZeroMemory(&desc, sizeof(desc));

	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;				// 셰이더에서 상수 버퍼 용도
	desc.ByteWidth = static_cast<uint32>(stride);				// T 구초제로 할당될 상수 버퍼

	if (canCpuWrite == true)
	{
		// GPU는 읽고, CPU가 중간에 Map으로 고침

		desc.Usage = D3D11_USAGE_DYNAMIC; // cpu 쓰기 가능
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		// Create 당시 데이터로 그대로 GPU 사용

		FAIL_MSG("Invalid constant buffer type. Null data immutable buffer is useless");

		desc.Usage = D3D11_USAGE_IMMUTABLE; // GPU만 읽을 수 있음
	}

	CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, nullptr, _mConstantBuffer.GetAddressOf()), "Constant buffer creation is failed");
}

inline void DXConstantBuffer::Init(const void* initData, std::size_t stride, uint32 slot, DXResourceUsageFlag::Type usageFlags, bool canCpuWrite)
{
	_mSlot = slot;
	_mUsageFlags = usageFlags;
	_mResourceFlags = (canCpuWrite == true ? DXResourceFlag::Updatable : DXResourceFlag::None);

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	//ZeroMemory(&desc, sizeof(desc));

	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;				// 셰이더에서 상수 버퍼 용도
	desc.ByteWidth = static_cast<uint32>(stride);				// T 구초제로 할당될 상수 버퍼

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
	data.pSysMem = initData;

	CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, &data, _mConstantBuffer.GetAddressOf()), "Constant buffer creation is failed");
}

inline bool DXConstantBuffer::UpdateData(const void* data, std::size_t stride) const
{
	if (IsUpdatable() == false)
	{
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE subResource;
	memset(&subResource, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

	DX_DEVICE_CONTEXT->Map(
		_mConstantBuffer.Get(),
		0,							// 하위 리소스 인덱스 번호
		D3D11_MAP_WRITE_DISCARD,	// CPU 읽기 쓰기 권한에 대한 타입
		0,							// GPU가 사용 중일 때, CPU의 작업 권한 플래그
		&subResource				// 하위 리소스 데이터들 (버퍼를 통해 전달될 데이터)
	);

	// 전달 데이터를 할당
	::memcpy(
		subResource.pData,
		data,
		sizeof(stride)
	);

	DX_DEVICE_CONTEXT->Unmap(
		_mConstantBuffer.Get(),
		0							// 하위 리소스 인덱스 번호
	);

	return true;
}

void DXConstantBuffer::PushData() const
{
	if (_mUsageFlags & DXResourceUsageFlag::Vertex)
	{
		DX_DEVICE_CONTEXT->VSSetConstantBuffers(_mSlot, 1, _mConstantBuffer.GetAddressOf());
	}

	if (_mUsageFlags & DXResourceUsageFlag::Domain)
	{
		DX_DEVICE_CONTEXT->DSSetConstantBuffers(_mSlot, 1, _mConstantBuffer.GetAddressOf());
	}

	if (_mUsageFlags & DXResourceUsageFlag::Hull)
	{
		DX_DEVICE_CONTEXT->HSSetConstantBuffers(_mSlot, 1, _mConstantBuffer.GetAddressOf());
	}

	if (_mUsageFlags & DXResourceUsageFlag::Geometry)
	{
		DX_DEVICE_CONTEXT->GSSetConstantBuffers(_mSlot, 1, _mConstantBuffer.GetAddressOf());
	}

	if (_mUsageFlags & DXResourceUsageFlag::Pixel)
	{
		DX_DEVICE_CONTEXT->PSSetConstantBuffers(_mSlot, 1, _mConstantBuffer.GetAddressOf());
	}

	if (_mUsageFlags & DXResourceUsageFlag::Compute)
	{
		DX_DEVICE_CONTEXT->CSSetConstantBuffers(_mSlot, 1, _mConstantBuffer.GetAddressOf());
	}
}