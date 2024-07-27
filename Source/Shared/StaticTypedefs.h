#ifndef StaticTypdefs_H
#define StaticTypdefs_H

#include <type_traits>

#include <list>
#include <vector>
#include <memory>

#include <QList>
#include <QRectF>

// test if a type is a list.
template<typename T>
struct is_list : std::false_type{};
template<typename... Ts>
struct is_list<std::list<Ts...>> : std::true_type{};
template<typename... Ts>
struct is_list<std::vector<Ts...>> : std::true_type{};
template<typename... Ts>
struct is_list<QList<Ts...>> : std::true_type{};

template <typename T>
struct is_unique_ptr : std::false_type {};
template <typename T>
struct is_unique_ptr<std::unique_ptr<T>> : std::true_type {};
template <typename T>
constexpr bool is_unique_ptr_v = is_unique_ptr<T>::value;

template <typename T>
struct is_shared_ptr : std::false_type {};
template <typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};
template <typename T>
constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

// Test if a type has a method named "METHOD_NAME".
#define HAS_METHOD(METHOD_NAME) \
template<class, class = void> \
struct has_##METHOD_NAME : std::false_type {}; \
template<class T> \
struct has_##METHOD_NAME<T, std::void_t<decltype(std::declval<T>().METHOD_NAME())>> : std::true_type {};

#define HAS_SETTER_METHOD(METHOD_NAME, ParamType) \
template<class, class, class = void> \
    struct has_##METHOD_NAME : std::false_type {}; \
template<class T, class ParamType> \
struct has_##METHOD_NAME<T, ParamType, std::void_t<decltype(std::declval<T>().METHOD_NAME(std::declval<ParamType>()))>> : std::true_type {};

HAS_METHOD(rect)
HAS_SETTER_METHOD(setRect, QRectF)

#endif // StaticTypdefs_H
