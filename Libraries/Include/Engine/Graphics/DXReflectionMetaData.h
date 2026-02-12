#pragma once

#include "Graphics/Resource/DXResourceInclude.h"

namespace DXShaderBindingType
{
	enum Type
	{
		CBuffer,
		Sampler,
		Texture,
		RWTexture,
		Structure,
		RWStructure,
		
		Count
	};
}

struct DXBindingMetaData
{
	uint32 mSlotIndex;
	DXResourceUsageFlag::Type mUsageFlags;
};

struct DXParameterMetaData
{
	/* 단순 검증용 */

	std::string mSemanticName;
	uint32 mSemanticIndex;
};

struct DXShaderLayoutMetaData
{
	/* 단순 검증용 */

	std::vector<DXParameterMetaData> mParams;
};

