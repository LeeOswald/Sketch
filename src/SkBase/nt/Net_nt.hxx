#include <wsk.h>


namespace sketch
{

namespace net
{


struct WskData
{
    WSK_CLIENT_DISPATCH wskCliDisp;
    WSK_CLIENT_NPI wskCliNpi;
    WSK_REGISTRATION wskReg;
    WSK_PROVIDER_NPI wskProvNpi;
};

extern WskData g_WskData;


} // namespace net {}

} // namespace sketch {}