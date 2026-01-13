#include "pch.h"
#include "ObjectManager.h"

#include "Core/Object.h"

#include "ResourceManager.h"

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
}

std::shared_ptr<Object> ObjectManager::CreateObject(ObjectInitializeParameters params)
{
	const std::function<Object*()>& constructor = params.mTypeInfo->GetConstructor();
	ASSERT_MSG(constructor != nullptr, "Can't create object because of invalid constructor");

	std::shared_ptr<Object> result = std::shared_ptr<Object>(constructor(), [](Object* p) {
		p->BeginDestroy();
		delete p;
		});
	ASSERT_MSG(result != nullptr, "Invalid object constructor");

	result->PostCreate();
	if (ShouldLoadProperties(params.mFlags) == true)
	{
		// TODO : RESOURCE_MANAGER
		result->PostLoad();
	}

	return result;
}
