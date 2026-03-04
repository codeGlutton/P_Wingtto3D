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

// 타입 정보 매크로
#pragma region TYPE_INFO_MACROS

#define DECLARE_VIRTUAL_TYPE virtual

#define DECLARE_STRUCT_TYPE(type)	\
public:								\
	using Base = type;				\
private:

#define GEN_STRUCT_REFLECTION_HEADER(type)						\
public:															\
	using Super = SuperDefineType<type>;						\
	using This = type;											\
	DECLARE_STRUCT_TYPE(type)									\
																\
public:															\
	virtual const StructTypeInfo& GetTypeInfo() const;			\
	static const StructTypeInfo& GetStaticTypeInfo();			\
																\
private:														\
	static StructTypeInfo& GetMutableStaticTypeInfo();			\
																\
private:														\
	inline static StructTypeInfo* _mTypeInfo = nullptr;			\
																\
private:														\
	inline static struct StructTypeReflector					\
	{															\
		StructTypeReflector()									\
		{														\
			BOOT_SYSTEM->AddType(								\
				[]() {											\
					return &type::GetStaticTypeInfo();			\
				}												\
			);													\
		}														\
	} _mTypeReflector;											\
																\
public:

#define GEN_STRUCT_REFLECTION_SOURCE(type)						\
	const StructTypeInfo& type::GetTypeInfo() const				\
	{															\
		return *_mTypeInfo;										\
	}															\
																\
	const StructTypeInfo& type::GetStaticTypeInfo()				\
	{															\
		return GetMutableStaticTypeInfo();						\
	}															\
																\
	StructTypeInfo& type::GetMutableStaticTypeInfo()			\
	{															\
		if (_mTypeInfo == nullptr)								\
		{														\
			static StructTypeInfo typeInfo						\
			{													\
				StructTypeInfoInitializer<This>(#type)			\
			};													\
			_mTypeInfo = &typeInfo;								\
		}														\
		return *_mTypeInfo;										\
	}

#define GEN_STRUCT_REFLECTION_INTERNAL(type, virtual_type)		\
public:															\
	using Super = SuperDefineType<type>;						\
	using This = type;											\
	DECLARE_STRUCT_TYPE(type)									\
																\
public:															\
	virtual_type const StructTypeInfo& GetTypeInfo() const		\
	{															\
		return *_mTypeInfo;										\
	}															\
																\
	static const StructTypeInfo& GetStaticTypeInfo()			\
	{															\
		return GetMutableStaticTypeInfo();						\
	}															\
																\
private:														\
	static StructTypeInfo& GetMutableStaticTypeInfo()			\
	{															\
		if (_mTypeInfo == nullptr)								\
		{														\
			static StructTypeInfo typeInfo						\
			{													\
				StructTypeInfoInitializer<This>(#type)			\
			};													\
			_mTypeInfo = &typeInfo;								\
		}														\
		return *_mTypeInfo;										\
	}															\
																\
private:														\
	inline static StructTypeInfo* _mTypeInfo = nullptr;			\
																\
private:														\
	inline static struct StructTypeReflector					\
	{															\
		StructTypeReflector()									\
		{														\
			BOOT_SYSTEM->AddType(								\
				[]() {											\
					return &type::GetStaticTypeInfo();			\
				}												\
			);													\
		}														\
	} _mTypeReflector;											\
																\
public:

#define GEN_BULK_STRUCT_REFLECTION_INTERNAL(type, virtual_type)	\
public:															\
	using Super = SuperDefineType<type>;						\
	using This = type;											\
	DECLARE_STRUCT_TYPE(type)									\
																\
public:															\
	virtual_type const StructTypeInfo& GetTypeInfo() const		\
	{															\
		return *_mTypeInfo;										\
	}															\
																\
	static const StructTypeInfo& GetStaticTypeInfo()			\
	{															\
		return GetMutableStaticTypeInfo();						\
	}															\
																\
private:														\
	static StructTypeInfo& GetMutableStaticTypeInfo()			\
	{															\
		if (_mTypeInfo == nullptr)								\
		{														\
			static BulkStructTypeInfo typeInfo					\
			{													\
				BulkStructTypeInfoInitializer<This>(			\
					#type,										\
					[]() {										\
					return new type;							\
					}											\
				)												\
			};													\
			_mTypeInfo = &typeInfo;								\
		}														\
		return *_mTypeInfo;										\
	}															\
																\
private:														\
	inline static StructTypeInfo* _mTypeInfo = nullptr;			\
																\
private:														\
	inline static struct StructTypeReflector					\
	{															\
		StructTypeReflector()									\
		{														\
			BOOT_SYSTEM->AddType(								\
				[]() {											\
					return &type::GetStaticTypeInfo();			\
				}												\
			);													\
		}														\
	} _mTypeReflector;											\
																\
public:

#define DECLARE_OBJECT_TYPE(type)	\
	friend class Package;			\
	friend class ObjectManager;		\
	DECLARE_STRUCT_TYPE(type)		\
private:

#define DECLARE_ABSTRACT_TYPE(type)																	\
public:																								\
	static std::shared_ptr<const type> GetDefaultObject()											\
	{																								\
		return nullptr;																				\
	}																								\
																									\
private:																							\
	static ObjectTypeInfo& GetMutableStaticTypeInfo()												\
	{																								\
		if (_mTypeInfo == nullptr)																	\
		{																							\
			static ObjectTypeInfo typeInfo															\
			{																						\
				ObjectTypeInfoInitializer<This>(#type)												\
			};																						\
			_mTypeInfo = &typeInfo;																	\
		}																							\
		return *_mTypeInfo;																			\
	}																								\
private:

#define DECLARE_NOT_ABSTRACT_TYPE(type)																\
public:																								\
	static std::shared_ptr<const type> GetDefaultObject()											\
	{																								\
		return std::reinterpret_pointer_cast<const type>(GetStaticTypeInfo().GetDefaultObject());	\
	}																								\
																									\
private:																							\
	static ObjectTypeInfo& GetMutableStaticTypeInfo()												\
	{																								\
		if (_mTypeInfo == nullptr)																	\
		{																							\
			static ObjectTypeInfo typeInfo															\
			{																						\
				ObjectTypeInfoInitializer<This>														\
				(																					\
					#type,																			\
					[]() {																			\
					return new type;																\
					}																				\
				)																					\
			};																						\
			_mTypeInfo = &typeInfo;																	\
		}																							\
		return *_mTypeInfo;																			\
	}																								\
private:

#define GEN_REFLECTION_INTERNAL(type, virtual_type)				\
public:															\
	using Super = SuperDefineType<type>;						\
	using Interfaces = InterfaceDefineType<type>;				\
	using This = type;											\
																\
public:															\
	virtual_type const ObjectTypeInfo& GetTypeInfo() const		\
	{															\
		return *_mTypeInfo;										\
	}															\
																\
	static const ObjectTypeInfo& GetStaticTypeInfo()			\
	{															\
		return GetMutableStaticTypeInfo();						\
	}															\
																\
private:														\
	inline static ObjectTypeInfo* _mTypeInfo = nullptr;			\
																\
private:														\
	inline static struct ObjectTypeReflector					\
	{															\
		ObjectTypeReflector()									\
		{														\
			BOOT_SYSTEM->AddType(								\
				[]() {											\
					return &type::GetStaticTypeInfo();			\
				},												\
				[]() {											\
					type::GetDefaultObject();					\
				}												\
			);													\
		}														\
	} _mTypeReflector;											\
																\
private:

#define GEN_INTERFACE_REFLECTION_INTERNAL(type)			\
	GEN_REFLECTION_INTERNAL(type, DECLARE_VIRTUAL_TYPE)	\
	DECLARE_ABSTRACT_TYPE(type)

#define GEN_ABSTRACT_REFLECTION_INTERNAL(type)			\
	GEN_REFLECTION_INTERNAL(type, DECLARE_VIRTUAL_TYPE)	\
	DECLARE_OBJECT_TYPE(type)							\
	DECLARE_ABSTRACT_TYPE(type)

#define GEN_OBJECT_REFLECTION_INTERNAL(type)			\
	GEN_REFLECTION_INTERNAL(type, DECLARE_VIRTUAL_TYPE)	\
	DECLARE_OBJECT_TYPE(type)							\
	DECLARE_NOT_ABSTRACT_TYPE(type)

#define GEN_REFLECTION(...) GEN_OBJECT_REFLECTION_INTERNAL(__VA_ARGS__)
#define GEN_INTERFACE_REFLECTION(...) GEN_INTERFACE_REFLECTION_INTERNAL(__VA_ARGS__)
#define GEN_ABSTRACT_REFLECTION(...) GEN_ABSTRACT_REFLECTION_INTERNAL(__VA_ARGS__)

#define GEN_MINIMUM_STRUCT_REFLECTION(...) GEN_STRUCT_REFLECTION_INTERNAL(__VA_ARGS__, )
#define GEN_STRUCT_REFLECTION(...) GEN_STRUCT_REFLECTION_INTERNAL(__VA_ARGS__, DECLARE_VIRTUAL_TYPE)

#define GEN_BULK_STRUCT_REFLECTION(...) GEN_BULK_STRUCT_REFLECTION_INTERNAL(__VA_ARGS__, DECLARE_VIRTUAL_TYPE)

#pragma endregion
// 타입 정보 매크로


// 멤버 변수 정보 매크로
#pragma region MEMBER_INFO_MACROS

#define METHOD(name)																							\
	inline static struct MethodReflector_##name																	\
	{																											\
		MethodReflector_##name()																				\
		{																										\
			BOOT_SYSTEM->AddTypeMember([](){																	\
				static MethodRegister<This> mRegister															\
				(																								\
					#name,																						\
					This::GetMutableStaticTypeInfo(),															\
					&This::##name																				\
				);																								\
			});																									\
		}																										\
	} mMethodReflector_##name;

#define PROPERTY(name)																									\
	inline static struct PropertyReflector_##name																		\
	{																													\
		PropertyReflector_##name()																						\
		{																												\
			BOOT_SYSTEM->AddTypeMember([](){																			\
				static PropertyRegister<This, decltype(This::##name), decltype(&This::##name), &This::##name> mRegister	\
				(																										\
					#name,																								\
					This::GetMutableStaticTypeInfo()																	\
				);																										\
			});																											\
		}																												\
	} mPropertyReflector_##name;

#pragma endregion
// 멤버 변수 정보 매크로
