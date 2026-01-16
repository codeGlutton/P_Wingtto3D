#pragma once

#include "Reflection/TypeInfo.h"

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
template<typename T>
struct TypeInfoResolver<T*, void>;

template<typename T>
struct TypeInfoResolver<std::shared_ptr<T>, void>;

template<typename T>
struct TypeInfoResolver<std::weak_ptr<T>, void>;

/**
 * 배열
 */
template<typename T, size_t N>
struct TypeInfoResolver<T[N], void>;

/**
 * 동적 배열
 */
template<typename T>
struct TypeInfoResolver<std::vector<T>, void>;

template<typename T>
struct TypeInfoResolver<std::list<T>, void>;

/**
 * 문자열
 */
template<>
struct TypeInfoResolver<std::string, void>;

template<>
struct TypeInfoResolver<std::wstring, void>;

#include "TypeUtils.inl"