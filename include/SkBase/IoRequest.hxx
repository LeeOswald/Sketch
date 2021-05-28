#pragma once

#include "../Sketch.hxx"

#include "Empty.hxx"
#include "IntrusiveList.hxx"


namespace sketch
{


template <class _Sub = Empty>
class IoRequest
    : public NonCopyable
{
public:
    struct Deleter
    {
        constexpr Deleter() noexcept = default;

        inline Deleter(const Deleter&) noexcept
        {
        }

        inline void operator()(IoRequest* p) const noexcept
        {
            IoRequest::destroy(p);
        }
    };

    typedef UniquePtr<IoRequest, Deleter> Ref;

    struct __declspec(novtable) ICompletion
    {
        virtual void complete(IoRequest* r) = 0;

    protected:
        virtual ~ICompletion() noexcept {}
    };

    enum Type
    {
        create,
        close,
        bind,
        connect,
        disconnect,
        read,
        write,
        control
    };

    enum Status
    {
        success = 0,
        pending = 1,
        failure = -1,
        cancelled = -2
    };

    ListNode<IoRequest> link;

    static void destroy(IoRequest* p) noexcept
    {
        if (p)
        {
            auto r = p->sub();
            r->~_Sub();

            p->~IoRequest();
            SkFree(p);
        }
    }

    template<class... Args>
    static Ref make(Type type, ICompletion* completion, const void* data, size_t size, Args&&... args) noexcept
    {
        size_t required = sizeof(IoRequest) + sizeof(_Sub) + size;
        auto p = SkMalloc(required);
        if (!p)
        {
            return Ref();
        }

        auto r = Ref(new (p) IoRequest(type, completion, data, size));

        auto s = r->sub();
        ::new ((void*)s) _Sub(sketch::forward<Args>(args)...);

        return r;
    }

    _Sub* sub() noexcept
    {
        return reinterpret_cast<_Sub*>(this + 1);
    }

    inline static constexpr IoRequest* master(_Sub* r) noexcept
    {
        return offset_pointer<IoRequest>(r, -ptrdiff_t(sizeof(IoRequest)));
    }

    void* data() noexcept
    {
        return offset_pointer<void>((this + 1), sizeof(_Sub));
    }

    Type type() const noexcept
    {
        return m_type;
    }

    size_t size() const noexcept
    {
        return m_size;
    }

    long status() const noexcept
    {
        return m_status;
    }

    size_t resultSize() const noexcept
    {
        return m_resultSize;
    }

    void setStatus(long status, size_t resultSize) noexcept
    {
        m_status = status;
        m_resultSize = resultSize;
    }

    ICompletion* completion() const noexcept
    {
        return m_completion;
    }

    void setCompletion(ICompletion* completion) noexcept
    {
        m_completion = completion;
    }

private:
    ~IoRequest() noexcept
    {
    }

    IoRequest(Type type, ICompletion* completion, const void* data, size_t size) noexcept
        : m_type(type)
        , m_completion(completion)
        , m_size(size)
        , m_status(0)
        , m_resultSize(0)
    {
        if (data && size)
        {
            memcpy(this->data(), data, size);
        }
    }

private:
    Type m_type;
    ICompletion* m_completion;
    size_t m_size;
    long m_status;
    size_t m_resultSize;
};



} // namespace sketch {}
