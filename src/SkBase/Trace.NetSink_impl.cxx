#include "Trace.NetSink_impl.hxx"

#include <jansson/sk_jansson.hxx>

namespace sketch
{

namespace log
{

namespace internal
{

char* _packRecord(Record::Ref r) noexcept
{
    char* result = nullptr;

#ifdef SKETCH_CXX_EXCEPTIONS
    try
    {
#endif
        if (r)
        {
            auto j = json_object();
            if (j)
            {
                json_object_set_new(j, "level", json_integer(r->level()));
                json_object_set_new(j, "module", json_string(r->module()));
                json_object_set_new(j, "file", json_string(r->file()));
                json_object_set_new(j, "line", json_integer(r->line()));

                {
                    auto date = r->time().unpack();
                    auto f = AnsiString::format(
                        "%02d:%02d:%02d.%04d",
                        date.hour(),
                        date.minute(),
                        date.second(),
                        date.millisecond()
                    );

                    json_object_set_new(j, "time", json_string(f.str()));
                }

                json_object_set_new(j, "pid", json_integer(r->pid()));
                json_object_set_new(j, "tid", json_integer(r->tid()));
#ifdef SKETCH_WIN_KM
                json_object_set_new(j, "irql", json_integer(r->irql()));
#endif
                json_object_set_new(j, "text", json_string(r->text().str()));

                result = json_dumps(j, JSON_COMPACT);

                json_decref(j);
            }
        }
#ifdef SKETCH_CXX_EXCEPTIONS
    }
    catch (sketch::Exception&)
    {
    }
#endif

    return result;
}


} // namespace internal {}

} // namespace log {}

} // namespace sketch {}
