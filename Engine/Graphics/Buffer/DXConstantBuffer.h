#pragma once

#include "Manager/DXGraphicSystem.h"

/**
 * 상수 버퍼 Wrapper 클래스
 */
template<typename T>
class DXConstantBuffer : public std::enable_shared_from_this<DXConstantBuffer<T>>
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
	uint32 GetStride() const
	{
		return sizeof(T);
	}

public:
	void Init();
	void Init(const T& initData);
	void PushData(const T& data);

private:
	ComPtr<ID3D11Buffer> _mConstantBuffer;
};

template<typename T>
inline void DXConstantBuffer<T>::Init()
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	ZeroMemory(&desc, sizeof(desc));

	desc.Usage = D3D11_USAGE_DYNAMIC;				// GPU는 읽을 수만 있고 CPU는 쓸 수만 있다
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 셰이더에서 상수 버퍼 용도
	desc.ByteWidth = sizeof(T);						// T 구초제로 할당될 상수 버퍼
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU 접근 플래그 설정

	CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, nullptr, _mConstantBuffer.GetAddressOf()), "Constant buffer creation is failed");
}

template<typename T>
inline void DXConstantBuffer<T>::Init(const T& initData)
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	ZeroMemory(&desc, sizeof(desc));

	desc.Usage = D3D11_USAGE_DYNAMIC;				// GPU는 읽을 수만 있고 CPU는 쓸 수만 있다
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 셰이더에서 상수 버퍼 용도
	desc.ByteWidth = sizeof(T);						// T 구초제로 할당될 상수 버퍼
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU 접근 플래그 설정

	// 관련 CPU 데이터 연결 (GPU에 복제됨)
	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));
	data.pSysMem = &initData;

	CHECK_WIN_MSG(DX_DEVICE->CreateBuffer(&desc, &data, _mConstantBuffer.GetAddressOf()), "Constant buffer creation is failed");
}

template<typename T>
inline void DXConstantBuffer<T>::PushData(const T& data)
{
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
		&data,
		sizeof(data)
	);

	DX_DEVICE_CONTEXT->Unmap(
		_mConstantBuffer.Get(),
		0							// 하위 리소스 인덱스 번호
	);
}

