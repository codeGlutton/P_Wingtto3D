#pragma once

class Object : public std::enable_shared_from_this<Object>
{
	GEN_REFLECTION(Object)

protected:
	Object() {}
	virtual ~Object() {}

protected:
	/**
	 * 실제 생성 시점
	 */
	virtual void PostCreate();
	/**
	 * 인스턴스 객체 로드 시점
	 */
	virtual void PostLoad();
	/**
	 * 실제 제거 시점
	 */
	virtual void BeginDestroy();
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

private:
	// 저장 계층구조
	PROPERTY(_mOuter)
	Object* _mOuter;

private:
	bool _mIsAlive = true;
};

