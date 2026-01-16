#pragma once

#include "Reflection/TypeInfo.h"

template<typename Ret, typename... Args>
class DelegateTypeInfo : public TypeInfo
{
	//using Ret = Ret;

public:
	DelegateTypeInfo()
	{
	}
};

