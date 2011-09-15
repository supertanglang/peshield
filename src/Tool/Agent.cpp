// Agent.cpp: implementation of the CAgent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <atlbase.h>
#include "Agent.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma warning( disable : 4244 ) // Disable warning no. 4244

//-------------------------------------------
// Consts/Macros and other stuff
//-------------------------------------------
#define SAFE_RELEASE(p)		{ if(p) { (p)->Release(); (p)=NULL; } }

//-------------------------------------------
// Constructor/Destructor
//-------------------------------------------
CAgent::CAgent()
{
	m_pAgentEx = NULL;
	m_pNotifySink = NULL;

	m_lNotifySinkID = -1;
}

CAgent::~CAgent()
{
	Close();
}

//-------------------------------------------
// Error functions
//-------------------------------------------
void CheckAndThrow(int nCode)
{
	if ( SUCCEEDED (nCode) )
		return; // No error

	// Otherwise throw an exception
	int* pError = new int;	// Alloc int
	*pError = nCode;		// Set error
	throw pError;			// Throw
}

void Throw(int nCode)
{
	int* pError = new int;	// Alloc int
	*pError = nCode;		// Set error
	throw pError;			// Throw
}

//-------------------------------------------
// SetCommand functions
//-------------------------------------------
BOOL CAgent::SetCommand(long lCharID,
						BOOL start = FALSE, 
						BOOL all = FALSE, 
						BOOL init = FALSE)
						
{
	BSTR exit;
	BSTR encode;
	BSTR decode;
	BSTR option;
	BSTR help;
	long commandID;
	long commandEncode;
	long commandDecode;
	long commandOption;
	long commandHelp;
	long lIndex = GetIndexFromID( lCharID );
	
	encode = CString(_T("암호화 (&D)")).AllocSysString();
	decode = CString(_T("복호화 (&E)")).AllocSysString();
	option = CString(_T("옵션 (&O)")).AllocSysString();
	help = CString(_T("도움말 (&I)")).AllocSysString();
	exit = CString(_T("종료 (&X)")).AllocSysString();
	// 명령에 대한 인터페이스를 얻어온다.
	CheckAndThrow (m_arChars[lIndex].pCharacterEx->QueryInterface(IID_IAgentCommands,(LPVOID*)
		&pCommand));
	
	// 명령어를 추가한다.
	// (caption, voice command, enable, visible, command ID)

	if (start == TRUE)
	{
		hRes = pCommand->Add(encode, NULL, true, true, &commandEncode);
		hRes = pCommand->Add(decode, NULL, true, true, &commandDecode);
		hRes = pCommand->Add(option, NULL, true, true, &commandOption);
		hRes = pCommand->Add(help, NULL, true, true, &commandHelp);
		hRes = pCommand->Add(exit, NULL, true, true, &commandID);
	}
	if (all == TRUE)
	{
		hRes = pCommand->Add(encode,NULL, false, true, &commandEncode);
		hRes = pCommand->Add(decode,NULL, false, true, &commandDecode);
		hRes = pCommand->Add(option,NULL, false, true, &commandOption);
		hRes = pCommand->Add(help, NULL, false, true, &commandHelp);
		hRes = pCommand->Add(exit, NULL, false, true, &commandID);
	}
	if (init == TRUE)
	{
		hRes = pCommand->RemoveAll();
	}
	

	m_commandExit	= commandID;
	m_commandEncode = commandEncode;
	m_commandDecode = commandDecode;
	m_commandOption = commandOption;
	m_commandHelp = commandHelp;

	if(FAILED(hRes)) return FALSE;
	
	SysFreeString(exit);
	SysFreeString(encode);
	SysFreeString(decode);
	SysFreeString(option);
	SysFreeString(help);

	return TRUE;
}

//-------------------------------------------
// Exit functions
//-------------------------------------------
BOOL CAgent::ByeBye(long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );
	Play("Wave");
	CheckAndThrow (m_arChars[lIndex].pCharacterEx->Hide(FALSE, &m_exitID));
	return TRUE;
}


//-------------------------------------------
// Initialize/uninitialize functions
//-------------------------------------------
void CAgent::Init( CAgent::CNotifySink* pNotifySink )
{	
	Close();

	CheckAndThrow ( CoCreateInstance(CLSID_AgentServer, NULL, CLSCTX_SERVER, 
		IID_IAgentEx, (LPVOID *)&m_pAgentEx) );

	if ( pNotifySink )
	{
		// Register NotifySink
		try 
		{
			pNotifySink->AddRef();
			CheckAndThrow ( m_pAgentEx->Register( pNotifySink, &m_lNotifySinkID ) ); // Register
			m_pNotifySink = pNotifySink;	// Store pointer
			m_pNotifySink->m_pAgent = this;
		}
		catch (int *pErr)
		{
			// An error
			pNotifySink->Release();
			throw pErr;
		}
	}
}

void CAgent::Close()
{
	// Close all characters
	while (m_arChars.GetSize() > 0)
		UnloadCharacter( m_arChars[0].lCharID );

	// Then Unregister NotifySink and close MsAgent
	if ( m_pNotifySink )
	{
		m_pNotifySink->m_pAgent = NULL;
		m_pAgentEx->Unregister( m_lNotifySinkID );
		m_pNotifySink->Release();
		m_pNotifySink = NULL;
	}
	
	SAFE_RELEASE( m_pAgentEx );
}

BOOL CAgent::IsInit()
{
	return (m_pAgentEx ? TRUE : FALSE );
}

void CAgent::LoadCharacter(CString strChar, long *pCharID)
{
	// Declare variables
	VARIANT vPath;
	long lCharID = -1, lReqID;
	IAgentCharacterEx* pCharacterEx = NULL;
	int nPos = -1;

	// Prepare character's definition file
	VariantInit(&vPath);
	vPath.vt = VT_BSTR;
	vPath.bstrVal = strChar.AllocSysString();

#ifdef BALLOONOPTION		
	COptionBalloon* pOptionBalloon = NULL;
#endif
#ifdef BALLOONWEB
	CWebBalloon* pWebBalloon = NULL;
#endif
	try
	{
		// Load character
		CheckAndThrow ( m_pAgentEx->Load( vPath, &lCharID, &lReqID) );

		// Get it's interface
		CheckAndThrow ( m_pAgentEx->GetCharacterEx( lCharID, &pCharacterEx ) );

		///////////////////////////////////
		// If we are here everything is ok.
		
		// Create tagCHAR struct and fill it
		tagCHAR tagChar;
		tagChar.lCharID = lCharID;
		tagChar.lReqID = lReqID;
		tagChar.pCharacterEx = pCharacterEx;
		
		// Create CBalloon
		tagChar.pBalloon = new CBalloon;
		if ( !tagChar.pBalloon )
			Throw( E_FAIL ); 

		// Query BalloonEx interface
		CheckAndThrow( pCharacterEx->QueryInterface( IID_IAgentBalloonEx, (void**)&tagChar.pBalloon->m_pBalloon ) );

		RECT rectAgent;
		CheckAndThrow( pCharacterEx->GetPosition( &rectAgent.left, &rectAgent.top ) );
		CheckAndThrow( pCharacterEx->GetSize( &rectAgent.right, &rectAgent.bottom ) );
		rectAgent.right += rectAgent.left;
		rectAgent.bottom += rectAgent.top;

#ifdef BALLOONOPTION		// Create MFC Option ballon
		pOptionBalloon = new COptionBalloon;
		if ( !pOptionBalloon->Create( "OptionBalloon", CRect(100, 100, 200, 200) ) )
			Throw( E_FAIL );
		pOptionBalloon->Move( rectAgent );
		
		pOptionBalloon->OptionNoModal = CAgent::CNotifySink::OptionBalloonProc;
		pOptionBalloon->m_lExtra = (long)this;
		pOptionBalloon->m_lExtra2 = lCharID;

		tagChar.pOptionBalloon = pOptionBalloon;
#endif
#ifdef BALLOONWEB		// Create MFC Web ballon
		pWebBalloon = new CWebBalloon;
		if ( !pWebBalloon->Create( "WebBalloon", CRect(100, 100, 200, 200) ) )
			Throw( E_FAIL );
		pWebBalloon->Move( rectAgent );
		
		pWebBalloon->WebNoModal = CAgent::CNotifySink::WebBalloonProc;
		pWebBalloon->m_lExtra = (long)this;
		pWebBalloon->m_lExtra2 = lCharID;

		tagChar.pWebBalloon = pWebBalloon;
#endif
		
		// Add struct to the array
		nPos = m_arChars.Add ( tagChar );
		// Return CharID
		*pCharID = lCharID;


		// Clean up
		SysFreeString( vPath.bstrVal );
		VariantClear( &vPath );

		return;
	}
	catch (int *pErr)
	{
		// CleanUp
		SysFreeString( vPath.bstrVal );
		VariantClear( &vPath );
		
		// Delete the struct from array if it was added
		if ( nPos != -1)
			m_arChars.RemoveAt ( nPos );

		// Unload the character if it was loaded
		if (lCharID != -1)
			m_pAgentEx->Unload ( lCharID );
		
		// Release the pointer if it is correct
		SAFE_RELEASE( pCharacterEx );

		#ifdef BALLOONOPTION
			if ( pOptionBalloon )
			{
				pOptionBalloon->DestroyWindow();
				pOptionBalloon->~COptionBalloon();
				delete pOptionBalloon;
			}
		#endif

		#ifdef BALLOONWEB
		if ( pWebBalloon )
		{
			pWebBalloon->DestroyWindow();
			pWebBalloon->~CWebBalloon();
			delete pWebBalloon;
		}
		#endif

		*pCharID = -1;

		throw pErr;
	}
}

// Close the character
void CAgent::UnloadCharacter(long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );
	
	tagCHAR tagChar = m_arChars.GetAt( lIndex );

#ifdef BALLOONOPTION
	// Destroy window
	tagChar.pOptionBalloon->DestroyWindow();
	delete tagChar.pOptionBalloon;
#endif
#ifdef BALLOONWEB
	// Destroy window
	tagChar.pWebBalloon->DestroyWindow();
	delete tagChar.pWebBalloon;
#endif
	// Release pointers
	SAFE_RELEASE( tagChar.pBalloon->m_pBalloon );	// BalloonEx
	SAFE_RELEASE( tagChar.pCharacterEx );		// CharacterEx

	if ( tagChar.pBalloon )
		delete tagChar.pBalloon;

	// And unload the character
	m_pAgentEx->Unload( tagChar.lCharID );

	m_arChars.RemoveAt( lIndex );

}

void CAgent::SetLanguageID(long lLangID, long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );

	CheckAndThrow( m_arChars[lIndex].pCharacterEx->SetLanguageID( lLangID ) );
}

long CAgent::GetLanguageID(long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );
	long lLangID;

	CheckAndThrow( m_arChars[lIndex].pCharacterEx->GetLanguageID( &lLangID ) );

	return lLangID;
}

void CAgent::ShowDefaultCharacterProperties( long x, long y, BOOL bUseDefault )
{
	//AllowSetForegroundWindow();
	CheckAndThrow( m_pAgentEx->ShowDefaultCharacterProperties( x, y, (long)bUseDefault ) );
}

void CAgent::ShowAgentPropertySheet( BOOL bShow )
{
	IAgentPropertySheet *pPropertySheet;
	
	CheckAndThrow( m_pAgentEx->QueryInterface( IID_IAgentPropertySheet, (void**)&pPropertySheet ) );

	CheckAndThrow( pPropertySheet->SetVisible( bShow ) );
	
	pPropertySheet->Release();

}

void CAgent::GetVersion( short* pMajor, short* pMinor )
{
	CheckAndThrow( m_pAgentEx->GetVersion( pMajor ,pMinor ) );
}

long CAgent::GetIndexFromID (long lID, BOOL bThrow)
{
	if ( lID == -1 )
	{
		// The default character is wanted to be used
		return GetIndexFromID( m_lDefaultCharID );
	}

	// Then search the array
	for (int i=0; i<m_arChars.GetSize(); i++)
	{
		if ( m_arChars[i].lCharID == lID )
			return i;
	}

	if ( bThrow )
		Throw( -2147213310 ); // The character ID is not valid

	return -1;	// Return
}

void CAgent::Show(BOOL bShow, BOOL bAnimation, long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );

	if (bShow)
		CheckAndThrow( m_arChars[lIndex].pCharacterEx->Show ( !bAnimation, &m_arChars[lIndex].lReqID ) );
	else
		CheckAndThrow( m_arChars[lIndex].pCharacterEx->Hide ( !bAnimation, &m_arChars[lIndex].lReqID ) );
}


//-------------------------------------------
// Animation
//-------------------------------------------

void CAgent::Play(BSTR bstrAnimation, long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->Play( bstrAnimation, 
		&m_arChars[lIndex].lReqID ) );
}

void CAgent::Play(CString strAnimation, long lCharID)
{
	CComBSTR bstrAnimation = strAnimation;
	Play( bstrAnimation );
}

void CAgent::Stop(long lReqID, long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->Stop( lReqID ) );
}
void CAgent::StopAll ( long lType, long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->StopAll( lType ) );
}

void CAgent::GestureAt(int x, int y, long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->GestureAt(x, y,
		&m_arChars[lIndex].lReqID ) );
}

void CAgent::GetAnimationNames(BSTR* pAnimations, long lCharID)
{
	CString strAnimations;
	GetAnimationNames( &strAnimations, lCharID );

	*pAnimations = strAnimations.AllocSysString();
}

void CAgent::GetAnimationNames(CString* pAnimations, long lCharID)
{
	IEnumVARIANT *pEnum;
	VARIANT vAnimName;
	DWORD dwRetrieved;
	CString strAnimation;
	IUnknown *punkEnum;

	long lIndex = GetIndexFromID( lCharID );

	// Obtain enum interface pointers
	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->GetAnimationNames(&punkEnum) );
	CheckAndThrow ( punkEnum->QueryInterface(IID_IEnumVARIANT, (LPVOID *)&pEnum) );


	while (TRUE)
	{
		HRESULT hRes = pEnum->Next(1, &vAnimName, &dwRetrieved);

		if (hRes != NOERROR)
			break;
		else
			CheckAndThrow( hRes );

		// vAnimName.bstrVal is the animation name
		strAnimation += vAnimName.bstrVal;
		strAnimation += char(59);
			

		VariantClear(&vAnimName);
	} 

	SAFE_RELEASE( pEnum );
	SAFE_RELEASE( punkEnum );

	*pAnimations = strAnimation;//2.AllocSysString();
}

void CAgent::MoveTo(int x, int y, int nSpeed, long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->MoveTo(x, y, nSpeed,
		&m_arChars[lIndex].lReqID ) );
}

POINT CAgent::GetPosition(long lCharID)
{
	POINT point;
	long lIndex = GetIndexFromID( lCharID );

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->GetPosition( &point.x, &point.y ) );

	return point;
}

SIZE CAgent::GetSize ( long lCharID )
{
	long lIndex = GetIndexFromID( lCharID );
	SIZE size;

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->GetSize( &size.cx, &size.cy ) );

	return size;
}

BOOL CAgent::GetVisible(long lCharID)
{
	BOOL bVisible;
	long lIndex = GetIndexFromID( lCharID );

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->GetVisible( (long*)&bVisible ) );

	return bVisible;
}

void CAgent::GetCharacterInfo(BSTR* pName, BSTR* pDescription, long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );

	// First get the name
	if ( pName )
		CheckAndThrow ( m_arChars[lIndex].pCharacterEx->GetName( pName ) );

	// And then the description
	if ( pDescription )
		CheckAndThrow ( m_arChars[lIndex].pCharacterEx->GetDescription( pDescription ) );
}

void CAgent::GetCharacterInfo(CString* pName, CString* pDescription, long lCharID)
{
	BSTR bstrName, bstrDescription;

	GetCharacterInfo( ( pName ? &bstrName : NULL), ( pDescription ? &bstrDescription : NULL), lCharID);

	if ( pName ) *pName = bstrName;
	if ( pDescription ) *pDescription = bstrDescription;
}

void CAgent::Speak(BSTR bstrText, BSTR bstrURL, long lCharID)
{
	// Standart balloon
	long lIndex = GetIndexFromID( lCharID );

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->Speak(bstrText, bstrURL,
		&m_arChars[lIndex].lReqID ) );
}

void CAgent::Speak(CString strText, CString strURL, long lCharID)
{
	// Standart balloon
	CComBSTR bstrText = strText, bstrURL = strURL;
	Speak( bstrText, bstrURL, lCharID );
}

void CAgent::SetSoundEffects(BOOL bEnable, long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->SetSoundEffectsOn( bEnable ) );
}

BOOL CAgent::GetSoundEffects(long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );
	BOOL bEnable;

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->GetSoundEffectsOn( (long*)&bEnable ) );

	return bEnable;
}

void CAgent::SetIdle(BOOL bEnable, long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->SetIdleOn( bEnable ) );
}

BOOL CAgent::GetIdle(long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );
	BOOL bEnable;

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->GetIdleOn( (long*)&bEnable ) );

	return bEnable;
}

void CAgent::SetAutoPopupMenu(BOOL bEnable, long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->SetAutoPopupMenu( bEnable ) );
}

BOOL CAgent::GetAutoPopupMenu(long lCharID)
{
	long lIndex = GetIndexFromID( lCharID );
	BOOL bEnable;

	CheckAndThrow ( m_arChars[lIndex].pCharacterEx->GetAutoPopupMenu( (long*)&bEnable ) );

	return bEnable;
}

// ReqID && CBalloon && COptionBalloon
long CAgent::GetLastReqID ( long lCharID )
{
	long lIndex = GetIndexFromID( lCharID );

	return m_arChars[lIndex].lReqID;
}

CAgent::CBalloon* CAgent::GetBalloon ( long lCharID )
{
	long lIndex = GetIndexFromID( lCharID );

	return m_arChars[lIndex].pBalloon;
}

#ifdef BALLOONOPTION
	COptionBalloon* CAgent::GetOptionBalloon ( long lCharID )
	{
		long lIndex = GetIndexFromID( lCharID );

		return m_arChars[lIndex].pOptionBalloon;
	}
#endif

#ifdef BALLOONWEB
	CWebBalloon* CAgent::GetWebBalloon ( long lCharID )
	{
		long lIndex = GetIndexFromID( lCharID );

		return m_arChars[lIndex].pWebBalloon;
	}
#endif