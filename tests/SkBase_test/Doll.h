#pragma once


template <bool _Verbose>
class Doll
{
public:
    static long& instances()
    {
        static long s_instances = 0;
        return s_instances;
    }

    ~Doll()
    {
        if (_Verbose) SketchDebug("Doll::~Doll() was v = %d", m_value);
        instances()--;
    }

    Doll()
        : m_value(0)
    {
        if (_Verbose) SketchDebug("Doll::Doll()");
        instances()++;
    }

    Doll(long v)
        : m_value(v)
    {
        if (_Verbose) SketchDebug("Doll::Doll(%d)", v);
        instances()++;
    }

    Doll(const Doll& o)
        : m_value(o.m_value)
    {
        if (_Verbose) SketchDebug("Doll::Doll(const Doll& = %d)", o.m_value);
        instances()++;
    }

    Doll& operator=(const Doll& o)
    {
        if (_Verbose) SketchDebug("Doll::operator=(const Doll& = %d) was v = %d", o.m_value, m_value);
        m_value = o.m_value;
        return *this;
    }

    Doll(Doll&& o)
        : m_value(o.m_value)
    {
        o.m_value = 0;

        if (_Verbose) SketchDebug("Doll::Doll(Doll&& = %d)", o.m_value);
        instances()++;
    }

    Doll& operator=(Doll&& o)
    {
        if (_Verbose) SketchDebug("Doll::operator=(Doll&& = %d) was v = %d", o.m_value, m_value);
        m_value = o.m_value;
        o.m_value = 0;
        return *this;
    }

public:
    long m_value;
};



