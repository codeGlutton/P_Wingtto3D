#include "pch.h"
#include "ResourceHeader.h"

#include "Manager/ResourceManager.h"

void ResourceHeader::PostLoad()
{
	Super::PostLoad();
	RESOURCE_MANAGER->NotifyToAddResourceHeader(std::static_pointer_cast<ResourceHeader>(shared_from_this()));
}
