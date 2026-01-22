#include "pch.h"
#include "Resource.h"

#include "Manager/ResourceManager.h"
#include "Core/Resource/ResourceHeader.h"

void Resource::PostCreate()
{
	Super::PostCreate();
	RESOURCE_MANAGER->NotifyToAddResource(std::static_pointer_cast<Resource>(shared_from_this()));
}

void Resource::BeginDestroy()
{
	RESOURCE_MANAGER->NotifyToRemoveResource(GetPath());
	Super::BeginDestroy();
}
