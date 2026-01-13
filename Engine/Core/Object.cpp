#include "pch.h"
#include "Object.h"

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

