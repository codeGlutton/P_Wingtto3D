#include "pch.h"
#include "TypeInfo.h"

bool TypeInfo::IsA(const TypeInfo& other) const
{
	if (this == &other)
	{
		return true;
	}

	return _mTypeHash == other._mTypeHash;
}


