#pragma once

namespace sketch
{


template <class _Ty, _Ty _v>
struct integral_constant
{
    static const _Ty value = _v;
    typedef _Ty value_type;
    typedef integral_constant<_Ty, _v> type;

    constexpr operator value_type() const
    {
        return value;
    }

    constexpr value_type operator()() const
    {
        return value;
    }
};


typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;


template <class _Ty, class _U> struct is_same;
template <class _Base, class _Derived> struct is_base_of;
template <class _From, class _To> struct is_convertible;
template <class _From, class _To> struct is_explicitly_convertible;

template <class _Ty> struct remove_const;
template <class _Ty> struct remove_volatile;
template <class _Ty> struct remove_cv;
template <class _Ty> struct add_const;
template <class _Ty> struct add_volatile;
template <class _Ty> struct add_cv;

template <class _Ty> struct remove_reference;
template <class _Ty> struct add_lvalue_reference;
template <class _Ty> struct add_rvalue_reference;
template <class _Ty> struct add_reference;


template <class _Ty>
typename add_rvalue_reference<_Ty>::type declval() noexcept;


template <class _Ty> struct make_signed;
template <class _Ty> struct make_unsigned;


template <class _Ty> struct remove_extent;
template <class _Ty> struct remove_all_extents;


template <class _Ty> struct remove_pointer;
template <class _Ty> struct add_pointer;



namespace aux
{

template <sketch::size_t _Align> struct aligner;

}


template <sketch::size_t _Len, sketch::size_t _Align = alignof(void*)>
struct aligned_storage;

template <sketch::size_t _Len, class... _Types> struct aligned_union;


template <class _Ty> struct decay;
template <bool, class _Ty = void> struct enable_if;
template <bool, class IfTrueType, class IfFalseType> struct conditional;

namespace __
{

template<bool _cond1, bool _cond2, class _IfTrueType, class _IfFalseType>
struct select_or;

}

template <class _Ty> struct is_void;
template <class _Ty> struct is_integral;
template <class _Ty> struct is_floating_point;
template <class _Ty> struct is_array;
template <class _Ty> struct is_pointer;
template <class _Ty> struct is_lvalue_reference;
template <class _Ty> struct is_rvalue_reference;
template <class _Ty> struct is_member_object_pointer;
template <class _Ty> struct is_member_function_pointer;
template <class _Ty> struct is_enum;
template <class _Ty> struct is_union;
template <class _Ty> struct is_class;
template <class _Ty> struct is_function;
template <class _RT> struct is_function<_RT()>;


template <class _Ty> struct is_reference;
template <class _Ty> struct is_arithmetic;
template <class _Ty> struct is_fundamental;
template <class _Ty> struct is_object;
template <class _Ty> struct is_member_pointer;
template <class _Ty> struct is_scalar;
template <class _Ty> struct is_compound;


template <class _Ty> struct is_const;
template <class _Ty> struct is_volatile;

namespace __
{

template<class _Ty> struct is_unknown_array;

}

template<class _Ty> struct is_trivial;
template<class _Ty> struct is_trivially_copyable;
template<class _Ty> struct is_standard_layout;

template <class _Ty> struct is_pod;
template <class _Ty> struct is_literal_type;
template <class _Ty> struct is_empty;
template <class _Ty> struct is_polymorphic;
template <class _Ty> struct is_abstract;

template <class _Ty> struct has_virtual_destructor;

template <class _Ty> struct has_default_constructor;
template <class _Ty> struct has_copy_constructor;
template <class _Ty> struct has_copy_assign;
template <class _Ty> struct has_move_constructor;
template <class _Ty> struct has_move_assign;

template <class _Ty> struct has_trivial_default_constructor;
template <class _Ty> struct has_trivial_copy_constructor;
template <class _Ty> struct has_trivial_move_constructor;

template <class _Ty> struct has_trivial_assign;
template <class _Ty> struct has_trivial_copy_assign;
template <class _Ty> struct has_trivial_move_assign;
template <class _Ty> struct has_trivial_destructor;

template <class _Ty> struct has_nothrow_default_constructor;
template <class _Ty> struct has_nothrow_copy_constructor;
template <class _Ty> struct has_nothrow_move_constructor;

template <class _Ty> struct has_nothrow_assign;
template <class _Ty> struct has_nothrow_copy_assign;
template <class _Ty> struct has_nothrow_move_assign;

template <class _Ty, class _U> struct is_assignable;
template <class _Ty> struct is_copy_assignable;
template <class _Ty> struct is_move_assignable;
template <class _Ty> struct is_destructible;

template <class _Ty, class _U> struct is_nothrow_assignable;
template <class _Ty> struct is_nothrow_copy_assignable;
template <class _Ty> struct is_nothrow_move_assignable;
template <class _Ty> struct is_nothrow_destructible;

template <class _Ty, class _U> struct is_trivially_assignable;
template <class _Ty> struct is_trivially_copy_assignable;
template <class _Ty> struct is_trivially_move_assignable;
template <class _Ty> struct is_trivially_destructible;


template <class _Ty, class... _Args> struct is_constructible;
template <class _Ty> struct is_default_constructible;
template <class _Ty> struct is_copy_constructible;
template <class _Ty> struct is_move_constructible;

template <class _Ty, class... _Args> struct is_trivially_constructible;
template <class _Ty> struct is_trivially_default_constructible;
template <class _Ty> struct is_trivially_copy_constructible;
template <class _Ty> struct is_trivially_move_constructible;

template <class _Ty, class... _Args> struct is_nothrow_constructible;
template <class _Ty> struct is_nothrow_default_constructible;
template <class _Ty> struct is_nothrow_copy_constructible;
template <class _Ty> struct is_nothrow_move_constructible;


template <class _Ty> struct is_signed;
template <class _Ty> struct is_unsigned;
template <class _Ty> struct alignment_of;
template <class _Ty> struct rank;
template <class _Ty, unsigned I = 0> struct extent;
template<class _Ty> struct is_trivial;


template <class ..._Ty> struct common_type;

template <class _Ty> struct underlying_type;


} // namespace sketch {}

