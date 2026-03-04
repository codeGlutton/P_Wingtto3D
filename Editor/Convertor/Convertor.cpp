#include "pch.h"
#include "Convertor.h"

#include "Manager/PathManager.h"
#include "Graphics/Resource/DXTexture.h"

bool TextureConvertor::LoadAndConvertToMemoryByPath(const std::wstring& filePath, OUT std::shared_ptr<Texture2DBulkData>& bulkData)
{
	std::filesystem::path fileFullPath = PATH_MANAGER->GetResourcePath() / filePath;
	return LoadAndConvertToMemoryByFullPath(fileFullPath.wstring(), bulkData);
}

bool TextureConvertor::LoadAndConvertToMemoryByFullPath(const std::wstring& fileFullPath, OUT std::shared_ptr<Texture2DBulkData>& bulkData)
{
	if (bulkData == nullptr)
	{
		EDITOR_LOG("Create output texture data because of empty bulk data");
		bulkData = std::make_shared<Texture2DBulkData>();
	}

	// 외부 라이브러리인 DirectTex를 활용
	// PNG, JPG 등 로드 함수가 존재
	// WIC (Window Image Component)는 다양한 양식 호환

	std::wstring fileExtension = std::filesystem::path(fileFullPath).extension().wstring();
	std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::towlower);

	DirectX::TexMetadata md;
	DirectX::ScratchImage image;
	HRESULT hr;
	{
		if (fileExtension == L".tga")
		{
			hr = DirectX::LoadFromTGAFile(
				fileFullPath.c_str(),		// 이미지 주소
				&md,						// OUT 메타데이터
				image						// OUT 스크래치 이미지
			);
		}
		else if (fileExtension == L".dds")
		{
			hr = DirectX::LoadFromDDSFile(
				fileFullPath.c_str(),		// 이미지 주소
				DirectX::DDS_FLAGS_NONE,	// DDS 로드 옵션
				&md,						// OUT 메타데이터
				image						// OUT 스크래치 이미지
			);
		}
		else
		{
			hr = DirectX::LoadFromWICFile(
				fileFullPath.c_str(),		// 이미지 주소
				DirectX::WIC_FLAGS_NONE,	// WIC 로드 옵션
				&md,						// OUT 메타데이터
				image						// OUT 스크래치 이미지
			);
		}

		if (FAILED(hr) == true)
		{
			EDITOR_LOG("Texture file can't be load");
			return false;
		}
	}

	DirectX::Blob blob;
	{
		hr = DirectX::SaveToDDSMemory(
			image.GetImages(),
			image.GetImageCount(),
			md,
			DirectX::DDS_FLAGS_NONE,
			blob
		);

		if (FAILED(hr) == true)
		{
			EDITOR_LOG("Texture can't be saved to dds memory");
			return false;
		}
	}

	bulkData->mValue.resize(blob.GetBufferSize());
	memcpy(bulkData->mValue.data(), blob.GetBufferPointer(), blob.GetBufferSize());

	bulkData->mIsLoad = true;
	return true;
}

//bool MeshConvertor::LoadAndConvertToMemory(const std::wstring& filePath, OUT std::shared_ptr<StaticMeshBulkData> bulkData)
//{
//	if (bulkData == nullptr)
//	{
//		return false;
//	}
//
//	std::shared_ptr<Assimp::Importer> importer = std::make_shared<Assimp::Importer>();
//
//}

bool FontConvertor::LoadAndConvertToMemoryByPath(const std::wstring& filePath, const std::vector<uint32>& pixelSizes, int32 atlasXSize, OUT std::vector<FontAtlasGenerator::Result>& bulkDatas)
{
	std::filesystem::path fileFullPath = PATH_MANAGER->GetResourcePath() / filePath;
	return LoadAndConvertToMemoryByFullPath(fileFullPath.wstring(), pixelSizes, atlasXSize, bulkDatas);
}

bool FontConvertor::LoadAndConvertToMemoryByFullPath(const std::wstring& fileFullPath, const std::vector<uint32>& pixelSizes, int32 atlasXSize, OUT std::vector<FontAtlasGenerator::Result>& bulkDatas)
{
	bulkDatas = FontAtlasGenerator::Create(fileFullPath, pixelSizes, atlasXSize);
	for (auto& bulkData : bulkDatas)
	{
		bulkData.mBulkData->mIsLoad = true;
	}
	return true;
}
