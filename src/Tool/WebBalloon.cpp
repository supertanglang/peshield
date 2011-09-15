// WebBalloon.cpp: implementation of the CWebBalloon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "WebBalloon.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define WM_SENDNOTIFY ( WM_APP + 2 )
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWebBalloon::CWebBalloon()
{
	m_pCheckBox = NULL;
	m_pEdit = NULL;
	m_nTextBoxHeight = 20;
	m_lExtra = m_lExtra2 = 0;
	m_hIcon = NULL;

	m_hParent = NULL;
	
	// Convert pixel height into device unit height
	int nHeight = -MulDiv( 10, GetDeviceCaps( GetDesktopWindow()->GetDC()->m_hDC, LOGPIXELSY), 72);
	
	m_cTitleFont.CreateFont( nHeight, 0, 0, 0, FW_BOLD, 0, 0, 0, 
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH, "");

	m_cTextFont.CreateFont( nHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0, 
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH, "");
}

CWebBalloon::~CWebBalloon()
{
}

BEGIN_MESSAGE_MAP(CWebBalloon, CBalloonWnd)
	//{{AFX_MSG_MAP(CWebBalloon)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_MESSAGE( WM_SENDNOTIFY , OnChild)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebBalloon message handlers


void CWebBalloon::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rectClient;
	GetClientRect ( &rectClient );
	
	// Draw text
	SetRect ( &rectClient, 0, 0, rectClient.Width(), 10000 );
	
	dc.SetBkMode( TRANSPARENT );
	dc.SelectObject( m_cTextFont );

	// Draw icon
	if ( m_hIcon )
	{
		::DrawIconEx( dc.m_hDC, 0, 0, m_hIcon, 32, 32, 0, NULL, DI_NORMAL );
		rectClient.left = 36;
	}

	// Draw title
	if ( m_strTitle != "" )
	{
		CFont* pOld = dc.SelectObject( &m_cTitleFont );

		rectClient.left = ( m_hIcon ? 36 : 0);
		rectClient.bottom = 10000;

		rectClient.top = dc.DrawText( m_strTitle, &rectClient, DT_WORDBREAK );
		
		dc.SelectObject( pOld );
		rectClient.top += 4;	// Margin
	}
	rectClient.left = 0;

	if ( m_hIcon )
	{
		if ( rectClient.top < 36 )
			rectClient.top = 36;
	}

	// Draw text
	if ( m_strText != "" )
	{
		CFont* pOld = dc.SelectObject( &m_cTextFont );

		rectClient.top = dc.DrawText( m_strText, &rectClient, DT_WORDBREAK );
	
		dc.SelectObject( pOld );

		rectClient.top += 4;	// Margin
	}
}

BOOL CWebBalloon::SetUpControls(int nButtons, int nRadios, DWORD dwFlags )
{
	CleanUp();
	CAgentButton* pButton = NULL;
	int nMove;
	CRect rectClient;
	CString strCaption;
	
	// TextBox
	if ( dwFlags & BI_TEXTBOX )
	{
		m_pEdit = new CAgentEdit2;
		if ( !m_pEdit->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL |
			ES_MULTILINE, CRect (0, 0, 150, 20), this, IDC_TEXTBOX ) )
		{
			// An error
			m_pEdit->DestroyWindow();
			delete m_pEdit;
			return FALSE;
		}
		m_pEdit->SetFont( &m_cTextFont );
	}
	
	// Radio
	for (int i=0; i< nRadios; i++)
	{
		strCaption.Format( "Radio%d", i );	// Window caption

		pButton = new CAgentButton;	// Alloc

		// Create, init, etc.
		if ( !pButton->Create( strCaption, WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_AUTORADIOBUTTON | BS_TEXT |
			( i == 0 ? BS_NOTIFY | WS_GROUP : 0),
			CRect( 0, 0, 150, 10), this, IDC_RADIOS + i) )
		{
			// An error: delete CAgentButton
			pButton->DestroyWindow();
			delete pButton;
			CleanUp();
			return FALSE;
		}

		pButton->Init( IDB_RADIO, 4 );	// Load bitmaps
		pButton->SetFont( &m_cTextFont );

		// Add pointer to the array
		m_arRadios.Add( pButton );
	}

	// Checkbox
	if ( dwFlags & BI_CHECKBOX )
	{
		m_pCheckBox = new CAgentButton;

		// Create, init, etc.
		if ( !m_pCheckBox->Create( "자동화 : 웹주소를 입력해 주세요~", WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_AUTORADIOBUTTON,
			CRect( 0, 00, 150, 10), this, IDC_CHECKBOX ) )
		{
			m_pCheckBox->DestroyWindow();
			delete m_pCheckBox;
			CleanUp();
			return FALSE;
		}
		
		m_pCheckBox->ModifyStyleEx( 0, WS_EX_NOPARENTNOTIFY );
		m_pCheckBox->Init( IDB_CHECKBOX, 4 );	// Load bitmaps
		m_pCheckBox->SetFont( &m_cTextFont );

		if ( dwFlags & BI_CHECKBOXSEL )
			m_pCheckBox->SetCheck( 1 );
	}

	nMove = 0;

	// Buttons
	for ( int j=0; j< nButtons; j++)
	{
		CString strTitle[] = {_T("확인"),_T("취소")};	// Window caption
		pButton = new CAgentButton;	// Alloc

		// Create, init, etc.
		if ( !pButton->Create( strTitle[j], WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_TEXT,
			CRect( nMove, 20, 100, 100), this, IDC_BUTTONS + j) )
		{
			// An error: delete CAgentButton
			pButton->DestroyWindow();
			delete pButton;
			CleanUp();
			return FALSE;
		}

		pButton->ModifyStyleEx( WS_EX_NOPARENTNOTIFY, 0 );
		pButton->Init( IDB_BUTTON, 3, TRUE );	// Load bitmaps
		pButton->SetFont( &m_cTextFont );

		// Calc position
		pButton->GetWindowRect( &rectClient );
		ScreenToClient( rectClient );
		nMove = rectClient.left + rectClient.Width() + 5;

		// Add pointer to the array
		m_arButtons.Add( pButton );
	}

	GetClientRect ( &rectClient );
	ClientSize( 200, 150 );
	return TRUE;	
}

void CWebBalloon::CleanUp()
{
	CAgentButton* pButton = NULL;

	// Go through the array and destroy all controls
	while ( m_arRadios.GetSize() > 0 )
	{
		pButton = m_arRadios.GetAt( 0 );
		pButton->DestroyWindow();
		delete pButton;

		m_arRadios.RemoveAt ( 0 );
	}

	while ( m_arButtons.GetSize() > 0 )
	{
		pButton = m_arButtons.GetAt( 0 );
		pButton->DestroyWindow();
		delete pButton;

		m_arButtons.RemoveAt ( 0 );
	}

	if ( m_pCheckBox )
	{
		m_pCheckBox->DestroyWindow();
		delete m_pCheckBox;
		m_pCheckBox = NULL;
	}

	if ( m_pEdit )
	{
		m_pEdit->DestroyWindow();
		delete m_pEdit;
		m_pEdit = NULL;
	}
}

// Move controls to correct position
void CWebBalloon::SetPosition( DWORD dwFlags )
{
	CPaintDC dc(this);

	const int nDefaultWidth = 200;
	int nButtonsWidth = 0, nWidth;
	CRect rectWindow, rectClient, rectDraw;

	// Set rects
	rectClient.SetRect( 0, 6, 200, 10000 );

	// Calc width
	if ( m_arButtons.GetSize() > 0 )
	{
		m_arButtons[m_arButtons.GetSize()-1]->GetWindowRect( &rectWindow );
		ScreenToClient( &rectWindow );
		nButtonsWidth = rectWindow.right;
	}


	nDefaultWidth > nButtonsWidth ? nWidth = nDefaultWidth : nWidth = nButtonsWidth; 


	// Draw icon
	if ( m_hIcon )
		::DrawIconEx( dc.m_hDC, 0, 0, m_hIcon, 32, 32, 0, NULL, DI_NORMAL );

	// Draw title
	if ( m_strTitle != "" )
	{
		CFont* pOld = dc.SelectObject( &m_cTitleFont );	// Select font

		rectDraw.SetRect( ( m_hIcon ? 36 : 0), 0, nWidth, 0 );
		dc.DrawText( m_strTitle, &rectDraw, DT_CALCRECT | ( ( dwFlags & BI_SINGLELINE ) ? DT_SINGLELINE : DT_WORDBREAK ) );

		dc.SelectObject( pOld );						// Bring back old font
		rectClient.top += rectDraw.Height() + 6;		// Add margin
		if ( rectClient.right < rectDraw.right ) rectClient.right = rectDraw.right;
	}

	if ( m_hIcon && rectClient.top < 36)
			rectClient.top = 36;

	// Draw text
	if ( m_strText != "" )
	{
		CFont* pOld = dc.SelectObject( &m_cTextFont ); // Select font

		rectDraw.SetRect( 0, rectClient.top, nWidth, 0 );
		dc.DrawText( m_strText, &rectDraw, DT_CALCRECT | ( ( dwFlags & BI_SINGLELINE ) ? DT_SINGLELINE : DT_WORDBREAK ) );
	
		dc.SelectObject( pOld );	// Bring back font
		rectClient.top += rectDraw.Height() + 6;		// Add margin
		if ( rectClient.right < rectDraw.right ) rectClient.right = rectDraw.right;
	}

	if ( rectClient.right < nButtonsWidth ) rectClient.right = nButtonsWidth;


	// Radios
	for (int i=0; i<m_arRadios.GetSize(); i++)
	{
		// Move radio
		m_arRadios[i]->SetWindowPos( NULL, rectClient.top, rectClient.top, rectClient.right, 0, SWP_NOZORDER );
		m_arRadios[i]->AutoSize();
		
		// Calc next pos
		m_arRadios[i]->GetWindowRect( &rectWindow );
		ScreenToClient( &rectWindow );
		rectClient.top = rectWindow.bottom;

	}
	if ( m_arRadios.GetSize() > 0 )
		rectClient.top += 6;		// Add margin

	// CheckBox
	if ( m_pCheckBox )
	{
		// Move CheckBox
		m_pCheckBox->SetWindowPos( NULL, 0, rectClient.top , rectClient.right, 0, SWP_NOZORDER );
		
		m_pCheckBox->AutoSize();

		// Calc next pos
		m_pCheckBox->GetWindowRect( &rectWindow );
		ScreenToClient( &rectWindow );
		rectClient.top = rectWindow.bottom + 6;	// Add margin
	}
	
	// Text
	if ( m_pEdit )
	{
		m_pEdit->SetWindowPos( NULL, 0, rectClient.top, rectClient.right, m_nTextBoxHeight, SWP_NOZORDER );

		// Calc next pos
		m_pEdit->GetWindowRect( &rectWindow );
		ScreenToClient( &rectWindow );
		rectClient.top = rectWindow.bottom + 6; // Add margin
	}

	// Buttons
	for (int j=0; j<m_arButtons.GetSize(); j++)
	{
		// Move button
		m_arButtons[j]->GetWindowRect( &rectWindow );
		ScreenToClient( &rectWindow );

		m_arButtons[j]->SetWindowPos( NULL, rectWindow.left, rectClient.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER );

		// We don't need to calc pos because all buttons are on the same height
	}
	if ( m_arButtons.GetSize() > 0 )
		rectClient.top += rectWindow.Height();

	ClientSize( rectClient.right, rectClient.top );	
}

long CWebBalloon::Show ( HWND hParent, DWORD dwFlags )
{
	int nRet = -1;
	// First calc position
	m_pEdit->SetWindowText(TOOL_DEF_WEB);
	CalcAndMove( m_rectParent );
	m_pEdit->SetSel(7,7);
	m_pEdit->SetFocus();

	::EnableWindow( m_hParent, TRUE );
	::EnableWindow( hParent, FALSE );
	m_hParent = hParent;
	m_dwFlags = dwFlags;

	// Show
	ShowWindow( SW_SHOW );
	if ( hParent )
		nRet = RunModalLoop( );

	// Enable parent
	::EnableWindow( hParent, TRUE );
	::BringWindowToTop( hParent );
	return nRet;
}

void CWebBalloon::Move ( RECT rectParent )
{
	m_rectParent = rectParent;
	CalcAndMove( rectParent );
}

BOOL CWebBalloon::SetButtonText(int nIndex, CString strText)
{
	if ( nIndex < 0 || ( m_arButtons.GetSize() - 1 ) < nIndex )
		return FALSE;

	// The button exists
	m_arButtons[nIndex]->SetWindowText( strText );
	return TRUE;
}

BOOL CWebBalloon::SetRadioText( int nIndex, CString strText )
{
	if ( nIndex < 0 || ( m_arRadios.GetSize() - 1 ) < nIndex )
		return FALSE;

	// The Radio exists
	m_arRadios[nIndex]->SetWindowText( strText );
	return TRUE;
}

BOOL CWebBalloon::SetCheckBoxText( CString strText )
{
	if ( !m_pCheckBox ) return FALSE;

	// The button exists
	m_pCheckBox->SetWindowText( strText );
	return TRUE;
}

void CWebBalloon::OnClose() 
{
	// NOTE: if you want to close the window/destroy you must call DestroyWindow();
	ShowWindow ( SW_HIDE );

	if ( ContinueModal() )
		EndModalLoop( 0 );
	
	// Call callback function
	if ( WebNoModal )
		WebNoModal ( -1, m_lExtra, m_lExtra2 );
}
LRESULT CWebBalloon::OnChild(WPARAM wParam, LPARAM lParam)
{
	DWORD dwStyle = GetWindowLong( (HWND)lParam, GWL_USERDATA );
	UINT nID = GetWindowLong( (HWND)lParam, GWL_ID );

	if ( dwStyle == BS_PUSHBUTTON )
		goto CloseBalloon;
	else if ( dwStyle == BS_RADIOBUTTON  && ( m_dwFlags & BI_RADIORETURN ) )
		goto CloseBalloon;
	else if ( dwStyle == BS_CHECKBOX && ( m_dwFlags & BI_CHECKRETURN ) )
		goto CloseBalloon;

	return 0;
CloseBalloon:
	// If we are here that means we should close the balloon
	ShowWindow( SW_HIDE );

	if ( ContinueModal() )
		EndModalLoop( nID );
	
	// Call callback function
	if ( WebNoModal )
		WebNoModal ( nID, m_lExtra, m_lExtra2 );

	return 0;
}

CString CWebBalloon::GetTextBox()
{
	CString strText;

	if ( m_pEdit )
		m_pEdit->GetWindowText( strText );

	return strText;
}

BOOL CWebBalloon::GetCheckBoxState()
{
	if ( m_pCheckBox )
		return m_pCheckBox->GetCheck();
	return -1;
}

long CWebBalloon::GetCheckedRadio()
{
	int nSize = m_arRadios.GetSize();
	int nSel = -1;
	if ( nSize > 0 )
	{
		nSel = GetCheckedRadioButton( IDC_RADIOS, IDC_RADIOS + nSize - 1 );
		if ( nSel != 0 )
			return nSel - IDC_RADIOS;
	}
	
	return -1;
}
