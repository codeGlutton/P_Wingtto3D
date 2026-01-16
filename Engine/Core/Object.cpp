#include "pch.h"
#include "Object.h"

#include "Core/Archive.h"

void Object::PostCreate()
{
}

void Object::PostLoad()
{
}

void Object::BeginDestroy()
{
	_mIsAlive = false;
}

void Object::Serialize(Archive& archive)
{
	GetTypeInfo().Serialize(archive, this);
}

void Object::Deserialize(Archive& archive)
{
	GetTypeInfo().Deserialize(archive, this);
}

