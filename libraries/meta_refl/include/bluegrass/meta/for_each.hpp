#pragma once

#include <tuple>
#include <type_traits>

#include "function_traits.hpp"

namespace bluegrass { namespace meta {

namespace detail {
    template <std::size_t Index>
    using size_t_ = std::integral_constant<std::size_t, Index >;

    ///////////////////// Structure that can be converted to reference to anything
    struct ubiq_constructor {
        std::size_t ignore;
        template <class Type> constexpr operator Type&() const noexcept; // Undefined, allows initialization of reference fields (T& and const T&)
    };

    ///////////////////// Structure that can be converted to reference to anything except reference to T
    template <class T>
    struct ubiq_constructor_except {
        template <class Type> constexpr operator std::enable_if_t<!std::is_same<T, Type>::value, Type&> () const noexcept; // Undefined
    };

    ///////////////////// Hand-made is_aggregate_initializable_n<T> trait

    // `std::is_constructible<T, ubiq_constructor_except<T>>` consumes a lot of time, so we made a separate lazy trait for it.
    template <std::size_t N, class T> struct is_single_field_and_aggregate_initializable: std::false_type {};
    template <class T> struct is_single_field_and_aggregate_initializable<1, T>: std::integral_constant<
        bool, !std::is_constructible<T, ubiq_constructor_except<T>>::value
    > {};

    // Hand-made is_aggregate<T> trait:
    // Aggregates could be constructed from `decltype(ubiq_constructor{I})...` but report that there's no constructor from `decltype(ubiq_constructor{I})...`
    // Special case for N == 1: `std::is_constructible<T, ubiq_constructor>` returns true if N == 1 and T is copy/move constructible.
    template <class T, std::size_t N>
    struct is_aggregate_initializable_n {
        template <std::size_t ...I>
        static constexpr bool is_not_constructible_n(std::index_sequence<I...>) noexcept {
            return !std::is_constructible<T, decltype(ubiq_constructor{I})...>::value
                || is_single_field_and_aggregate_initializable<N, T>::value
            ;
        }

        static constexpr bool value =
            std::is_empty<T>::value
            || std::is_fundamental<T>::value
            || is_not_constructible_n(std::make_index_sequence<N>{})
        ;
    };

        ///////////////////// Helper for SFINAE on fields count
    template <class T, std::size_t... I>
    constexpr auto enable_if_constructible_helper(std::index_sequence<I...>) noexcept
        -> typename std::add_pointer<decltype(T{ ubiq_constructor{I}... })>::type;

    template <class T, std::size_t N, class /*Enable*/ = decltype( enable_if_constructible_helper<T>(std::make_index_sequence<N>()) ) >
    using enable_if_constructible_helper_t = std::size_t;

    ///////////////////// Non greedy fields count search. Templates instantiation depth is log(sizeof(T)), templates instantiation count is log(sizeof(T)).
    template <class T, std::size_t N>
    constexpr std::size_t detect_fields_count(size_t_<N>, size_t_<N>, long) noexcept {
        return N;
    }

    template <class T, std::size_t Begin, std::size_t Middle>
    constexpr std::size_t detect_fields_count(size_t_<Begin>, size_t_<Middle>, int) noexcept;

    template <class T, std::size_t Begin, std::size_t Middle>
    constexpr auto detect_fields_count(size_t_<Begin>, size_t_<Middle>, long) noexcept
        -> enable_if_constructible_helper_t<T, Middle>
    {
        using next_t = size_t_<Middle + (Middle - Begin + 1) / 2>;
        return detect_fields_count<T>(size_t_<Middle>{}, next_t{}, 1L);
    }

    template <class T, std::size_t Begin, std::size_t Middle>
    constexpr std::size_t detect_fields_count(size_t_<Begin>, size_t_<Middle>, int) noexcept {
        using next_t = size_t_<(Begin + Middle) / 2>;
        return detect_fields_count<T>(size_t_<Begin>{}, next_t{}, 1L);
    }

    ///////////////////// Greedy search. Templates instantiation depth is log(sizeof(T)), templates instantiation count is log(sizeof(T))*T in worst case.
    template <class T, std::size_t N>
    constexpr auto detect_fields_count_greedy_remember(size_t_<N>, long) noexcept
        -> enable_if_constructible_helper_t<T, N>
    {
        return N;
    }

    template <class T, std::size_t N>
    constexpr std::size_t detect_fields_count_greedy_remember(size_t_<N>, int) noexcept {
        return 0;
    }

    template <class T, std::size_t N>
    constexpr std::size_t detect_fields_count_greedy(size_t_<N>, size_t_<N>) noexcept {
        return detect_fields_count_greedy_remember<T>(size_t_<N>{}, 1L);
    }

    template <class T, std::size_t Begin, std::size_t Last>
    constexpr std::size_t detect_fields_count_greedy(size_t_<Begin>, size_t_<Last>) noexcept {
        constexpr std::size_t middle = Begin + (Last - Begin) / 2;
        constexpr std::size_t fields_count_big = detect_fields_count_greedy<T>(size_t_<middle + 1>{}, size_t_<Last>{});
        constexpr std::size_t fields_count_small = detect_fields_count_greedy<T>(size_t_<Begin>{}, size_t_<fields_count_big ? Begin : middle>{});
        return fields_count_big ? fields_count_big : fields_count_small;
    }

    ///////////////////// Choosing between greedy and non greedy search.
    template <class T, std::size_t N>
    constexpr auto detect_fields_count_dispatch(size_t_<N>, long) noexcept
        -> decltype(sizeof(T{}))
    {
        return detect_fields_count<T>(size_t_<0>{}, size_t_<N / 2 + 1>{}, 1L);
    }

    template <class T, std::size_t N>
    constexpr std::size_t detect_fields_count_dispatch(size_t_<N>, int) noexcept {
        // T is not default aggregate initialzable. It means that at least one of the members is not default constructible,
        // so we have to check all the aggregate initializations for T up to N parameters and return the bigest succeeded
        // (we can not use binary search for detecting fields count).
        return detect_fields_count_greedy<T>(size_t_<0>{}, size_t_<N>{});
    }

    ///////////////////// Returns non-flattened fields count
    template <class T>
    constexpr std::size_t fields_count() noexcept {
        using type = std::remove_cv_t<T>;

        static_assert(
            !std::is_reference<type>::value,
            "Attempt to get fields count on a reference. This is not allowed because that could hide an issue and different library users expect different behavior in that case."
        );

        static_assert(
            std::is_copy_constructible<std::remove_all_extents_t<type>>::value,
            "Type and each field in the type must be copy constructible."
        );

        static_assert(
            !std::is_polymorphic<type>::value,
            "Type must have no virtual function, because otherwise it is not aggregate initializable."
        );

    #ifdef __cpp_lib_is_aggregate
        static_assert(
            std::is_aggregate<type>::value             // Does not return `true` for build in types.
            || std::is_standard_layout<type>::value,   // Does not return `true` for structs that have non standard layout members.
            "Type must be aggregate initializable."
        );
    #endif

        constexpr std::size_t max_fields_count = (sizeof(type) * 8); // We multiply by 8 because we may have bitfields in T
        constexpr std::size_t result = detect_fields_count_dispatch<type>(size_t_<max_fields_count>{}, 1L);

        static_assert(
            is_aggregate_initializable_n<type, result>::value,
            "Types with user specified constructors (non-aggregate initializable types) are not supported."
        );

        static_assert(
            result != 0 || std::is_empty<type>::value || std::is_fundamental<type>::value || std::is_reference<type>::value,
            "Something went wrong. Please report this issue to the github along with the structure you're reflecting."
        );

        return result;
    }

    namespace sequence_tuple {

    template <std::size_t N, class T>
    struct base_from_member {
        T value;
    };

    template <class I, class ...Tail>
    struct tuple_base;

    template <std::size_t... I, class ...Tail>
    struct tuple_base< std::index_sequence<I...>, Tail... >
        : base_from_member<I , Tail>...
    {
        static constexpr std::size_t size_v = sizeof...(I);

        // We do not use `noexcept` in the following functions, because if user forget to put one then clang will issue an error:
        // "error: exception specification of explicitly defaulted default constructor does not match the calculated one".
        constexpr tuple_base() = default;
        constexpr tuple_base(tuple_base&&) = default;
        constexpr tuple_base(const tuple_base&) = default;

        constexpr tuple_base(Tail... v) noexcept
            : base_from_member<I, Tail>{ v }...
        {}
    };

    template <>
    struct tuple_base<std::index_sequence<> > {
        static constexpr std::size_t size_v = 0;
    };

    template <std::size_t N, class T>
    constexpr T& get_impl(base_from_member<N, T>& t) noexcept {
        return t.value;
    }

    template <std::size_t N, class T>
    constexpr const T& get_impl(const base_from_member<N, T>& t) noexcept {
        return t.value;
    }

    template <std::size_t N, class T>
    constexpr volatile T& get_impl(volatile base_from_member<N, T>& t) noexcept {
        return t.value;
    }

    template <std::size_t N, class T>
    constexpr const volatile T& get_impl(const volatile base_from_member<N, T>& t) noexcept {
        return t.value;
    }

    template <std::size_t N, class T>
    constexpr T&& get_impl(base_from_member<N, T>&& t) noexcept {
        return std::forward<T>(t.value);
    }

    template <class ...Values>
    struct tuple: tuple_base<
        std::make_index_sequence<sizeof...(Values)>,
        Values...>
    {
        using tuple_base<
            std::make_index_sequence<sizeof...(Values)>,
            Values...
        >::tuple_base;
    };

    template <std::size_t N, class ...T>
    constexpr decltype(auto) get(tuple<T...>& t) noexcept {
        static_assert(N < tuple<T...>::size_v, "Tuple index out of bounds");
        return get_impl<N>(t);
    }

    template <std::size_t N, class ...T>
    constexpr decltype(auto) get(const tuple<T...>& t) noexcept {
        static_assert(N < tuple<T...>::size_v, "Tuple index out of bounds");
        return get_impl<N>(t);
    }

    template <std::size_t N, class ...T>
    constexpr decltype(auto) get(const volatile tuple<T...>& t) noexcept {
        static_assert(N < tuple<T...>::size_v, "Tuple index out of bounds");
        return get_impl<N>(t);
    }

    template <std::size_t N, class ...T>
    constexpr decltype(auto) get(volatile tuple<T...>& t) noexcept {
        static_assert(N < tuple<T...>::size_v, "Tuple index out of bounds");
        return get_impl<N>(t);
    }

    template <std::size_t N, class ...T>
    constexpr decltype(auto) get(tuple<T...>&& t) noexcept {
        static_assert(N < tuple<T...>::size_v, "Tuple index out of bounds");
        return get_impl<N>(std::move(t));
    }

    template <std::size_t I, class T>
    using tuple_element = std::remove_reference< decltype(
            detail::sequence_tuple::get<I>( std::declval<T>() )
        ) >;
    } //sequence_tuple

    template <class... Args>
    constexpr auto make_tuple_of_references(Args&&... args) noexcept {
        return sequence_tuple::tuple<Args&...>{ args... };
    }

    template <class T>
    constexpr auto tie_as_tuple(T&, size_t_<0>) noexcept {
        return sequence_tuple::tuple<>{};
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<1>, std::enable_if_t<std::is_class< std::remove_cv_t<T> >::value>* = 0) noexcept {
        auto& [a] = val;
        return make_tuple_of_references(a);
    }


    template <class T>
        constexpr auto tie_as_tuple(T& val, size_t_<1>, std::enable_if_t<!std::is_class< std::remove_cv_t<T> >::value>* = 0) noexcept {
        return make_tuple_of_references( val );
    }


    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<2>) noexcept {
        auto& [a,b] = val;
        return make_tuple_of_references(a,b);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<3>) noexcept {
        auto& [a,b,c] = val;
        return make_tuple_of_references(a,b,c);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<4>) noexcept {
        auto& [a,b,c,d] = val;
        return make_tuple_of_references(a,b,c,d);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<5>) noexcept {
        auto& [a,b,c,d,e] = val;
        return make_tuple_of_references(a,b,c,d,e);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<6>) noexcept {
        auto& [a,b,c,d,e,f] = val;
        return make_tuple_of_references(a,b,c,d,e,f);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<7>) noexcept {
        auto& [a,b,c,d,e,f,g] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<8>) noexcept {
        auto& [a,b,c,d,e,f,g,h] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<9>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<10>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<11>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<12>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<13>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<14>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<15>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<16>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<17>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<18>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<19>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<20>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<21>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<22>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<23>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<24>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<25>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<26>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<27>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<28>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<29>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<30>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<31>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G);
    }

    template <class T>
    constexpr auto tie_as_tuple(T& val, size_t_<32>) noexcept {
        auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H] = val;
        return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H);
    }
template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<33>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<34>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<35>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<36>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<37>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<38>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<39>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<40>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<41>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<42>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<43>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<44>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<45>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<46>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<47>) noexcept {
  auto& [a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z] = val;
  return make_tuple_of_references(a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z);
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<48>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<49>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<50>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<51>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<52>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<53>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<54>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<55>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<56>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<57>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<58>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<59>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al,am
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al,am
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<60>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al,am,an
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al,am,an
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<61>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al,am,an,ap
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al,am,an,ap
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<62>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al,am,an,ap,aq
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al,am,an,ap,aq
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<63>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al,am,an,ap,aq,ar
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al,am,an,ap,aq,ar
  );
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<64>) noexcept {
  auto& [
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al,am,an,ap,aq,ar,as
  ] = val;

  return make_tuple_of_references(
    a,b,c,d,e,f,g,h,j,k,l,m,n,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,J,K,L,M,N,P,Q,R,S,U,V,W,X,Y,Z,
    aa,ab,ac,ad,ae,af,ag,ah,aj,ak,al,am,an,ap,aq,ar,as
  );
}

    template <class T>
    constexpr auto tie_as_tuple(T& val) noexcept {
        typedef size_t_<fields_count<T>()> fields_count_tag;
        return detail::tie_as_tuple(val, fields_count_tag{});
    }

    template <class T, class F, std::size_t... I>
    void for_each_field_dispatcher(T& t, F&& f, std::index_sequence<I...>) {
        std::forward<F>(f)(
            detail::tie_as_tuple(t)
        );
    }

    template <class T, class F, class I, class = decltype(std::declval<F>()(std::declval<T>(), I{}))>
    void for_each_field_impl_apply(T&& v, F&& f, I i, long) {
        std::forward<F>(f)(std::forward<T>(v), i);
    }

    template <class T, class F, class I>
    void for_each_field_impl_apply(T&& v, F&& f, I /*i*/, int) {
        std::forward<F>(f)(std::forward<T>(v));
    }

    template <class T, class F, std::size_t... I>
    void for_each_field_impl(T& t, F&& f, std::index_sequence<I...>, std::false_type /*move_values*/) {
        const int v[] = {(
            for_each_field_impl_apply(sequence_tuple::get<I>(t), std::forward<F>(f), size_t_<I>{}, 1L),
            0
        )...};
        (void)v;
    }


    template <class T, class F, std::size_t... I>
    void for_each_field_impl(T& t, F&& f, std::index_sequence<I...>, std::true_type /*move_values*/) {
        const int v[] = {(
            for_each_field_impl_apply(sequence_tuple::get<I>(std::move(t)), std::forward<F>(f), size_t_<I>{}, 1L),
            0
        )...};
        (void)v;
    }
} //detail

/// Calls `func` for each field of a `value`. Copied from boost library
///
/// \b Requires: C++17 or \constexprinit{C++14 constexpr aggregate intializable type}.
///
/// \param func must have one of the following signatures:
///     * any_return_type func(U&& field)                // field of value is perfect forwarded to function
///     * any_return_type func(U&& field, std::size_t i)
///     * any_return_type func(U&& value, I i)  // Here I is an `std::integral_constant<size_t, field_index>`
///
/// \param value To each field of this variable will be the `func` applied.
///
/// \rcast14
///
/// \b Example:
/// \code
///     struct my_struct { int i, short s; };
///     int sum = 0;
///     for_each_field(my_struct{20, 22}, [&sum](const auto& field) { sum += field; });
///     assert(sum == 42);
/// \endcode
template <class T, class F>
void for_each_field(T&& value, F&& func) {
    constexpr std::size_t fields_count_val = detail::fields_count<std::remove_reference_t<T>>();

    detail::for_each_field_dispatcher(
        value,
        [f = std::forward<F>(func), fields_count_val](auto&& t) mutable {

            detail::for_each_field_impl(
                t,
                std::forward<F>(f),
                std::make_index_sequence<fields_count_val>{},
                std::is_rvalue_reference<T&&>{}
            );
        },
        std::make_index_sequence<fields_count_val>{}
    );
}


template <size_t I = 0, template<typename...> typename Tuple, typename Fn, typename... Ts>
constexpr size_t for_each_impl(Tuple<Ts...>& tup, Fn fn) {
    if constexpr(I < sizeof...(Ts)) {
        using result_t = std::decay_t<decltype(fn(std::get<I>(tup)))>;
        if constexpr (std::is_same_v<void, result_t>) {
            fn(std::get<I>(tup));
        }
        else {
            if (fn(std::get<I>(tup)))
                return I;
        }

        return for_each_impl<I + 1>(tup, fn);
    }

    return I;
}

/// @brief iterates over tuple elements
/// @tparam Fn functor called on each element. If it returns value convertible to true
///         then for_each stops processing more elements and returns index of element
///         if function returns void or false it continues iterating until it reaches the end
/// @tparam Tuple<...> tuple type
/// @param t tuple
/// @param f functor
/// @return index of last processed element or size of tuple (if all elements were iterated)
template <typename Fn, typename Tuple>
constexpr inline auto for_each( Tuple& t, Fn f ) {
    return for_each_impl(t, f);
}

template<typename Fn, typename Tuple>
constexpr inline auto for_each( const Tuple& t, Fn f ) {
    return for_each_impl(const_cast<Tuple&>(t), f);
}

}} // bluegrass::meta