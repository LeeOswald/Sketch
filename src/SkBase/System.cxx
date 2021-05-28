#include <SkBase/System.hxx>
#include <SkBase/nt/Registry.hxx>

namespace sketch
{

namespace system
{


String machineName()
{
    nt::RegistryKey k(
        L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName",
        nt::RegistryKey::KeyOpen,
        nt::RegistryKey::KeyRead,
        ErrorState::Mode::neverThrow
    );

    if (!k.valid())
    {
        return String();
    }

    String s;
    return k.readString(L"ComputerName", s) ? s : String();
}

} // namespace system {}

} // namespace sketch {}
