#include "common.h"


using namespace sketch;


SKETCH_TEST_BEGIN(Error, "Error")

#ifdef SKETCH_WIN_UM
    auto r03 = SkError("This is where it all started");
    auto r02 = SkNtError2(((NTSTATUS)0xC00001ADL), "Even deeper NT error", r03);
    auto r01 = SkNtError2(((NTSTATUS)0xC0000002L), L"Some inner NT error", r02);
    auto r00 = SkNtError2(((NTSTATUS)0xC0000001L), String(), r01);
    auto r0 = SkWin32Error2(E_OUTOFMEMORY, String(), r00);
    auto r = SkGenericError2(GenericError::EOutOfMemory, "This is a GenericError EOutOfMemory", r0);
#endif

#ifdef SKETCH_WIN_KM
    auto r03 = SkError("This is where it all started");
    auto r02 = SkNtError2(((NTSTATUS)0xC0E90003L), "Some inner NT error", r03);
    auto r = SkGenericError2(GenericError::EOutOfMemory, "This is a GenericError EOutOfMemory", r02);
#endif

    SkLogError(r);

SKETCH_TEST_END()


#ifdef SKETCH_CXX_EXCEPTIONS

SKETCH_TEST_BEGIN(Exception, "Exception")

    {
        bool caught = false;

        try
        {
            auto r0 = SkWin32Error(E_OUTOFMEMORY, "Inner E_OUTOFMEMORY exception");
            auto r = SkGenericError2(GenericError::EOutOfMemory, "GenericError EOutOfMemory exception", r0);

            throw Exception(r);
        }
        catch (Exception& e)
        {
            SkLogError(e.error());
            caught = true;
        }

        SKETCH_TEST_ASSERT(caught);
    }

#ifdef SKETCH_STL_AVAILABLE
    {
        bool caught = false;

        try
        {
            auto r = SkGenericError(GenericError::EOutOfMemory, "GenericError EOutOfMemory exception caught as an std::exception");

            throw Exception(r);
        }
        catch (std::exception& e)
        {
            SketchError(e.what());
            caught = true;
        }

        SKETCH_TEST_ASSERT(caught);
    }
#endif

SKETCH_TEST_END()

#endif // SKETCH_CXX_EXCEPTIONS
