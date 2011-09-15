// BalloonWnd.cpp : implementation file
//

#include "stdafx.h"
#include "BalloonWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning ( disable : 4244 )
#define B_MARGIN		8
#define B_ARROW			20
#define B_NCAREA		( B_MARGIN + B_ARROW )

/////////////////////////////////////////////////////////////////////////////
// CBalloonWnd

CBalloonWnd::CBalloonWnd()
{
	m_crBackground = RGB(255,255,224);
	m_crFrame = RGB(0,0,0);

	m_aAtom = NULL;

	m_nArrowFM = 10;
	m_nArrowSM = 20;

	m_nHMargin = 20;
	m_nVMargin = 20;

	m_nRoundX = 20;
	m_nRoundY = 20;
	m_nAMargin = 10;
}

CBalloonWnd::~CBalloonWnd()
{
}


BEGIN_MESSAGE_MAP(CBalloonWnd, CWnd)
	//{{AFX_MSG_MAP(CBalloonWnd)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBalloonWnd message handlers

void CBalloonWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	::InflateRect(&lpncsp->rgrc[0], -B_NCAREA, -B_NCAREA);
}

void CBalloonWnd::OnNcPaint() 
{
	CWindowDC dc(this);
	CRect rect, rectClient;
	
	// Obtain and convert rects
	GetWindowRect(&rect);
	GetClientRect(&rectClient);
	ScreenToClient(&rect);
	
	// Calc dimensions
	rectClient.OffsetRect(-rect.left, -rect.top);
	rect.OffsetRect(-rect.left, -rect.top);
	dc.ExcludeClipRect(&rectClient);

	dc.FillSolidRect(&rect, m_crBackground);

	CBrush brush;
	brush.CreateSolidBrush( m_crFrame );
	dc.FrameRgn( &m_rWindowRgn, &brush, 1, 1);
}

BOOL CBalloonWnd::OnEraseBkgnd(CDC* pDC) 
{
	RECT rect;
	GetClientRect( &rect );
	pDC->FillSolidRect( &rect, m_crBackground );
	
	return CWnd::OnEraseBkgnd(pDC);
}

BOOL CBalloonWnd::Create(LPCTSTR strTitle, CRect rectClient, CWnd* pParent) 
{
	// Register
	WNDCLASSEX wcx; 

    // Fill in the window class structure with parameters 
    // that describe the main window. 

	if ( !pParent ) pParent = AfxGetMainWnd();

	wcx.cbSize = sizeof(wcx);                 // size of structure 
    wcx.style =  CS_SAVEBITS;				  // save screen under
    wcx.lpfnWndProc = AfxWndProc;             // points to window procedure 
    wcx.cbClsExtra = 0;                       // no extra class memory 
    wcx.cbWndExtra = 0;                       // no extra window memory 
    wcx.hInstance = AfxGetInstanceHandle();   // handle to instance 
    wcx.hIcon = NULL;                         // no app. icon 
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW); // predefined arrow 
    wcx.hbrBackground = ::GetSysColorBrush(COLOR_INFOBK); // no background brush 
    wcx.lpszMenuName =  NULL;                 // no menu resource 
    wcx.lpszClassName = "BalloonWndClass";   // name of window class 
    wcx.hIconSm = NULL;                      // no small class icon
	
    // Register the window class. 
    if ( !m_aAtom )
	{
		m_aAtom = RegisterClassEx(&wcx);
		if ( !m_aAtom )
		{
			// Try to get atom
			if ( !GetClassInfoEx( AfxGetInstanceHandle(), _T("BalloonWndClass"), &wcx ) )
				return FALSE;
		}
			
	}

	if ( !CreateEx( WS_EX_PALETTEWINDOW, _T("BalloonWndClass"), strTitle, WS_POPUPWINDOW, rectClient, pParent, 0, NULL) )
      return FALSE;

	return TRUE;
}

// Set window's size
void CBalloonWnd::SetSize()
{
	CRect rect, rectClient;
	GetClientRect( &rectClient );
	GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.OffsetRect(-rect.left, -rect.top );

	CRgn rgnMain, rgnArrow, rgnAll;
	rgnMain.CreateRoundRectRgn( B_ARROW, B_ARROW, rect.right - B_ARROW,
		rect.bottom - B_ARROW, m_nRoundX, m_nRoundY );
	
	// Create the arrow 
	CPoint pArrow[3];
	switch(m_nArrowSide)
	{
		case B_ARROWLEFT:
			pArrow[0].x = B_NCAREA;
			pArrow[0].y = m_nVMargin + B_ARROW;
			pArrow[1].x = 0;
			pArrow[1].y = B_ARROW + m_nVMargin + m_nArrowFM;
			pArrow[2].x = B_NCAREA;
			pArrow[2].y = B_ARROW + m_nVMargin + m_nArrowSM;
			break;
		case B_ARROWRIGHT:
			pArrow[0].x = rect.right - B_NCAREA;
			pArrow[0].y = m_nVMargin + B_ARROW;
			pArrow[1].x = rect.right;
			pArrow[1].y = B_ARROW + m_nVMargin + m_nArrowFM;
			pArrow[2].x = pArrow[0].x;
			pArrow[2].y = B_ARROW + m_nVMargin + m_nArrowSM;
			break;
		case B_ARROWTOP:
			pArrow[0].x = B_ARROW + m_nHMargin;
			pArrow[0].y = B_NCAREA;
			pArrow[1].x = B_ARROW + m_nHMargin + m_nArrowFM;
			pArrow[1].y = 0;
			pArrow[2].x = B_ARROW + m_nHMargin + m_nArrowSM;
			pArrow[2].y = B_NCAREA;
			break;
		case B_ARROWBOTTOM:
			pArrow[0].x = B_ARROW + m_nHMargin;
			pArrow[0].y = rect.bottom - B_NCAREA;
			pArrow[1].x = B_ARROW + m_nHMargin + m_nArrowFM;
			pArrow[1].y = rect.bottom;
			pArrow[2].x = B_ARROW + m_nHMargin + m_nArrowSM;
			pArrow[2].y = rect.bottom - B_NCAREA;
			break;
	}

	rgnArrow.CreatePolygonRgn( &pArrow[0], 3, ALTERNATE);

	// Init rgns
	if (m_rWindowRgn.m_hObject == NULL)
		m_rWindowRgn.CreateRectRgn(0, 0, 0, 0);
	rgnAll.CreateRectRgn(0, 0, 1, 1);

	// Combine rgns
	rgnAll.CombineRgn( &rgnArrow, &rgnMain, RGN_OR );
	m_rWindowRgn.CombineRgn( &rgnArrow, &rgnMain, RGN_OR );

	SetWindowRgn( rgnAll, TRUE );
}

// Show window
BOOL CBalloonWnd::ShowBalloon(POINT point, int nArrowSide)
{	
	point.x -= B_ARROW;
	point.y -= B_ARROW;

	m_nArrowSide = nArrowSide;
	SetSize();

	SetWindowPos(NULL, point.x, point.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );

	::ShowWindow(m_hWnd, TRUE);
	
	return TRUE;
}

// Automaticly calc window pos. and show
void CBalloonWnd::ShowBalloonEx( CRect rectParent)
{
	CalcAndMove( rectParent );
	::ShowWindow(m_hWnd, TRUE);
}

void CBalloonWnd::CalcAndMove( CRect rectParent )
{
	CRect rectBalloon, rectScreen;
	int nArrowSide;

	// Calc window pos
	GetClientRect( &rectBalloon );
	rectBalloon.InflateRect(0,0, B_MARGIN * 2, B_MARGIN * 2);
	
	// Resize parent window - margin
	rectParent.InflateRect( m_nAMargin, m_nAMargin );

	// Get screen size
	GetDesktopWindow()->GetWindowRect( &rectScreen );

	// Top
	if ( rectBalloon.Height() + B_ARROW <= rectParent.top )
	{
		// It's enough space
		nArrowSide = B_ARROWBOTTOM;

		rectBalloon.OffsetRect( 0, rectParent.top - rectBalloon.Height() - B_ARROW );
		rectBalloon.OffsetRect( rectParent.left - ( rectBalloon.Width() - rectParent.Width() ) * 0.5, 0) ;

		m_nHMargin = ( rectBalloon.Width() - m_nArrowSM ) * 0.5;

		goto CorrectPos;
	}

	// Right
	if ( rectBalloon.Width() + B_ARROW <= (rectScreen.right - rectParent.right) )
	{
		// OK
		nArrowSide = B_ARROWLEFT;
		
		rectBalloon.OffsetRect( 0, rectParent.top + ( (rectParent.Height() - rectBalloon.Height()) * 0.5 ) );
		rectBalloon.OffsetRect( rectParent.right, 0) ;
	
		m_nVMargin = ( rectBalloon.Height() - m_nArrowSM ) * 0.5;

		goto CorrectPos;
	}

	// Left
	if ( rectBalloon.Width() + B_ARROW <= rectParent.left )
	{
		// OK
		nArrowSide = B_ARROWRIGHT;

		rectBalloon.OffsetRect( 0, rectParent.top + ( ( rectParent.Height() - rectBalloon.Height()) * 0.5 ) );
		rectBalloon.OffsetRect( rectParent.left - rectBalloon.Width() - B_ARROW, 0) ;

		m_nVMargin = ( rectBalloon.Height() - m_nArrowSM ) * 0.5;

		goto CorrectPos;
	}
	
	// Bottom
	if ( rectBalloon.Height() + B_ARROW <= (rectScreen.bottom- rectParent.bottom) )
	{
		// OK
		nArrowSide = B_ARROWTOP;

		rectBalloon.OffsetRect( 0, rectParent.bottom + B_ARROW );
		rectBalloon.OffsetRect( rectParent.left - ( rectBalloon.Width() - rectParent.Width() ) * 0.5, 0) ;

		m_nHMargin = ( rectBalloon.Width() - m_nArrowSM ) * 0.5;

		goto CorrectPos;
	}

	// Nowhere is sufficient room
	nArrowSide = B_ARROWRIGHT;
	rectBalloon.OffsetRect( -rectBalloon.left, -rectBalloon.top );
	
	goto End;

CorrectPos:
	if ( rectBalloon.left < 0 )
	{
		m_nHMargin += -rectBalloon.left;
		rectBalloon.OffsetRect( -rectBalloon.left, 0 );
	}

	goto End;
	
End:
	m_nArrowSide = nArrowSide;
	SetSize();

	SetWindowPos(NULL, rectBalloon.left - B_ARROW, rectBalloon.top - B_ARROW, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER );
}

// Change window's size
void CBalloonWnd::ClientSize(int nWidth, int nHeight)
{
	nWidth += B_NCAREA * 2;
	nHeight += B_NCAREA * 2;

	SetWindowPos(NULL, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER);
	SetSize();
}

BOOL CBalloonWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	BOOL bRet = CWnd::PreCreateWindow(cs);

	// Create invisible window
 	if ( !::IsWindow(m_wndInvisible.m_hWnd) )
 	{
 		LPCTSTR pstrOwnerClass = AfxRegisterWndClass(0);
		if (!m_wndInvisible.CreateEx(0, pstrOwnerClass, _T(""), WS_POPUP,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				NULL, 0))
			return FALSE;
 	}

	cs.hwndParent = m_wndInvisible.m_hWnd;
	
	return bRet;
}
