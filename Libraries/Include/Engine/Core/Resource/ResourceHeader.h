#pragma once

#include "Core/Object.h"
	
#include "Core/Resource/Resource.h"
#include "Core/Resource/Package/Package.h"

/**
 * 리소스 미리보기 정보 (하드 레퍼런싱 시, 실제 리소스 로드)
 */
class ResourceHeader : public Object
{
	GEN_REFLECTION(ResourceHeader)

	friend class ResourceManager;

public:
	PROPERTY(mResourceName)
	std::wstring mResourceName;
	PROPERTY(mResourcePtr)
	SoftObjectPtr<Package> mResourcePtr;
	PROPERTY(mResourceTypeInfo)
	SubClass<Resource> mResourceTypeInfo;

private:
	std::weak_ptr<Resource> _mTarget;
};

