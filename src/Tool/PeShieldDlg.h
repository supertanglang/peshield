// PeShieldDlg.h : header file
//

#if !defined(AFX_PESHIELDDLG_H__929D4260_53E3_4EE6_A56E_7EF3696DD8FB__INCLUDED_)
#define AFX_PESHIELDDLG_H__929D4260_53E3_4EE6_A56E_7EF3696DD8FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Sink.h"
/////////////////////////////////////////////////////////////////////////////
// CPeShieldDlg dialog

class CPeShieldDlg : public CDialog
{
private:
	CAgent m_cAgent;
	CSink* m_pSink;
	BOOL	m_bIcon;

public:
	CSink*	GetSink() { return m_pSink; }
	CAgent*	GetAgent() { return &m_cAgent; }

// Construction
public:
	CPeShieldDlg(CWnd* pParent = NULL);	// standard constructor
	~CPeShieldDlg();
	void OptionBalloon();
	void OnEncode();
	void OnDecode();
	void Encrypt(CString strWeb);
	void WebBalloon();

private:

// Dialog Data
	//{{AFX_DATA(CPeShieldDlg)
	enum { IDD = IDD_PESHIELD_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPeShieldDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPeShieldDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PESHIELDDLG_H__929D4260_53E3_4EE6_A56E_7EF3696DD8FB__INCLUDED_)
