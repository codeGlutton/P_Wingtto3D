#pragma once

#include <type_traits>

class Object;

template<typename... Args>
constexpr bool HasAnyReference = (std::is_reference_v<Args> || ...);

/* Super */

template <typename T, typename = void>
constexpr bool HasSuper = false;
template <typename T>
constexpr bool HasSuper<T, std::void_t<typename T::Super>> = true;

template<typename T, typename U = void>
struct SuperTypeDefiner
{
	using Type = void;
};
template<typename T>
struct SuperTypeDefiner<T, std::void_t<typename T::Origin>>
{
	using Type = typename T::Origin;
};
template<typename T>
using SuperType = SuperTypeDefiner<T>::Type;


/* Interface */

template <typename T, typename = void>
constexpr bool HasInterface = false;
template <typename T>
constexpr bool HasInterface<T, std::void_t<typename T::Interfaces>> = true;

template <typename T, typename = void>
constexpr bool IsSameSuperInterface = true;
template <typename T>
constexpr bool IsSameSuperInterface<T, std::void_t<
	typename T::Interfaces
	>> = false;
template <typename T>
constexpr bool IsSameSuperInterface<T, std::void_t<
	typename T::Interfaces,
	typename T::Super,
	typename T::Super::Interfaces
	>> = std::is_same_v<typename T::Interfaces, typename T::Super::Interfaces>;

template<typename T, typename U = void>
struct InterfaceTypeDefiner
{
	using Type = void;
};
template<typename T>
struct InterfaceTypeDefiner<T, std::void_t<typename T::Interfaces>>
{
	using Type = typename T::Interfaces;
};
template<typename T>
using InterfaceType = InterfaceTypeDefiner<T>::Type;


/* Object */

template<typename T>
constexpr bool IsChildOfObject = std::is_base_of_v<Object, T>;
