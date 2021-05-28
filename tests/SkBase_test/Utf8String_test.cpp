#include "common.h"

using namespace sketch;

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::SkU8")
    auto s = SkU8("UTF-8 string literal");

    SKETCH_TEST_ASSERT(s.length() == 20);
    SKETCH_TEST_ASSERT(strlen(s.str()) == 20);
    SKETCH_TEST_ASSERT(!s.empty());
    SKETCH_TEST_ASSERT(!strcmp(s.str(), "UTF-8 string literal"));

    SKETCH_TEST_ASSERT(s.isSame(s));
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::Utf8String()")
    Utf8String s;
    SKETCH_TEST_ASSERT(s.length() == 0);
    SKETCH_TEST_ASSERT(s.empty());
    SKETCH_TEST_ASSERT(*s.str() == 0);
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::Utf8String(const wchar_t*)")
    const wchar_t* u16 = L"UTF-16 (wchar_t) text";
    Utf8String s(u16);
    SKETCH_TEST_ASSERT(s.length() == wcslen(u16));
    SKETCH_TEST_ASSERT(!s.empty());
    SKETCH_TEST_ASSERT(!strcmp(s.str(), "UTF-16 (wchar_t) text"));

    Utf8String s2(L"");
    SKETCH_TEST_ASSERT(s2.empty());
    SKETCH_TEST_ASSERT(*s2.str() == 0);

    Utf8String s3((const wchar_t*)nullptr);
    SKETCH_TEST_ASSERT(s3.empty());
    SKETCH_TEST_ASSERT(*s3.str() == 0);
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::Utf8String(const char*)")
    const char* u8 = "UTF-8 (char) text";
    Utf8String s(u8);
    SKETCH_TEST_ASSERT(s.length() == strlen(u8));
    SKETCH_TEST_ASSERT(!s.empty());
    SKETCH_TEST_ASSERT(!strcmp(s.str(), u8));

    Utf8String s2("");
    SKETCH_TEST_ASSERT(s2.empty());
    SKETCH_TEST_ASSERT(*s2.str() == 0);

    Utf8String s3((const char*)nullptr);
    SKETCH_TEST_ASSERT(s3.empty());
    SKETCH_TEST_ASSERT(*s3.str() == 0);
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::operator=(const Utf8String&)")
    Utf8String s("UTF-8 (char) text");

    Utf8String s2;
    s2 = s;

    SKETCH_TEST_ASSERT(s2.length() == s.length());
    SKETCH_TEST_ASSERT(!strcmp(s.str(), s2.str()));

    SKETCH_TEST_ASSERT(s2.isSame(s));

    // self-assignment
    s2 = s2;
    SKETCH_TEST_ASSERT(s2.length() == s.length());
    SKETCH_TEST_ASSERT(!strcmp(s.str(), s2.str()));

    SKETCH_TEST_ASSERT(s2.isSame(s2));
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::Utf8String(Utf8String&&)")
    const char* u8 = "UTF-8 (char) text";
    Utf8String s(u8);

    Utf8String s2(sketch::move(s));

    SKETCH_TEST_ASSERT(!s2.isSame(s));

    SKETCH_TEST_ASSERT(s2.length() == strlen(u8));
    SKETCH_TEST_ASSERT(!strcmp(s2.str(), u8));

    SKETCH_TEST_ASSERT(s.empty());
    SKETCH_TEST_ASSERT(s.length() == 0);
    SKETCH_TEST_ASSERT(!*s.str());
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::operator=(Utf8String&&)")
    const char* u8 = "UTF-8 (char) text";
    Utf8String s(u8);

    Utf8String s2;
    s2 = sketch::move(s);

    SKETCH_TEST_ASSERT(!s2.isSame(s));

    SKETCH_TEST_ASSERT(s2.length() == strlen(u8));
    SKETCH_TEST_ASSERT(!strcmp(s2.str(), u8));

    SKETCH_TEST_ASSERT(s.empty());
    SKETCH_TEST_ASSERT(s.length() == 0);
    SKETCH_TEST_ASSERT(!*s.str());
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::compare(const char*, size_t)")
    const char* u8 = "UTF-8 (char) text";
    Utf8String s(u8);

    SKETCH_TEST_ASSERT(s.compare(s.str()) == 0);
    SKETCH_TEST_ASSERT(s.compare(u8) == 0);

    SKETCH_TEST_ASSERT(s.compare("A") > 0);
    SKETCH_TEST_ASSERT(s.compare("Z") < 0);
    SKETCH_TEST_ASSERT(s.compare("") > 0);

    SKETCH_TEST_ASSERT(s.compare("UTF", 3) == 0);
    SKETCH_TEST_ASSERT(s.compare("UTF", 4) > 0);
    SKETCH_TEST_ASSERT(s.compare("UTF", 2) == 0);
    SKETCH_TEST_ASSERT(s.compare("", 2) > 0);
    SKETCH_TEST_ASSERT(s.compare("", 0) == 0);
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::compareNoCase(const char*, size_t)")
    const char* u8 = "UTF-8 (char) text";
    Utf8String s(u8);

    SKETCH_TEST_ASSERT(s.compareNoCase(s.str()) == 0);
    SKETCH_TEST_ASSERT(s.compareNoCase(u8) == 0);

    SKETCH_TEST_ASSERT(s.compareNoCase("A") > 0);
    SKETCH_TEST_ASSERT(s.compareNoCase("Z") < 0);
    SKETCH_TEST_ASSERT(s.compareNoCase("") > 0);

    SKETCH_TEST_ASSERT(s.compareNoCase("UTF", 3) == 0);
    SKETCH_TEST_ASSERT(s.compareNoCase("UTF", 4) > 0);
    SKETCH_TEST_ASSERT(s.compareNoCase("UTF", 2) == 0);
    SKETCH_TEST_ASSERT(s.compareNoCase("", 2) > 0);
    SKETCH_TEST_ASSERT(s.compareNoCase("", 0) == 0);

    SKETCH_TEST_ASSERT(s.compareNoCase("utf", 3) == 0);
    SKETCH_TEST_ASSERT(s.compareNoCase("utf", 4) > 0);
    SKETCH_TEST_ASSERT(s.compareNoCase("utf", 2) == 0);
    SKETCH_TEST_ASSERT(s.compareNoCase("", 2) > 0);
    SKETCH_TEST_ASSERT(s.compareNoCase("", 0) == 0);
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::operator==(const Utf8String& a, const Utf8String& b)")
    Utf8String s1;
    SKETCH_TEST_ASSERT(s1 == s1);

    Utf8String s2;
    SKETCH_TEST_ASSERT(s2 == s1);

    Utf8String s3("");
    SKETCH_TEST_ASSERT(s3 == s1);

    Utf8String s4("Yyy");
    SKETCH_TEST_ASSERT(!(s4 == s3));
    SKETCH_TEST_ASSERT(s4 == s4);

    Utf8String s5(s4);
    SKETCH_TEST_ASSERT(s5 == s4);

    Utf8String s6;
    s6 = s4;
    SKETCH_TEST_ASSERT(s6 == s4);

    Utf8String s7("ZZZ");
    SKETCH_TEST_ASSERT(!(s7 == s4));

    Utf8String s8("Yyy");
    SKETCH_TEST_ASSERT(s8 == s4);
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::operator!=(const Utf8String&, const Utf8String&)")
    Utf8String s1;
    SKETCH_TEST_ASSERT(!(s1 != s1));

    Utf8String s2;
    SKETCH_TEST_ASSERT(!(s2 != s1));

    Utf8String s3("");
    SKETCH_TEST_ASSERT(!(s3 != s1));

    Utf8String s4("Yyy");
    SKETCH_TEST_ASSERT(s4 != s3);
    SKETCH_TEST_ASSERT(!(s4 != s4));

    Utf8String s5(s4);
    SKETCH_TEST_ASSERT(!(s5 != s4));

    Utf8String s6;
    s6 = s4;
    SKETCH_TEST_ASSERT(!(s6 != s4));

    Utf8String s7("ZZZ");
    SKETCH_TEST_ASSERT(s7 != s4);

    Utf8String s8("Yyy");
    SKETCH_TEST_ASSERT(!(s8 != s4));
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::operator<(const Utf8String& a, const Utf8String& b)")
    Utf8String s1;
    SKETCH_TEST_ASSERT(!(s1 < s1));

    Utf8String s2;
    SKETCH_TEST_ASSERT(!(s2 < s1));

    Utf8String s3("");
    SKETCH_TEST_ASSERT(!(s3 < s1));

    Utf8String s4("Yyy");
    SKETCH_TEST_ASSERT(!(s4 < s3));
    SKETCH_TEST_ASSERT(s3 < s4);

    Utf8String s5(s4);
    SKETCH_TEST_ASSERT(!(s5 < s4));

    Utf8String s6;
    s6 = s4;
    SKETCH_TEST_ASSERT(!(s6 < s4));

    Utf8String s7("ZZZ");
    SKETCH_TEST_ASSERT(!(s7 < s4));
    SKETCH_TEST_ASSERT(s4 < s7);

    Utf8String s8("Yyy");
    SKETCH_TEST_ASSERT(!(s8 < s4));
    SKETCH_TEST_ASSERT(!(s4 < s8));
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::operator>(const Utf8String& a, const Utf8String& b)")
    Utf8String s1;
    SKETCH_TEST_ASSERT(!(s1 > s1));

    Utf8String s2;
    SKETCH_TEST_ASSERT(!(s2 > s1));

    Utf8String s3("");
    SKETCH_TEST_ASSERT(!(s3 > s1));

    Utf8String s4("Yyy");
    SKETCH_TEST_ASSERT(s4 > s3);
    SKETCH_TEST_ASSERT(!(s3 > s4));

    Utf8String s5(s4);
    SKETCH_TEST_ASSERT(!(s5 > s4));

    Utf8String s6;
    s6 = s4;
    SKETCH_TEST_ASSERT(!(s6 > s4));

    Utf8String s7("ZZZ");
    SKETCH_TEST_ASSERT(s7 > s4);
    SKETCH_TEST_ASSERT(!(s4 > s7));

    Utf8String s8("Yyy");
    SKETCH_TEST_ASSERT(!(s8 > s4));
    SKETCH_TEST_ASSERT(!(s4 > s8));
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Utf8String, "Utf8String::append(const char*, size_t)")
    Utf8String s;

    s.append("");
    SKETCH_TEST_ASSERT(s.empty());
    SKETCH_TEST_ASSERT(s.length() == 0);
    SKETCH_TEST_ASSERT(!*s.str());

    s.append("", 0);
    SKETCH_TEST_ASSERT(s.empty());
    SKETCH_TEST_ASSERT(s.length() == 0);
    SKETCH_TEST_ASSERT(!*s.str());

    s.append("12345");
    SKETCH_TEST_ASSERT(s == Utf8String("12345"));

    s.append("67890");
    SKETCH_TEST_ASSERT(s == Utf8String("1234567890"));

    // create an additional reference
    Utf8String s2(s);
    SKETCH_TEST_ASSERT(s2.isSame(s));
    s.append("abcde");
    SKETCH_TEST_ASSERT(s == Utf8String("1234567890abcde"));
    SKETCH_TEST_ASSERT(!s2.isSame(s));

    Utf8String s3(s);
    SKETCH_TEST_ASSERT(s3.isSame(s));
    s.append("fghji");
    SKETCH_TEST_ASSERT(s == Utf8String("1234567890abcdefghji"));
    SKETCH_TEST_ASSERT(!s3.isSame(s));
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(Utf8String, "Utf8String::operator+()")
    SKETCH_TEST_ASSERT(Utf8String("") + Utf8String("") == Utf8String(""));
    SKETCH_TEST_ASSERT(Utf8String("") + "" == Utf8String(""));
    SKETCH_TEST_ASSERT("" + Utf8String("") == Utf8String(""));

    SKETCH_TEST_ASSERT(Utf8String("abc") + Utf8String("def") == Utf8String("abcdef"));
    SKETCH_TEST_ASSERT(Utf8String("abc") + "def" == Utf8String("abcdef"));
    SKETCH_TEST_ASSERT("abc" + Utf8String("def") == Utf8String("abcdef"));
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(Utf8String, "Utf8String::format()")
    SKETCH_TEST_ASSERT(Utf8String::format("") == Utf8String());
    SKETCH_TEST_ASSERT(Utf8String::format("Test") == Utf8String("Test"));
    SKETCH_TEST_ASSERT(Utf8String::format("Test %d", 12) == Utf8String("Test 12"));
    SKETCH_TEST_ASSERT(Utf8String::format("Test %S", L"UTF-16") == Utf8String("Test UTF-16"));
    SKETCH_TEST_ASSERT(Utf8String::format("Test %s", "UTF-8") == Utf8String("Test UTF-8"));
SKETCH_TEST_END()


