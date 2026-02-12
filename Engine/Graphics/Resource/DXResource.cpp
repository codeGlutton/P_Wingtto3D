#include "pch.h"
#include "DXResource.h"

#include "Manager/ResourceManager.h"

DXSharedResource::DXSharedResource()
{
}

DXSharedResource::~DXSharedResource()
{
	RESOURCE_MANAGER->NotifyToRemoveRenderResource(
		GetSharedResourceType(),
		GetFullPath()
	);
}

void DXSharedResource::PostCreate()
{
	RESOURCE_MANAGER->NotifyToAddRenderResource(
		GetSharedResourceType(),
		std::static_pointer_cast<DXSharedResource>(shared_from_this())
	);
}

