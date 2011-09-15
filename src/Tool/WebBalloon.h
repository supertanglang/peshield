// WebBalloon.h: interface for the CWebBalloon class.
//
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------
//			  P  E  S  H  I  E  L  D       A  G  E  N  T
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: HAM DON WOOK
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------


#if !defined(gS_WebBalloon_DEFINED)
#define gS_WebBalloon_DEFINED

#include <afxtempl.h>
#include "BalloonWnd.h"
#include "AgentButton.h"
#include "AgentEdit2.h"

typedef long (__stdcall *pWebNoModal) (long, long, long);

#define BI_CHECKBOX		0x0001
#define BI_CHECKBOXSEL	0x0002
#define BI_TEXTBOX		0x0004
#define BI_SINGLELINE	0x0008

#define BI_RADIORETURN	0x0010
#define BI_CHECKRETURN	0x0020

#define IDC_TEXTBOX		101
#define IDC_CHECKBOX	110
#define IDC_BUTTONS		120
#define IDC_RADIOS		160

class CWebBalloon : public CBalloonWnd  
{
public:
	void Move ( RECT rectParent );
	long Show ( HWND hParent, DWORD dwFlags = 0 );
	void SetPosition( DWORD dwFlags = 0 );
	void CleanUp();
	BOOL SetUpControls ( int nButtons,  int nRadios = 0, DWORD dwFlags = 0 );
	CWebBalloon();
	virtual ~CWebBalloon();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWebBalloon)
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg LRESULT OnChild(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	long GetCheckedRadio();
	BOOL GetCheckBoxState();
	CString GetTextBox();
	BOOL SetButtonText( int nIndex, CString strText );
	BOOL SetRadioText( int nIndex, CString strText );
	BOOL SetCheckBoxText( CString strText );

	CFont m_cTitleFont, m_cTextFont;
	CString m_strTitle, m_strText;
	HICON m_hIcon;
	int m_nTextBoxHeight;

	pWebNoModal WebNoModal; // Callback function
	long m_lExtra, m_lExtra2;
private:
	HWND m_hParent;
	RECT m_rectParent;

	CArray <CAgentButton*, CAgentButton*> m_arButtons;	// Buttons
	CArray <CAgentButton*, CAgentButton*> m_arRadios;	// Radios
	CAgentEdit2* m_pEdit;								// Edit
	CAgentButton *m_pCheckBox;							// Checkbox

	int m_nButtons, m_nRadios;
	DWORD m_dwFlags;
};

#endif // !defined(gS_WebBalloon_DEFINED)
