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

		// 없다면 만들어주기
		if (texture == nullptr)
		{
			EDITOR_LOG("Create texture to %s", ConvertWStringToUtf8(relativePath).c_str());

			std::shared_ptr<ResourcePackage> package = PACKAGE_MANAGER->LoadPackage<ResourcePackage>(relativePath);
			texture = RESOURCE_MANAGER->CreateResource<Texture2D>(package, ObjectCreateFlag::DeferredLoad);

			std::shared_ptr<Texture2DBulkData> newBulkData;
			ASSERT_MSG(TextureConvertor::LoadAndConvertToMemoryByFullPath(fileFullPath, newBulkData) == true, "Drop file is invalid");
			
			texture->PushBulkData(newBulkData);
			texture->Save();
		}
		else
		{
			EDITOR_LOG("Already loaded resource : %s", ConvertWStringToUtf8(relativePath).c_str());
		}
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
			const std::wstring fontFullPath = (((
				relativePath.replace_extension()
				+= L"_")
				+= bulkData.mSuffixName)
				+= ext)
				.wstring();

			std::shared_ptr<Font> font = RESOURCE_MANAGER->LoadOrGetResource<Font>(fontFullPath);
			
			// 없다면 만들어주기
			if (font == nullptr)
			{
				EDITOR_LOG("Create font to %s", ConvertWStringToUtf8(fontFullPath).c_str());

				std::shared_ptr<ResourcePackage> package = PACKAGE_MANAGER->LoadPackage<ResourcePackage>(fontFullPath);
				font = RESOURCE_MANAGER->CreateResource<Font>(package, ObjectCreateFlag::DeferredLoad);

				font->PushBulkData(bulkData.mBulkData);
				font->Save();
			}
			else
			{
				EDITOR_LOG("Already loaded resource : %s", ConvertWStringToUtf8(fontFullPath).c_str());
			}
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
	AppModeBase::RegisterDefaultResources();

	const std::wstring packagePath = PATH_MANAGER->GetEngineResourceFolderName();

	// 기본 흰 텍스처
	{
		const std::wstring resourcePath = packagePath + L"\\Texture\\T_White";
		std::shared_ptr<Texture2D> whiteTex = RESOURCE_MANAGER->LoadOrGetResource<Texture2D>(resourcePath);

		if (whiteTex == nullptr)
		{
			const std::wstring resourceFullPath = PATH_MANAGER->GetEngineResourcePath().wstring() + L"\\Texture\\T_White.png";
			OnDropFile(resourceFullPath.c_str());

			whiteTex = RESOURCE_MANAGER->LoadOrGetResource<Texture2D>(resourcePath);
		}

		whiteTex->GetProxy();
		_mDefaultResources.push_back(whiteTex);
	}

	// 기본 폰트
	{
		const std::wstring resourcePath = packagePath + L"\\Font\\F_Cafe24Decobox_Regular";
		std::shared_ptr<Font> defaultFont = RESOURCE_MANAGER->LoadOrGetResource<Font>(resourcePath);

		// 없다면 만들어주기
		if (defaultFont == nullptr)
		{
			const std::wstring resourceFullPath = PATH_MANAGER->GetEngineResourcePath().wstring() + L"\\Font\\F_Cafe24Decobox.ttf";
			OnDropFile(resourceFullPath.c_str());

			defaultFont = RESOURCE_MANAGER->LoadOrGetResource<Font>(resourcePath);
		}

		defaultFont->GetProxies();
		_mDefaultResources.push_back(defaultFont);
	}
}

void Editor::UnregisterDefaultResources()
{
	AppModeBase::UnregisterDefaultResources();
}

