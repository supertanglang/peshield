// AgentEdit.cpp : implementation file
//

#include "stdafx.h"
#include "PeShield.h"
#include "AgentEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgentEdit

CAgentEdit::CAgentEdit()
{
}

CAgentEdit::~CAgentEdit()
{
}


BEGIN_MESSAGE_MAP(CAgentEdit, CEdit)
	//{{AFX_MSG_MAP(CAgentEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgentEdit message handlers

void CAgentEdit::SetLines ( int nLines, int nFontSize )
{
	CRect rect;
	GetWindowRect ( &rect );
	SetWindowPos( NULL, 0, 0, rect.Width(), nFontSize * nLines, SWP_NOZORDER | SWP_NOMOVE );
}

BOOL CAgentEdit::PreTranslateMessage(MSG* pMsg) 
{

	if(pMsg->message == WM_KEYDOWN) 
    {
		HWND m_parent = NULL;	
        if(pMsg->wParam == VK_RETURN )    	
        {
			MAINDLG->GetSink()->OptionBalloon(259,120);
			MAINDLG->GetAgent()->GetOptionBalloon()->ShowWindow(0);
            return TRUE;
        }
    }
	return CEdit::PreTranslateMessage(pMsg);
}

BOOL CAgentEdit::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_STATICEDGE;
	
	return CEdit::PreCreateWindow(cs);
}
