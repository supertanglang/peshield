// AgentButton.cpp : implementation file
//

#include "StdAfx.h"
#include "AgentButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning ( disable : 4244 )

#define WM_UNCHECK ( WM_APP + 1 )
#define WM_SENDNOTIFY ( WM_APP + 2 )
/////////////////////////////////////////////////////////////////////////////
// CAgentButton

CAgentButton::CAgentButton()
{
	m_crBackground = GetSysColor( COLOR_INFOBK );
	m_crText= GetSysColor ( COLOR_BTNTEXT );
	
	m_bMouseDown = m_bTracking = m_bChecked = m_nImages = m_dwStyle = 0;
	m_lMargin = MAKELPARAM ( 2, 2 );
	m_lTextMargin = MAKELPARAM ( 2, 2 );
}

CAgentButton::~CAgentButton()
{
}


BEGIN_MESSAGE_MAP(CAgentButton, CButton)
//{{AFX_MSG_MAP(CAgentButton)
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_KILLFOCUS()
ON_WM_KEYDOWN()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONDBLCLK()
//}}AFX_MSG_MAP
ON_MESSAGE( WM_MOUSELEAVE, OnMouseLeave )
ON_MESSAGE( WM_UNCHECK , OnUnCheck )		// Uncheck message
ON_MESSAGE( BM_SETCHECK , OnBMSetCheck )
ON_MESSAGE( BM_GETCHECK , OnBMGetCheck )
ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgentButton message handlers


void CAgentButton::PreSubclassWindow() 
{
	m_dwStyle = GetButtonStyle();
	if ( ( m_dwStyle & BS_AUTOCHECKBOX ) == BS_AUTOCHECKBOX )
		m_dwStyle = BS_CHECKBOX;
	else if ( ( m_dwStyle & BS_AUTORADIOBUTTON ) == BS_AUTORADIOBUTTON )
		m_dwStyle = BS_RADIOBUTTON;
	else m_dwStyle = BS_PUSHBUTTON;
	
	SetWindowLong( m_hWnd, GWL_USERDATA, m_dwStyle );
	
	CButton::PreSubclassWindow();
	ModifyStyle(0, BS_OWNERDRAW);
}

void CAgentButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	//Check if the button state in not in inconsistent mode...
	POINT point;
	if ( m_bMouseDown && ::GetCapture() == m_hWnd && ::GetCursorPos( &point ) )
	{
		if ( ::WindowFromPoint( point ) == m_hWnd)
		{
			if ( (GetState() & BST_PUSHED) != BST_PUSHED)
			{
				//TRACE("* Inconsistency up detected! Fixing.\n");
				SetState(TRUE);
				return;
			}
		}
	}
	else 
	{
		if ( (GetState() & BST_PUSHED ) == BST_PUSHED )
		{
			//TRACE("* Inconsistency up detected! Fixing.\n");
			SetState(FALSE);
			return;
		}
	}
	
	// Obtain main info
	CString strCaption;
	CDC *pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	CRect rectClient = lpDrawItemStruct->rcItem;
	
	GetWindowText(strCaption);
	pDC->SetBkMode( TRANSPARENT );
	pDC->FillSolidRect( rectClient, m_crBackground );
	
	long x = ( m_dwStyle == BS_PUSHBUTTON ? 0 : HIWORD ( m_lMargin ) );
	long y = ( m_dwStyle == BS_PUSHBUTTON ? 0 : LOWORD ( m_lMargin ) );
	
	// Calc text rect
	CRect rectText;
	if ( m_dwStyle == BS_PUSHBUTTON )
		rectText = rectClient;
	else
	{
		rectText.left = m_bitmapInfo.bmWidth + HIWORD( m_lMargin ) + HIWORD( m_lTextMargin );
		rectText.top = LOWORD ( m_lTextMargin );
		rectText.right = rectClient.Width() - rectText.left - HIWORD ( m_lTextMargin );
		rectText.bottom = rectText.top + 10000;
	}
	
	if (lpDrawItemStruct->itemState & ODS_DISABLED) // DISABLED BUTTON
	{
		
	}
	else
	{
		if ( lpDrawItemStruct->itemState & ODS_SELECTED ) // SELECTED (DOWN) BUTTON
		{
			if ( m_dwStyle == BS_PUSHBUTTON ) rectText.OffsetRect( 1, 1 ); // Move text rect
			DrawState( pDC, x, y, 2 );
		}
		else if ( m_bChecked )
		{
			DrawState( pDC, x, y, 3 );
		}
		else
		{											
			if ( m_bTracking ) // TRACKING
			{
				//rectText.OffsetRect( -1, -1 );
				DrawState( pDC, x, y, 1 );
			}
			else
			{
				if ( (lpDrawItemStruct->itemState & ODS_FOCUS) ) // FOCUS BUTTON
				{
					DrawState( pDC, x, y, 1 );
					
				}
				else // DEFAULT BUTTON
				{
					DrawState( pDC, x, y, 0 );
				}
			}
		}
		// Draw text
		/*int nHeight = */pDC->DrawText( strCaption, rectText, m_dwStyle == BS_PUSHBUTTON ?
		DT_VCENTER | DT_CENTER | DT_SINGLELINE : DT_WORDBREAK );
	}
}

void CAgentButton::AutoSize()
{
	if (m_dwStyle == BS_PUSHBUTTON )
		SetWindowPos ( NULL, 0, 0, m_bitmapInfo.bmWidth, m_bitmapInfo.bmHeight / m_nImages , SWP_NOMOVE | SWP_NOZORDER );
	else
	{
		CPaintDC dc(this);
		CRect rectText, rectClient;
		CString strCaption;
		GetWindowText( strCaption );
		GetClientRect( &rectClient );
		
		rectText.left = m_bitmapInfo.bmWidth + HIWORD( m_lMargin ) + HIWORD( m_lTextMargin );
		rectText.top = LOWORD ( m_lTextMargin );
		rectText.right = rectClient.Width() - rectText.left - HIWORD ( m_lTextMargin );
		rectText.bottom = rectText.top + 10000;
		
		int nHeight = dc.DrawText( strCaption, rectText, DT_WORDBREAK );
		
		if ( nHeight != rectClient.Height() )
			SetWindowPos ( NULL, 0, 0, rectClient.Width(), nHeight + LOWORD (m_lTextMargin) * 2, SWP_NOMOVE | SWP_NOZORDER );
	}
	
}

void CAgentButton::DrawState( CDC* pDC, long x, long y, int nState )
{
	pDC->BitBlt( x, y, m_bitmapInfo.bmWidth, m_bitmapInfo.bmHeight / m_nImages, &m_dcBitmap,
		0, nState * (m_bitmapInfo.bmHeight / m_nImages), SRCCOPY );
}

// Init control, load bitmaps and resize control
void CAgentButton::Init( UINT nIDResource, int nImages, BOOL bAutoSize )
{
	m_dcBitmap.CreateCompatibleDC( NULL );
	m_cBitmap.LoadBitmap( nIDResource );
	m_dcBitmap.SelectObject ( & m_cBitmap );
	m_cBitmap.GetBitmap( &m_bitmapInfo );
	m_nImages = nImages;
	
	// Auto size
	if ( bAutoSize )
		AutoSize();	
}

void CAgentButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	SendMessage(WM_LBUTTONDOWN, nFlags, MAKELPARAM(point.x, point.y));
}

void CAgentButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//If we are tracking this button, cancel it
    if ( m_bTracking)
	{
		TRACKMOUSEEVENT t = { sizeof(TRACKMOUSEEVENT), TME_CANCEL | TME_LEAVE, m_hWnd, 0 };
        if ( ::_TrackMouseEvent( &t ) )
            m_bTracking = FALSE;
    }
	
	m_bMouseDown = TRUE;
	
    //Default-process the message
	CButton::OnLButtonDown(nFlags, point);
}

void CAgentButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( m_dwStyle )  //track mouse for radio & check buttons
	{
		POINT point2 = point;
		::ClientToScreen( m_hWnd, &point2 );
		HWND hWndMouse = ::WindowFromPoint( point2 );
		if (hWndMouse == m_hWnd ) // mouse is in button
		{
			if ( m_dwStyle == BS_CHECKBOX ) SetCheck( m_bChecked ? 0 : 1);
			if ( m_dwStyle == BS_RADIOBUTTON ) SetCheck(1);
			
			//SendNotify( BN_CLICKED, GetWindowLong( m_hWnd, GWL_ID ) );
		}
	}
	
    //Default-process the message
    m_bMouseDown = FALSE;
	
	CButton::OnLButtonUp(nFlags, point);
}

void CAgentButton::OnMouseMove(UINT nFlags, CPoint point) 
{
    if ( m_bMouseDown && ::GetCapture() == m_hWnd )
	{
		POINT point2 = point;
        ::ClientToScreen( m_hWnd, &point2 );
        HWND mouse_wnd = ::WindowFromPoint( point2 );
		
        BOOL bPressed = ( ( GetState() & BST_PUSHED ) == BST_PUSHED );
        BOOL bNeedPressed = (mouse_wnd == m_hWnd);
        if ( bPressed != bNeedPressed )
		{
            SetState( bNeedPressed ? TRUE : FALSE);
            Invalidate();
        }
    }
	else 
	{
        if ( !m_bTracking )
		{
            TRACKMOUSEEVENT t = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
            if (::_TrackMouseEvent(&t))
			{
                m_bTracking = TRUE;
                Invalidate();
            }
        }
    }
	
    //Forward this event to superclass
    CButton::OnMouseMove(nFlags, point);
}

LRESULT CAgentButton::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
	ASSERT (m_bTracking);
    m_bTracking = FALSE;
    Invalidate();
	return 0;
}

void CAgentButton::OnKillFocus(CWnd* pNewWnd) 
{
	if ( ::GetCapture() == m_hWnd )
	{
        ::ReleaseCapture();
        ASSERT ( !m_bTracking );
    }
	
	m_bMouseDown = FALSE;
    CButton::OnKillFocus(pNewWnd);
	
}

LRESULT CAgentButton::OnUnCheck(WPARAM wparam, LPARAM lParam)
{
	if ( m_bChecked && m_dwStyle != BS_CHECKBOX )
	{
		m_bChecked = FALSE;
		Invalidate();
	}
	return 0;
}

void CAgentButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ( m_dwStyle && nChar == ' ' )
	{
		if ( m_dwStyle == BS_CHECKBOX) SetCheck( m_bChecked ? 0 : 1 );
		if ( m_dwStyle == BS_RADIOBUTTON) SetCheck ( 1 );
	}
	CButton::OnKeyDown(nChar, nRepCnt, nFlags);	
}

LRESULT CAgentButton::OnBMSetCheck(WPARAM wParam, LPARAM lParam)
{
	m_bChecked = wParam != 0;
	
	switch ( m_dwStyle )
	{
	case BS_RADIOBUTTON:
		if (m_bChecked)
		{
			//uncheck the other radio buttons (in the same group)
			HWND hthis, hwnd2, hpwnd;
			hpwnd = GetParent()->GetSafeHwnd();	//get button parent handle
			hwnd2 = hthis = GetSafeHwnd();			//get this button handle
			if (hthis && hpwnd)
			{				//consistency check
				for( ; ; )
				{	//scan the buttons within the group
					hwnd2=::GetNextDlgGroupItem(hpwnd,hwnd2,0);
					//until we reach again this button
					if ( hwnd2==hthis || hwnd2 == NULL ) break;
					
					//post the uncheck message
					::PostMessage(hwnd2, WM_UNCHECK, 0, 0);
				}
			}
		}
		break;
	case BS_PUSHBUTTON:
		m_bChecked = FALSE;
		ASSERT(false); // Must be a Check or Radio button to use this function
	}
	
	Invalidate();
	return 0;
}

LRESULT CAgentButton::OnBMGetCheck(WPARAM wparam, LPARAM lParam)
{
	return m_bChecked;
}

void CAgentButton::SendNotify( WPARAM wParam, LPARAM lParam )
{
	HWND hParent = ::GetParent( m_hWnd );
	::SendMessage( hParent, WM_SENDNOTIFY, wParam, lParam );
}

void CAgentButton::OnClicked()
{
	SendNotify( BN_CLICKED, (LPARAM)m_hWnd );
}