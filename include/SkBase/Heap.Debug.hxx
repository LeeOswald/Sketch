#pragma once

#include "../Sketch.hxx"

#include <SkBase/IntrusiveList.hxx>
#include <SkBase/Spinlock.hxx>


namespace sketch
{

template <class _Under>
class DebugHeap
    : public _Under
{
public:
    enum
    {
        DumpAsLeaks = 0x0001,
        DumpBlocks = 0x0002,
        DumpMemory = 0x0003,
        DumpStats = 0x0004
    };

    ~DebugHeap() noexcept
    {
        // free any leaked memory
        auto h = m_list.pull_front();
        while (h)
        {
            _Under::free(h);

            h = m_list.pull_front();
        }
    }

    inline DebugHeap(bool breakOnErrors = true) noexcept
        : m_break(breakOnErrors)
        , m_totalAllocations(0)
        , m_currentAllocations(0)
        , m_peakAllocations(0)
        , m_totalBytes(0)
        , m_currentBytes(0)
        , m_peakBytes(0)
        , m_largest(0)
        , m_smallest(uint64_t(-1))
    {
    }

    void* allocate(size_t size, const char* file, int line) noexcept
    {
        SK_ASSERT(size > 0);

        size_t required = size + sizeof(Header) + sizeof(Tail);
        auto p = _Under::allocate(required);
        if (!p)
        {
            return nullptr;
        }

        _initialize(p, size, file, line);

        auto h = static_cast<Header*>(p);
        auto data = _data(h);

        _check(data, true);

        _record(h);

        return data;
    }

    void free(void* p) noexcept
    {
        SK_ASSERT(p);

        _check(p, m_break);

        auto h = _header(p);
        h->heap = nullptr;

        _remove(h);

        _Under::free(h);
    }

    void dump(int flags) noexcept
    {
#ifdef SKETCH_CXX_EXCEPTIONS
        try
        {
#endif
            Spinlock::Guard g(m_lock);
            {
                if (flags & DumpAsLeaks)
                {
                    auto n = m_list.size();
                    if (n)
                    {
                        SketchError("%d leaked blocks detected in heap %p", n, this);
                    }
                }
                else if (flags & DumpStats)
                {
                    SketchInfo("Statistics for heap %p", this);
                }
                else if (flags & DumpBlocks)
                {
                    SketchInfo("Blocks in heap %p", this);
                }

                if (flags & DumpStats)
                {
                    _dumpStats();
                }

                if (flags & DumpBlocks)
                {
                    for (const Header& h : m_list)
                    {
                        _dump(&h, flags);
                    }
                }
            }

#ifdef SKETCH_CXX_EXCEPTIONS
        }
        catch (...)
        {
        }
#endif
    }

private:
    static const size_t GuardDwords = 2;
    static const uint32_t GuardValue = 0xfeeddaad;
    static const uint8_t FillValue = 0xaa;
    static const size_t BytesToDump = 64;

    struct Header
    {
        ListNode<Header> node;
        DebugHeap<_Under>* heap;
        size_t size;
        const char* file;
        int line;
        uint32_t guardPre[GuardDwords];
    };

    struct Tail
    {
        size_t size;
        uint32_t guardPost[GuardDwords];
    };

    __forceinline static Header* _header(void* p) noexcept
    {
        auto h = static_cast<Header*>(p);
        return h - 1;
    }

    __forceinline static Tail* _tail(Header* h) noexcept
    {
        auto p = _data(h);
        return reinterpret_cast<Tail*>(offset_pointer<uint8_t>(p, h->size));
    }

    __forceinline static void* _data(Header* h) noexcept
    {
        return h + 1;
    }

    __forceinline static const void* _data(const Header* h) noexcept
    {
        return h + 1;
    }

    void _initialize(void* p, size_t size, const char* file, int line) noexcept
    {
        auto h = static_cast<Header*>(p);
        h->heap = this;
        h->size = size;
        h->file = file;
        h->line = line;

        for (size_t i = 0; i < GuardDwords; i++)
        {
            h->guardPre[i] = GuardValue;
        }

        auto d = _data(h);
        memset(d, FillValue, size);

        auto t = _tail(h);
        t->size = size;
        for (size_t i = 0; i < GuardDwords; i++)
        {
            t->guardPost[i] = GuardValue;
        }
    }

    bool _check(void* p, bool breakOnErrors) noexcept
    {
        auto h = _header(p);
        if (h->heap != this)
        {
            SketchError("Unknown block at %p does not come from this (%p) heap", p, this);
            if (breakOnErrors) __debugbreak();
            return false;
        }

        auto t = _tail(h);
        if (h->size != t->size)
        {
            SketchError("Corrupted %d bytes long block at %p [%s]:%d", h->size, p, h->file, h->line);
            if (breakOnErrors) __debugbreak();
            return false;
        }

        for (size_t i = 0; i < GuardDwords; i++)
        {
            if (h->guardPre[i] != GuardValue)
            {
                SketchError("Memory corruption before %d bytes long block at %p [%s]:%d", h->size, p, h->file, h->line);
                if (breakOnErrors) __debugbreak();
                return false;
            }
        }

        for (size_t i = 0; i < GuardDwords; i++)
        {
            if (t->guardPost[i] != GuardValue)
            {
                SketchError("Memory corruption after %d bytes long block at %p [%s]:%d", h->size, p, h->file, h->line);
                if (breakOnErrors) __debugbreak();
                return false;
            }
        }

        return true;
    }

    static void _bytesToHexDump(const void *data, size_t size, char *buffer, size_t bufferSize) noexcept
    {
        static const char sHexDigits[] = "0123456789ABCDEF";

        bufferSize -= 1; // space for '\0'

        auto *p = static_cast<const uint8_t *>(data);
        while (size && bufferSize)
        {
            if (p != data)
            {
                // space between bytes
                *buffer++ = ' ';

                if (!--bufferSize)
                {
                    break;
                }
            }

            // hi
            *buffer++ = sHexDigits[((*p) & 0xf0) >> 4];
            if (!--bufferSize)
            {
                break;
            }

            // lo
            *buffer++ = sHexDigits[(*p) & 0x0f];
            --bufferSize;

            p++;
            size--;
        }

        *buffer = '\0';
    }

    static bool _printable(char c) noexcept
    {
        return ((c >= 33) && (c <= 126));
    }

    static void _bytesToTextDump(const void *data, size_t size, char *buffer, size_t bufferSize) noexcept
    {
        bufferSize -= 1; // space for '\0'

        auto p = static_cast<const char*>(data);
        while (size && bufferSize)
        {
            if (_printable(*p))
            {
                *buffer++ = *p;
            }
            else
            {
                *buffer++ = '.';
            }

            bufferSize--;

            p++;
            size--;
        }

        *buffer = '\0';
    }

    void _record(Header* h) noexcept
    {
        Spinlock::Guard g(m_lock);
        {
            m_list.push_back(h);

            m_currentAllocations++;
            if (m_currentAllocations > m_peakAllocations)
            {
                m_peakAllocations = m_currentAllocations;
            }

            m_totalAllocations++;

            m_currentBytes += h->size;
            if (m_currentBytes >= m_peakBytes)
            {
                m_peakBytes = m_currentBytes;
            }

            m_totalBytes += h->size;

            if (m_largest < h->size)
            {
                m_largest = h->size;
            }

            if ((m_smallest == uint64_t(-1)) || (m_smallest > h->size))
            {
                m_smallest = h->size;
            }
        }
    }

    void _remove(Header* h) noexcept
    {
        Spinlock::Guard g(m_lock);
        {
            m_list.erase(h);

            m_currentAllocations--;
            SK_ASSERT(m_currentAllocations >= 0);

            m_currentBytes -= h->size;
            SK_ASSERT(m_currentBytes >= 0);
        }
    }

    void _dumpStats()
    {
        SketchInfo("Current: %I64d bytes in %I64d allocations", m_currentBytes, m_currentAllocations);
        SketchInfo("Total: %I64d bytes in %I64d allocations", m_totalBytes, m_totalAllocations);
        SketchInfo("Peak: %I64d bytes", m_peakBytes);
        SketchInfo("Peak: %I64d allocations", m_peakAllocations);
        SketchInfo("Smallest block: %I64d bytes", m_smallest);
        SketchInfo("Largest block: %I64d bytes", m_largest);
    }

    static void _dump(const Header* h, int flags)
    {
        auto data = _data(h);

        if (flags & DumpMemory)
        {
            char hexDump[BytesToDump * 3 + 1];
            char textDump[BytesToDump + 1];
            _bytesToHexDump(data, h->size, hexDump, _countof(hexDump));
            _bytesToTextDump(data, h->size, textDump, _countof(textDump));

            SketchInfo("%d bytes long block at %p [%s]:%d \"%s\" [%s]", h->size, data, h->file, h->line, textDump, hexDump);
        }
        else
        {
            SketchInfo("%d bytes long block at %p [%s]:%d", h->size, data, h->file, h->line);
        }
    }

private:
    bool m_break;
    Spinlock m_lock;
    IntrusiveList<Header, &Header::node> m_list;
    int64_t m_totalAllocations;
    int64_t m_currentAllocations;
    int64_t m_peakAllocations;
    int64_t m_totalBytes;
    int64_t m_currentBytes;
    int64_t m_peakBytes;
    uint64_t m_largest;
    uint64_t m_smallest;
};


} // namespace sketch {}
