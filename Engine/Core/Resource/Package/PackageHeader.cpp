#include "pch.h"
#include "PackageHeader.h"

#include "Core/Archive.h"
#include "Core/Resource/BulkData.h"

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

	const std::size_t bulkDataSize = mBulkClassNames.size();
	mBulkDatas.resize(bulkDataSize);
	for (std::size_t i = 0; i < bulkDataSize; ++i)
	{
		const BulkStructTypeInfo& bulkTypeInfo = *static_cast<const BulkStructTypeInfo*>(BOOT_SYSTEM->GetStructTypeInfo(mBulkClassNames[i].c_str()));
		const std::function<BulkData*()>& constructor = bulkTypeInfo.GetConstructor();
		mBulkDatas[i] = std::shared_ptr<BulkData>(constructor(), [](BulkData* p) {
			delete p;
			});
		bulkTypeInfo.Deserialize(archive, mBulkDatas[i].get());
	}
}