#include "pch.h"
#include "PackageHeader.h"

#include "Core/Archive.h"
#include "Core/Resource/BulkWrapper.h"

void PackageHeader::Serialize(Archive& archive) const
{
	archive.ChangeTarget(ArchiveMemoryTarget::Header);

	const StructTypeInfo& typeInfo = GetStaticTypeInfo();
	typeInfo.Serialize(archive, this);

	for (const std::shared_ptr<BulkData>& bulkData : mBulkDatas)
	{
		bulkData->GetTypeInfo().Serialize(archive, bulkData.get());
	}
}

void PackageHeader::Deserialize(Archive& archive)
{
	archive.ChangeTarget(ArchiveMemoryTarget::Header);

	const StructTypeInfo& typeInfo = GetStaticTypeInfo();
	typeInfo.Deserialize(archive, this);

	for (const std::shared_ptr<BulkData>& bulkData : mBulkDatas)
	{
		bulkData->GetTypeInfo().Deserialize(archive, bulkData.get());
	}
}