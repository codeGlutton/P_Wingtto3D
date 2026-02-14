#pragma once

#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

#include "Core/Resource/BulkData.h"

struct Texture2DBulkData;
struct StaticMeshBulkData;

class TextureConvertor
{
	static bool LoadAndConvertToMemory(const std::wstring& filePath, OUT std::shared_ptr<Texture2DBulkData> bulkData);
};

class MeshConvertor
{
	//static bool LoadAndConvertToMemory(const std::wstring& filePath, OUT std::shared_ptr<StaticMeshBulkData> bulkData, OUT std::shared_ptr<MaterialBulkData> );
};