#pragma once

#include "../Sketch.hxx"


namespace sketch
{


struct __declspec(novtable) ISimpleDelegate
{
    virtual void run(void* ctx) = 0;

protected:
    virtual ~ISimpleDelegate() noexcept {}
};


} // namespace sketch {}
