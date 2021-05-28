#include "stdafx.h"
#include "Tracer.h"
#include "MainDlg.h"
#include "resource.h"
#include "FakeClient.h"

#include <SkBase/init.hxx>
#include <SkBase/UniquePtr.hxx>
#include <SkUser/CommandLine.hxx>

#include <crtdbg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace tracer
{

BEGIN_MESSAGE_MAP(Application, CWinApp)
END_MESSAGE_MAP()

Application g_app;

Application::Application()
{

}

BOOL Application::InitInstance()
{
    if (!sketch::SkBase::initialize())
    {
        return FALSE;
    }
    
#ifdef SKETCH_DBG
    int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(tmpFlag);
#endif    
    
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    AfxEnableControlContainer();

    SetRegistryKey(_T("SkTracer"));

    sketch::log::initialize("Tracer");

    sketch::CommandLine cl(::GetCommandLineW());
        
#ifdef SKETCH_DBG
    bool debug = true;
#else
    bool debug = false;
#endif

    debug = debug || cl.present(L"debug");

    if (debug)
    {
        sketch::log::registerSink("Debugger", sketch::log::TraceSink0::make(), sketch::log::DefaultTraceFormatter::make(), sketch::log::DefaultTraceFilter::make(sketch::log::Debug));
    }

    WSADATA wd;
    ::WSAStartup(0x0202, &wd);

    long port = 665;
    cl.singleValue(port, L"port");

    bool tcp = cl.present(L"tcp");

    auto enableFake = cl.present(L"fake");
    sketch::UniquePtr<FakeClient> fakeClient;
    if (enableFake)
    {
        try
        {
            fakeClient.reset(new FakeClient(
                tcp ? FakeClient::Protocol::tcp : FakeClient::Protocol::udp,
                L"127.0.0.1",
                uint16_t(port)
            ));
        }
        catch (sketch::Exception& e)
        {
            auto inner = e.error();
            auto f = inner->format(u"Unexpected exception: {MS} {FL}");

            auto l = sketch::toSystemLocale(f);
            SketchError(l.str());
        }
        catch (std::exception& e)
        {
            SketchError(e.what());
        }
    }

    m_accelerators = ::LoadAcceleratorsW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

    MainDlg dlg(nullptr, tcp? MainDlg::Protocol::Tcp : MainDlg::Protocol::Udp, uint16_t(port));
    m_pMainWnd = &dlg;
    dlg.DoModal();

    if (enableFake)
    {
        fakeClient.reset();
    }

    ::WSACleanup();

    sketch::log::finalize();

#ifndef _AFXDLL
    ControlBarCleanUp();
#endif

    sketch::SkBase::finalize();

    return FALSE;
}

BOOL Application::ProcessMessageFilter(int code, LPMSG lpMsg)
{
    if (code >= 0 && m_pMainWnd && m_accelerators)
    {
        if (::TranslateAcceleratorW(m_pMainWnd->m_hWnd, m_accelerators, lpMsg))
        {
            return TRUE;
        }
    }

    return CWinApp::ProcessMessageFilter(code, lpMsg);
}

} // namespace tracer {}
