#pragma once

struct BulkData;
class Archive;

/**
 * 패키징 중에 비동기 직렬화 가능한 부분
 */
struct PackageHeader
{
	GEN_STRUCT_REFLECTION(PackageHeader)

public:
	void Serialize(Archive& archive) const;
	void Deserialize(Archive& archive);

	/* 로드 데이터 */
public:
	PROPERTY(mObjectClassNames)
	std::vector<std::string> mObjectClassNames;
	PROPERTY(mLinkedExternalPackageClassNames)
	std::vector<std::string> mLinkedExternalPackageClassNames;
	PROPERTY(mLinkedExternalPackages)
	std::vector<std::wstring> mLinkedExternalPackages;
	PROPERTY(mBulkClassNames)
	std::vector<std::string> mBulkClassNames;
	std::vector<std::shared_ptr<BulkData>> mBulkDatas;
};

