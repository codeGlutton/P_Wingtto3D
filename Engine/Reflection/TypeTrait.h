#pragma once

#include <type_traits>

class Object;

template<typename... Args>
constexpr bool HasAnyReference = (std::is_reference_v<Args> || ...);


/* Shared Ptr */

template<typename T>
constexpr bool IsSharedPtr_Internal = false;
template<typename T>
constexpr bool IsSharedPtr_Internal<std::shared_ptr<T>> = true;
template<typename T>
constexpr bool IsSharedPtr = IsSharedPtr_Internal<std::remove_cvref_t<T>>;

template<typename T>
constexpr bool IsWeakPtr_Internal = false;
template<typename T>
constexpr bool IsWeakPtr_Internal<std::weak_ptr<T>> = true;
template<typename T>
constexpr bool IsWeakPtr = IsWeakPtr_Internal<std::remove_cvref_t<T>>;


/* Container */

template <typename T, typename = void>
constexpr bool UseAllocator = false;
template <typename T>
constexpr bool UseAllocator<T, std::void_t<typename T::allocator_type>> = true;

template <typename T>
concept AdaptorContainer = requires(T container, typename T::value_type element) {
	typename T::container_type;
	container.push(element);
	container.pop();
};
template <typename T>
constexpr bool IsAdaptorContainer = AdaptorContainer<T>;

template <typename T>
concept IteratorContainer = std::ranges::range<T> && requires {
	typename T::value_type;
};
template <typename T>
constexpr bool IsIteratorContanier = IteratorContainer<T>;

template <typename T>
using IteratorElementType = std::ranges::range_value_t<T>;


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
struct SuperTypeDefiner<T, std::void_t<typename T::Base>>
{
	using Type = typename T::Base;
};
template<typename T>
using SuperDefineType = SuperTypeDefiner<T>::Type;

template<typename T, typename U = void>
struct SuperTypeFinder
{
	using Type = void;
};
template<typename T>
struct SuperTypeFinder<T, std::void_t<typename T::Super>>
{
	using Type = typename T::Super;
};
template<typename T>
using SuperType = SuperTypeFinder<T>::Type;


/* Interface */

template <typename T, typename = void>
constexpr bool IsInterface = true;
template <typename T>
constexpr bool IsInterface<T, std::void_t<typename T::Base>> = false;

template <typename T, typename = void>
constexpr bool HasNewInterface = false;
template <typename T>
constexpr bool HasNewInterface<T, std::void_t<typename T::Interfaces>> = true;

template<typename T, typename U = void>
struct InterfaceTypeFinderInternal
{
	using Type = void;
};
template<typename T>
struct InterfaceTypeFinderInternal<T, std::void_t<typename T::Interfaces>>
{
	using Type = typename T::Interfaces;
};
template<typename T, typename U = void>
struct InterfaceTypeFinder
{
	using Type = InterfaceTypeFinderInternal<T>::Type;
};
template<typename T>
struct InterfaceTypeFinder<T, std::void_t<typename T::Interfaces, typename T::Super, typename T::Super::Interfaces>>
{
	using Type = std::conditional_t<std::is_same_v<typename T::Interfaces, typename T::Super::Interfaces>, void, typename T::Interfaces>;
};
template<typename T>
using InterfaceDefineType = InterfaceTypeFinder<T>::Type;
template<typename T>
using InterfaceType = InterfaceTypeFinder<T>::Type;


/* Object */

template<typename T>
constexpr bool IsChildOfObject = std::is_base_of_v<Object, T>;


/* Bulk */

template <typename T, typename = void>
constexpr bool IsBulk = false;
template <typename T>
constexpr bool IsBulk<T, std::void_t<typename T::Bulk>> = true;