#if !defined( gS_BalloonWnd_DEFINED )
#define gS_BalloonWnd_DEFINED

#define B_ARROWLEFT		0
#define B_ARROWRIGHT	1
#define B_ARROWTOP		2
#define B_ARROWBOTTOM	3

/////////////////////////////////////////////////////////////////////////////
// CBalloonWnd window

//----------------------------------------------------------------------
//			  P  E  S  H  I  E  L  D       A  G  E  N  T
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: HAM DON WOOK
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

class CBalloonWnd : public CWnd
{
public:
	CBalloonWnd();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBalloonWnd)
	public:
	virtual BOOL Create(LPCTSTR strTitle, CRect rectClient, CWnd* pParent = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

public:
	void SetSize();
	void ClientSize( int nWidth, int nHeight );
	void ShowBalloonEx( CRect rectParent );
	BOOL ShowBalloon( POINT point, int nArrowSide );
	virtual ~CBalloonWnd();

protected:
	CWnd m_wndInvisible;
	//{{AFX_MSG(CBalloonWnd)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CRgn m_rWindowRgn;
	int m_nArrowSide;
	ATOM m_aAtom;
public:
	void CalcAndMove ( CRect rectParent );
	unsigned int m_nRoundX, m_nRoundY;
	unsigned int m_nArrowFM, m_nArrowSM;
	unsigned int m_nVMargin, m_nHMargin;
	unsigned int m_nAMargin;
	COLORREF m_crBackground, m_crFrame;
};

#endif // !defined(gS_BalloonWnd_DEFINED)
