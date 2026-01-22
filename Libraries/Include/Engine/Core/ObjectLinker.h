#pragma once

class Object;
struct PackageHeader;

/**
 * 패키지 별 임시 데이터
 */
struct PackageLinkData
{
	std::shared_ptr<PackageHeader> mPackageHeader;
	std::vector<std::shared_ptr<Object>> mObjects;

	std::unordered_map<const Object*, std::size_t> mObjectIndexMap;
	std::unordered_map<const BulkData*, std::size_t> mBulkDataIndexMap;
};

/**
 * 오브젝트를 직렬화 및 역직렬화하는데 필요한 임시 데이터 묶음
 */
struct ObjectLinker
{
	// 패키지마다의 링크 정보
	std::unordered_map<std::wstring, PackageLinkData> mLinkDataMap;
};

