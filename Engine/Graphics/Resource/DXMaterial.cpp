#include "pch.h"
#include "DXMaterial.h"

#include "Manager/ResourceManager.h"

#include "Graphics/Resource/DXTexture.h"
#include "Graphics/State/DXSamplerState.h"
#include "Graphics/State/DXRasterizerState.h"
#include "Graphics/State/DXBlendState.h"
#include "Graphics/Resource/DXShader.h"

DXMaterial::DXMaterial()
{
	_mResourceFlags = DXResourceFlag::None;
	_mSharedResourceType = DXSharedResourceType::Material;
}

DXMaterial::~DXMaterial()
{
}

void DXMaterial::Init(std::shared_ptr<MaterialBulkData> bulkData, std::vector<ConstantBufferData> bufferBulkDatas, std::vector<TextureBinding> textures)
{
	_mBoundMatCBuffers.clear();
	_mBoundTextures.clear();
	_mBoundSamplerStates.clear();

	_mBoundTextures = std::move(textures);
	for (ConstantBufferData& bufferData : bufferBulkDatas)
	{
		std::shared_ptr<DXConstantBuffer> buffer = std::make_shared<DXConstantBuffer>();
		buffer->Init(bufferData.second);

		_mBoundMatCBuffers.push_back({ bufferData.first, buffer });
	}
	for (SamplerStateData& samplerData : bulkData->mSamplerStateDatas)
	{
		std::shared_ptr<DXSamplerState> sampler = RESOURCE_MANAGER->CreateOrGetTransientRenderResource<DXSamplerState>(samplerData.second, DXSharedResourceType::SamplerState);
		ASSERT_MSG(sampler != nullptr, "Can't find target resource");

		_mBoundSamplerStates.push_back({ samplerData.first, sampler });
	}

	_mRasterizerState = RESOURCE_MANAGER->CreateOrGetTransientRenderResource<DXRasterizerState>(bulkData->mRasterizerStateName, DXSharedResourceType::RasterizerState);
	ASSERT_MSG(_mRasterizerState != nullptr, "Can't find target resource");
	_mBlendState = RESOURCE_MANAGER->CreateOrGetTransientRenderResource<DXBlendState>(bulkData->mBlendStateName, DXSharedResourceType::BlendState);
	ASSERT_MSG(_mBlendState != nullptr, "Can't find target resource");
	_mShader = RESOURCE_MANAGER->CreateOrGetTransientRenderResource<DXMinimumGraphicShader>(bulkData->mBlendStateName, DXSharedResourceType::Shader);
	ASSERT_MSG(_mShader != nullptr, "Can't find target resource");
}

void DXMaterial::PushData() const
{
	for (auto& pair : _mBoundMatCBuffers)
	{
		_mShader->Bind(pair.first, pair.second);
	}
	for (auto& pair : _mBoundTextures)
	{
		_mShader->Bind(pair.first, pair.second);
	}
	for (auto& pair : _mBoundSamplerStates)
	{
		_mShader->Bind(pair.first, pair.second);
	}
	
	_mRasterizerState->PushData();
	_mBlendState->PushData();
}
