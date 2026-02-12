#include "pch.h"
#include "Material.h"

#include "Manager/ResourceManager.h"

Material::Material()
{
	_mBulkData = std::make_shared<MaterialBulkData>();
}

Material::~Material()
{
}

void Material::PostCreate()
{
	Super::PostCreate();
	_mProxy = CreateRefCounting<std::shared_ptr<DXMaterial>, MainThreadType::Render>();
}

void Material::PostLoad()
{
	Super::PostLoad();

	_mProxy->mData = RESOURCE_MANAGER->CreateRenderResource<DXMaterial>(GetName(), GetFullPath());
	_mNeedInitProxy = true;
}

const Material::ProxyType& Material::GetProxy() const
{
	if (_mNeedInitProxy == true)
	{
		_mNeedInitProxy = false;

		std::vector<DXMaterial::ConstantBufferData> bufferDatas;
		for (auto& matBufferData : _mMatConstantDatas)
		{
			std::shared_ptr<ConstantBufferBulkData> buffer = matBufferData.second->_mBulkData;
			bufferDatas.push_back({ matBufferData.first, buffer });
		}
		std::vector<DXMaterial::TextureBinding> textureBindings;
		for (auto& textureData : _mBoundTextures)
		{
			std::shared_ptr<DXTextureBase> texture = textureData.second->GetProxy()->mData;
			textureBindings.push_back({ textureData.first, texture });
		}

		THREAD_MANAGER->PushRenderThreadLogicUpdateJob(ObjectPool<Job>::MakeShared(
			[bulkData = _mBulkData, renderResource = _mProxy->mData, bufferBindings = std::move(bufferDatas), textureBindings = std::move(textureBindings)]() {
				renderResource->Init(bulkData, bufferBindings, textureBindings);
			})
		);
	}
	return _mProxy;
}