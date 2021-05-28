#include "stdafx.h"
#include "Tracer.h"
#include "FilterDlg.h"
#include "afxdialogex.h"
#include "resource.h"

namespace tracer
{

IMPLEMENT_DYNAMIC(FilterDlg, CDialogEx)

FilterDlg::FilterDlg(CWnd* pParent)
	: CDialogEx(IDD_FILTERS, pParent)
{

}

FilterDlg::~FilterDlg()
{
}

void FilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_FILTER_DEBUG, m_btnDebug);
  DDX_Control(pDX, IDC_FILTER_TRACE, m_btnInfo);
  DDX_Control(pDX, IDC_FILTER_WARN, m_btnWarn);
  DDX_Control(pDX, IDC_FILTER_ERROR, m_btnError);
  DDX_Control(pDX, IDC_FILTER_INIFIN, m_btnHighest);

  DDX_Control(pDX, IDC_COMPONENT_NAME, m_editComponent);
}


BEGIN_MESSAGE_MAP(FilterDlg, CDialogEx)
  ON_BN_CLICKED(IDOK, &FilterDlg::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL FilterDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  auto app = AfxGetApp();

  m_btnDebug.SetCheck(app->GetProfileIntW(L"filter", L"debug", 1) ? BST_CHECKED : BST_UNCHECKED);
  m_btnInfo.SetCheck(app->GetProfileIntW(L"filter", L"info", 1) ? BST_CHECKED : BST_UNCHECKED);
  m_btnWarn.SetCheck(app->GetProfileIntW(L"filter", L"warn", 1) ? BST_CHECKED : BST_UNCHECKED);
  m_btnError.SetCheck(app->GetProfileIntW(L"filter", L"error", 1) ? BST_CHECKED : BST_UNCHECKED);
  m_btnHighest.SetCheck(app->GetProfileIntW(L"filter", L"highest", 1) ? BST_CHECKED : BST_UNCHECKED);

  m_editComponent.SetWindowTextW(app->GetProfileStringW(L"filter", L"component", L"*"));

  return TRUE;
}

void FilterDlg::OnBnClickedOk()
{
  auto app = AfxGetApp();

  app->WriteProfileInt(L"filter", L"debug", (m_btnDebug.GetCheck() == BST_CHECKED) ? 1 : 0);
  app->WriteProfileInt(L"filter", L"info", (m_btnInfo.GetCheck() == BST_CHECKED) ? 1 : 0);
  app->WriteProfileInt(L"filter", L"warn", (m_btnWarn.GetCheck() == BST_CHECKED) ? 1 : 0);
  app->WriteProfileInt(L"filter", L"error", (m_btnError.GetCheck() == BST_CHECKED) ? 1 : 0);
  app->WriteProfileInt(L"filter", L"highest", (m_btnHighest.GetCheck() == BST_CHECKED) ? 1 : 0);

  CString s;
  m_editComponent.GetWindowTextW(s);
  app->WriteProfileStringW(L"filter", L"component", s);

  CDialogEx::OnOK();
}

} // namespace tracer {}
