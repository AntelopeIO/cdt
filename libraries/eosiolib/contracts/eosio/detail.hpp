#pragma once

#include "../../core/eosio/ignore.hpp"

namespace eosio { namespace detail {

   /// @cond INTERNAL

   template <typename T>
   struct unwrap { typedef T type; };

   template <typename T>
   struct unwrap<ignore<T>> { typedef T type; };

   template <typename R, typename Act, typename... Args>
   auto get_args(R(Act::*p)(Args...)) {
      return std::tuple<std::decay_t<typename unwrap<Args>::type>...>{};
   }

   template <typename R, typename Act, typename... Args>
   auto get_args_nounwrap(R(Act::*p)(Args...)) {
      return std::tuple<std::decay_t<Args>...>{};
   }

   template <auto Function>
   using deduced = decltype(get_args(Function));

   template <auto Function>
   using deduced_nounwrap = decltype(get_args_nounwrap(Function));

   template <typename T>
   struct convert { typedef T type; };

   template <>
   struct convert<const char*> { typedef std::string type; };

   template <>
   struct convert<char*> { typedef std::string type; };

   template <typename T, typename U>
   struct is_same { static constexpr bool value = std::is_convertible<T,U>::value; };

   template <typename U>
   struct is_same<bool,U> { static constexpr bool value = std::is_integral<U>::value; };

   template <typename T>
   struct is_same<T,bool> { static constexpr bool value = std::is_integral<T>::value; };

   // Full specialization to resolve ambiguity introduced by partial specializations
   // is_same<bool,U> and is_same<T,bool>
   template <>
   struct is_same<bool, bool> { static constexpr bool value = true; };

   template <size_t N, size_t I, auto Arg, auto... Args>
   struct get_nth_impl { static constexpr auto value  = get_nth_impl<N,I+1,Args...>::value; };

   template <size_t N, auto Arg, auto... Args>
   struct get_nth_impl<N, N, Arg, Args...> { static constexpr auto value = Arg; };

   template <size_t N, auto... Args>
   struct get_nth { static constexpr auto value  = get_nth_impl<N,0,Args...>::value; };

   template <auto Function, size_t I, typename T, typename... Rest>
   struct check_types {
      static_assert(detail::is_same<typename convert<T>::type, typename convert<typename std::tuple_element<I, deduced<Function>>::type>::type>::value);
      using type = check_types<Function, I+1, Rest...>;
      static constexpr bool value = true;
   };
   template <auto Function, size_t I, typename T>
   struct check_types<Function, I, T> {
      static_assert(detail::is_same<typename convert<T>::type, typename convert<typename std::tuple_element<I, deduced<Function>>::type>::type>::value);
      static constexpr bool value = true;
   };

   template <auto Function, typename... Ts>
   constexpr bool type_check() {
      static_assert(sizeof...(Ts) == std::tuple_size<deduced<Function>>::value);
      if constexpr (sizeof...(Ts) != 0)
         return check_types<Function, 0, Ts...>::value;
      return true;
   }

   // For non-function-pointers (function_traits is undefined)
   template <typename T>
   struct function_traits;
   
   // For non-const member function
   template <typename Class, typename Ret, typename... Args>
   struct function_traits<Ret (Class::*)(Args...)> {
       using return_type = Ret;
   };
   
   // For const member function
   template <typename Class, typename Ret, typename... Args>
   struct function_traits<Ret (Class::*)(Args...) const> {
       using return_type = Ret;
   };
   /// @endcond
}} // eosio detail
