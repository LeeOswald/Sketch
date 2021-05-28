#pragma once


namespace tracer
{

class FilterDlg
    : public CDialogEx
{
    DECLARE_DYNAMIC(FilterDlg)

public:
    FilterDlg(CWnd* pParent = nullptr);
    virtual ~FilterDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_FILTERS };
#endif

protected:
    CButton m_btnDebug;
    CButton m_btnInfo;
    CButton m_btnWarn;
    CButton m_btnError;
    CButton m_btnHighest;
    CEdit m_editComponent;

    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedOk();
};

} // namespace tracer {}
