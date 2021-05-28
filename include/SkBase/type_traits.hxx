#pragma once


#include "../Sketch.hxx"

#include "limits.hxx"
#include "type_traits_fwd.hxx"

#define SK_CHECK_TRAIT(X) static_assert((X), "Traits check failed on '" #X "'")

#include "msvc/type_traits_msvc.hxx"


namespace sketch
{

namespace __
{

struct sfinae_small
{
    char x;
};

struct sfinae_big
{
    char x[2];
};

typedef sfinae_small sfinae_passed_tag;
typedef sfinae_big sfinae_failed_tag;

#define SKETCH_SFINAE_EVAL(test) (sizeof(test) == sizeof(sketch::__::sfinae_passed_tag))



template<typename T>
struct type2type
{
    typedef T type;
};

template<size_t v>
struct int2type
    : integral_constant<size_t, v>
{
};

template<int i>
struct index_type
    : integral_constant<int, i>
{
};

template<bool b>
struct bool_type
    : integral_constant<bool, b>
{
};

template<typename T>
struct __is_complete_impl
{
private:
    static typename add_rvalue_reference<T>::type __t();

    template<typename U>
    static char test(U u);

    static int test(...);

public:
    typedef integral_constant<bool, sizeof(test(__t())) == sizeof(char)> result;
};


template<typename T>
struct is_complete :
    public __is_complete_impl<T>::result
{
};


template<typename T>
struct no_dtor :
    public bool_type<is_pod<T>::value || has_trivial_destructor<T>::value>
{
};


template<typename T>
inline typename decay<T>::type decay_copy(T&& v)
{
    return sketch::forward<T>(v);
}


struct check_meta_rel
{
    SK_CHECK_TRAIT((is_same<volatile int, volatile int>::value));
};

struct check_is_base_of
{
    struct Base {};
    struct Derived : public Base {};
    class Foo {};

    SK_CHECK_TRAIT((is_base_of<Base, Derived>::value));
    SK_CHECK_TRAIT((is_base_of<Base, Base>::value));
    SK_CHECK_TRAIT((is_base_of<Derived, Derived>::value));
    SK_CHECK_TRAIT((is_base_of<Derived, Base>::value == false));

    SK_CHECK_TRAIT((is_base_of<Base, Foo>::value == false));
    SK_CHECK_TRAIT((is_base_of<Foo, Derived>::value == false));
    SK_CHECK_TRAIT((is_base_of<Foo, Foo>::value));

    SK_CHECK_TRAIT((is_base_of<int, int>::value == false));
    SK_CHECK_TRAIT((is_base_of<short, int>::value) == false);
};

struct check_meta_trans_cv
{
    SK_CHECK_TRAIT((is_same<remove_const<volatile const int>::type, volatile int>::value));
    SK_CHECK_TRAIT((is_same<remove_const<const int>::type, const int>::value) == 0);
    SK_CHECK_TRAIT((is_same<remove_volatile<volatile int>::type, volatile int>::value) == 0);
    SK_CHECK_TRAIT((is_same<remove_volatile<volatile const int>::type, const int>::value));
    SK_CHECK_TRAIT((is_same<remove_cv<const int>::type, const int>::value) == 0);
    SK_CHECK_TRAIT((is_same<remove_cv<volatile int>::type, volatile int>::value) == 0);
    SK_CHECK_TRAIT((is_same<remove_cv<volatile const int>::type, int>::value));
    SK_CHECK_TRAIT((is_same<add_const<volatile int>::type, volatile const int>::value));
    SK_CHECK_TRAIT((is_same<add_const<int>::type, const int>::value));
    SK_CHECK_TRAIT((is_same<add_volatile<const int>::type, volatile const int>::value));
    SK_CHECK_TRAIT((is_same<add_volatile<int>::type, volatile int>::value));
    SK_CHECK_TRAIT((is_same<add_cv<int>::type, volatile const int>::value));
};

struct check_meta_trans_ref
{
    SK_CHECK_TRAIT((is_same<remove_reference<int&>::type, int&>::value) == 0);
    SK_CHECK_TRAIT((is_same<remove_reference<int&>::type, int>::value));
    SK_CHECK_TRAIT((is_same<add_lvalue_reference<int&>::type, int&>::value));
    SK_CHECK_TRAIT((is_same<add_lvalue_reference<int>::type, int&>::value));
};

struct check_meta_trans_arr
{
    SK_CHECK_TRAIT((is_same<remove_extent<int[][3]>::type, int[3]>::value));
    SK_CHECK_TRAIT((is_same<remove_extent<const int>::type, const int>::value));
    SK_CHECK_TRAIT((is_same<remove_extent<volatile int[]>::type, volatile int>::value));
    SK_CHECK_TRAIT((is_same<remove_extent<const volatile int[2]>::type, const volatile int>::value));

    SK_CHECK_TRAIT((is_same<remove_all_extents<int>::type, int>::value));
    SK_CHECK_TRAIT((is_same<remove_all_extents<int[2]>::type, int>::value));
    SK_CHECK_TRAIT((is_same<remove_all_extents<int[2][3]>::type, int>::value));
    SK_CHECK_TRAIT((is_same<remove_all_extents<int[][3][6]>::type, int>::value));
};

struct check_meta_trans_ptr
{
    SK_CHECK_TRAIT((is_same<remove_pointer<const int*>::type, const int>::value));
    SK_CHECK_TRAIT((is_same<remove_pointer<int*>::type, int*>::value) == 0);

    SK_CHECK_TRAIT((is_same<add_pointer<volatile int>::type, volatile int*>::value));
    SK_CHECK_TRAIT((is_same<add_pointer<int*&>::type, int**>::value));
};

struct check_meta_unary_cat
{
    SK_CHECK_TRAIT(is_void<void>::value);
    SK_CHECK_TRAIT(is_void<const void>::value);
    SK_CHECK_TRAIT(is_void<volatile void>::value);
    SK_CHECK_TRAIT(is_void<const volatile void>::value);

    SK_CHECK_TRAIT(is_integral<volatile const int>::value);
    SK_CHECK_TRAIT(is_integral<void>::value == false);

#ifndef SKETCH_KM
    SK_CHECK_TRAIT(is_floating_point<double>::value);
    SK_CHECK_TRAIT(is_floating_point<int>::value == false);
#endif

    SK_CHECK_TRAIT(is_array<int>::value == false);
    SK_CHECK_TRAIT(is_array<int[][2]>::value);

    SK_CHECK_TRAIT(is_pointer<void*>::value);
    SK_CHECK_TRAIT(is_pointer<void* const>::value);
    SK_CHECK_TRAIT(is_pointer<void* volatile>::value);
    SK_CHECK_TRAIT(is_pointer<void* const volatile>::value);

    SK_CHECK_TRAIT(is_lvalue_reference<volatile int&>::value);
};

} // namespace __


} // namespace sketch {}



#undef SK_CHECK_TRAIT

