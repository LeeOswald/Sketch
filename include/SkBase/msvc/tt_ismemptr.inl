#undef KSTL_TT_TCV_

#ifndef KSTL_TT_TCV
#define KSTL_TT_TCV_

template <class _T, class _RT>
struct is_member_object_pointer<_RT _T::* KSTL_TT_PCV> 
    : public true_type 
{
};

#else
#define KSTL_TT_TCV_ KSTL_TT_TCV
#endif

template <class _T, class _RT>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)()KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(...)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1 ...)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2 ...)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3 ...)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4 ...)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4, class A5>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4, A5)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4, class A5>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4, A5 ...)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4, class A5, class A6>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4, A5, A6)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4, class A5, class A6>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4, A5, A6 ...)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7 ...)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7, A8)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7, A8 ...)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7, A8, A9)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7, A8, A9 ...)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)KSTL_TT_TCV_> : public true_type {};

template <class _T, class _RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
struct is_member_function_pointer<_RT (_T::* KSTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10 ...)KSTL_TT_TCV_> : public true_type {};
