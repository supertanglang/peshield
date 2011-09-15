#if !defined(AFX_AGENTBUTTON_H__81F452ED_CE5A_408A_B51D_D695A43F41FF__INCLUDED_)
#define AFX_AGENTBUTTON_H__81F452ED_CE5A_408A_B51D_D695A43F41FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgentButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAgentButton window

//----------------------------------------------------------------------
//			  P  E  S  H  I  E  L  D       A  G  E  N  T
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: HAM DON WOOK
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------


class CAgentButton : public CButton
{
// Construction
public:
	CAgentButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgentButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgentButton();

	// Generated message map functions
protected:
	void SendNotify( WPARAM wParam, LPARAM lParam );
	//{{AFX_MSG(CAgentButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnBMSetCheck(WPARAM, LPARAM);
	afx_msg LRESULT OnBMGetCheck(WPARAM, LPARAM);
	afx_msg LRESULT OnUnCheck(WPARAM, LPARAM);
	afx_msg void OnClicked();
	DECLARE_MESSAGE_MAP()
public:
	void AutoSize();
	long m_lMargin, m_lTextMargin;
	void Init( UINT nIDResource, int nImages, BOOL bAutoSize = FALSE );
	COLORREF m_crBackground, m_crText;
private:
	void DrawState ( CDC* pDC, long x, long y, int nState );
	BOOL m_bMouseDown;
	BOOL m_bTracking;
	BOOL m_bChecked;
	int m_nImages;

	DWORD m_dwStyle;
	CBitmap m_cBitmap;
	CDC m_dcBitmap;
	BITMAP m_bitmapInfo;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGENTBUTTON_H__81F452ED_CE5A_408A_B51D_D695A43F41FF__INCLUDED_)
