#include "common.h"

#include <SkBase/nt/Registry.hxx>

using namespace sketch;


SKETCH_TEST_BEGIN(RegistryKey, "RegistryKey::RegistryKey(KeyOpen)")
    SKETCH_TEST_ASSERT(nt::RegistryKey::keyExists(L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName"));
    SKETCH_TEST_ASSERT(!nt::RegistryKey::keyExists(L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ComputerName\\PassiveComputerName"));

    nt::RegistryKey k(
        L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName",
        nt::RegistryKey::KeyOpen,
        nt::RegistryKey::KeyRead,
        ErrorState::Mode::neverThrow
    );

    SKETCH_TEST_ASSERT(k.valid());
    if (k.valid())
    {
        String s;
        SKETCH_TEST_ASSERT(k.readString(L"ComputerName", s));
        SketchInfo("Computer name is [%s]", toSystemLocale(s).str());
    }

SKETCH_TEST_END()


SKETCH_TEST_BEGIN(RegistryKey, "RegistryKey::RegistryKey(KeyCreate)")

    static const wchar_t* K = L"\\Registry\\Machine\\Software\\Sketch";
    static const wchar_t* K1 = L"\\Registry\\Machine\\Software\\Sketch\\k1";

    nt::RegistryKey::deleteKey(K1);
    nt::RegistryKey::deleteKey(K);

    {
        nt::RegistryKey k(
            K,
            nt::RegistryKey::KeyCreate,
            nt::RegistryKey::KeyWrite,
            ErrorState::Mode::neverThrow
        );

        SKETCH_TEST_ASSERT(k.valid());

        if (k.valid())
        {
            SKETCH_TEST_ASSERT(nt::RegistryKey::keyExists(K));

            nt::RegistryKey k1(
                K1,
                nt::RegistryKey::KeyCreate,
                nt::RegistryKey::KeyWrite,
                ErrorState::Mode::neverThrow
            );

            SKETCH_TEST_ASSERT(k1.valid());

            if (k1.valid())
            {
                SKETCH_TEST_ASSERT(nt::RegistryKey::keyExists(K1));

                k1.writeString(L"v1", L"string");
                k1.writeDWord(L"v2", 12345);
            }
        }
    }

    {
        nt::RegistryKey k(
            K,
            nt::RegistryKey::KeyOpen,
            nt::RegistryKey::KeyRead,
            ErrorState::Mode::neverThrow
        );

        SKETCH_TEST_ASSERT(k.valid());

        if (k.valid())
        {
            nt::RegistryKey k1(
                K1,
                nt::RegistryKey::KeyOpen,
                nt::RegistryKey::KeyRead,
                ErrorState::Mode::neverThrow
            );

            SKETCH_TEST_ASSERT(k1.valid());

            if (k1.valid())
            {
                String s;
                SKETCH_TEST_ASSERT(k1.readString(L"v1", s));
                SKETCH_TEST_ASSERT(s == L"string");

                uint32_t dw;
                SKETCH_TEST_ASSERT(k1.readDWord(L"v2", dw));
                SKETCH_TEST_ASSERT(dw == 12345);
            }
        }
    }

    {
        nt::RegistryKey k(
            K,
            nt::RegistryKey::KeyOpen,
            nt::RegistryKey::KeyDelete,
            ErrorState::Mode::neverThrow
        );

        SKETCH_TEST_ASSERT(k.valid());

        if (k.valid())
        {
            nt::RegistryKey k1(
                K1,
                nt::RegistryKey::KeyOpen,
                nt::RegistryKey::KeyDelete,
                ErrorState::Mode::neverThrow
            );

            SKETCH_TEST_ASSERT(k1.valid());

            if (k1.valid())
            {
                k1.deleteKey();
            }

            k.deleteKey();
        }
    }

    SKETCH_TEST_ASSERT(!nt::RegistryKey::keyExists(K));
    SKETCH_TEST_ASSERT(!nt::RegistryKey::keyExists(K1));

SKETCH_TEST_END()

