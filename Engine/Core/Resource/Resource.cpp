#include "pch.h"
#include "Resource.h"

#include "Manager/ResourceManager.h"
#include "Core/Resource/ResourceHeader.h"

#include "Manager/PackageManager.h"

void Resource::PostLoad()
{
	Super::PostLoad();
	RESOURCE_MANAGER->NotifyToAddResource(std::static_pointer_cast<Resource>(shared_from_this()));

	if (_mHeader == nullptr)
	{
		MakeHeader();
	}
}

void Resource::BeginDestroy()
{
	RESOURCE_MANAGER->NotifyToRemoveResource(GetPath());
	Super::BeginDestroy();
}

bool Resource::HasBulkData() const
{
	return false;
}

void Resource::Save() const
{
	PACKAGE_MANAGER->SavePackage(GetOuter()->GetPath());
}

void Resource::SaveAsync(std::function<void()> callback) const
{
	PACKAGE_MANAGER->SavePackageAsync(GetOuter()->GetPath(), callback);
}

void Resource::MakeHeader()
{
	_mHeader = RESOURCE_MANAGER->CreateResourceHeader(GetName(), std::static_pointer_cast<Resource>(shared_from_this()));
}
