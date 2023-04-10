#pragma once
/**
 * @file typelist.hpp
 *
 * @brief Defines a template for manipulating and storing compile-time lists of types
 */

#include <type_traits>
#include <functional>
#include <stdexcept>
#include <string>

namespace bluegrass { namespace meta {

/// This namespace contains the List type, and all of the operations and queries which can be performed upon it
namespace TypeList {

// Forward declare the List so impl can see it
template<typename...> struct List;

namespace impl {
using TypeList::List;

template<typename, template<typename...> class> struct apply;
template<typename... Ts, template<typename...> class Delegate>
struct apply<List<Ts...>, Delegate> { using type = Delegate<Ts...>; };

template<typename... Ts>
struct length;
template<> struct length<> { constexpr static std::size_t value = 0; };
template<typename T, typename... Ts>
struct length<T, Ts...> { constexpr static std::size_t value = length<Ts...>::value+1; };

template<typename...> struct concat;
template<typename... OldTypes, typename... NewTypes>
struct concat<List<OldTypes...>, List<NewTypes...>> {
   using type = List<OldTypes..., NewTypes...>;
};
template<typename... OldTypes, typename... NewTypes, typename NextList, typename... Lists>
struct concat<List<OldTypes...>, List<NewTypes...>, NextList, Lists...> {
   using type = typename concat<List<OldTypes..., NewTypes...>, NextList, Lists...>::type;
};

template<std::size_t count> struct makeSequence;
template<> struct makeSequence<0> { using type = List<>; };
template<> struct makeSequence<1> { using type = List<std::integral_constant<std::size_t, 0>>; };
template<std::size_t count>
struct makeSequence {
   using type = typename concat<typename makeSequence<count-1>::type,
                                List<std::integral_constant<std::size_t, count-1>>>::type;
};

template<typename, typename> struct transform;
template<typename... ListTypes, typename Transformer>
struct transform<List<ListTypes...>, Transformer> {
   using type = List<typename Transformer::template transform<ListTypes>::type...>;
};

template<typename Search, typename List> struct indexOf;
template<typename Search> struct indexOf<Search, List<>> { constexpr static int value = -1; };
template<typename Search, typename T, typename... Ts>
struct indexOf<Search, List<T, Ts...>> {
    constexpr static int deeper = indexOf<Search, List<Ts...>>::value;
    constexpr static int value = std::is_same<Search, T>::value? 0 : (deeper == -1? -1 : deeper + 1);
};

template<typename...> struct concatUnique;
template<typename... Uniques>
struct concatUnique<List<Uniques...>, List<>> {
   using type = List<Uniques...>;
};
template<typename... Uniques, typename T>
struct concatUnique<List<Uniques...>, List<T>> {
   using type = std::conditional_t<indexOf<T, List<Uniques...>>::value >= 0,
                                   List<Uniques...>, List<Uniques..., T>>;
};
template<typename... Uniques, typename T1, typename T2, typename... Types>
struct concatUnique<List<Uniques...>, List<T1, T2, Types...>> {
   using type = typename concatUnique<
      typename concatUnique<List<Uniques...>, List<T1>>::type, List<T2, Types...>>::type;
};
template<typename... Uniques, typename... Lists>
struct concatUnique<List<Uniques...>, List<>, Lists...> {
   using type = typename concatUnique<List<Uniques...>, Lists...>::type;
};
template<typename Uniques, typename L1a, typename... L1s, typename L2, typename... Lists>
struct concatUnique<Uniques, List<L1a, L1s...>, L2, Lists...> {
   using type = typename concatUnique<typename concatUnique<Uniques, List<L1a, L1s...>>::type, L2, Lists...>::type;
};

template<typename, std::size_t> struct at;
template<typename T, typename... Types>
struct at<List<T, Types...>, 0> { using type = T; };
template<typename T, typename... Types, std::size_t index>
struct at<List<T, Types...>, index> : at<List<Types...>, index-1> {};

template<typename, typename, std::size_t> struct removeAt;
template<typename... Left, typename T, typename... Right>
struct removeAt<List<Left...>, List<T, Right...>, 0> { using type = List<Left..., Right...>; };
template<typename... Left, typename T, typename... Right, std::size_t index>
struct removeAt<List<Left...>, List<T, Right...>, index> {
   using type = typename removeAt<List<Left..., T>, List<Right...>, index-1>::type;
};

template<template<typename> class Filter, typename Filtered, typename List> struct filter;
template<template<typename> class Filter, typename... Filtered>
struct filter<Filter, List<Filtered...>, List<>> { using type = List<Filtered...>; };
template<template<typename> class Filter, typename... Filtered, typename T1, typename... Types>
struct filter<Filter, List<Filtered...>, List<T1, Types...>> {
   using type = typename std::conditional_t<Filter<T1>::value,
                                           filter<Filter, List<Filtered..., T1>, List<Types...>>,
                                           filter<Filter, List<Filtered...>, List<Types...>>>::type;
};

template<typename, typename, std::size_t, std::size_t, typename = void> struct slice;
template<typename... Results, typename... Types, std::size_t index>
struct slice<List<Results...>, List<Types...>, index, index, void> { using type = List<Results...>; };
template<typename... Results, typename T, typename... Types, std::size_t end>
struct slice<List<Results...>, List<T, Types...>, 0, end, std::enable_if_t<end != 0>>
        : slice<List<Results..., T>, List<Types...>, 0, end-1> {};
template<typename T, typename... Types, std::size_t start, std::size_t end>
struct slice<List<>, List<T, Types...>, start, end, std::enable_if_t<start != 0>>
        : slice<List<>, List<Types...>, start-1, end-1> {};

template<typename, typename> struct zip;
template<>
struct zip<List<>, List<>> { using type = List<>; };
template<typename A, typename... As, typename B, typename... Bs>
struct zip<List<A, As...>, List<B, Bs...>> {
   using type = typename concat<List<List<A, B>>, typename zip<List<As...>, List<Bs...>>::type>::type;
};

template<typename Callable, typename Ret, typename T>
Ret dispatch_helper(Callable& c) { return c(T()); }

} // namespace impl

/// The actual List type
template<typename... Types>
struct List { using type = List; };

/// Apply a List of types as arguments to another template
template<typename List, template<typename...> class Delegate>
using apply = typename impl::apply<List, Delegate>::type;

/// Get the number of types in a List
template<typename List>
constexpr static std::size_t length() { return apply<List, impl::length>::value; }

/// Concatenate two or more typeLists together
template<typename... Lists>
using concat = typename impl::concat<Lists...>::type;

/// Create a List of sequential integers ranging from [0, count)
template<std::size_t count>
using makeSequence = typename impl::makeSequence<count>::type;

/// Template to build TypeLists using the following syntax:
/// builder<>::type::add<T1>::add<T2>::add<T3>[...]::finalize
/// Or:
/// builder<>::type::addList<List<T1, T2>>::addList<T3, T4>>[...]::finalize
template<typename L = List<>>
struct builder {
   template<typename NewType> using add = typename builder<typename impl::concat<L, List<NewType>>::type>::type;
   template<typename NewList> using addList = typename builder<typename impl::concat<L, NewList>::type>::type;
   using type = builder;
   using finalize = L;
};

/// Transform elements of a TypeList
template<typename List, typename Transformer>
using transform = typename impl::transform<List, Transformer>::type;

/// Get the index of the given type within a List, or -1 if type is not found
template<typename List, typename T>
constexpr static int indexOf() { return impl::indexOf<T, List>::value; }

/// Check if a given type is in a List
template<typename List, typename T>
constexpr static bool contains() { return impl::indexOf<T, List>::value != -1; }

/// Remove duplicate items from one or more TypeLists and concatenate them all together
template<typename... TypeLists>
using concatUnique = typename impl::concatUnique<List<>, TypeLists...>::type;

/// Get the type at the specified List index
template<typename List, std::size_t index>
using at = typename impl::at<List, index>::type;

/// Get the type at the beginning of the List
template<typename List>
using first = at<List, 0>;
/// Get the type at the end of the List
template<typename List>
using last = at<List, length<List>()-1>;

/// Get the List with the element at the given index removed
template<typename L, std::size_t index>
using removeAt = typename impl::removeAt<List<>, L, index>::type;

/// Get the List with the given type removed
template<typename L, typename Remove>
using removeElement = removeAt<L, indexOf<L, Remove>()>;

/// Get a List with all elements that do not pass a filter removed
template<typename L, template<typename> class Filter>
using filter = typename impl::filter<Filter, List<>, L>::type;

/// Template to invert a filter, i.e. filter<myList, InvertFilter<myfilter>::type>
template<template<typename> class Filter>
struct InvertFilter {
   template<typename T>
   struct type { constexpr static bool value = !Filter<T>::value; };
};

/// Take the sublist at indexes [start, end)
template<typename L, std::size_t start, std::size_t end = length<L>()>
using slice = typename impl::slice<List<>, L, start, end>::type;

/// Zip two equal-length TypeLists together, i.e. zip<List<X, Y>, List<A, B>> == List<List<X, A>, List<Y, B>>
template<typename ListA, typename ListB>
using zip = typename impl::zip<ListA, ListB>::type;

/// Add indexes to types in the List, i.e. index<List<A, B, C>> == List<List<0, A>, List<1, B>, List<2, C>> where
/// 0, 1, and 2 are std::integral_constants of type std::size_t
template<typename List>
using index = typename impl::zip<typename impl::makeSequence<length<List>()>::type, List>::type;

/// This namespace contains some utilities that provide runtime operations on TypeLists
namespace runtime {
/// Type wrapper object allowing arbitrary types to be passed to functions as information rather than data
template<typename T> struct wrapper { using type = T; };

/**
 * @brief Index into the TypeList for a type T, and invoke the callable with an argument wrapper<T>()
 * @param index Index of the type in the TypeList to invoke the callable with
 * @param c The callable to invoke
 * @return The value returned by the callable
 * @note The callable return type must be the same for all List elements
 *
 * If index is out of bounds, returns null optional
 */
template<typename... Types, typename Callable, typename = std::enable_if_t<impl::length<Types...>::value != 0>,
         typename Return = decltype(std::declval<Callable>()(wrapper<at<List<Types...>, 0>>()))>
Return Dispatch(List<Types...>, uint64_t index, Callable c) {
   static std::function<Return(Callable&)> call_table[] =
      { impl::dispatch_helper<Callable, Return, wrapper<Types>>... };
   if (index < impl::length<Types...>::value) return call_table[index](c);
#if __cpp_exceptions
   throw std::out_of_range("Index " + std::to_string(index) + " is not in list of "
                           + std::to_string(length<List<Types...>>()) + " elements.");
#else
   std::abort();
#endif
}
template<typename List, typename Callable>
auto Dispatch(List l, int64_t index, Callable c) {
   if (index >= 0)
       return Dispatch(l, uint64_t(index), std::move(c));
#if __cpp_exceptions
   throw std::out_of_range("Index " + std::to_string(index) + " is not in list of "
                           + std::to_string(length<List>()) + " elements.");
#else
   std::abort();
#endif
}

/// @brief Invoke the provided callable with an argument wrapper<Type>() for each type in the List
template<typename... Types, typename Callable>
void ForEach(List<Types...>, Callable c) {
   bool trues[] = { [](Callable& c, auto t) { c(t); return true; }(c, wrapper<Types>())... };
   (void)(trues);
}

} } } } // namespace bluegrass::meta::TypeList::runtime
