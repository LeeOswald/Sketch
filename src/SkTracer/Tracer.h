#pragma once


namespace tracer
{

class Application
    : public CWinApp
{
public:
    Application();

private:
    virtual BOOL InitInstance();
    virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);

    HACCEL m_accelerators;

    DECLARE_MESSAGE_MAP()
};

extern Application g_app;

} // namespace tracer {}
