#if !defined(AFX_AGENTEDIT_H__76BE0E03_6A67_472B_9F4D_303F596B9C58__INCLUDED_)
#define AFX_AGENTEDIT_H__76BE0E03_6A67_472B_9F4D_303F596B9C58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgentEdit.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CAgentEdit window

//----------------------------------------------------------------------
//			  P  E  S  H  I  E  L  D       A  G  E  N  T
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: HAM DON WOOK
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

class CAgentEdit : public CEdit
{
// Construction
public:
	void SetLines ( int nLines, int nFontSize );
	CAgentEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgentEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgentEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAgentEdit)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BALLOONEDIT_H__FB1E7B89_29ED_4250_B3F0_82820073C1A6__INCLUDED_)
