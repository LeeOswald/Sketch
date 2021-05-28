#include <SkBase/Random.hxx>

#include <bcrypt.h>


namespace sketch
{

namespace random
{

static BCRYPT_ALG_HANDLE g_Provider = nullptr;


bool initialize() noexcept
{
    SK_ASSERT(!g_Provider);

    auto status = BCryptOpenAlgorithmProvider(
        &g_Provider,
        BCRYPT_RNG_ALGORITHM,
        MS_PRIMITIVE_PROVIDER,
        BCRYPT_PROV_DISPATCH
    );

    return NT_SUCCESS(status);
}

void finalize() noexcept
{
    if (g_Provider)
    {
        BCryptCloseAlgorithmProvider(g_Provider, 0);
        g_Provider = nullptr;
    }
}

bool generate(void* buffer, size_t size) noexcept
{
    if (!g_Provider) return false;

    auto status = BCryptGenRandom(g_Provider, PUCHAR(buffer), ULONG(size), 0);
    SK_ASSERT(NT_SUCCESS(status));

    return NT_SUCCESS(status);
}


} // namespace random {}

} // namespace sketch {}
