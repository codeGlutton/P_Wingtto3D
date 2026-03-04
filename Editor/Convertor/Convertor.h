#pragma once

#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

#include "Core/Resource/BulkData.h"
#include "Tool/FontAtlasGenerator.h"

struct Texture2DBulkData;
struct StaticMeshBulkData;

class TextureConvertor
{
public:
	static bool LoadAndConvertToMemoryByPath(const std::wstring& filePath, OUT std::shared_ptr<Texture2DBulkData>& bulkData);
	static bool LoadAndConvertToMemoryByFullPath(const std::wstring& fileFullPath, OUT std::shared_ptr<Texture2DBulkData>& bulkData);
};

class MeshConvertor
{
public:
	//static bool LoadAndConvertToMemory(const std::wstring& filePath, OUT std::shared_ptr<StaticMeshBulkData>& bulkData, OUT std::shared_ptr<MaterialBulkData> );
};

class FontConvertor
{
public:
	static bool LoadAndConvertToMemoryByPath(const std::wstring& filePath, const std::vector<uint32>& pixelSizes, int32 atlasXSize, OUT std::vector<FontAtlasGenerator::Result>& bulkDatas);
	static bool LoadAndConvertToMemoryByFullPath(const std::wstring& fileFullPath, const std::vector<uint32>& pixelSizes, int32 atlasXSize, OUT std::vector<FontAtlasGenerator::Result>& bulkDatas);
};
