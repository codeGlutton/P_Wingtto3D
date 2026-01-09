#pragma once

class ObjectTypeInfo;

/**
 * 인터페이스 Reflect를 위한 Wrapper 클래스
 */
template <typename... Args>
struct InterfaceReflector abstract : public Args...
{
public:
	using Interfaces = InterfaceReflector<Args...>;

public:
	static constexpr auto GetInterfaceInfos() 
	{
		return std::array<const ObjectTypeInfo*, sizeof...(Args)>{&Args::GetStaticTypeInfo()...};
	}
};

