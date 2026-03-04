#include "pch.h"
#include "DXInputLayout.h"

#include "Manager/DXGraphicSystem.h"

#include "Graphics/Resource/DXShader.h"

DXInputLayout::DXInputLayout()
{
}

DXInputLayout::~DXInputLayout()
{
}

void DXInputLayout::Init(const std::vector<D3D11_INPUT_ELEMENT_DESC>& descs, const DXVertexShader& shader)
{
	if (descs.empty() == true)
	{
		_mLayout.Reset();
		return;
	}

	// 데이터 바이트 구성이 어떻게 되어 있는지를 알림
	CHECK_WIN_MSG(DX_DEVICE->CreateInputLayout(
		descs.data(), 
		static_cast<int32>(descs.size()),
		shader._mBlob->GetBufferPointer(), 
		shader._mBlob->GetBufferSize(),
		_mLayout.ReleaseAndGetAddressOf()
	), "Input layout creation is failed");
}

void DXInputLayout::PushData() const
{
	DX_DEVICE_CONTEXT->IASetInputLayout(_mLayout.Get());
}