#pragma once


#include <vector>

namespace tracer
{

class ResizableDialog
    : public CDialogEx
{
public:
    ResizableDialog(UINT nIDTemplate, CWnd* pParent = NULL);

    void AutoMove(int iID, double dXMovePct, double dYMovePct, double dXSizePct, double dYSizePct);

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()

public:
    bool m_bShowGripper;         // ignored if not WS_THICKFRAME

private:
    struct SMovingChild
    {
        HWND m_hWnd;
        double m_dXMoveFrac;
        double m_dYMoveFrac;
        double m_dXSizeFrac;
        double m_dYSizeFrac;
        CRect m_rcInitial;
    };

    typedef std::vector<SMovingChild> MovingChildren;

    MovingChildren m_MovingChildren;
    CSize m_szInitial;
    CSize m_szMinimum;
    HWND m_hGripper;
};

} // namespace tracer {}
