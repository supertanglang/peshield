#if !defined(AFX_AGENTEDIT2_H__87C66FC5_0EEC_4F5B_BEF8_0063C0070C3C__INCLUDED_)
#define AFX_AGENTEDIT2_H__87C66FC5_0EEC_4F5B_BEF8_0063C0070C3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgentEdit2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAgentEdit2 window

//----------------------------------------------------------------------
//			  P  E  S  H  I  E  L  D       A  G  E  N  T
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: HAM DON WOOK
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

class CAgentEdit2 : public CEdit
{
// Construction
public:
	void SetLines ( int nLines, int nFontSize );
	CAgentEdit2();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgentEdit2)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgentEdit2();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAgentEdit2)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGENTEDIT2_H__87C66FC5_0EEC_4F5B_BEF8_0063C0070C3C__INCLUDED_)
