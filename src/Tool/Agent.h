//----------------------------------------------------------------------
//			  P  E  S  H  I  E  L  D       A  G  E  N  T
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: HAM DON WOOK
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#if !defined(gS_MsAgent_DEFINED)
#define gS_MsAgent_DEFINED

// compilation consts
#define BALLOONOPTION	// Option MFC balloon
#define BALLOONWEB		// Web MFC balloon


// MsAgent
#include "StdAfx.h"
#include "AgtErr.h"

// CArray
#include <afxtempl.h>

#ifdef BALLOONOPTION
	#include "OptionBalloon.h"	
#endif
#ifdef BALLOONWEB
	#include "WebBalloon.h"	
#endif

void CheckAndThrow (int nCode);
void Throw (int nCode);

class CAgent
{
public:

	class CBalloon
	{
	public:
		CBalloon() {};
		~CBalloon() {};
		
		BOOL GetEnabled ();

		long GetNumLines ();
		long GetNumCharsPerLine ();
		inline void SetNumLines ( long lLines );
		inline void SetNumCharsPerLine ( long lChars );

		CString GetFontName ();
		inline void SetFontName ( CString strFontName );
		long GetFontSize ();
		inline void SetFontSize ( long lSize );
		short GetFontCharSet ();
		inline void SetFontCharSet ( short sCharSet );

		BOOL GetFontBold ();
		BOOL GetFontItalic ();
		BOOL GetFontStrikethru ();
		BOOL GetFontUnderline ();

		COLORREF GetForeColor ();
		COLORREF GetBackColor ();
		COLORREF GetBorderColor ();

		BOOL GetVisible ( );
		inline void SetVisible ( BOOL bVisible );

		DWORD GetStyle ( );
		inline void SetStyle ( DWORD dwStyle );

	public:
		IAgentBalloonEx* m_pBalloon;
	};

private:
	// This structure contains main characters info
	typedef struct tagCHAR
	{
		IAgentCharacterEx* pCharacterEx;
		long lCharID, lReqID;
		CBalloon* pBalloon;

	#ifdef BALLOONOPTION
		COptionBalloon* pOptionBalloon;// MFC Option balloon
	#endif
	#ifdef BALLOONWEB
		CWebBalloon* pWebBalloon;// MFC Web balloon
	#endif
		
	}tagCHAR;
public:
	
	class CNotifySink : public IAgentNotifySinkEx
	{
	public:
		CNotifySink();
		~CNotifySink();

		STDMETHODIMP QueryInterface(REFIID, LPVOID FAR *);
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		STDMETHODIMP GetTypeInfoCount(UINT *);
		STDMETHODIMP GetTypeInfo(UINT, LCID, ITypeInfo **);
		STDMETHODIMP GetIDsOfNames(REFIID, OLECHAR **, UINT, LCID, DISPID *);
		STDMETHODIMP Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS *, VARIANT *, EXCEPINFO *, UINT *);

		virtual STDMETHODIMP Command(long dwCommandID, IUnknown * punkUserInput);
		virtual STDMETHODIMP ActivateInputState(long dwCharID, long bActivated);
		virtual STDMETHODIMP Restart();
		virtual STDMETHODIMP Shutdown();
		virtual STDMETHODIMP VisibleState(long dwCharID, long bVisible, long lCause);
		virtual STDMETHODIMP Click(long dwCharID, short fwKeys, long x, long y);
		virtual STDMETHODIMP DblClick(long dwCharID, short fwKeys, long x, long y);
		virtual STDMETHODIMP DragStart(long dwCharID, short fwKeys, long x, long y);
		virtual STDMETHODIMP DragComplete(long dwCharID, short fwKeys, long x, long y);
		virtual STDMETHODIMP RequestStart(long dwRequestID);
		virtual STDMETHODIMP RequestComplete(long dwRequestID, long hrStatus);
		virtual STDMETHODIMP BookMark(long dwBookMarkID);
		virtual STDMETHODIMP Idle(long dwCharID, long bStart);
		virtual STDMETHODIMP Move(long dwCharID, long x, long y, long lCause);
		virtual STDMETHODIMP Size(long dwCharID, long lWidth, long lHeight);
		virtual STDMETHODIMP BalloonVisibleState(long dwCharID, long bVisible);

		virtual STDMETHODIMP HelpComplete(long dwCharID, long dwCommandID, long dwCause);
		virtual STDMETHODIMP ListeningState(long dwCharacterID, long bListenState, long dwCause);
		virtual STDMETHODIMP DefaultCharacterChange(BSTR bszGUID);
		virtual STDMETHODIMP AgentPropertyChange();
		virtual STDMETHODIMP ActiveClientChange(long dwCharID, long lStatus);

#ifdef BALLOONOPTION
		virtual STDMETHODIMP OptionBalloon( long dwCharID, long lEvent );
		static STDMETHODIMP CALLBACK OptionBalloonProc( long lRetVal, long lAgent, long dwCharID );
#endif
#ifdef BALLOONWEB
		virtual STDMETHODIMP WebBalloon( long dwCharID, long lEvent );
		static STDMETHODIMP CALLBACK WebBalloonProc( long lRetVal, long lAgent, long dwCharID );
#endif
		CAgent *m_pAgent;
	private:
		ULONG	m_cRefs;
	};

	BOOL SetCommand(long lCharID,BOOL start, BOOL all, BOOL init);
	BOOL ByeBye(long lCharID);
	// Initialize/uninitialize
	void Init ( CNotifySink* pNotifySink = NULL);
	void Close ();
	void LoadCharacter ( CString strChar, long *pCharID );
	void UnloadCharacter ( long lCharID);
	BOOL IsInit ();

	/*inline */void ShowDefaultCharacterProperties ( long x, long y, BOOL bUseDefault );
	void ShowAgentPropertySheet ( BOOL bShow );

	void GetVersion ( short* pMajor, short* pMinor );

	long GetIndexFromID (long lID, BOOL bThrow = TRUE);

	void Show ( BOOL bShow = TRUE, BOOL bAnimation = TRUE, long lCharID = -1 );

	void Play ( BSTR bstrAnimation, long lCharID = -1 );
	void Play ( CString strAnimation, long lCharID = -1 );

	void GetAnimationNames ( BSTR* pAnimations, long lCharID = -1 );
	void GetAnimationNames ( CString* pAnimations, long lCharID = -1 );

	void Stop ( long lRequestID, long lCharID = -1);
	void StopAll ( long lType = STOP_TYPE_PLAY, long lCharID = -1);	// NOTE: see top of the file for available values

	void GestureAt ( int x, int y, long lCharID = -1);
	void MoveTo ( int x, int y, int nSpeed=1000, long lCharID = -1); // nSpeed=0 - no animation

	POINT GetPosition ( long lCharID = -1 );
	SIZE GetSize ( long lCharID = -1 );
	BOOL GetVisible ( long lCharID = -1 );

	void GetCharacterInfo ( BSTR* pName, BSTR* pDescription, long lCharID = -1);
	void GetCharacterInfo ( CString* pName, CString* pDescription, long lCharID = -1);

	void Speak ( BSTR bstrText, BSTR bstrURL = NULL, long lCharID = -1);	// NOTE: (b)strURL has not been 
	void Speak ( CString strText, CString strURL = "", long lCharID = -1);// implemented in MFC balloon yet

	void SetSoundEffects ( BOOL bEnable, long lCharID = -1);
	BOOL GetSoundEffects ( long lCharID = -1);
	void SetIdle ( BOOL bEnable, long lCharID = -1);
	BOOL GetIdle ( long lCharID = -1);
	void SetAutoPopupMenu ( BOOL bEnable, long lCharID = -1);
	BOOL GetAutoPopupMenu ( long lCharID = -1);

	// Language functions
	void SetLanguageID ( long lLangID, long lCharID = -1); // NOTE: all ID are enumed at the beginning of the header file
	long GetLanguageID ( long lCharID = -1);

	// Balloon


	// ReqID && CBalloon && COptionBalloon
	long GetLastReqID ( long lCharID = -1 );
	CAgent::CBalloon* GetBalloon ( long lCharID = -1 );

#ifdef BALLOONOPTION
	COptionBalloon* GetOptionBalloon ( long lCharID = -1 );
#endif
#ifdef BALLOONWEB
	CWebBalloon* GetWebBalloon ( long lCharID = -1 );
#endif

	// Error info

	// Constructor/Destructor
	CAgent();
	virtual ~CAgent();

public:
	long m_lDefaultCharID;				// 디폴트 캐릭터 ID
	CNotifySink* m_pNotifySink;			// NotifySink pointer if set
	HRESULT hRes;                       // 수행 결과 검사용
	long m_commandExit;					// 메뉴 CommandID
	long m_commandEncode;
	long m_commandDecode;
	long m_commandOption;
	long m_commandHelp;
	long m_exitID;
	IAgentCommands *pCommand;

private:
	CArray<tagCHAR, tagCHAR> m_arChars;	// The array contains characters' IDs & pointers
	IAgentEx* m_pAgentEx;				// MsAgent main pointer
	long m_lNotifySinkID;				// NotifySink ID (used to unregister)
};

#endif // !defined( gS_MsAgent_DEFINED )

