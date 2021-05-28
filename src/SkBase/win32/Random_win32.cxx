#include <SkBase/Random.hxx>

#include <wincrypt.h>


namespace sketch
{

namespace random
{

static HCRYPTPROV g_Provider = 0;


bool initialize() noexcept
{
    SK_ASSERT(!g_Provider);

    return !!CryptAcquireContext(&g_Provider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
}

void finalize() noexcept
{
    if (g_Provider)
    {
        CryptReleaseContext(g_Provider, 0);
        g_Provider = 0;
    }
}

bool generate(void* buffer, size_t size) noexcept
{
    if (!g_Provider) return false;

    auto status = CryptGenRandom(g_Provider, static_cast<DWORD>(size), static_cast<BYTE*>(buffer));
    SK_ASSERT(status);

    return !!status;
}


} // namespace random {}

} // namespace sketch {}
