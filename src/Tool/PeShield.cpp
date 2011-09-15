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
	
	//���� ���� ����
	// Mutex ����
    BOOL bFound = FALSE; 
    HANDLE hMutexOneInstance = ::CreateMutex(NULL, TRUE, _T("Unique Name of Mutex")); 
    
	// ���� �̹� ������� �ִٸ� Instance�� �̹� ������
    if(::GetLastError() == ERROR_ALREADY_EXISTS) 
        bFound = TRUE;
    if(hMutexOneInstance)
        ::ReleaseMutex(hMutexOneInstance);
	
    // �̹� �ϳ��� Instance�� �����ϸ� ���α׷� ����
    if(bFound)
    {
        AfxMessageBox("�˼��մϴ�. \r\n���α׷��� ���� �� �Դϴ�.");
        return FALSE;
    }
	
	//
	//Registry ���
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
				AfxMessageBox("Pe Shield ������Ʈ�� �� ������ ���� �߽��ϴ�.");
			}
		}
		else
		{
			AfxMessageBox("Pe Shield ������Ʈ�� Ű ������ ���� �߽��ϴ�.");
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
