#include "pch.h"
#include "ObjectManager.h"

#include "Core/Object.h"
#include "Core/Resource/Package.h"

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
		if (std::shared_ptr<Package> package = p->_mOuter.lock())
		{
			package->DownRefCount();
		}
		else
		{
			delete p;
		}
		});
	ASSERT_MSG(result != nullptr, "Invalid object constructor");

	result->_mOuter = params.mOuter;
	result->_mName = params.mName;
	if (params.mOuter != nullptr)
	{
		result->_mPath = params.mOuter == nullptr ? params.mName : (params.mOuter->GetPath() + params.mName);
	}
	else
	{
		result->_mPath = params.mPath;
	}

	if (isCDO == false)
	{
		for (std::size_t i = 0; _mNameSet.find(result->_mName) != _mNameSet.end(); ++i)
		{
			result->_mName = params.mName + std::to_wstring(i);
		}

		result->PostCreate();
		if (ShouldLoadProperties(params.mFlags) == true)
		{
			result->PostLoad();
		}
	}

	return result;
}

void ObjectManager::NotifyToAddObject(const std::wstring& name)
{
	ASSERT_MSG(_mNameSet.find(name) == _mNameSet.end(), "Overlapped object name");
	_mNameSet.insert(name);
}

void ObjectManager::NotifyToRemoveObject(const std::wstring& name)
{
	ASSERT_MSG(_mNameSet.find(name) != _mNameSet.end(), "Invalid object name can't erase");
	_mNameSet.erase(name);
}

