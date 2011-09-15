// Sink.cpp: implementation of the CSink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PeShield.h"
#include "PeShieldDlg.h"
#include "Sink.h"
#include "../Library/h/reg.h"
#include "../Library/h/io.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSink::CSink()
{
	TCHAR tszWeb[MAX_URL] = {0,};
	// 레지스트리의 웹 주소 얻기
	DWORD cbValue;

	if(PeShield_RegGetValue(
		TOOL_REGKEY,
		&cbValue,
		tszWeb,
		TOOL_REGVALUE_WEB,
		NULL))
	{
		strWeb = tszWeb;
		
		if (strWeb == TOOL_DEF_WEB)
			bChecked = FALSE;
		else
			bChecked = TRUE;
	}
	else
	{
		m_pAgent->Speak(_T("Pe Shield 레지스트리 값을 얻는데 실패 했습니다!!"));
		m_pAgent->Speak(_T("프로그램 종료 후 다시 실행해 주세요!!"));
	}
	
	
}

CSink::~CSink()
{
	
}

//-------------------------------------------
// IAgentNotifySink methods
//-------------------------------------------

STDMETHODIMP CSink::Command(long dwCommandID, IUnknown * punkUserInput)
{
	if(	m_pAgent->m_commandExit == dwCommandID)
	{
		m_pAgent->ByeBye(m_pAgent->m_lDefaultCharID);
	}
	if (m_pAgent->m_commandEncode == dwCommandID)
	{
		MAINDLG->OnEncode();
	}
	if (m_pAgent->m_commandDecode == dwCommandID)
	{
		MAINDLG->OnDecode();
	}
	if (m_pAgent->m_commandOption == dwCommandID)
	{
		MAINDLG->OptionBalloon();
	}
	if (m_pAgent->m_commandHelp == dwCommandID)
	{
		TCHAR				szWinPath[MAX_PATH+1];
		TCHAR				szLoaderPath[MAX_PATH+1];
		TCHAR				szLoaderDir[MAX_PATH+1];
		TCHAR				szHelpCmd[MAX_PATH+1];
		STARTUPINFO			StartupInfo = {0};
		PROCESS_INFORMATION	ProcessInfo;
		GetWindowsDirectory(szWinPath, MAX_PATH);
		GetModuleFileName(AfxGetInstanceHandle(), szLoaderPath, MAX_PATH);
		PeShield_GetFileDirectory(szLoaderDir, szLoaderPath);
		wsprintf(szHelpCmd, _T("%s\\hh.exe %s\\help.chm"), szWinPath, szLoaderDir);
		StartupInfo.cb = sizeof(StartupInfo);
		CreateProcess(
			NULL,
			szHelpCmd,
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			NULL,
			&StartupInfo,
			&ProcessInfo
			);
		WaitForInputIdle(ProcessInfo.hProcess, INFINITE);
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
	}
	return NOERROR;
}

STDMETHODIMP CSink::ActivateInputState(long dwCharID, long bActivated)
{
	return NOERROR;
}

STDMETHODIMP CSink::VisibleState(long dwCharID, long bVisible, long lCause)
{
	return NOERROR;
}


STDMETHODIMP CSink::Click(long dwCharID, short fwKeys, long x, long y)
{
	return NOERROR;
}


STDMETHODIMP CSink::DblClick(long dwCharID, short fwKeys, long x, long y)
{
	return NOERROR;
}


STDMETHODIMP CSink::DragStart(long dwCharID, short fwKeys, long x, long y)
{
	return NOERROR;
}


STDMETHODIMP CSink::DragComplete(long dwCharID, short fwKeys, long x, long y)
{
	return NOERROR;
}


STDMETHODIMP CSink::RequestStart(long dwRequestID)
{
	return NOERROR;
}


STDMETHODIMP CSink::RequestComplete(long dwRequestID, long hrStatus)
{
	if (dwRequestID == m_pAgent->m_exitID)
		PostQuitMessage(0);
	return NOERROR;
}


STDMETHODIMP CSink::BookMark(long dwBookMarkID)
{
	return NOERROR;
}


STDMETHODIMP CSink::Idle(long dwCharID, long bStart)
{
	return NOERROR;
}


STDMETHODIMP CSink::Move(long dwCharID, long x, long y, long lCause)
{
	CNotifySink::Move( dwCharID, x, y, lCause );
	return NOERROR;
}


STDMETHODIMP CSink::Size(long dwCharID, long lWidth, long lHeight)
{
	return NOERROR;
}


STDMETHODIMP CSink::BalloonVisibleState(long dwCharID, long bVisible)
{
	return NOERROR;
}


STDMETHODIMP CSink::HelpComplete(long dwCharID, long dwCommandID, long dwCause)
{
	return NOERROR;
}


STDMETHODIMP CSink::ListeningState(long dwCharacterID, long bListenState, long dwCause)
{
	return NOERROR;
}


STDMETHODIMP CSink::DefaultCharacterChange(BSTR bszGUID)
{
	return NOERROR;
}


STDMETHODIMP CSink::AgentPropertyChange()
{
	return NOERROR;
}


STDMETHODIMP CSink::ActiveClientChange(long dwCharID, long lStatus)
{
	return NOERROR;
}

STDMETHODIMP CSink::OptionBalloon( long dwCharID, long lEvent )
{
	try
	{
		if ( lEvent == -1 )
		{
#ifdef _DEBUG
			m_pAgent->Speak(_T("옵션 말풍선을 불러오지 못했습니다."));
			m_pAgent->Speak(_T("다시 실행해 주세요!!"));
#endif //_DEBUG
		}
		else
		{
			if (lEvent == 120)
			{	
				bChecked = m_pAgent->GetOptionBalloon( dwCharID )->GetCheckBoxState();
				
				if (bChecked)
				{
					strWeb = m_pAgent->GetOptionBalloon( dwCharID )->GetTextBox();
					m_pAgent->Play("Suggest");
					m_pAgent->Speak(MESSAGE(IDS_SUC_WEBADDRESS));
				}
				else
				{
					strWeb = TOOL_DEF_WEB;
				}
				if(!PeShield_RegSetValue(
						TOOL_REGKEY, 
						(const PVOID)(LPCTSTR)strWeb,
						strWeb.GetLength() * sizeof(TCHAR), 
						TOOL_REGVALUE_WEB,
						REG_SZ))
				{
					m_pAgent->Play("Decline");
					m_pAgent->Speak(_T("Pe Shield 레지스트리 값 설정에 실패했습니다!!"));
					m_pAgent->Speak(_T("프로그램 종료 후 다시 실행해 주세요!!"));
				}
			}
			
		}
	}
	catch (int *pErr)
	{
		delete pErr;
	}

	return NOERROR;
}

STDMETHODIMP CSink::WebBalloon( long dwCharID, long lEvent )
{
	try
	{
		if ( lEvent == -1 )
		{
#ifdef _DEBUG
			m_pAgent->Speak(_T("웹 주소 말풍선을 불러오지 못했습니다."));
			m_pAgent->Speak(_T("다시 실행해 주세요!!"));
#endif //_DEBUG
		}
		else
		{
			CString strText;
			if (lEvent == 120)
			{
				try
				{
					strText = m_pAgent->GetWebBalloon( dwCharID )->GetTextBox();
					m_pAgent->GetWebBalloon()->ShowWindow(0);
					MAINDLG->Encrypt(strText);

				}
				catch (int *pErr)
				{
					m_pAgent->GetWebBalloon()->ShowWindow(0);
					m_pAgent->Play("Decline");
					m_pAgent->Speak(_T("웹주소 말풍선을 불러오지 못했습니다"));
					delete pErr;
				}
			}
			
		}
	}
	catch (int *pErr)
	{
#ifdef _DEBUG
		m_pAgent->Play("Decline");
		m_pAgent->Speak(_T("웹주소 말풍선을 불러오지 못했습니다"));
#endif //_DEBUG
		delete pErr;
	}

	return NOERROR;
}