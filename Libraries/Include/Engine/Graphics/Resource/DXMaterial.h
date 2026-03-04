#pragma once

#include "Graphics/Resource/DXResource.h"
#include "Core/Resource/BulkData.h"

#include "Graphics/Buffer/DXConstantBuffer.h"
#include "Graphics/Resource/DXShader.h"

class DXTextureBase;
class DXSamplerState;
class DXRasterizerState;
class DXBlendState;
class DXMinimumGraphicShader;

struct MaterialBulkData;

class DXMaterial : public DXSharedResource
{
	friend class ResourceManager;

public:
	using BindingName = std::string;
	using ResourceName = std::wstring;

	using ConstantBufferData = std::pair<BindingName, std::shared_ptr<ConstantBufferBulkData>>;
	using SamplerStateData = std::pair<BindingName, ResourceName>;

	using ConstantBufferBinding = std::pair<BindingName, std::shared_ptr<DXConstantBuffer>>;
	using TextureBinding = std::pair<BindingName, std::shared_ptr<DXTextureBase>>;
	using SamplerStateBinding = std::pair<BindingName, std::shared_ptr<DXSamplerState>>;

protected:
	DXMaterial();
	~DXMaterial();

public:
	void Init(std::shared_ptr<MaterialBulkData> bulkData, std::vector<ConstantBufferData> bufferBulkDatas, std::vector<TextureBinding> textures);
	void PushData() const;

public:
	template<typename T> requires std::is_base_of_v<DXResource, T>
	void PushTransientData(const std::string& name, std::shared_ptr<T> resource) const;

private:
	std::vector<ConstantBufferBinding> _mBoundMatCBuffers;			// 직접 전달
	std::vector<TextureBinding> _mBoundTextures;					// 직접 전달
	std::vector<SamplerStateBinding> _mBoundSamplerStates;			// 직접 전달

	std::shared_ptr<DXRasterizerState> _mRasterizerState;			// 미리 등록된 객체 사용
	std::shared_ptr<DXBlendState> _mBlendState;						// 미리 등록된 객체 사용
	std::shared_ptr<DXMinimumGraphicShader> _mShader;				// 미리 등록된 객체 사용
};

struct MaterialBulkData : public BulkData
{
	GEN_BULK_STRUCT_REFLECTION(MaterialBulkData)

public:
	PROPERTY(mSamplerStateDatas)
	std::vector<DXMaterial::SamplerStateData> mSamplerStateDatas;

public:
	PROPERTY(mRasterizerStateName)
	DXMaterial::ResourceName mRasterizerStateName;
	PROPERTY(mBlendStateName)
	DXMaterial::ResourceName mBlendStateName;
	PROPERTY(mShaderName)
	DXMaterial::ResourceName mShaderName;
};

template<typename T> requires std::is_base_of_v<DXResource, T>
inline void DXMaterial::PushTransientData(const std::string& name, std::shared_ptr<T> resource) const
{
	_mShader->Bind(name, resource);
}
