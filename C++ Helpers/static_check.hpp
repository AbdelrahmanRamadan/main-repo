#ifndef STATIC_CHECK_HPP
#define STATIC_CHECK_HPP

#include <type_traits>

template <typename T, typename... Rest>
struct is_any : std::false_type {};

template <typename T, typename U>
struct is_any<T, U> : std::is_same<T, U> {};

template <typename T, typename First, typename... Rest>
struct is_any<T, First, Rest...> : std::integral_constant<bool, std::is_same<T, First>::value || is_any<T, Rest...>::value > {};

template <typename T, typename... Rest>
struct is_all_same : std::false_type {};

template <typename T>
struct is_all_same<T> : std::true_type {};

template <typename T, typename U>
struct is_all_same<T, U> : std::is_same<T, U> {};

template <typename T, typename First, typename... Rest>
struct is_all_same<T, First, Rest...> : std::integral_constant<bool, std::is_same<T, First>::value && is_all_same<T, Rest...>::value > {};




#endif //STATIC_CHECK_HPP
