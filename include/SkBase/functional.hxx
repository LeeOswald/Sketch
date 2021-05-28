#pragma once

#include "type_traits.hxx"


namespace sketch
{

template <class _Arg, class _Result>
struct unary_function
{
    typedef _Arg argument_type;
    typedef _Result result_type;
};

template <class _Arg1, class _Arg2, class _Result>
struct binary_function
{
    typedef _Arg1 first_argument_type;
    typedef _Arg2 second_argument_type;
    typedef _Result result_type;
};


template <class _Ty = void>
struct plus : binary_function<_Ty, _Ty, _Ty>
{
    _Ty operator()(const _Ty& x, const _Ty& y) const { return x + y; }
};

template <class _Ty = void>
struct minus : binary_function<_Ty, _Ty, _Ty>
{
    _Ty operator()(const _Ty& x, const _Ty& y) const { return x - y; }
};

template <class _Ty = void>
struct multiplies : binary_function<_Ty, _Ty, _Ty>
{
    _Ty operator()(const _Ty& x, const _Ty& y) const { return x * y; }
};

template <class _Ty = void>
struct divides : binary_function<_Ty, _Ty, _Ty>
{
    _Ty operator()(const _Ty& x, const _Ty& y) const { return x / y; }
};

template <class _Ty = void>
struct modulus : binary_function<_Ty, _Ty, _Ty>
{
    _Ty operator()(const _Ty& x, const _Ty& y) const { return x % y; }
};

template <class _Ty = void>
struct negate : unary_function<_Ty, _Ty>
{
    _Ty operator()(const _Ty& x) const { return -x; }
};


namespace __
{

template<class _Ty, class _U = void>
struct is_transparent
{
private:
    template<class X> static sfinae_passed_tag test(typename X::is_transparent* = 0);
    template<class X> static sfinae_failed_tag test(...);
public:
    static const bool value = SKETCH_SFINAE_EVAL(test<_Ty>(0));
};

} // namespace __ {}

template<>
struct plus<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) + sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) + sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct minus<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) - sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) - sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct multiplies<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) * sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) * sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct divides<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) / sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) / sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct modulus<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) % sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) % sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct negate<void>
{
    template <class _Ty>
    auto operator()(_Ty&& t) const ->
        decltype(-sketch::forward<_Ty>(t))
    {
        return  -sketch::forward<_Ty>(t);
    }

    typedef void is_transparent;
};


template <class _Ty = void>
struct equal_to : binary_function<_Ty, _Ty, bool>
{
    bool operator()(const _Ty& x, const _Ty& y) const { return x == y; }
};

template <class _Ty = void>
struct not_equal_to : binary_function<_Ty, _Ty, bool>
{
    bool operator()(const _Ty& x, const _Ty& y) const { return x != y; }
};

template <class _Ty = void>
struct greater : binary_function<_Ty, _Ty, bool>
{
    bool operator()(const _Ty& x, const _Ty& y) const { return x > y; }
};

template <class _Ty = void>
struct less : binary_function<_Ty, _Ty, bool>
{
    bool operator()(const _Ty& x, const _Ty& y) const { return x < y; }
};

template <class _Ty = void>
struct greater_equal : binary_function<_Ty, _Ty, bool>
{
    bool operator()(const _Ty& x, const _Ty& y) const { return x >= y; }
};

template <class _Ty = void>
struct less_equal : binary_function<_Ty, _Ty, bool>
{
    bool operator()(const _Ty& x, const _Ty& y) const { return x <= y; }
};



template<>
struct equal_to<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) == sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) == sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct not_equal_to<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) != sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) != sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct greater<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) > sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) > sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct less<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) < sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) < sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct greater_equal<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) >= sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) >= sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct less_equal<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) <= sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) <= sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};


template <class _Ty = void>
struct logical_and : binary_function<_Ty, _Ty, bool>
{
    bool operator()(const _Ty& x, const _Ty& y) const { return x && y; }
};

template <class _Ty = void>
struct logical_or : binary_function<_Ty, _Ty, bool>
{
    bool operator()(const _Ty& x, const _Ty& y) const { return x || y; }
};

template <class _Ty = void>
struct logical_not : unary_function<_Ty, bool>
{
    bool operator()(const _Ty& x) const { return !x; }
};


template<>
struct logical_and<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) && sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) && sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct logical_or<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) || sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) || sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct logical_not<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t) const ->
        decltype(!sketch::forward<_Ty>(t))
    {
        return  !sketch::forward<_Ty>(t);
    }

    typedef void is_transparent;
};


template <class _Ty = void> struct bit_and : binary_function<_Ty, _Ty, _Ty>
{
    _Ty operator()(const _Ty& x, const _Ty& y) const
    {
        return x & y;
    }
};

template <class _Ty = void> struct bit_or : binary_function<_Ty, _Ty, _Ty>
{
    _Ty operator()(const _Ty& x, const _Ty& y) const
    {
        return x | y;
    }
};

template <class _Ty = void> struct bit_xor : binary_function<_Ty, _Ty, _Ty>
{
    _Ty operator()(const _Ty& x, const _Ty& y) const
    {
        return x ^ y;
    }
};

template <class _Ty = void> struct bit_not : unary_function<_Ty, _Ty>
{
    _Ty operator()(const _Ty& x) const
    {
        return ~x;
    }
};


template<>
struct bit_and<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) & sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) & sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct bit_or<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) | sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) | sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct bit_xor<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t, _U&& u) const ->
        decltype(sketch::forward<_Ty>(t) ^ sketch::forward<_U>(u))
    {
        return sketch::forward<_Ty>(t) ^ sketch::forward<_U>(u);
    }

    typedef void is_transparent;
};

template<>
struct bit_not<void>
{
    template <class _Ty, class _U>
    auto operator()(_Ty&& t) const ->
        decltype(~sketch::forward<_Ty>(t))
    {
        return  ~sketch::forward<_Ty>(t);
    }

    typedef void is_transparent;
};


template <class _Predicate>
class unary_negate
    : public unary_function<typename _Predicate::argument_type, bool>
{
    const _Predicate& pred;
    void operator=(const unary_negate &) = delete;

public:
    explicit unary_negate(const _Predicate& pred)
        : pred(pred)
    {
    }

    bool operator()(const typename _Predicate::argument_type& x) const
    {
        return !pred(x);
    }
};

template <class _Predicate>
inline unary_negate<_Predicate> not1(const _Predicate& pred)
{
    return unary_negate<_Predicate>(pred);
}

template <class _Predicate>
class binary_negate
    : public binary_function<typename _Predicate::first_argument_type, typename _Predicate::second_argument_type, bool>
{
    const _Predicate& pred;
    void operator=(const binary_negate &) = delete;

public:
    explicit binary_negate(const _Predicate& pred)
        : pred(pred)
    {
    }

    bool operator()(const typename _Predicate::first_argument_type& x, const typename _Predicate::second_argument_type& y) const
    {
        return !pred(x, y);
    }
};

template <class _Predicate>
inline binary_negate<_Predicate> not2(const _Predicate& pred)
{
    return binary_negate<_Predicate>(pred);
}


template <class _Arg, class _Result>
class pointer_to_unary_function
    : public unary_function<_Arg, _Result>
{
    _Result(*f)(_Arg);

public:
    explicit pointer_to_unary_function(_Result(*f)(_Arg))
        : f(f)
    {
    }

    _Result operator()(_Arg x) const
    {
        return f(x);
    }
};

template <class _Arg, class _Result>
inline pointer_to_unary_function<_Arg, _Result> ptr_fun(_Result(*f)(_Arg))
{
    return pointer_to_unary_function<_Arg, _Result>(f);
}


template <class _Arg1, class _Arg2, class _Result>
class pointer_to_binary_function
    : public binary_function<_Arg1, _Arg2, _Result>
{
    _Result(*f)(_Arg1, _Arg2);

public:
    explicit pointer_to_binary_function(_Result(*f)(_Arg1, _Arg2))
        : f(f)
    {
    }

    _Result operator()(_Arg1 x, _Arg2 y) const
    {
        return f(x, y);
    }
};

template <class _Arg1, class _Arg2, class _Result>
inline pointer_to_binary_function<_Arg1, _Arg2, _Result> ptr_fun(_Result(*f)(_Arg1, _Arg2))
{
    return pointer_to_binary_function<_Arg1, _Arg2, _Result>(f);
}



template <class _Result, class _Ty>
class mem_fun_t
    : public unary_function<_Ty*, _Result>
{
    _Result(_Ty::*m)();

public:
    explicit mem_fun_t(_Result(_Ty::*p)()) : m(p) {}
    _Result operator()(_Ty* p) const { return (p->*m)(); }
};

template <class _Result, class _Ty>
class const_mem_fun_t
    : public unary_function<_Ty*, _Result>
{
    _Result(_Ty::*m)() const;

public:
    explicit const_mem_fun_t(_Result(_Ty::*p)() const) : m(p) {}
    _Result operator()(const _Ty* p) const { return (p->*m)(); }
};

template <class _Result, class _Ty, class _A>
class mem_fun1_t
    : public binary_function<_Ty*, _A, _Result>
{
    _Result(_Ty::*m)(_A);

public:
    explicit mem_fun1_t(_Result(_Ty::*p)(_A)) : m(p) {}
    _Result operator()(_Ty* p, _A x) const { return (p->*m)(x); }
};

template <class _Result, class _Ty, class _A>
class const_mem_fun1_t
    : public binary_function<_Ty*, _A, _Result>
{
    _Result(_Ty::*m)(_A) const;

public:
    explicit const_mem_fun1_t(_Result(_Ty::*p)(_A) const) : m(p) {}
    _Result operator()(const _Ty* p, _A x) const { return (p->*m)(x); }
};

template<class _Result, class _Ty>
inline mem_fun_t<_Result, _Ty> mem_fun(_Result(_Ty::*f)())
{
    return mem_fun_t<_Result, _Ty>(f);
}

template <class _Result, class _Ty>
inline const_mem_fun_t<_Result, _Ty> mem_fun(_Result(_Ty::*f)() const)
{
    return const_mem_fun_t<_Result, _Ty>(f);
}

template<class _Result, class _Ty, class _A>
inline mem_fun1_t<_Result, _Ty, _A> mem_fun(_Result(_Ty::*f)(_A))
{
    return mem_fun1_t<_Result, _Ty, _A>(f);
}

template <class _Result, class _Ty, class _A>
inline const_mem_fun1_t<_Result, _Ty, _A> mem_fun(_Result(_Ty::*f)(_A) const)
{
    return const_mem_fun1_t<_Result, _Ty, _A>(f);
}

template <class _Result, class _Ty>
class mem_fun_ref_t
    : public unary_function<_Ty, _Result>
{
    _Result(_Ty::*m)();

public:
    explicit mem_fun_ref_t(_Result(_Ty::*p)()) : m(p) {}
    _Result operator()(_Ty& p) const { return (p.*m)(); }
};

template <class _Result, class _Ty>
class const_mem_fun_ref_t
    : public unary_function<_Ty, _Result>
{
    _Result(_Ty::*m)() const;

public:
    explicit const_mem_fun_ref_t(_Result(_Ty::*p)() const) : m(p) {}
    _Result operator()(const _Ty& p) const { return (p.*m)(); }
};

template <class _Result, class _Ty, class _A>
class mem_fun1_ref_t
    : public binary_function<_Ty, _A, _Result>
{
    _Result(_Ty::*m)(_A);

public:
    explicit mem_fun1_ref_t(_Result(_Ty::*p)(_A)) : m(p) {}
    _Result operator()(_Ty& p, _A x) const { return (p.*m)(x); }
};

template <class _Result, class _Ty, class _A>
class const_mem_fun1_ref_t
    : public binary_function<_Ty, _A, _Result>
{
    _Result(_Ty::*m)(_A) const;

public:
    explicit const_mem_fun1_ref_t(_Result(_Ty::*p)(_A) const) : m(p) {}
    _Result operator()(const _Ty& p, _A x) const { return (p.*m)(x); }
};

template<class _Result, class _Ty>
inline mem_fun_ref_t<_Result, _Ty> mem_fun_ref(_Result(_Ty::*f)())
{
    return mem_fun_ref_t<_Result, _Ty>(f);
}

template <class _Result, class _Ty>
inline const_mem_fun_ref_t<_Result, _Ty> mem_fun_ref(_Result(_Ty::*f)() const)
{
    return const_mem_fun_ref_t<_Result, _Ty>(f);
}

template<class _Result, class _Ty, class _A>
inline mem_fun1_ref_t<_Result, _Ty, _A> mem_fun_ref(_Result(_Ty::*f)(_A))
{
    return mem_fun1_ref_t<_Result, _Ty, _A>(f);
}

template <class _Result, class _Ty, class _A>
inline const_mem_fun1_ref_t<_Result, _Ty, _A> mem_fun_ref(_Result(_Ty::*f)(_A) const)
{
    return const_mem_fun1_ref_t<_Result, _Ty, _A>(f);
}


template<class _Ty> struct hash;

template<> struct hash<bool>;
template<> struct hash<char>;
template<> struct hash<signed char>;
template<> struct hash<unsigned char>;
template<> struct hash<char16_t>;
template<> struct hash<char32_t>;
template<> struct hash<wchar_t>;
template<> struct hash<short>;
template<> struct hash<unsigned short>;
template<> struct hash<int>;
template<> struct hash<unsigned int>;
template<> struct hash<long>;
template<> struct hash<long long>;
template<> struct hash<unsigned long>;
template<> struct hash<unsigned long long>;

template<class _Ty> struct hash<_Ty*>;


template<class _Ty>
struct hash
    : unary_function<_Ty, size_t>
{
    size_t operator()(_Ty val) const noexcept
    {
        static_assert(false, "hash<_Ty> must be specialized!");
    }
};


template<class _Ty>
struct hash<_Ty*>
    : unary_function<_Ty*, size_t>
{
    size_t operator()(_Ty* val) const noexcept
    {
        return reinterpret_cast<size_t>(val);
    }
};


#define SKETCH_INTEGRAL_HASH_IMPL(_Ty) \
template<> struct hash<_Ty> \
    : unary_function<_Ty, size_t> \
{ \
    inline size_t operator()(argument_type val) const noexcept \
    { \
        return static_cast<size_t>(val); \
    } \
}


SKETCH_INTEGRAL_HASH_IMPL(bool);
SKETCH_INTEGRAL_HASH_IMPL(char);
SKETCH_INTEGRAL_HASH_IMPL(signed char);
SKETCH_INTEGRAL_HASH_IMPL(unsigned char);
SKETCH_INTEGRAL_HASH_IMPL(wchar_t);
SKETCH_INTEGRAL_HASH_IMPL(short);

#ifdef _NATIVE_WCHAR_T_DEFINED
SKETCH_INTEGRAL_HASH_IMPL(unsigned short);
#endif

SKETCH_INTEGRAL_HASH_IMPL(int);
SKETCH_INTEGRAL_HASH_IMPL(unsigned int);
SKETCH_INTEGRAL_HASH_IMPL(long);
SKETCH_INTEGRAL_HASH_IMPL(unsigned long);


#ifdef SKETCH_64
SKETCH_INTEGRAL_HASH_IMPL(long long);
SKETCH_INTEGRAL_HASH_IMPL(unsigned long long);
#endif

SKETCH_INTEGRAL_HASH_IMPL(char16_t);
SKETCH_INTEGRAL_HASH_IMPL(char32_t);

#undef SKETCH_INTEGRAL_HASH_IMPL


} // namespace sketch {}
