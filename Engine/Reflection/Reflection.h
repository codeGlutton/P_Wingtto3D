/*********************************************************************
 * \file   Reflection.h
 * \brief  
 * DynamicCast 최적화 및 기본값 변경 여부를 확인하기 위한 Reflection
 * 
 * 참고 자료
 * https://xtozero.tistory.com/8
 * 
 * \author 모호재
 * \date   January 2026
 *********************************************************************/

#pragma once

#define GEN_INTERFACE_REFLECTION_ORIGIN(type)									\
	friend SuperTypeDefiner;													\
	friend TypeInfoInitializer;													\
	friend ObjectTypeInfoInitializer;											\
																				\
public:																			\
	using Super = SuperType<type>;												\
	using Interfaces = InterfaceType<type>;										\
	using This = type;															\
																				\
public:																			\
	virtual const ObjectTypeInfo& GetTypeInfo() const							\
	{																			\
		return _mTypeInfo;														\
	}																			\
																				\
	static const ObjectTypeInfo& GetStaticTypeInfo()							\
	{																			\
		return GetMutableStaticTypeInfo();										\
	}																			\
																				\
private:																		\
	static ObjectTypeInfo& GetMutableStaticTypeInfo()							\
	{																			\
		static ObjectTypeInfo typeInfo{ObjectTypeInfoInitializer<This>(#type)};	\
		return typeInfo;														\
	}																			\
																				\
private:																		\
	inline static ObjectTypeInfo& _mTypeInfo = GetMutableStaticTypeInfo();		\
																				\
private:

#define GEN_REFLECTION_ORIGIN(type)									\
	GEN_INTERFACE_REFLECTION(type)									\
public:																\
	using Origin = type;											\
																	\
private:

#define GEN_INTERFACE_REFLECTION(...) GEN_INTERFACE_REFLECTION_ORIGIN(__VA_ARGS__)
#define GEN_REFLECTION(...) GEN_REFLECTION_ORIGIN(__VA_ARGS__)

#define METHOD(name)																							\
	inline static struct MethodReflector_##name																	\
	{																											\
		MethodReflector_##name()																				\
		{																										\
			static MethodRegister<This> mRegister																\
			(																									\
				#name,																							\
				This::GetMutableStaticTypeInfo(),																\
				&This::##name																					\
			);																									\
		}																										\
	} mMethodReflector_##name;

#define PROPERTY(name)																							\
	inline static struct PropertyReflector_##name																\
	{																											\
		PropertyReflector_##name()																				\
		{																										\
			static PropertyRegister<This, decltype(name), decltype(&This::##name), &This::##name> mRegister		\
			(																									\
				#name,																							\
				This::GetMutableStaticTypeInfo()																\
			);																									\
		}																										\
	} mPropertyReflector_##name;

