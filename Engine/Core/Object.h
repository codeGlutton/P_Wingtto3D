#pragma once

class Object : public std::enable_shared_from_this<Object>
{
	GEN_REFLECTION(Object)

public:
	Object() {}
	virtual ~Object() {}

public:
	//virtual void Serialize(FArchive& Ar) = 0;

public:
	bool IsA(const TypeInfo* typeInfo) const
	{
		if (typeInfo == nullptr)
		{
			return false;
		}
		return typeInfo->IsA(GetTypeInfo());
	}

	template<typename T>
	bool IsA() const
	{
		return IsA(T::GetStaticTypeInfo());
	}

public:
	// 저장 계층구조
	PROPERTY(_mOuter)
	Object* _mOuter;
};

