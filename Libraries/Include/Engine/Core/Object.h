#pragma once

class Archive;

class Object abstract : public std::enable_shared_from_this<Object>
{
	GEN_ABSTRACT_REFLECTION(Object)

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

protected:
	virtual void Serialize(Archive& archive);
	virtual void Deserialize(Archive& archive);

private:
	// 저장 계층구조
	PROPERTY(_mOuter)
	Object* _mOuter;

private:
	bool _mIsAlive = true;
};

