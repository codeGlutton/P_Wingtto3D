/*********************************************************************
 * \file   TypeInfo.h
 * \brief  
 * DynamicCast 최적화 및 기본값 변경 여부를 확인하기 위한 TypeInfo 관련 객체들
 *
 * 참고 자료
 * https://xtozero.tistory.com/8
 * 
 * \author 모호재
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "Reflection/TypeTrait.h"

class TypeInfo;
class Archive;

/**
 * 타입 정보 초기화 구조체
 */
template <typename T>
struct TypeInfoInitializer
{
	TypeInfoInitializer(size_t size = 0) :
		mSize(size)
	{
	}

public:
	size_t mSize;
};

/**
 * 타입 정보 클래스 (unreal의 uclass와 유사)
 */
class TypeInfo abstract
{
public:
	template <typename T>
	explicit TypeInfo(const TypeInfoInitializer<T>& initializer) :
		_mTypeHash(typeid(T).hash_code()),
		_mSize(initializer.mSize),
		_mFullName(typeid(T).name())
	{
	}

public:
	/**
	 * 동적으로 타입 일치 확인
	 * \param other 비교 대상 
	 * \return 타입 동일 여부 
	 */
	bool IsA(const TypeInfo& other) const;
	
	template<typename T>
	bool IsA() const;

public:
	virtual bool IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const = 0;
	virtual void Serialize(OUT Archive& archive, const void* inst) const = 0;
	virtual void Deserialize(Archive& archive, OUT void* inst) const = 0;

private:
	size_t _mTypeHash;
	size_t _mSize;
	std::string _mFullName = nullptr;
};

template<typename T>
inline bool TypeInfo::IsA() const
{
	return IsA(GetStaticTypeInfo<T>());
}


