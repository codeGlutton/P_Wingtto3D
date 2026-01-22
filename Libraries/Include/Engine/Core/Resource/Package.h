#pragma once

#include "Core/Object.h"
#include "Core/Resource/PackageHeader.h"
#include "Utils/Thread/Lock.h"

struct ObjectLinker;
struct BulkData;

class IPackageRuntimeOwner abstract
{
public:
	virtual void RegisterPackage(std::shared_ptr<Package> package) = 0;

	virtual void Save() = 0;
	virtual void Load() = 0;
};

/**
 * 연관 오브젝트의 데이터 패키지
 */
class Package abstract : public Object
{
	GEN_ABSTRACT_REFLECTION(Package)

	friend class PackageManager;

protected:
	virtual void PostLoad() override;
	virtual void RegisterPackage();
	virtual void UnregisterPackage();
	virtual void BeginDestroy() override;

protected:
	virtual void Serialize(Archive& archive) const override;
	virtual void Deserialize(Archive& archive) override;

public:
	virtual bool IsValid() const override
	{
		return Super::IsValid() && (_mIsDeleted == false);
	}

protected:
	const std::vector<std::string>& GetExternalPackageClassNames(std::shared_ptr<ObjectLinker> linker) const;
	const std::vector<std::wstring>& GetExternalPackagePaths(std::shared_ptr<ObjectLinker> linker) const;
	/**
	 * 저장 시, 스레드에게 헤더 부분을 작성하도록 시키기위해서 링커 안에 해당 패키지 Header 객체 생성
	 * \param linker 여러 패키지에 대한 정보가 담긴 링커 객체
	 */
	void MakeHeader(std::shared_ptr<ObjectLinker> linker) const;
	/**
	 * 로드 시, 전달받은 헤더 데이터로 빈 객체들 우선 생성
	 * \param linker 여러 패키지에 대한 정보가 담긴 링커 객체
	 */
	void CreateEmptyObjects(std::shared_ptr<ObjectLinker> linker);

	/* 연관 오브젝트 생성 및 제거 시 카운트 기록 */
protected:
	void UpRefCount();
	void DownRefCount();

private:
	virtual std::shared_ptr<Object> RequestToCreateObject(const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags) = 0;

protected:
	std::vector<Object*> _mChildObjects;
	std::vector<std::weak_ptr<Object>> _mChildSharedObjects;
	uint64 _mRefCount = 0;
	bool _mIsDeleted = false;
	mutable Lock _mLock;
};

class AppWindowPackage : public Package
{
	GEN_REFLECTION(AppWindowPackage)

private:
	virtual std::shared_ptr<Object> RequestToCreateObject(const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags) override;
};

class ResourcePreviewPackage : public Package
{
	GEN_REFLECTION(ResourcePreviewPackage)

private:
	virtual std::shared_ptr<Object> RequestToCreateObject(const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags) override;
};

class ResourcePackage : public Package
{
	GEN_REFLECTION(ResourcePackage)

private:
	virtual std::shared_ptr<Object> RequestToCreateObject(const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags) override;
};


