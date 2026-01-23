#include "pch.h"
#include "ObjectManager.h"

#include "Core/Object.h"
#include "Core/Resource/Package/Package.h"

#include "ResourceManager.h"

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
}

void ObjectManager::Init()
{
}

void ObjectManager::Destroy()
{
}

std::shared_ptr<Object> ObjectManager::CreateObject(ObjectInitializeParameters params) const
{
	const std::function<Object*()>& constructor = params.mTypeInfo->GetConstructor();
	ASSERT_MSG(constructor != nullptr, "Can't create object because of invalid constructor");

	bool isCDO = params.mFlags & ObjectCreateFlag::CDO;
	std::shared_ptr<Object> result = std::shared_ptr<Object>(constructor(), [isCDO](Object* p) {
		if (isCDO == false)
		{
			p->BeginDestroy();
		}
		delete p;
		});
	ASSERT_MSG(result != nullptr, "Invalid object constructor");

	if (isCDO == false)
	{
		// 기본 값 대입
		result->_mOuter = params.mOuter;
		if (params.mOuter != nullptr)
		{
			result->_mPath = params.mOuter->GetPath();
		}
		else
		{
			result->_mPath = params.mPath;
		}
		result->SetName(params.mName);

		result->PostCreate();
		if (ShouldLoadProperties(params.mFlags) == true)
		{
			result->PostLoad();
		}
	}

	return result;
}

void ObjectManager::RequestToRename(const std::wstring& path, OUT std::wstring& rename) const
{
	if (rename.empty() == true)
	{
		rename = L"NewObject";
	}

	const std::wstring fullPath = path + L'.' + rename;

	// 중복 이름 수정
	if (_mFullPathNameSet.find(fullPath) != _mFullPathNameSet.end())
	{
		std::size_t i = 0ull;
		for (; _mFullPathNameSet.find(fullPath + std::to_wstring(i)) != _mFullPathNameSet.end(); ++i)
		{
		}
		rename += std::to_wstring(i);
	}
}

void ObjectManager::NotifyToAddObject(const std::wstring& fullPath)
{
	if (fullPath.empty() == true)
	{
		return;
	}
	_mFullPathNameSet.insert(fullPath);
}

void ObjectManager::NotifyToRemoveObject(const std::wstring& fullPath)
{
	if (fullPath.empty() == true)
	{
		return;
	}
	_mFullPathNameSet.erase(fullPath);
}

