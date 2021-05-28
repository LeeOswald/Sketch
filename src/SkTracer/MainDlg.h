#pragma once

#include "ResizableDialog.h"
#include "TcpServer.h"
#include "UdpServer.h"

#include <SkBase/Mutex.hxx>
#include <SkBase/Vector.hxx>

#include <list>

namespace tracer
{

class MainDlg
    : public ResizableDialog
    , public tracer::IMessageCallback
{
public:
    enum class Protocol
    {
        Tcp,
        Udp
    };

    MainDlg(CWnd* pParent, Protocol proto, uint16_t port);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SHADOW_DIALOG };
#endif
    void tracer::IMessageCallback::message(protocol::Record::Ref r) override;

private:
    static CStringW recordToString(const protocol::Record* r);

    void reloadFilters();
    bool filterMessage(const protocol::Record* r) const;
    void reapplyFilters();

    void captureSettings();
    void restoreSettings();

    void saveSelection();
    void restoreSelection();

    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnBnClickedClear();
    afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedAutoscroll();
    afx_msg void OnBnClickedFind();
    afx_msg void OnDestroy();
    afx_msg LRESULT OnNewRecord(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBnClickedFilters();
    afx_msg void OnCopyRows();
    afx_msg void OnSelectAll();

private:
    enum
    {
        SH_NEW_RECORD = (WM_USER + 1)
    };

    enum
    {
        SH_MAX_RECORDS = 1000000,
        SH_RECORDS_TO_DISCARD = 500
    };

    Protocol m_proto;
    uint16_t m_port;

    HICON m_hIcon;
    CListCtrl m_messageView;
    CButton m_autoScrollBtn;
    CEdit m_findBox;

    bool m_autoScroll{ false };
    sketch::Vector<protocol::Record*> m_database;
    sketch::Vector<protocol::Record::Ref> m_databaseUnfiltered;
    CString m_lastSearch;
    int m_lastFound{ -1 };

    std::list<protocol::Record::Ref> m_queue;
    sketch::Mutex m_lock;

    bool m_filterDebug{ false };
    bool m_filterInfo{ false };
    bool m_filterWarn{ false };
    bool m_filterError{ false };
    bool m_filterHighest{ false };
    CString m_filterComponent;

    TcpServer::Ref m_serverTcp;
    UdpServer::Ref m_serverUdp;
};


} // namespace tracer {}
