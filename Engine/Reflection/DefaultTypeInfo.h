#pragma once

#include "Reflection/TypeInfo.h"

/**
 * void
 */
class VoidTypeInfo : public TypeInfo
{
private:
	VoidTypeInfo() :
		TypeInfo(TypeInfoInitializer<void>())
	{
	}

public:
	static const VoidTypeInfo mStatic;
};

/**
 * 정수 및 실수
 */
template<typename T>
class NumericTypeInfo : public TypeInfo
{
private:
	NumericTypeInfo() :
		TypeInfo(TypeInfoInitializer<T>())
	{
	}

public:
	static const NumericTypeInfo<T> mStatic;
};

template<typename T>
const NumericTypeInfo<T> NumericTypeInfo<T>::mStatic;

/**
 * 배열
 */
template<typename T, size_t N>
class ArrayTypeInfo : public TypeInfo
{
	using Element = T;

private:
	ArrayTypeInfo() :
		TypeInfo(TypeInfoInitializer<T>(N))
	{
	}

public:
	static const ArrayTypeInfo<T, N> mStatic;
};

template<typename T, size_t N>
const ArrayTypeInfo<T, N> ArrayTypeInfo<T, N>::mStatic;

/**
 * 포인터
 */
template<typename T> requires IsChildOfObject<T>
class PointerTypeInfo : public TypeInfo
{
private:
	PointerTypeInfo() :
		TypeInfo(TypeInfoInitializer<T*>())
	{
	}

public:
	static const PointerTypeInfo<T> mStatic;
};

template<typename T> requires IsChildOfObject<T>
const PointerTypeInfo<T> PointerTypeInfo<T>::mStatic;
