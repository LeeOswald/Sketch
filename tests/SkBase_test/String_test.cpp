#include "common.h"

using namespace sketch;


SKETCH_TEST_BEGIN(String, "String::SkU16")
    auto s = SkU16("UTF-16 string literal");

    SKETCH_TEST_ASSERT(s.length() == 21);
    SKETCH_TEST_ASSERT(wcslen(s.utf16()) == 21);
    SKETCH_TEST_ASSERT(!s.empty());
    SKETCH_TEST_ASSERT(!wcscmp(s.utf16(), L"UTF-16 string literal"));

    SKETCH_TEST_ASSERT(s.isSame(s));
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::String()")
    String s;
    SKETCH_TEST_ASSERT(s.length() == 0);
    SKETCH_TEST_ASSERT(s.empty());
    SKETCH_TEST_ASSERT(*s.str() == 0);
    SKETCH_TEST_ASSERT(*s.utf16() == 0);
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::String(const wchar_t*)")
    const wchar_t* u16 = L"UTF-16 (wchar_t) text";
    String s(u16);
    SKETCH_TEST_ASSERT(s.length() == wcslen(u16));
    SKETCH_TEST_ASSERT(!s.empty());
    SKETCH_TEST_ASSERT(!wcscmp(s.utf16(), u16));

    String s2(L"");
    SKETCH_TEST_ASSERT(s2.empty());
    SKETCH_TEST_ASSERT(*s2.utf16() == 0);

    String s3((const wchar_t*)nullptr);
    SKETCH_TEST_ASSERT(s3.length() == 0);
    SKETCH_TEST_ASSERT(s3.empty());
    SKETCH_TEST_ASSERT(*s3.str() == 0);
    SKETCH_TEST_ASSERT(*s3.utf16() == 0);
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::String(const char*)")
    const char* u8 = "UTF-8 text";
    const wchar_t* u16 = L"UTF-8 text";
    String s8(u8);
    String s16(u16);
    SKETCH_TEST_ASSERT(s8 == s16);

    String s2("");
    SKETCH_TEST_ASSERT(s2.empty());
    SKETCH_TEST_ASSERT(*s2.utf16() == 0);

    String s3((const char*)nullptr);
    SKETCH_TEST_ASSERT(s3.length() == 0);
    SKETCH_TEST_ASSERT(s3.empty());
    SKETCH_TEST_ASSERT(*s3.str() == 0);
    SKETCH_TEST_ASSERT(*s3.utf16() == 0);
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::String(const String&)")
    const wchar_t* u16 = L"UTF-16 (wchar_t) text";
    String s(u16);

    String s2(s);

    SKETCH_TEST_ASSERT(s2.length() == s.length());
    SKETCH_TEST_ASSERT(!wcscmp(s.utf16(), s2.utf16()));

    SKETCH_TEST_ASSERT(s.isSame(s2));
    SKETCH_TEST_ASSERT(s2.isSame(s));
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::operator=(const String&)")
    const wchar_t* u16 = L"UTF-16 (wchar_t) text";
    String s(u16);

    String s2;
    s2 = s;

    SKETCH_TEST_ASSERT(s2.length() == s.length());
    SKETCH_TEST_ASSERT(!wcscmp(s.utf16(), s2.utf16()));

    SKETCH_TEST_ASSERT(s2.isSame(s));

    // self-assignment
    s2 = s2;
    SKETCH_TEST_ASSERT(s2.length() == s.length());
    SKETCH_TEST_ASSERT(!wcscmp(s.utf16(), s2.utf16()));

    SKETCH_TEST_ASSERT(s2.isSame(s2));
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::String(String&&)")
    const wchar_t* u16 = L"UTF-16 (wchar_t) text";
    String s(u16);

    String s2(sketch::move(s));

    SKETCH_TEST_ASSERT(!s2.isSame(s));

    SKETCH_TEST_ASSERT(s2.length() == wcslen(u16));
    SKETCH_TEST_ASSERT(!wcscmp(s2.utf16(), u16));

    SKETCH_TEST_ASSERT(s.empty());
    SKETCH_TEST_ASSERT(s.length() == 0);
    SKETCH_TEST_ASSERT(!*s.utf16());
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::operator=(String&&)")
    const wchar_t* u16 = L"UTF-16 (wchar_t) text";
    String s(u16);

    String s2;
    s2 = sketch::move(s);

    SKETCH_TEST_ASSERT(!s2.isSame(s));

    SKETCH_TEST_ASSERT(s2.length() == wcslen(u16));
    SKETCH_TEST_ASSERT(!wcscmp(s2.utf16(), u16));

    SKETCH_TEST_ASSERT(s.empty());
    SKETCH_TEST_ASSERT(s.length() == 0);
    SKETCH_TEST_ASSERT(!*s.utf16());
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::compare(const UChar*, size_t)")
    const UChar* ustr = u"Unicode (UChar) text";
    String s(ustr);

    SKETCH_TEST_ASSERT(s.compare(s.str()) == 0);
    SKETCH_TEST_ASSERT(s.compare(ustr) == 0);

    SKETCH_TEST_ASSERT(s.compare(u"A") > 0);
    SKETCH_TEST_ASSERT(s.compare(u"Z") < 0);
    SKETCH_TEST_ASSERT(s.compare(u"") > 0);

    SKETCH_TEST_ASSERT(s.compare(u"Uni", 3) == 0);
    SKETCH_TEST_ASSERT(s.compare(u"Uni", 4) > 0);
    SKETCH_TEST_ASSERT(s.compare(u"Uni", 2) == 0);
    SKETCH_TEST_ASSERT(s.compare(u"", 2) > 0);
    SKETCH_TEST_ASSERT(s.compare(u"", 0) == 0);
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::compareNoCase(const UChar*, size_t)")
    const UChar* ustr = u"Unicode (UChar) text";
    String s(ustr);

    SKETCH_TEST_ASSERT(s.compareNoCase(s.str()) == 0);
    SKETCH_TEST_ASSERT(s.compareNoCase(ustr) == 0);

    SKETCH_TEST_ASSERT(s.compareNoCase(u"A") > 0);
    SKETCH_TEST_ASSERT(s.compareNoCase(u"Z") < 0);
    SKETCH_TEST_ASSERT(s.compareNoCase(u"") > 0);

    SKETCH_TEST_ASSERT(s.compareNoCase(u"Uni", 3) == 0);
    SKETCH_TEST_ASSERT(s.compareNoCase(u"Uni", 4) > 0);
    SKETCH_TEST_ASSERT(s.compareNoCase(u"Uni", 2) == 0);
    SKETCH_TEST_ASSERT(s.compareNoCase(u"", 2) > 0);
    SKETCH_TEST_ASSERT(s.compareNoCase(u"", 0) == 0);

    SKETCH_TEST_ASSERT(s.compareNoCase(u"UNI", 3) == 0);
    SKETCH_TEST_ASSERT(s.compareNoCase(u"UNI", 4) > 0);
    SKETCH_TEST_ASSERT(s.compareNoCase(u"UNI", 2) == 0);
    SKETCH_TEST_ASSERT(s.compareNoCase(u"", 2) > 0);
    SKETCH_TEST_ASSERT(s.compareNoCase(u"", 0) == 0);
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::operator==(const String& a, const String& b)")
    String s1;
    SKETCH_TEST_ASSERT(s1 == s1);

    String s2;
    SKETCH_TEST_ASSERT(s2 == s1);

    String s3(u"");
    SKETCH_TEST_ASSERT(s3 == s1);

    String s4(u"Yyy");
    SKETCH_TEST_ASSERT(!(s4 == s3));
    SKETCH_TEST_ASSERT(s4 == s4);

    String s5(s4);
    SKETCH_TEST_ASSERT(s5 == s4);

    String s6;
    s6 = s4;
    SKETCH_TEST_ASSERT(s6 == s4);

    String s7(u"ZZZ");
    SKETCH_TEST_ASSERT(!(s7 == s4));

    String s8(u"Yyy");
    SKETCH_TEST_ASSERT(s8 == s4);
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::operator!=(const String&, const String&)")
    String s1;
    SKETCH_TEST_ASSERT(!(s1 != s1));

    String s2;
    SKETCH_TEST_ASSERT(!(s2 != s1));

    String s3(u"");
    SKETCH_TEST_ASSERT(!(s3 != s1));

    String s4(u"Yyy");
    SKETCH_TEST_ASSERT(s4 != s3);
    SKETCH_TEST_ASSERT(!(s4 != s4));

    String s5(s4);
    SKETCH_TEST_ASSERT(!(s5 != s4));

    String s6;
    s6 = s4;
    SKETCH_TEST_ASSERT(!(s6 != s4));

    String s7(u"ZZZ");
    SKETCH_TEST_ASSERT(s7 != s4);

    String s8(u"Yyy");
    SKETCH_TEST_ASSERT(!(s8 != s4));
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::operator<(const String& a, const String& b)")
    String s1;
    SKETCH_TEST_ASSERT(!(s1 < s1));

    String s2;
    SKETCH_TEST_ASSERT(!(s2 < s1));

    String s3(u"");
    SKETCH_TEST_ASSERT(!(s3 < s1));

    String s4(u"Yyy");
    SKETCH_TEST_ASSERT(!(s4 < s3));
    SKETCH_TEST_ASSERT(s3 < s4);

    String s5(s4);
    SKETCH_TEST_ASSERT(!(s5 < s4));

    String s6;
    s6 = s4;
    SKETCH_TEST_ASSERT(!(s6 < s4));

    String s7(u"ZZZ");
    SKETCH_TEST_ASSERT(!(s7 < s4));
    SKETCH_TEST_ASSERT(s4 < s7);

    String s8(u"Yyy");
    SKETCH_TEST_ASSERT(!(s8 < s4));
    SKETCH_TEST_ASSERT(!(s4 < s8));
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::operator>(const String& a, const String& b)")
    String s1;
    SKETCH_TEST_ASSERT(!(s1 > s1));

    String s2;
    SKETCH_TEST_ASSERT(!(s2 > s1));

    String s3(u"");
    SKETCH_TEST_ASSERT(!(s3 > s1));

    String s4(u"Yyy");
    SKETCH_TEST_ASSERT(s4 > s3);
    SKETCH_TEST_ASSERT(!(s3 > s4));

    String s5(s4);
    SKETCH_TEST_ASSERT(!(s5 > s4));

    String s6;
    s6 = s4;
    SKETCH_TEST_ASSERT(!(s6 > s4));

    String s7(u"ZZZ");
    SKETCH_TEST_ASSERT(s7 > s4);
    SKETCH_TEST_ASSERT(!(s4 > s7));

    String s8(u"Yyy");
    SKETCH_TEST_ASSERT(!(s8 > s4));
    SKETCH_TEST_ASSERT(!(s4 > s8));
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::append(const wchar_t*, size_t)")
    String s;

    s.append(L"");
    SKETCH_TEST_ASSERT(s.empty());
    SKETCH_TEST_ASSERT(s.length() == 0);
    SKETCH_TEST_ASSERT(!*s.utf16());

    s.append(L"", 0);
    SKETCH_TEST_ASSERT(s.empty());
    SKETCH_TEST_ASSERT(s.length() == 0);
    SKETCH_TEST_ASSERT(!*s.utf16());

    s.append(L"12345");
    SKETCH_TEST_ASSERT(s == String(L"12345"));

    s.append(L"67890");
    SKETCH_TEST_ASSERT(s == String(L"1234567890"));

    // create an additional reference
    String s2(s);
    SKETCH_TEST_ASSERT(s2.isSame(s));
    s.append(L"abcde");
    SKETCH_TEST_ASSERT(s == String(L"1234567890abcde"));
    SKETCH_TEST_ASSERT(!s2.isSame(s));

    String s3(s);
    SKETCH_TEST_ASSERT(s3.isSame(s));
    s.append(L"fghji");
    SKETCH_TEST_ASSERT(s == String(L"1234567890abcdefghji"));
    SKETCH_TEST_ASSERT(!s3.isSame(s));
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::operator+()")
    SKETCH_TEST_ASSERT(String(L"") + String(L"") == String(L""));
    SKETCH_TEST_ASSERT(String(L"") + L"" == String(L""));
    SKETCH_TEST_ASSERT(L"" + String(L"") == String(L""));

    SKETCH_TEST_ASSERT(String(L"abc") + String(L"def") == String(L"abcdef"));
    SKETCH_TEST_ASSERT(String(L"abc") + L"def" == String(L"abcdef"));
    SKETCH_TEST_ASSERT(L"abc" + String(L"def") == String(L"abcdef"));
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(String, "String::format()")
    SKETCH_TEST_ASSERT(String::format(L"") == String());
    SKETCH_TEST_ASSERT(String::format(L"Test") == String(L"Test"));
    SKETCH_TEST_ASSERT(String::format(L"Test %d", 12) == String(L"Test 12"));
    SKETCH_TEST_ASSERT(String::format(L"Test %s", L"UTF-16") == String(L"Test UTF-16"));
    SKETCH_TEST_ASSERT(String::format(L"Test %S", "UTF-8") == String(L"Test UTF-8"));
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(String, "String::trim*()")
    SKETCH_TEST_ASSERT(String(L"\r \npapa pi\t").trimLeft() == String(L"papa pi\t"));
    SKETCH_TEST_ASSERT(String(L"papa pi\t").trimLeft() == String(L"papa pi\t"));
    SKETCH_TEST_ASSERT(String().trimLeft().empty());
    SKETCH_TEST_ASSERT(String(L"    ").trimLeft().empty());

    SKETCH_TEST_ASSERT(String(L"\r \npapa pi\t").trimRight() == String(L"\r \npapa pi"));
    SKETCH_TEST_ASSERT(String(L"\r \npapa pi").trimRight() == String(L"\r \npapa pi"));
    SKETCH_TEST_ASSERT(String().trimRight().empty());
    SKETCH_TEST_ASSERT(String(L"    ").trimRight().empty());

    SKETCH_TEST_ASSERT(String(L"\r \npapa pi\t").trim() == String(L"papa pi"));
    SKETCH_TEST_ASSERT(String(L"    ").trim().empty());
SKETCH_TEST_END()

