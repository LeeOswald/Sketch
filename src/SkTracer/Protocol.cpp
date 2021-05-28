#include "stdafx.h"
#include "Protocol.h"

#include <jansson/sk_jansson.hxx>


namespace protocol
{

Record::Ref Record::unpack(const char* jsonRaw)
{
    Record::Ref r(new Record);

    json_error_t e;
    auto j = json_loads(jsonRaw, 0, &e);
    if (!j)
    {
        throw sketch::Exception(SkError("Failed to parse a JSON record"));
    }

    if (json_is_object(j))
    {
        const char *key;
        json_t* value;
        json_object_foreach(j, key, value)
        {
            if (json_is_string(value))
            {
                CStringW v(json_string_value(value));
                if (!strcmp(key, "module"))
                {
                    r->module = v;
                }
                else if (!strcmp(key, "file"))
                {
                    r->file = v;
                }
                else if (!strcmp(key, "time"))
                {
                    r->time = v;
                }
                else if (!strcmp(key, "text"))
                {
                    r->text = v;
                }
            }
            else if (json_is_integer(value))
            {
                int v = int(json_integer_value(value));
                if (!strcmp(key, "level"))
                {
                    r->level = Level(v);
                }
                else if (!strcmp(key, "line"))
                {
                    r->line = v;
                }
                else if (!strcmp(key, "pid"))
                {
                    r->pid = v;
                }
                else if (!strcmp(key, "tid"))
                {
                    r->tid = v;
                }
                else if (!strcmp(key, "irql"))
                {
                    r->irql = v;
                }
            }
        }
    }
    else
    {
        throw sketch::Exception(SkError("JSON record is not an object"));
    }

    json_delete(j);

    return r;
}

} // namespace protocol {}
