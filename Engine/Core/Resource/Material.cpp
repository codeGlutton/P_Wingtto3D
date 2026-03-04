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

	_mProxy->mData = RESOURCE_MANAGER->CreateRenderResource<DXMaterial>(GetName(), GetPath());
	_mNeedInitProxy = true;
}

bool Material::HasBulkData() const
{
	return _mBulkData->mIsLoad;
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

#ifdef _EDITOR

void Material::PushBulkData(std::shared_ptr<MaterialBulkData> bulkData)
{
	ASSERT_MSG(HasBulkData() == false, "Already bulk data is existed");

	_mBulkData = bulkData;
	PostLoad();
}

void Material::UpdateProxy(std::shared_ptr<MaterialBulkData> newBulkData, const std::vector<std::pair<std::string, std::shared_ptr<ConstantDataBase>>>& newMatConstantDatas, const std::vector<std::pair<std::string, std::shared_ptr<Texture2D>>>& newBoundTextures)
{
	_mBulkData = newBulkData;
	_mMatConstantDatas = newMatConstantDatas;
	_mBoundTextures = newBoundTextures;

	_mNeedInitProxy = true;
}

#endif
