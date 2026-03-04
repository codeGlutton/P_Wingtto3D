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
#include "Core/Resource/Font.h"

#include "Convertor/Convertor.h"

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

void Editor::OnDropFile(const wchar_t* fileFullPath)
{
	std::filesystem::path path = fileFullPath;
	std::filesystem::path ext = path.extension();
	std::filesystem::path relativePath = path.lexically_relative(PATH_MANAGER->GetResourcePath());
	if (path.empty() == true)
	{
		EDITOR_LOG("Please load only file in resource folder!");
		return;
	}

	std::wstring extStr = ext.wstring();
	std::transform(extStr.begin(), extStr.end(), extStr.begin(), ::towlower);

	if (extStr == L".png" || extStr == L".tga" || extStr == L".dds" || extStr == L".jpg" || extStr == L".gif" || extStr == L".bmp" || extStr == L".tiff")
	{
		// 텍스처
		std::shared_ptr<Texture2D> texture = RESOURCE_MANAGER->LoadOrGetResource<Texture2D>(relativePath);
		if (texture->HasBulkData() == true)
		{
			EDITOR_LOG("Already loaded resource");
			return;
		}
		std::shared_ptr<Texture2DBulkData> newBulkData;
		ASSERT_MSG(TextureConvertor::LoadAndConvertToMemoryByFullPath(fileFullPath, newBulkData) == true, "Drop file is invalid");
		texture->PushBulkData(newBulkData);
	}
	else if (extStr == L".ttf" || extStr == L".otf")
	{
		// 폰트
		std::vector<FontAtlasGenerator::Result> newBulkDatas;
		static std::vector<uint32> pixelSizes = {16, 32, 64, 128, 256, 512};
		static int32 atlasXSize = 2048;

		ASSERT_MSG(FontConvertor::LoadAndConvertToMemoryByFullPath(fileFullPath, pixelSizes, atlasXSize, newBulkDatas) == true, "Drop file is invalid");
		
		for (auto& bulkData : newBulkDatas)
		{
			std::shared_ptr<Font> font = RESOURCE_MANAGER->LoadOrGetResource<Font>(relativePath.wstring() + L"_" + bulkData.mSuffixName);
			if (font->HasBulkData() == true)
			{
				EDITOR_LOG("Already loaded resource");
				return;
			}
			font->PushBulkData(bulkData.mBulkData);
		}
	}
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

			matUI->Save();
		}
		matUI->GetProxy();
		_mDefaultResources.push_back(matUI);
	}
}

void Editor::UnregisterDefaultResources()
{
	_mDefaultResources.clear();
}

