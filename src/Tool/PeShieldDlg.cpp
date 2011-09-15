// PeShieldDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PeShield.h"
#include "PeShieldDlg.h"
#include "Tool.h"
#include "..\Library\h\comm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPeShieldDlg dialog

CPeShieldDlg::CPeShieldDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPeShieldDlg::IDD, pParent), m_pSink(NULL)
{
	//{{AFX_DATA_INIT(CPeShieldDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CPeShieldDlg::~CPeShieldDlg()
{
	if (m_pSink)
		delete m_pSink;
}

void CPeShieldDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPeShieldDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPeShieldDlg, CDialog)
	//{{AFX_MSG_MAP(CPeShieldDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPeShieldDlg message handlers

BOOL CPeShieldDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	int nWidth = GetSystemMetrics(SM_CXFULLSCREEN);   // 바탕화면 클라이언트 영역 넓이
	int nHeight = GetSystemMetrics(SM_CYFULLSCREEN);  // 바탕화면 클라이언트 영역  높이
	
	//
	//Initialize Pe Shield Tool data
	//
	if (!PeShield_ToolStartup(AfxGetInstanceHandle()))
	{
#ifdef _DEBUG
		AfxMessageBox(_T("Fail to load Pe Shield Tool data"));
		EndDialog(1);
#endif // _DEBUG
	}

	//
	//MS Agent 초기화
	//
	try
	{
		m_pSink = new CSink();
		m_cAgent.Init ( m_pSink );
		m_cAgent.LoadCharacter ( "merlin.acs", &m_cAgent.m_lDefaultCharID );
		m_cAgent.SetLanguageID ( 0x0409 );
		m_cAgent.MoveTo(nWidth-200,nHeight-170,0);
		m_cAgent.Show( !m_cAgent.GetVisible() );
		m_cAgent.Play("Greet");
		m_cAgent.Speak(MESSAGE(IDS_START_MESSAGE));
		m_cAgent.Speak(MESSAGE(IDS_EXPLAINATION_MESSAGE));
		m_cAgent.SetCommand(m_cAgent.m_lDefaultCharID, TRUE, FALSE, FALSE);
	}
	catch (int *pErr)
	{
		// An error
		AfxMessageBox ("Init err");
		PostQuitMessage( 0 );
		delete pErr;
	}

	//최상위 윈도우
	this->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPeShieldDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPeShieldDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPeShieldDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPeShieldDlg::OptionBalloon()
{
	UpdateData();
	HWND hParent = NULL;
	try
	{
		DWORD dwFlags = 0;
		dwFlags |= BI_CHECKBOX; 
		dwFlags |= BI_TEXTBOX;

		m_cAgent.GetOptionBalloon()->SetUpControls( 2, 0, dwFlags );
		m_cAgent.GetOptionBalloon()->SetPosition(  );
		m_cAgent.GetOptionBalloon()->Show( hParent );
	}
	catch (int *pErr)
	{
#ifdef _DEBUG
		m_cAgent.Speak(_T("옵션 말풍선을 불러오지 못했습니다."));
		m_cAgent.Speak(_T("다시 실행해 주세요."));
#endif // _DEBUG
		delete pErr;
	}
}

void CPeShieldDlg::WebBalloon()
{
	UpdateData();
	HWND hParent = NULL;
	CString strText = _T(" 웹 주소를 입력해주세요");
	try
	{
		DWORD dwFlags = 0;
		dwFlags |= BI_TEXTBOX;
		m_cAgent.GetWebBalloon()->m_hIcon = m_hIcon;
		m_cAgent.GetWebBalloon()->SetUpControls( 2, 0, dwFlags );
		m_cAgent.GetWebBalloon()->m_strTitle = strText;
		m_cAgent.GetWebBalloon()->SetPosition( );
		m_cAgent.GetWebBalloon()->Show( hParent );
	}
	catch (int *pErr)
	{
#ifdef _DEBUG
		m_cAgent.Speak(_T("웹 주소 말풍선을 불러오지 못했습니다."));
		m_cAgent.Speak(_T("다시 실행해 주세요!!"));
#endif // _DEBUG
		delete pErr;
	}
}

void CPeShieldDlg::OnEncode()
{
	if (m_pSink->bChecked == FALSE)
	{
		WebBalloon();
	}
	else
	{
		Encrypt(m_pSink->strWeb);
	}
}

void CPeShieldDlg::OnDecode()
{
	char szFilter[] = "실행파일 (*.exe)|*.exe||";
	
	CFileDialog dlg(TRUE,NULL,NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
		OFN_LONGNAMES,szFilter);

	m_cAgent.SetCommand(m_cAgent.m_lDefaultCharID, FALSE, FALSE, TRUE);
	m_cAgent.SetCommand(m_cAgent.m_lDefaultCharID, FALSE, TRUE, FALSE);
	
	CString pathname;
	DECRYPT_RESULT	DecryptResult;
	
	if(IDOK == dlg.DoModal())
	{
		pathname	= dlg.GetPathName();
	
		m_cAgent.Play("Writing");
		DecryptResult = PeShield_DecryptExe(
			pathname,
			1024,
			NULL,
			NULL
			);

		m_cAgent.StopAll();

		switch (DecryptResult)
		{
		case DEC_SUCCEEDED:
			m_cAgent.Play("Announce");
			m_cAgent.Speak(MESSAGE(IDS_DEC_SUCCESS));
			break;
			
		case DEC_NOT_ENCRYPTED:	
			m_cAgent.Play("Decline");
			m_cAgent.Speak(MESSAGE(IDS_DEC_NOT_ENCRYPTED));
			break;
		case DEC_NOT_EXECUTABLE:
			m_cAgent.Play("Decline");
			m_cAgent.Speak(MESSAGE(IDS_DEC_NOT_EXECUTABLE));
			break;
		case DEC_MAPPING_FAILED:
			m_cAgent.Play("Decline");
			m_cAgent.Speak(MESSAGE(IDS_DEC_MAPPING_FAILED));
			break;

		default:
			m_cAgent.Play("Decline");
			m_cAgent.Speak(MESSAGE(IDS_DEFAULT));
			break;
		}
    }
	m_cAgent.SetCommand(m_cAgent.m_lDefaultCharID, FALSE, FALSE, TRUE);
	m_cAgent.SetCommand(m_cAgent.m_lDefaultCharID, TRUE, FALSE, FALSE);
}

void CPeShieldDlg::Encrypt(CString strWeb)
{
	char szFilter[] = "실행파일 (*.exe)|*.exe||";
	
	CFileDialog dlg(TRUE,NULL,NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
		OFN_LONGNAMES ,szFilter);

	m_cAgent.SetCommand(m_cAgent.m_lDefaultCharID, FALSE, FALSE, TRUE);
	m_cAgent.SetCommand(m_cAgent.m_lDefaultCharID, FALSE, TRUE, FALSE);
	
	CString pathname;	
	ENCRYPT_RESULT	EncryptResult;
	
	if(IDOK == dlg.DoModal())
	{
		pathname = dlg.GetPathName();
		m_cAgent.Play("Processing");
		
		EncryptResult = PeShield_EncryptExe(
			pathname,
			strWeb,
			1024,
			NULL,
			NULL
			);
		
		m_cAgent.StopAll();
		
		switch (EncryptResult)
		{
		case ENC_SUCCEEDED:
			m_cAgent.Play("Announce");
			m_cAgent.Speak(MESSAGE(IDS_ENC_SUCCEEDED));
			break;
			
		case ENC_ALREADY_ENCRYPTED:	
			m_cAgent.Play("Decline");
			m_cAgent.Speak(MESSAGE(IDS_ENC_ALREADY_ENCRYPTED));
			break;
		case ENC_INVALID_WEB:
			m_cAgent.Play("Decline");
			m_cAgent.Speak(MESSAGE(IDS_ENC_INVALID_WEB));
			break;
		case ENC_NOT_EXECUTABLE:
			m_cAgent.Play("Decline");
			m_cAgent.Speak(MESSAGE(IDS_ENC_NOT_EXECUTABLE));
			break;
		case ENC_MAPPING_FAILED:
			m_cAgent.Play("Decline");
			m_cAgent.Speak(MESSAGE(IDS_ENC_MAPPING_FAILED));
			break;
		default:
			m_cAgent.Play("Decline");
			m_cAgent.Speak(MESSAGE(IDS_DEFAULT));
			break;
		}
	}
	m_cAgent.SetCommand(m_cAgent.m_lDefaultCharID, FALSE, FALSE, TRUE);
	m_cAgent.SetCommand(m_cAgent.m_lDefaultCharID, TRUE, FALSE, FALSE);
}

BOOL CPeShieldDlg::PreTranslateMessage(MSG* pMsg) 
{

	return CDialog::PreTranslateMessage(pMsg);
}

void CPeShieldDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	PeShield_ToolCleanup();
	
}
