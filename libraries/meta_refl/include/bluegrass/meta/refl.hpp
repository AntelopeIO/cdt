#pragma once

#include "function_traits.hpp"
#include "utility.hpp"
#include "preprocessor.hpp"
#include "typelist.hpp"

#include <array>
#include <tuple>
#include <string_view>

namespace bluegrass { namespace meta {
   template <typename T>
   constexpr inline std::string_view type_name() {
      constexpr std::string_view full_name = __PRETTY_FUNCTION__;
      constexpr auto start = full_name.find("T = ");
#ifdef __clang__
      constexpr auto end = full_name.find("]");
#elif __GNUC__
      constexpr auto end = full_name.find(";");
#else
#error "Currently only supporting Clang and GCC compilers"
#endif

      return full_name.substr(start+4, end - start - 4);
   }

   // tag used to define an invalid fields for field_types
   struct invalid_fields {};

   namespace detail {
      namespace TL = TypeList;

      template <typename C>
      constexpr inline auto which_field_types() {
         if constexpr ( BLUEGRASS_HAS_MEMBER_TY(C, _bluegrass_meta_refl_valid) )
            return flatten_parameters_t<&C::_bluegrass_meta_refl_fields>{};
         else
            return invalid_fields{};
      }

      template <typename T>
      constexpr inline std::size_t fields_size() {
         if constexpr (std::is_same_v<T, invalid_fields>)
            return 0;
         else
            return TypeList::length<T>();
      }

      template<typename... Args>
      constexpr inline auto va_args_count_helper(Args&&...) { return sizeof...(Args); }

      template<typename Class>
      struct TransformToFieldPointer {
         template<typename Field>
         struct transform {
            using type = Field Class::*;
         };
      };
      
      // Given a reflected class type, infer the type of a tuple of pointer-to-member values for all of its members
      // Not to be called -- usable in decltype context only
      template<typename Class>
      inline auto field_ptrs_tuple_helper(Class&) ->
         TL::apply<TL::transform<flatten_parameters_t<&Class::_bluegrass_meta_refl_fields>,
                                                      TransformToFieldPointer<Class>>,
                   std::tuple>;

   } // ns bluegrass::meta::detail

   template <typename C>
   struct meta_object {
      constexpr static inline std::string_view name = type_name<C>();

      using field_types = decltype(detail::which_field_types<C>());
      template <std::size_t N>
      using field_type = TypeList::at<field_types, N>;
      constexpr static inline std::size_t field_count = detail::fields_size<field_types>();
      constexpr static auto field_names = C::_bluegrass_meta_refl_field_names();

      template <std::size_t N>
      constexpr static inline auto& get_field(C& c) {
         auto ptr = c.template _bluegrass_meta_refl_field_ptr<N>();
         return c.*ptr;
      }

      template <std::size_t N>
      constexpr static inline auto& get_field(const C& c) {
         auto ptr = c.template _bluegrass_meta_refl_field_ptr<N>();
         return c.*ptr;
      }

      template <typename MaybeConstC, typename F>
      constexpr inline static void for_each_field(MaybeConstC& c, F&& f) {
         static_assert(std::is_same_v<std::decay_t<MaybeConstC>, C>, "Provided object must be of reflected type");
         if constexpr (field_count == 0)
            return;
         else
            TypeList::Runtime::ForEach(TypeList::makeSequence<field_count>{}, [&c, &f](auto w) {
               constexpr auto Index = decltype(w)::type::value;
               f(get_field<Index>(c));
            });
      }
   };

}} // ns bluegrass::meta

#define BLUEGRASS_META_ADDR( CLASS, FIELD ) &CLASS::FIELD
#define BLUEGRASS_META_DECLTYPE( ignore, FIELD ) decltype(FIELD)
#define BLUEGRASS_META_PASS_STR( ignore, X ) #X

#define BLUEGRASS_META_VA_ARGS_SIZE(...)                         \
   bluegrass::meta::detail::va_args_count_helper(                \
         BLUEGRASS_META_FOREACH(                                 \
            BLUEGRASS_META_PASS_STR, "ignored", ##__VA_ARGS__))

// NOTE: There's no fundamental reason the ..._field_ptr(s) methods can't be static like the others, and it would be
// much nicer if they were; however, a static method can't use the decltype(*this) trick to get the class type, and
// without the class type, we can't (a) reference members directly, nor (b) declare a pointer to member variable.
// If we adjusted this macro's interface to take the class name (or came up with another trick like decltype(*this)),
// we could use option (b) above and make everything static.
#define BLUEGRASS_META_REFL(...)                                                                      \
   static constexpr void _bluegrass_meta_refl_valid();                                                \
   static void _bluegrass_meta_refl_fields                                                            \
      ( BLUEGRASS_META_FOREACH(BLUEGRASS_META_DECLTYPE, "ignored", ##__VA_ARGS__) ){}                 \
   inline auto _bluegrass_meta_refl_field_ptrs() const                                                \
     -> decltype(bluegrass::meta::detail::field_ptrs_tuple_helper(std::declval<decltype(*this)>())) { \
      using ClassType = std::decay_t<decltype(*this)>;                                                \
      return {BLUEGRASS_META_FOREACH(BLUEGRASS_META_ADDR, ClassType, ##__VA_ARGS__)};                 \
   }                                                                                                  \
   template <std::size_t N>                                                                           \
   inline auto _bluegrass_meta_refl_field_ptr() const                                                 \
         -> std::tuple_element_t<N, decltype(_bluegrass_meta_refl_field_ptrs())> {                    \
     return std::get<N>(_bluegrass_meta_refl_field_ptrs());                                           \
   }                                                                                                  \
   constexpr inline static auto _bluegrass_meta_refl_field_names() {                                  \
      return std::array<std::string_view, BLUEGRASS_META_VA_ARGS_SIZE(__VA_ARGS__)> {                 \
         BLUEGRASS_META_FOREACH(BLUEGRASS_META_PASS_STR, "ignored", ##__VA_ARGS__)                    \
      };                                                                                              \
   }
#define BLUEGRASS_META_REFL_BASES(...) \
    static void _bluegrass_meta_refl_bases( __VA_ARGS__ ){}

