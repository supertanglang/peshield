// PeShield.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PeShield.h"
#include "PeShieldDlg.h"
#include "..\Library\h\reg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPeShieldApp

BEGIN_MESSAGE_MAP(CPeShieldApp, CWinApp)
	//{{AFX_MSG_MAP(CPeShieldApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPeShieldApp construction

CPeShieldApp::CPeShieldApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CPeShieldApp::~CPeShieldApp()
{
	CoUninitialize();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPeShieldApp object

CPeShieldApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPeShieldApp initialization

BOOL CPeShieldApp::InitInstance()
{
	AfxEnableControlContainer();
	
	CoInitialize( NULL );
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	
	//중족 실행 방지
	// Mutex 생성
    BOOL bFound = FALSE; 
    HANDLE hMutexOneInstance = ::CreateMutex(NULL, TRUE, _T("Unique Name of Mutex")); 
    
	// 만약 이미 만들어져 있다면 Instance가 이미 존재함
    if(::GetLastError() == ERROR_ALREADY_EXISTS) 
        bFound = TRUE;
    if(hMutexOneInstance)
        ::ReleaseMutex(hMutexOneInstance);
	
    // 이미 하나의 Instance가 존재하면 프로그램 종료
    if(bFound)
    {
        AfxMessageBox("죄송합니다. \r\n프로그램이 실행 중 입니다.");
        return FALSE;
    }
	
	//
	//Registry 등록
	//
	if (!PeShield_RegDoesKeyExist(TOOL_REGKEY))
	{
		if(PeShield_RegCreateKey(TOOL_REGKEY))
		{
			if(!PeShield_RegSetValue(
				TOOL_REGKEY,
				TOOL_DEF_WEB, 
				lstrlen(TOOL_DEF_WEB) * sizeof(TCHAR),
				TOOL_REGVALUE_WEB,
				REG_SZ))
			{
				AfxMessageBox("Pe Shield 레지스트리 값 설정에 실패 했습니다.");
			}
		}
		else
		{
			AfxMessageBox("Pe Shield 레지스트리 키 생성에 실패 했습니다.");
		}
	}
	
	CPeShieldDlg* PeShield = new CPeShieldDlg;
	m_pMainWnd = PeShield;
	PeShield->Create(IDD_PESHIELD_DIALOG);
	PeShield->ShowWindow(SW_HIDE);

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}
