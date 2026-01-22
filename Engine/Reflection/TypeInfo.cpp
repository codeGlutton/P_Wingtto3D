#include "pch.h"
#include "TypeInfo.h"

#include "Reflection/StructTypeInfo.h"

GEN_STRUCT_REFLECTION_SOURCE(TypeInfo)

bool TypeInfo::IsA(const TypeInfo& other) const
{
	if (this == &other)
	{
		return true;
	}

	return _mTypeHash == other._mTypeHash;
}


