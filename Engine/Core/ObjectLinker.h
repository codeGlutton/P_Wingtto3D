#pragma once

class Object;
struct PackageHeader;

/**
 * 패키지 별 임시 데이터
 */
struct PackageLinkData
{
	/* IO 스레드에서 수집 정보 */
public:
	std::shared_ptr<PackageHeader> mPackageHeader;

	/* 외부 패키징 종속적 */
public:
	std::unordered_map<std::wstring, std::shared_ptr<Object>> mObjectPtrMap;
	std::unordered_map<const Object*, std::wstring> mObjectFullPathMap;

	/* 외부 패키징 독립적 */
public:
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

