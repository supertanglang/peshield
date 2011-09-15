// PeShield.h : main header file for the PESHIELD application
//

#if !defined(AFX_PESHIELD_H__1FEDDBA1_5B0D_439F_A8F9_B5F0BEE5F607__INCLUDED_)
#define AFX_PESHIELD_H__1FEDDBA1_5B0D_439F_A8F9_B5F0BEE5F607__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "PeShieldDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CPeShieldApp:
// See PeShield.cpp for the implementation of this class
//

class CPeShieldApp : public CWinApp
{
private:
	
public:
	CPeShieldApp();
	~CPeShieldApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPeShieldApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPeShieldApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

#define APP			((CPeShieldApp*)AfxGetApp())
#define MAINDLG		((CPeShieldDlg*)AfxGetMainWnd())

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PESHIELD_H__1FEDDBA1_5B0D_439F_A8F9_B5F0BEE5F607__INCLUDED_)
