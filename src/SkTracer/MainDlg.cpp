#include "stdafx.h"
#include "Tracer.h"
#include "LocalSink.h"
#include "MainDlg.h"
#include "afxdialogex.h"
#include "FilterDlg.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static const wchar_t* gLevels[] =
{
    L"DEBUG",
    L"INFO",
    L"WARN",
    L"ERROR",
    L"HIGHEST",
    L"OFF"
};

static COLORREF gTextColors[] =
{
    RGB(13, 31, 249), // DEBUG
    RGB(57, 184, 35), // INFO
    RGB(198, 21, 171), // WARN
    RGB(255, 0, 0), // ERROR
    RGB(0, 0, 0), // HIGHEST
    RGB(0, 0, 0) // OFF
};

static COLORREF gBackColors[] =
{
    RGB(255, 255, 255), // DEBUG
    RGB(255, 255, 255), // INFO
    RGB(255, 255, 255), // WARN
    RGB(0, 0, 0), // ERROR
    RGB(255, 255, 255), // HIGHEST
    RGB(255, 255, 255) // OFF
};

namespace tracer
{

MainDlg::MainDlg(CWnd* pParent, Protocol proto, uint16_t port)
    : ResizableDialog(IDD_SHADOW_DIALOG, pParent)
    , m_proto(proto)
    , m_port(port)
    , m_hIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME))
{

}

void MainDlg::DoDataExchange(CDataExchange* pDX)
{
    ResizableDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_MESSAGE_VIEW, m_messageView);
    DDX_Control(pDX, IDC_AUTOSCROLL, m_autoScrollBtn);
    DDX_Control(pDX, IDC_FIND_TEXT, m_findBox);
}

BEGIN_MESSAGE_MAP(MainDlg, ResizableDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
    ON_BN_CLICKED(IDC_CLEAR, &MainDlg::OnBnClickedClear)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_MESSAGE_VIEW, OnGetdispinfoList)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_MESSAGE_VIEW, OnCustomdrawList)
    ON_BN_CLICKED(IDC_AUTOSCROLL, &MainDlg::OnBnClickedAutoscroll)
    ON_BN_CLICKED(IDC_FIND, &MainDlg::OnBnClickedFind)
    ON_WM_DESTROY()
    ON_MESSAGE(SH_NEW_RECORD, &MainDlg::OnNewRecord)
    ON_BN_CLICKED(IDC_FILTERS, &MainDlg::OnBnClickedFilters)
    ON_COMMAND(ID_COPY_ROW, &MainDlg::OnCopyRows)
    ON_COMMAND(ID_SELECT_ALL, &MainDlg::OnSelectAll)
END_MESSAGE_MAP()


void MainDlg::OnCopyRows()
{
    POSITION pos = m_messageView.GetFirstSelectedItemPosition();
    if (!pos)
    {
        return;
    }

    CStringW text;

    while (pos)
    {
        int i = m_messageView.GetNextSelectedItem(pos);

        const auto& r = m_database[i];
        auto s = recordToString(r);

        text.Append(s);
        text.Append(L"\n");
    }

    OpenClipboard();
    EmptyClipboard();

    size_t required = (text.GetLength() + 1) * sizeof(wchar_t);
    auto handle = ::GlobalAlloc(GMEM_MOVEABLE, required);
    auto ptr = static_cast<wchar_t*>(::GlobalLock(handle));
    memcpy(ptr, text, required);
    ::GlobalUnlock(handle);

    SetClipboardData(CF_UNICODETEXT, handle);

    CloseClipboard();
}

static inline int clampedSubtract(int x, int s, int min)
{
    x -= s;
    if (x < min) x = min;
    return x;
}

void MainDlg::reloadFilters()
{
    auto app = AfxGetApp();

    m_filterDebug = app->GetProfileIntW(L"filter", L"debug", 1) != 0;
    m_filterInfo = app->GetProfileIntW(L"filter", L"info", 1) != 0;
    m_filterWarn = app->GetProfileIntW(L"filter", L"warn", 1) != 0;
    m_filterError = app->GetProfileIntW(L"filter", L"error", 1) != 0;
    m_filterHighest = app->GetProfileIntW(L"filter", L"highest", 1) != 0;

    m_filterComponent = app->GetProfileStringW(L"filter", L"component", L"*");
}

BOOL MainDlg::OnInitDialog()
{
    sketch::log::registerSink("Local", LocalSink::make(this), sketch::log::NullTraceFormatter::make(), sketch::log::DefaultTraceFilter::make(sketch::log::Debug));

    ResizableDialog::OnInitDialog();

    reloadFilters();

    try
    {
        if (m_proto == Protocol::Tcp)
        {
            m_serverTcp.reset(new tracer::TcpServer(m_port, this));
        }
        else if (m_proto == Protocol::Udp)
        {
            m_serverUdp.reset(new tracer::UdpServer(m_port, this));
        }
    }
    catch (sketch::Exception& e)
    {
        auto inner = e.error();
        auto f = inner->format(u"Failed to start the server: {MS} {FL}");

        auto l = sketch::toSystemLocale(f);
        SketchError(l.str());

        AfxMessageBox(f.utf16(), MB_ICONERROR | MB_OK);

        return FALSE;
    }

    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    AutoMove(IDC_MESSAGE_VIEW, 0, 0, 100, 100);
    AutoMove(IDC_AUTOSCROLL, 0, 100, 0, 0);
    AutoMove(IDC_CLEAR, 100, 100, 0, 0);
    AutoMove(IDC_FILTERS, 100, 100, 0, 0);

    RECT rc;
    m_messageView.GetClientRect(&rc);
    int w = rc.right - rc.left;
    m_messageView.InsertColumn(0, _T("Time"), LVCFMT_LEFT, 100);
    w = clampedSubtract(w, 100, 50);

    m_messageView.InsertColumn(1, _T("Level"), LVCFMT_LEFT, 100);
    w = clampedSubtract(w, 100, 50);

    m_messageView.InsertColumn(2, _T("Component"), LVCFMT_LEFT, 100);
    w = clampedSubtract(w, 100, 50);

    m_messageView.InsertColumn(3, _T("Context"), LVCFMT_LEFT, 130);
    w = clampedSubtract(w, 130, 50);

    m_messageView.InsertColumn(4, _T("Text"), LVCFMT_LEFT, w);

    ListView_SetExtendedListViewStyle(m_messageView.m_hWnd, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    m_autoScrollBtn.SetCheck(m_autoScroll ? BST_CHECKED : BST_UNCHECKED);

    restoreSettings();

    SketchInfo("Listening on %s port %d", (m_proto == Protocol::Tcp ? "TCP" : "UDP"), m_port);

    return TRUE;
}

void MainDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        ResizableDialog::OnPaint();
    }
}

HCURSOR MainDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void MainDlg::OnSize(UINT nType, int cx, int cy)
{
    ResizableDialog::OnSize(nType, cx, cy);
}

void MainDlg::OnGetMinMaxInfo(MINMAXINFO* pMMI)
{
    pMMI->ptMinTrackSize.x = 300;
    pMMI->ptMinTrackSize.y = 300;
}

void MainDlg::OnBnClickedClear()
{
    m_database.clear();

    m_messageView.SetItemCount(0);
}

CStringW MainDlg::recordToString(const protocol::Record* r)
{
    CStringW result;

    {
        result.Append(r->time);
        result.Append(L" | ");
    }

    {
        auto lvl = r->level;
        result.Append(gLevels[lvl]);
        result.Append(L" | ");
    }

    {
        result.Append(r->module);
        result.Append(L" | ");
    }

    {
        if (r->irql != -1)
        {
            CStringW s;
            s.Format(L"%d:%d@%d", r->pid, r->tid, r->irql);
            result.Append(s);
        }
        else
        {
            CStringW s;
            s.Format(L"%d:%d", r->pid, r->tid);
            result.Append(s);
        }

        result.Append(L" | ");
    }

    {
        result.Append(r->text);
    }

    return result;
}

void MainDlg::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult)
{
    auto pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
    auto pItem = &(pDispInfo)->item;

    auto itemid = pItem->iItem;

    if (pItem->mask & LVIF_TEXT)
    {
        CString text;

        if (pItem->iSubItem == 0) // [time]
        {
            text = m_database[itemid]->time;
        }
        else if (pItem->iSubItem == 1) // [level]
        {
            auto lvl = m_database[itemid]->level;
            text = gLevels[lvl];
        }
        else if (pItem->iSubItem == 2) // [component]
        {
            text = m_database[itemid]->module;
        }
        else if (pItem->iSubItem == 3) // [context]
        {
            if (m_database[itemid]->irql != -1)
            {
                text.Format(L"%d:%d@%d", m_database[itemid]->pid, m_database[itemid]->tid, m_database[itemid]->irql);
            }
            else
            {
                text.Format(L"%d:%d", m_database[itemid]->pid, m_database[itemid]->tid);
            }
        }
        else if (pItem->iSubItem == 4) // [text]
        {
            text = m_database[itemid]->text;

            if (text.Find(L'\t') != -1)
                text.Replace(L"\t", L"   ");
        }

        ::lstrcpynW(pItem->pszText, text, pItem->cchTextMax);
    }

    *pResult = 0;
}

void MainDlg::OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

    // Take the default processing unless we
    // set this to something else below.
    *pResult = CDRF_DODEFAULT;

    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
    if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        // This is the prepaint stage for an item. Here's where we set the
        // item's text color. Our return value will tell Windows to draw the
        // item itself, but it will use the new color we set here.
        // We'll cycle the colors through red, green, and light blue.

        auto id = pLVCD->nmcd.dwItemSpec;
        pLVCD->clrText = gTextColors[m_database[id]->level];
        pLVCD->clrTextBk = gBackColors[m_database[id]->level];

        // Tell Windows to paint the control itself.
        *pResult = CDRF_DODEFAULT;
    }
}

void MainDlg::OnBnClickedAutoscroll()
{
    m_autoScroll = (m_autoScrollBtn.GetCheck() == BST_CHECKED);

    captureSettings();
}

void MainDlg::OnBnClickedFind()
{
    CString text;
    m_findBox.GetWindowTextW(text);

    if (m_lastSearch != text)
    {
        m_lastFound = -1;
        m_lastSearch = text;
    }

    if (text.IsEmpty())
    {
        return;
    }

    auto find = text;
    find.MakeUpper();
    bool found = false;
    for (auto i = ((m_lastFound >= 0) ? (m_lastFound + 1) : 0); i < int(m_database.size()); i++)
    {
        auto s = m_database[i]->text;
        s.MakeUpper();
        if (s.Find(find) >= 0)
        {
            m_messageView.SetItemState(-1, 0, LVIS_SELECTED);
            m_messageView.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
            m_messageView.EnsureVisible(i, FALSE);
            m_messageView.SetFocus();

            m_lastFound = i;
            found = true;
            break;
        }
    }

    if (!found)
    {
        m_lastFound = -1;
        m_lastSearch.Empty();

        CString s;
        s.Format(L"No \"%s\" found", text);
        AfxMessageBox(s, MB_ICONEXCLAMATION | MB_OK);
    }
}

void MainDlg::OnSelectAll()
{
    m_messageView.SetItemState(-1, LVIS_SELECTED, LVIS_SELECTED);
}

void MainDlg::OnDestroy()
{
    m_serverTcp.reset();
    m_serverUdp.reset();

    captureSettings();

    sketch::log::unregisterSink("Local");

    ResizableDialog::OnDestroy();
}

void MainDlg::captureSettings()
{
    auto app = AfxGetApp();

    {
        RECT rc;
        GetWindowRect(&rc);
        app->WriteProfileInt(L"window", L"x", rc.left);
        app->WriteProfileInt(L"window", L"y", rc.top);
        app->WriteProfileInt(L"window", L"w", rc.right - rc.left);
        app->WriteProfileInt(L"window", L"h", rc.bottom - rc.top);
    }

    {
        app->WriteProfileInt(L"ui", L"autoscroll", m_autoScroll ? 1 : 0);
    }
}

void MainDlg::restoreSettings()
{
    auto app = AfxGetApp();

    {
        int x = app->GetProfileIntW(L"window", L"x", 100);
        int y = app->GetProfileIntW(L"window", L"y", 100);
        int w = app->GetProfileIntW(L"window", L"w", 1024);
        int h = app->GetProfileIntW(L"window", L"h", 768);

        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (w < 320) w = 320;
        if (h < 200) h = 200;

        MoveWindow(x, y, w, h, FALSE);
    }

    {
        m_autoScroll = (app->GetProfileIntW(L"ui", L"autoscroll", 1) != 0);
        m_autoScrollBtn.SetCheck(m_autoScroll ? BST_CHECKED : BST_UNCHECKED);
    }
}

bool MainDlg::filterMessage(const protocol::Record* r) const
{
    if (r->level == protocol::Debug && !m_filterDebug) return false;
    if (r->level == protocol::Info && !m_filterInfo) return false;
    if (r->level == protocol::Warning && !m_filterWarn) return false;
    if (r->level == protocol::Error && !m_filterError) return false;
    if (r->level == protocol::Highest && !m_filterHighest) return false;

    if (m_filterComponent.IsEmpty() || m_filterComponent == L"*") return true;
    if (m_filterComponent.CompareNoCase(r->module) != 0) return false;

    return true;
}

void MainDlg::message(protocol::Record::Ref r)
{
    sketch::Mutex::Guard lk(m_lock);

    bool wasEmpty = m_queue.empty();
    m_queue.push_back(std::move(r));
    if (wasEmpty)
    {
        PostMessageW(SH_NEW_RECORD, 0, 0);
    }
}

void MainDlg::reapplyFilters()
{
    m_messageView.SetItemCount(0);

    m_database.clear();

    for (const auto& r : m_databaseUnfiltered)
    {
        if (filterMessage(r.get()))
        {
            m_database.push_back(r.get());
        }
    }

    m_messageView.SetItemCount(int(m_database.size()));

    if (m_autoScroll)
    {
        m_messageView.EnsureVisible(int(m_database.size()) - 1, FALSE);
    }
}

void MainDlg::saveSelection()
{
    for (auto& r : m_databaseUnfiltered)
    {
        r->selected = false;
    }

    auto n = m_database.size();
    for (size_t i = 0; i < n; i++)
    {
        bool selected = !!m_messageView.GetItemState(int(i), LVIS_SELECTED);
        m_database[i]->selected = selected;
    }
}

void MainDlg::restoreSelection()
{
    // first, remove the existing selection
    m_messageView.SetItemState(-1, 0, LVIS_SELECTED);

    // and then restore based on the saved states
    auto n = m_database.size();
    for (size_t i = 0; i < n; i++)
    {
        if (m_database[i]->selected)
        {
            m_messageView.SetItemState(int(i), LVIS_SELECTED, LVIS_SELECTED);
        }
    }
}

LRESULT MainDlg::OnNewRecord(WPARAM wParam, LPARAM lParam)
{
    wParam;
    lParam;

    sketch::Mutex::Guard lk(m_lock);

    while (!m_queue.empty())
    {
        auto r = std::move(m_queue.front());
        m_queue.pop_front();

        auto p = r.get();
        m_databaseUnfiltered.push_back(std::move(r));
        if (filterMessage(p))
        {
            m_database.push_back(p);
        }
    }

    // have we got too much records to keep in memory?
    bool discareded = false;
    if (m_databaseUnfiltered.size() > SH_MAX_RECORDS)
    {
        saveSelection();

        // discard oldest records
        size_t discardedCount = 0;
        auto iUnfiltered = m_databaseUnfiltered.begin();
        auto iFiltered = m_database.begin();
        while (discardedCount < SH_RECORDS_TO_DISCARD)
        {
            SK_ASSERT(iUnfiltered != m_databaseUnfiltered.end());
            if (filterMessage(iUnfiltered->get()))
            {
                discardedCount++;
                m_database.erase(iFiltered);
                iFiltered = m_database.begin();
            }

            m_databaseUnfiltered.erase(iUnfiltered);
            iUnfiltered = m_databaseUnfiltered.begin();
        }

        discareded = true;
    }

    auto newCount = m_database.size();

    m_messageView.SetItemCountEx(int(newCount), LVSICF_NOSCROLL);

    if (m_autoScroll)
    {
        m_messageView.EnsureVisible(int(newCount) - 1, FALSE);
    }

    if (discareded)
    {
        restoreSelection();
    }

    return 0;
}

void MainDlg::OnBnClickedFilters()
{
    FilterDlg dlg(this);
    if (dlg.DoModal() != IDOK)
    {
        return;
    }

    reloadFilters();
    reapplyFilters();
}

} // namespace tracer {}
