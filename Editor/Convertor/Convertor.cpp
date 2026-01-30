#include "pch.h"
#include "Convertor.h"

#include "Manager/PathManager.h"
#include "Graphics/Resource/DXTexture.h"

bool TextureConvertor::LoadAndConvertToMemory(const std::wstring& filePath, OUT std::shared_ptr<Texture2DBulkData> bulkData)
{
	if (bulkData == nullptr)
	{
		return false;
	}

	// 외부 라이브러리인 DirectTex를 활용
	// PNG, JPG 등 로드 함수가 존재
	// WIC (Window Image Component)는 다양한 양식 호환

	std::filesystem::path fileFullPath = PATH_MANAGER->GetResourcePath() / filePath;
	std::wstring fileExtension = fileFullPath.extension().wstring();
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

		//CHECK_WIN_MSG(hr, "Texture file can't be load");
		if (FAILED(hr) == true)
		{
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

		//CHECK_WIN_MSG(hr, "Texture can't be saved to dds memory");
		if (FAILED(hr) == true)
		{
			return false;
		}
	}

	bulkData->mValue.resize(blob.GetBufferSize());
	memcpy(bulkData->mValue.data(), blob.GetBufferPointer(), blob.GetBufferSize());

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
