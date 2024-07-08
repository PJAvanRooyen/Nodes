#ifndef StaticTypdefs_H
#define StaticTypdefs_H

#include <type_traits>

#include <list>
#include <vector>

#include <QList>

// test if a type is a list.
template<typename T>
struct is_list : std::false_type{};
template<typename... Ts>
struct is_list<std::list<Ts...>> : std::true_type{};
template<typename... Ts>
struct is_list<std::vector<Ts...>> : std::true_type{};
template<typename... Ts>
struct is_list<QList<Ts...>> : std::true_type{};

// Test if a type has a method named "METHOD_NAME".
#define HAS_METHOD(METHOD_NAME) \
template<class, class = void> \
struct has_##METHOD_NAME : std::false_type {}; \
template<class T> \
struct has_##METHOD_NAME<T, std::void_t<decltype(std::declval<T>().METHOD_NAME())>> : std::true_type {};

HAS_METHOD(pos)
HAS_METHOD(rect)
HAS_METHOD(size)

#endif // StaticTypdefs_H
