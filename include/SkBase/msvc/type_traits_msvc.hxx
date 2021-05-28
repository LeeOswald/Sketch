#pragma once



namespace sketch
{


template <class _Ty, class _U>
struct is_same
    : public false_type
{
};

template <class _Ty>
struct is_same<_Ty, _Ty>
    : public true_type
{
};

template <class _Base, class _Derived>
struct is_base_of
    : public integral_constant<bool, __is_base_of(_Base, _Derived)>
{
};

template <class _From, class _To>
struct is_convertible
    : public integral_constant<bool, __is_convertible_to(_From, _To)>
{
};

template <class _Ty>
struct remove_const
{
    typedef _Ty type;
};

template <class _Ty>
struct remove_const<const _Ty>
{
    typedef _Ty type;
};

template <class _Ty>
struct remove_volatile
{
    typedef _Ty type;
};

template <class _Ty>
struct remove_volatile<volatile _Ty>
{
    typedef _Ty type;
};

template <class _Ty>
struct remove_cv
{
    typedef _Ty type;
};

template <class _Ty>
struct remove_cv<const _Ty>
{
    typedef _Ty type;
};

template <class _Ty>
struct remove_cv<volatile _Ty>
{
    typedef _Ty type;
};

template <class _Ty>
struct remove_cv<volatile const _Ty>
{
    typedef _Ty type;
};

template <class _Ty>
struct add_const
{
    typedef const _Ty type;
};

template <class _Ty>
struct add_const<_Ty&>
{
    typedef const _Ty& type;
};

template <class _Ty>
struct add_volatile
{
    typedef volatile _Ty type;
};

template <class _Ty>
struct add_cv
{
    typedef const volatile _Ty type;
};

template <class _Ty>
struct remove_reference
{
    typedef _Ty type;
};

template <class _Ty>
struct remove_reference<_Ty&>
{
    typedef _Ty type;
};

template <class _Ty>
struct remove_reference<_Ty&&>
{
    typedef _Ty type;
};

template <class _Ty>
struct add_lvalue_reference
{
    typedef _Ty& type;
};

template <class _Ty>
struct add_lvalue_reference<_Ty&>
{
    typedef _Ty& type;
};

template <class _Ty>
struct add_rvalue_reference
{
    typedef _Ty&& type;
};

template <class _Ty>
struct add_rvalue_reference<_Ty&>
{
    typedef _Ty&& type;
};

template <class _Ty>
struct add_rvalue_reference<_Ty&&>
{
    typedef _Ty&& type;
};

template <class _Ty>
struct _add_rvalue_reference
{
    typedef _Ty&& type;
};

template <class _Ty>
struct _add_rvalue_reference<_Ty&>
{
    typedef _Ty&& type;
};

template <class _Ty>
struct _add_rvalue_reference<_Ty&&>
{
    typedef _Ty&& type;
};

template <class _Ty>
struct add_reference
    : public add_lvalue_reference<_Ty>
{
};


namespace __
{

template<typename _Ty> struct change_sign;

template<>
struct change_sign<unsigned char>
{
    typedef signed char type;
};

template<>
struct change_sign<signed char>
{
    typedef unsigned char type;
};

template<>
struct change_sign<unsigned short int>
{
    typedef signed short int type;
};

template<>
struct change_sign<signed short int>
{
    typedef unsigned short int type;
};

template<>
struct change_sign<unsigned int>
{
    typedef signed int type;
};

template<>
struct change_sign<signed int>
{
    typedef unsigned int type;
};

template<>
struct change_sign<unsigned long int>
{
    typedef signed long int type;
};

template<>
struct change_sign<signed long int>
{
    typedef unsigned long int type;
};

template<size_t _SizeOfT> struct map_to_fundamental;

template<>
struct map_to_fundamental<0>
{
    typedef void type;
};

template<>
struct map_to_fundamental<1>
{
    typedef int8_t type;
};

template<>
struct map_to_fundamental<2>
{
    typedef int16_t type;
};

template<>
struct map_to_fundamental<4>
{
    typedef int32_t type;
};

template<>
struct map_to_fundamental<8>
{
    typedef int64_t type;
};

template<typename _EnumT>
struct enum_base
{
    typedef typename map_to_fundamental<sizeof(_EnumT)>::type type;
};

template <class _Ty, bool _IsEnum> struct make_signed2
{
private:
    static const bool isconst = is_const<_Ty>::value;
    static const bool isvolatile = is_volatile<_Ty>::value;

    typedef typename remove_cv<_Ty>::type _U;

    typedef typename conditional<is_signed<_U>::value, _U, typename __::change_sign<_U>::type>::type V;

    typedef typename conditional<isvolatile, typename add_volatile<V>::type, V>::type W;
    typedef typename conditional<isconst, typename add_const<W>::type, W>::type X;

public:
    typedef X type;
};

template <class _Ty, bool isenum> struct make_unsigned2
{
    static_assert(is_integral<_Ty>::value, "_Ty shall be integral type");
    static_assert((!is_same<_Ty, bool>::value), "_Ty shall not be a bool type");
private:
    static const bool isconst = is_const<_Ty>::value, isvolatile = is_volatile<_Ty>::value;
    typedef typename remove_cv<_Ty>::type _U;
    typedef typename conditional<is_unsigned<_U>::value, _U, typename __::change_sign<_U>::type>::type V;
    typedef typename conditional<isvolatile, typename add_volatile<V>::type, V>::type W;
    typedef typename conditional<isconst, typename add_const<W>::type, W>::type X;
public:
    typedef X type;
};

template<class _Ty> struct make_signed2<_Ty, true>
{
    typedef typename enum_base<_Ty>::type type;
};

template<class _Ty> struct make_unsigned2<_Ty, true>
{
    typedef typename make_unsigned2<typename enum_base<_Ty>::type, false>::type type;
};


} // namespace __ {}


template<class _Ty>
struct make_signed
{
    static_assert(is_integral<_Ty>::value || is_enum<_Ty>::value, "_Ty shall be an integral type or enumeration");
    static_assert((!is_same<_Ty, bool>::value), "_Ty shall not be a bool type");

    typedef typename __::make_signed2<_Ty, is_enum<_Ty>::value>::type type;
};

template<class _Ty>
struct make_unsigned
{
    static_assert(is_integral<_Ty>::value || is_enum<_Ty>::value, "_Ty shall be an integral type or enumeration");
    static_assert((!is_same<_Ty, bool>::value), "_Ty shall not be a bool type");

    typedef typename __::make_unsigned2<_Ty, is_enum<_Ty>::value>::type type;
};

template<>
struct make_signed<char>
{
    typedef signed char type;
};

template<>
struct make_unsigned<char>
{
    typedef unsigned char type;
};


template <class _Ty>
struct remove_extent
{
    typedef _Ty type;
};

template <class _Ty>
struct remove_extent<_Ty[]>
{
    typedef _Ty type;
};

template <class _Ty, size_t _Size>
struct remove_extent<_Ty[_Size]>
{
    typedef _Ty type;
};

template <class _Ty>
struct remove_all_extents
{
    typedef _Ty type;
};

template <class _Ty>
struct remove_all_extents <_Ty[]>
{
    typedef typename remove_all_extents<_Ty>::type type;
};

template <class _Ty, size_t _Size>
struct remove_all_extents<_Ty[_Size]>
{
    typedef typename remove_all_extents<_Ty>::type type;
};



template <class _Ty>
struct remove_pointer
{
    typedef _Ty type;
};

template <class _Ty>
struct remove_pointer<_Ty*>
{
    typedef _Ty type;
};


template <class _Ty>
struct add_pointer
{
    typedef _Ty* type;
};

template <class _Ty>
struct add_pointer<_Ty&>
{
    typedef typename remove_reference<_Ty>::type* type;
};


template <class _Ty> struct decay;

template <bool _Condition, class _Ty> struct enable_if {};

template <class _Ty>
struct enable_if<true, _Ty>
{
    typedef _Ty type;
};

template <bool, class _IfTrueType, class _IfFalseType> struct conditional;

template <class _Ty, class _F>
struct conditional<true, _Ty, _F>
{
    typedef _Ty type;
};

template <class _Ty, class _F>
struct conditional<false, _Ty, _F>
{
    typedef _F type;
};


namespace __
{


template<bool _cond1, bool _cond2, class _IfTrueType, class _IfFalseType>
struct select_or
{
    typedef _IfFalseType type;
};

template<bool _cond2, class _Ty, class _F>
struct select_or<true, _cond2, _Ty, _F>
{
    typedef _Ty type;
};

template<bool _cond1, class _Ty, class _F>
struct select_or<_cond1, true, _Ty, _F>
{
    typedef _Ty type;
};

template<class _Ty, class _F>
struct select_or<true, true, _Ty, _F>
{
    typedef _Ty type;
};

template<bool _cond1, bool _cond2, class _IfTrueType, class _IfFalseType>
struct select_and
{
    typedef _IfFalseType type;
};

template<class _Ty, class _F>
struct select_and<true, true, _Ty, _F>
{
    typedef _Ty type;
};


} // namespace __ {}


#define KSTL__STL_DEF_TRAIT(trait)\
    template <class _Ty> struct trait : public integral_constant<bool, __##trait(_Ty)> {};
#define KSTL__STL_DEF_TRAIT2(trait, builtin_trait)\
    template <class _Ty> struct trait : public integral_constant<bool, __##builtin_trait(_Ty)> {};



template <class _Ty>
struct is_void
    : public false_type
{
};

template <>
struct is_void<void>
    : public true_type
{
};

template <>
struct is_void<const void>
    : public true_type
{
};

template <>
struct is_void<volatile void>
    : public true_type
{
};

template <>
struct is_void<const volatile void>
    : public true_type
{
};


template <class _Ty>
struct is_array
    : public false_type
{
};

template <class _Ty>
struct is_array<_Ty[]>
    : public true_type
{
};

template <class _Ty, size_t _N>
struct is_array<_Ty[_N]>
    : public true_type
{
};

template <class _Ty>
struct is_pointer
    : public false_type
{
};

template <class _Ty>
struct is_pointer<_Ty*>
    : public true_type
{
};

template <class _Ty>
struct is_pointer<_Ty* const>
    : public true_type
{
};

template <class _Ty>
struct is_pointer<_Ty* volatile>
    : public true_type
{
};

template <class _Ty>
struct is_pointer<_Ty* const volatile>
    : public true_type
{
};

template <class _Ty>
struct is_lvalue_reference
    : public false_type
{
};

template <class _Ty>
struct is_lvalue_reference<_Ty&>
    : public true_type
{
};

template <class _Ty>
struct is_rvalue_reference
    : public false_type
{
};

template <class _Ty>
struct is_rvalue_reference<_Ty&&>
    : public true_type
{
};

template <class _Ty>
struct is_member_object_pointer
    : public false_type
{
};

template <class _Ty>
struct is_member_function_pointer
    : public false_type
{
};


#undef KSTL_TT_TCV

#undef KSTL_TT_PCV
#define KSTL_TT_PCV
#include "tt_ismemptr.inl"
#undef KSTL_TT_PCV
#define KSTL_TT_PCV const
#include "tt_ismemptr.inl"
#undef KSTL_TT_PCV
#define KSTL_TT_PCV volatile
#include "tt_ismemptr.inl"
#undef KSTL_TT_PCV
#define KSTL_TT_PCV const volatile
#include "tt_ismemptr.inl"

#undef KSTL_TT_TCV
#define KSTL_TT_TCV const

#undef KSTL_TT_PCV
#define KSTL_TT_PCV
#include "tt_ismemptr.inl"
#undef KSTL_TT_PCV
#define KSTL_TT_PCV const
#include "tt_ismemptr.inl"
#undef KSTL_TT_PCV
#define KSTL_TT_PCV volatile
#include "tt_ismemptr.inl"
#undef KSTL_TT_PCV
#define KSTL_TT_PCV const volatile
#include "tt_ismemptr.inl"

#undef KSTL_TT_TCV
#define KSTL_TT_TCV volatile

#undef KSTL_TT_PCV
#define KSTL_TT_PCV
#include "tt_ismemptr.inl"
#undef KSTL_TT_PCV
#define KSTL_TT_PCV const
#include "tt_ismemptr.inl"
#undef KSTL_TT_PCV
#define KSTL_TT_PCV volatile
#include "tt_ismemptr.inl"
#undef KSTL_TT_PCV
#define KSTL_TT_PCV const volatile
#include "tt_ismemptr.inl"

#undef KSTL_TT_TCV
#define KSTL_TT_TCV const volatile

#undef KSTL_TT_PCV
#define KSTL_TT_PCV
#include "tt_ismemptr.inl"
#undef KSTL_TT_PCV
#define KSTL_TT_PCV const
#include "tt_ismemptr.inl"
#undef KSTL_TT_PCV
#define KSTL_TT_PCV volatile
#include "tt_ismemptr.inl"
#undef KSTL_TT_PCV
#define KSTL_TT_PCV const volatile
#include "tt_ismemptr.inl"

#undef KSTL_TT_PCV
#undef KSTL_TT_TCV


KSTL__STL_DEF_TRAIT(is_enum)

KSTL__STL_DEF_TRAIT(is_union)

KSTL__STL_DEF_TRAIT(is_class)


template <class _Ty>
struct is_function
    : public false_type
{
};


#include "tt_isfuncc.inl"

#ifndef _M_X64
#define KSTL_TT_CC __cdecl
#include "tt_isfunc.inl"
#undef  KSTL_TT_CC
#define KSTL_TT_CC __stdcall
#include "tt_isfunc.inl"
#undef  KSTL_TT_CC
#define KSTL_TT_CC __fastcall
#include "tt_isfunc.inl"
#undef  KSTL_TT_CC
#else
#define KSTL_TT_CC
#include "tt_isfunc.inl"
#undef  KSTL_TT_CC
#endif


namespace __
{

template<class _Ty> struct is_int_helper
{
    template<class>
    struct func_limits
    {
        static const bool is_integer = false;
        static const bool is_iec559 = false;
    };

    typedef typename remove_cv<_Ty>::type U;
    typedef typename conditional<is_function<U>::value || is_array<U>::value, func_limits<U>, numeric_limits<U> >::type type;
};

} // namespace _ {}


template <class _Ty>
struct is_integral
    : public integral_constant<bool, __::is_int_helper<_Ty>::type::is_integer>
{
};

#ifndef SKETCH_KM

template <class _Ty>
struct is_floating_point
    : public integral_constant<bool, __::is_int_helper<_Ty>::type::is_iec559>
{
};

#endif

template <class _Ty>
struct decay
{
private:
    typedef typename remove_reference<_Ty>::type U;

public:
    typedef typename conditional<
        is_array<U>::value,
        typename remove_extent<U>::type*,
        typename conditional<is_function<U>::value,
            typename add_pointer<U>::type,
            typename remove_cv<U>::type
            >::type
        >::type
        type;
};


template <class _Ty>
struct is_reference
    : public integral_constant<
    bool,
    is_lvalue_reference<_Ty>::value || is_rvalue_reference<_Ty>::value
    >
{
};

template <class _Ty>
struct is_arithmetic
    : public integral_constant<
    bool,
    is_integral<_Ty>::value || numeric_limits<_Ty>::is_iec559
    >
{
};

template <class _Ty> struct is_fundamental
    : public integral_constant<
    bool,
    is_arithmetic<_Ty>::value || is_void<_Ty>::value
    >
{
};

template <class _Ty>
struct is_object
    : public integral_constant<
    bool,
    !is_void<_Ty>::value && !is_reference<_Ty>::value && !is_function<_Ty>::value
    >
{
};

template <class _Ty>
struct is_member_pointer
    : public integral_constant<
    bool,
    is_member_object_pointer<_Ty>::value || is_member_function_pointer<_Ty>::value
    >
{
};

template <class _Ty>
struct is_scalar
    : public integral_constant<
    bool,
    is_enum<_Ty>::value || is_integral<_Ty>::value || is_pointer<_Ty>::value || is_member_pointer<_Ty>::value
    >
{
};

template <class _Ty>
struct is_compound
    : public integral_constant<
    bool,
    is_array<_Ty>::value
        || is_function<_Ty>::value
        || is_pointer<_Ty>::value
        || is_reference<_Ty>::value
        || is_class<_Ty>::value
        || is_union<_Ty>::value
        || is_enum<_Ty>::value
        || is_member_pointer<_Ty>::value
    >
{
};



template <class _Ty>
struct is_const
    : public false_type
{
};

template <class _Ty>
struct is_const<const _Ty>
    : public true_type
{
};

template <class _Ty>
struct is_volatile
    : public false_type
{
};

template <class _Ty>
struct is_volatile<volatile _Ty>
    : public true_type
{
};


namespace __
{

template<class _Ty>
struct is_unknown_array
    : public false_type
{
};

template<class _Ty>
struct is_unknown_array<_Ty[]>
    : public true_type
{
};

template<class _Ty, size_t _Size>
struct is_unknown_array<_Ty[_Size]>
    : public false_type
{
};

} // namespace _ {}


template<class _Ty> struct is_trivial;
template<class _Ty> struct is_standard_layout;


template <class _Ty>
struct is_pod
    : public integral_constant<
    bool,
    is_scalar<typename remove_extent<_Ty>::type>::value
    || __is_pod(typename remove_extent<_Ty>::type)
    >
{
};

template<class _Ty>
struct is_standard_layout :
    public is_pod<_Ty>
{
};


KSTL__STL_DEF_TRAIT(is_empty)

KSTL__STL_DEF_TRAIT(is_polymorphic)

KSTL__STL_DEF_TRAIT(is_abstract)

KSTL__STL_DEF_TRAIT2(has_copy_constructor, has_copy)
KSTL__STL_DEF_TRAIT2(has_copy_assign, has_assign)

KSTL__STL_DEF_TRAIT2(has_trivial_default_constructor, has_trivial_constructor)
KSTL__STL_DEF_TRAIT2(has_trivial_copy_constructor, has_trivial_copy)
KSTL__STL_DEF_TRAIT(has_trivial_assign)
KSTL__STL_DEF_TRAIT(has_trivial_destructor)

KSTL__STL_DEF_TRAIT2(has_nothrow_default_constructor, has_nothrow_constructor)
KSTL__STL_DEF_TRAIT2(has_nothrow_copy_constructor, has_nothrow_copy)
KSTL__STL_DEF_TRAIT(has_nothrow_assign)

KSTL__STL_DEF_TRAIT2(is_destructible, has_user_destructor)
KSTL__STL_DEF_TRAIT(has_virtual_destructor)


template<class _Ty, class... _Args>
struct is_constructible
    : sketch::integral_constant<bool, __is_constructible(_Ty, _Args...)>
{
};

template<class _Ty>
struct is_default_constructible
    : is_constructible<_Ty>
{
};

template<class _Ty>
struct is_copy_constructible
    : is_constructible<_Ty, typename add_lvalue_reference<typename add_const<_Ty>::type>::type>
{
};

template<class _Ty>
struct is_move_constructible
    : is_constructible<_Ty, typename add_rvalue_reference<_Ty>::type>
{
};


template <class _Ty>
struct is_signed
    : public integral_constant<bool, (static_cast<_Ty>(-1) < 0)>
{
};

template <class _Ty>
struct is_unsigned
    : public integral_constant<bool, (static_cast<_Ty>(-1) > 0)>
{
};


template <class _Ty>
struct alignment_of
    : public integral_constant<size_t, alignof(_Ty)>
{
};

template <>
struct alignment_of<void>
    : public integral_constant<size_t, 0>
{
};

template <>
struct alignment_of<const void>
    : public integral_constant<size_t, 0>
{
};

template <>
struct alignment_of<volatile void>
    : public integral_constant<size_t, 0>
{
};

template <>
struct alignment_of<const volatile void>
    : public integral_constant<size_t, 0>
{
};


template <class _Ty>
struct rank
    : public integral_constant<size_t, 0>
{
};

template <class _Ty>
struct rank<_Ty[]>
    : public integral_constant<size_t, rank<_Ty>::value + 1>
{
};

template <class _Ty, size_t _Size>
struct rank<_Ty[_Size]>
    : public integral_constant<size_t, rank<_Ty>::value + 1>
{
};


template <class _Ty, unsigned _I>
struct extent
    : public integral_constant<size_t, 0>
{
};

template <class _Ty, unsigned _I>
struct extent<_Ty[], _I>
    : public integral_constant<size_t, extent<_Ty, _I - 1>::value>
{
};

template <class _Ty, unsigned _I, size_t _Size>
struct extent<_Ty[_Size], _I>
    : public integral_constant<size_t, extent<_Ty, _I - 1>::value>
{
};

template <class _Ty, size_t _Size>
struct extent<_Ty[_Size], 0>
    : public integral_constant<size_t, _Size>
{
};

template<class _Ty>
struct is_trivial
    : public integral_constant<
    bool,
    (
        has_trivial_default_constructor<_Ty>::value
        && has_trivial_copy_constructor<_Ty>::value
        && has_trivial_assign<_Ty>::value
        && has_trivial_destructor<_Ty>::value
    )
    >
{
};


template <class... _Ty> struct common_type;

template <class _Ty>
struct common_type<_Ty>
{
    typedef _Ty type;
};

template <class _Ty, class _U>
struct common_type<_Ty, _U>
{
private:
    static _Ty&& __t();
    static _U&& __u();

public:
    typedef decltype(true ? __t() : __u()) type;
};

template <class _Ty, class _U, class... _V>
struct common_type<_Ty, _U, _V...>
{
    typedef typename common_type<typename common_type<_Ty, _U>::type, _V...>::type type;
};


template<typename _Ty>
struct enum_base
{
    static_assert(is_enum<_Ty>::value, "_Ty shall be an enumeration type");

private:
    typedef typename __::map_to_fundamental<sizeof(_Ty)>::type U;
    typedef conditional<is_unsigned<_Ty>::value, typename make_unsigned<U>::type, U> V;

public:
    typedef typename V::type type;
};


} // namespace sketch {}
