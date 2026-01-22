#include "pch.h"
#include "Object.h"

#include "Core/Archive.h"

void Object::PostCreate()
{
	_mIsAlive = true;
}

void Object::PostLoad()
{
	OBJECT_MANAGER->NotifyToAddObject(_mName);
}

void Object::BeginDestroy()
{
	OBJECT_MANAGER->NotifyToRemoveObject(_mName);
	_mIsAlive = false;
}

void Object::Serialize(Archive& archive) const
{
	GetTypeInfo().Serialize(archive, this);
}

void Object::Deserialize(Archive& archive)
{
	GetTypeInfo().Deserialize(archive, this);
}

