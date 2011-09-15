// AgentEdit2.cpp : implementation file
//

#include "stdafx.h"
#include "peshield.h"
#include "AgentEdit2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgentEdit2

CAgentEdit2::CAgentEdit2()
{
}

CAgentEdit2::~CAgentEdit2()
{
}


BEGIN_MESSAGE_MAP(CAgentEdit2, CEdit)
	//{{AFX_MSG_MAP(CAgentEdit2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgentEdit2 message handlers
void CAgentEdit2::SetLines ( int nLines, int nFontSize )
{
	CRect rect;
	GetWindowRect ( &rect );
	SetWindowPos( NULL, 0, 0, rect.Width(), nFontSize * nLines, SWP_NOZORDER | SWP_NOMOVE );
}

BOOL CAgentEdit2::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN) 
    {
		HWND m_parent = NULL;	
        if(pMsg->wParam == VK_RETURN )    	
        {
			MAINDLG->GetSink()->WebBalloon(259,120);
            return TRUE;
        }
    }
	return CEdit::PreTranslateMessage(pMsg);
}

BOOL CAgentEdit2::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_STATICEDGE;
	
	return CEdit::PreCreateWindow(cs);
}
