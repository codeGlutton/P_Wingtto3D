#pragma once

#include "TypeTrait.h"

class ObjectTypeInfo;

/**
 * 인터페이스 Reflect를 위한 Wrapper 클래스
 */
template <typename... Args>
struct InterfaceReflector : public Args...
{
public:
	using Super = void;
	using Interfaces = InterfaceReflector<Args...>;

public:
	static constexpr auto GetInterfaceInfos() 
	{
		return std::array<const ObjectTypeInfo*, sizeof...(Args)>{&Args::GetStaticTypeInfo()...};
	}
	template<typename T>
	static constexpr auto GetInterfaceOffsets()
	{
		int32 tmp = 0;
		T* ptr = reinterpret_cast<T*>(&tmp);

		return std::array<std::size_t, sizeof...(Args)>{
			(reinterpret_cast<std::size_t>(static_cast<Args*>(ptr)) - reinterpret_cast<std::size_t>(ptr))...
		};
	}
};

template <typename Base, typename... Args> requires (!IsInterface<Base>)
struct InterfaceReflector<Base, Args...> : public Base, public Args...
{
public:
	using Super = Base;
	using Interfaces = InterfaceReflector<Args...>;

public:
	static constexpr auto GetInterfaceInfos()
	{
		return std::array<const ObjectTypeInfo*, sizeof...(Args)>{&Args::GetStaticTypeInfo()...};
	}
	template<typename T>
	static auto GetInterfaceOffsets()
	{
		T* ptr = nullptr;

		return std::array<std::size_t, sizeof...(Args)>{
			(reinterpret_cast<std::size_t>(static_cast<Args*>(ptr)) - reinterpret_cast<std::size_t>(ptr))...
		};
	}
};

template <>
struct InterfaceReflector<>
{
	InterfaceReflector()
	{
		FAIL_MSG("Inherit class is nothing");
	}
};
