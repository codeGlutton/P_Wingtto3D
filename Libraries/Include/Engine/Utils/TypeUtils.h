#pragma once

#include "Reflection/TypeInfo.h"
#include "Core/CommonType/SoftObjectPtr.h"
#include "Core/CommonType/SubClass.h"

template<typename To, typename From>
To* Cast(From* src);

template<typename To>
To* Cast(void* src, const ObjectTypeInfo& srcTypeInfo);

template<typename To, typename From>
std::shared_ptr<To> CastSharedPointer(std::shared_ptr<From> src);

template<typename To>
std::shared_ptr<To> CastSharedPointer(std::shared_ptr<void> src, const ObjectTypeInfo& srcTypeInfo);

/**
 * 정수 및 실수 타입
 */
template<typename T, typename C = void>
struct TypeInfoResolver;

/**
 * void 타입
 */
template<>
struct TypeInfoResolver<void, void>;

/**
 * 열거형
 */
template<typename T>
struct TypeInfoResolver<T, std::enable_if_t<std::is_enum_v<T>>>;

/**
 * 클래스 및 구조체
 */
template<typename T>
struct TypeInfoResolver<T, std::enable_if_t<std::is_class_v<T> && HasSuper<T>>>;

/**
 * 포인터
 */
template<typename T> requires IsChildOfObject<T>
struct TypeInfoResolver<T*, void>;

template<typename T> requires IsChildOfObject<T>
struct TypeInfoResolver<std::shared_ptr<T>, void>;

template<typename T> requires IsChildOfObject<T>
struct TypeInfoResolver<std::weak_ptr<T>, void>;

template<typename T> requires IsChildOfObject<T>
struct TypeInfoResolver<SubClass<T>, void>;

template<typename T> requires IsChildOfObject<T>
struct TypeInfoResolver<SoftObjectPtr<T>, void>;

template<typename T> requires IsBulk<T>
struct TypeInfoResolver<std::shared_ptr<T>, void>;

/**
 * Pair
 */
template<typename K, typename D>
struct TypeInfoResolver<std::pair<K, D>, void>;

/**
 * 배열
 */
template<typename T, size_t N>
struct TypeInfoResolver<T[N], void>;

template<typename T, size_t N>
struct TypeInfoResolver<std::array<T, N>, void>;

/**
 * 어댑터를 제외한 동적 배열 및 문자열
 */
template<typename T>
struct TypeInfoResolver<T, std::enable_if_t<IsIteratorContanier<T> && UseAllocator<T>>>;

#include "TypeUtils.inl"