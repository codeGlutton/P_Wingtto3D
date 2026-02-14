#include "pch.h"
#include "Editor.h"

#include "Manager/WorldGameInputManager.h"
#include "Input/DefaultEditorInputMappingContext.h"

#include "Manager/PackageManager.h"
#include "Core/Resource/Package/Package.h"

#include "Manager/PathManager.h"
#include "Manager/ResourceManager.h"
#include "Core/Resource/Resource.h"
#include "Core/Resource/Texture.h"
#include "Core/Resource/Material.h"

Editor::Editor()
{
}

Editor::~Editor()
{
}

void Editor::Init()
{
	AppModeBase::Init();
}

void Editor::Update()
{
	AppModeBase::Update();
}

void Editor::End()
{
	AppModeBase::End();
}

void Editor::BeginThread()
{
	WORLD_INPUT_MANAGER->AddMappingContext(&DefaultEditorInputMappingContext::GetStaticTypeInfo(), 0);

	AppModeBase::BeginThread();
}

void Editor::RegisterDefaultResources()
{
	std::wstring packagePath = PATH_MANAGER->GetEngineResourceFolderName();

	/* 메테리얼 */

	// UI
	{
		std::wstring resourcePath = packagePath + L"\\M_UI";
		std::shared_ptr<Material> matUI = RESOURCE_MANAGER->LoadOrGetResource<Material>(resourcePath);

		// 없다면 만들어주기
		if (matUI == nullptr)
		{
			DEBUG_LOG("Create UI material to %s", ConvertWStringToUtf8(resourcePath).c_str());

			std::shared_ptr<ResourcePackage> package = PACKAGE_MANAGER->LoadPackage<ResourcePackage>(resourcePath);
			matUI = RESOURCE_MANAGER->CreateResource<Material>(package);

			std::shared_ptr<MaterialBulkData> bulkData = std::make_shared<MaterialBulkData>();
			bulkData->mSamplerStateDatas.push_back({ "linearWrapSS" , L"LinearWrap" });
			bulkData->mBlendStateName = L"DefaultAlpha";
			bulkData->mRasterizerStateName = L"BackCulling";
			bulkData->mShaderName = L"UI";
			matUI->UpdateProxy(
				bulkData,
				std::vector<std::pair<std::string, std::shared_ptr<ConstantDataBase>>>(),
				std::vector<std::pair<std::string, std::shared_ptr<Texture2D>>>()
			);
		}
		matUI->GetProxy();
		_mDefaultResources.push_back(matUI);
	}
}

void Editor::UnregisterDefaultResources()
{
	_mDefaultResources.clear();
}

