#pragma once


#include "Atomic.hxx"
#include "Event.hxx"


namespace sketch
{


class RundownGuard
    : public NonCopyable
{
public:
    inline ~RundownGuard() noexcept
    {
    }

    inline RundownGuard() noexcept
        : m_ptr(nullptr)
    {
    }

    bool increment() noexcept
    {
        intptr_t val = (intptr_t)m_ptr;
        intptr_t newVal = val + RUNDOWN_COUNT_INC;

        do
        {
            if (((intptr_t)m_ptr) & RUNDOWN_ACTIVE)
            {
                return false;
            }

            if (atomic::compare_exchange(&m_ptr, (void*)newVal, (void*)val) == (void*)val)
            {
                return true;
            }
        } while (1);
    }

    void decrement()
    {
        intptr_t val = (intptr_t)m_ptr;

        do
        {
            if (val & RUNDOWN_ACTIVE)
            {
                auto wb = reinterpret_cast<WaitBlock*>(val & (~RUNDOWN_ACTIVE));

                if (atomic::increment(&wb->count) == 0)
                {
                    wb->event->set();
                }

                return;
            }
            else
            {
                intptr_t newVal = val - RUNDOWN_COUNT_INC;

                if (atomic::compare_exchange(&m_ptr, (void*)newVal, (void*)val) == (void*)val)
                {
                    return;
                }
            }

        } while (1);
    }

    void wait()
    {
        intptr_t val = 0;
        if (atomic::compare_exchange(&m_ptr, (void*)RUNDOWN_ACTIVE, (void*)val) == (void*)val)
        {
            return;
        }

        WaitBlock wb;
        do
        {
            intptr_t waitCnt = (val >> RUNDOWN_COUNT_SHIFT);
            if ((waitCnt > 0) && !wb.eventCreated())
            {
                wb.makeEvent();
            }

            wb.count = waitCnt;

            intptr_t newVal = ((intptr_t)(&wb)) | RUNDOWN_ACTIVE;

            if (atomic::compare_exchange(&m_ptr, (void*)newVal, (void*)val) == (void*)val)
            {
                if (waitCnt > 0)
                {
                    wb.event->wait(Event::infinite);
                    SK_ASSERT(wb.count == 0);
                }

                wb.destroyEvent();
                return;
            }

            SK_ASSERT((val & RUNDOWN_ACTIVE) == 0);
        } while (1);
    }

private:
    static const intptr_t RUNDOWN_ACTIVE = 0x1;
    static const intptr_t RUNDOWN_COUNT_SHIFT = 0x1;
    static const intptr_t RUNDOWN_COUNT_INC = (1 << RUNDOWN_COUNT_SHIFT);

    struct WaitBlock
        : public NonCopyable
    {
        WaitBlock() noexcept
        {
        }

        bool eventCreated() const noexcept
        {
            return !!event;
        }

        void makeEvent()
        {
            event = new (eventPlace) Event(Event::manualReset);
        }

        void destroyEvent() noexcept
        {
            if (event)
            {
                auto p = event;
                event = nullptr;
                p->~Event();
            }
        }

        atomic_t count{ 0 };
        Event* event{ nullptr };

    private:
        uint8_t eventPlace[sizeof(Event)];
    };

private:
    void* volatile m_ptr;
};


} // namespace sketch {}
