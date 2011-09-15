// NotifySink.cpp: implementation of the CNotifySink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Agent.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//-------------------------------------------
// Constructor/Destructor
//-------------------------------------------
CAgent::CNotifySink::CNotifySink()
{
	m_cRefs = 0;
	m_pAgent = NULL;
}

CAgent::CNotifySink::~CNotifySink()
{
	
}

//-------------------------------------------
// IUnknown methods
//-------------------------------------------

STDMETHODIMP CAgent::CNotifySink::QueryInterface (REFIID riid, LPVOID *ppv)
{
    *ppv = NULL;

    if ((riid == IID_IUnknown) || (riid == IID_IAgentNotifySink))
        *ppv = this;

    if (*ppv == NULL)
        return E_NOINTERFACE;

    ((LPUNKNOWN)*ppv)->AddRef();

    return NOERROR;
}


STDMETHODIMP_ (ULONG) CAgent::CNotifySink::AddRef()
{
	return ++m_cRefs;
}


STDMETHODIMP_(ULONG) CAgent::CNotifySink::Release()
{
	if (--m_cRefs != 0)
		return m_cRefs;

	delete this;
	return 0;
}

//-------------------------------------------
// IDispatch methods
//-------------------------------------------

STDMETHODIMP CAgent::CNotifySink::GetTypeInfoCount(UINT *pctInfo)
{
    *pctInfo = 1;
    return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::GetTypeInfo(UINT itInfo, LCID lcid, ITypeInfo **ppTypeInfo)
{
    HRESULT hRes;
    ITypeLib *pLib;

    *ppTypeInfo = NULL;

	if (itInfo != 0)
        return TYPE_E_ELEMENTNOTFOUND;

    if (ppTypeInfo == NULL)
        return E_POINTER;

	if ((PRIMARYLANGID(lcid) != LANG_NEUTRAL) && 
		(PRIMARYLANGID(lcid) != LANG_ENGLISH))
		return DISP_E_UNKNOWNLCID;

    hRes = LoadRegTypeLib(LIBID_AgentServerObjects, 
						  1, 
						  0,
						  PRIMARYLANGID(lcid), 
						  &pLib);
    if (FAILED(hRes))
        return hRes;

	hRes = pLib->GetTypeInfoOfGuid(IID_IAgentNotifySink, ppTypeInfo);
        
	pLib->Release();

    if (FAILED(hRes))
        return hRes;

    (*ppTypeInfo)->AddRef();

    return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::GetIDsOfNames(REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispID)
{
    HRESULT hRes;
    ITypeInfo *pInfo;

	// REFIID must be NULL

	if (riid != IID_NULL)
        return ResultFromScode(DISP_E_UNKNOWNINTERFACE);

	// Get the TypeInfo for the specified lcid

    hRes = GetTypeInfo(0, lcid, &pInfo);

	if (FAILED(hRes))
		return hRes;

	// Use the TypeInfo to get the DISPIDs of the specified names.
	// That's the whole point here.  Let TypeInfo do the work so
	// we don't have to.

	hRes = pInfo->GetIDsOfNames(rgszNames, cNames, rgDispID);

	pInfo->Release();

    return hRes;
}


STDMETHODIMP CAgent::CNotifySink::Invoke(DISPID dispID, REFIID riid, LCID lcid, 
									 unsigned short wFlags, DISPPARAMS *pDispParams, 
									 VARIANT *pVarResult, EXCEPINFO *pExcepInfo, 
									 UINT *puArgErr)
{
	HRESULT hRes;
	ITypeInfo *pInfo;

    // The riid parameter is always supposed to be IID_NULL

	if (riid != IID_NULL)
        return DISP_E_UNKNOWNINTERFACE;

	// Get the type info for the specified lcid

    hRes = GetTypeInfo(0, lcid, &pInfo);

    if (FAILED(hRes))
        return hRes;

    // Clear exceptions

    SetErrorInfo(0L, NULL);

	hRes = pInfo->Invoke(this, 
						 dispID, 
						 wFlags, 
						 pDispParams, 
						 pVarResult, 
						 pExcepInfo, 
						 puArgErr);


    pInfo->Release();

    return hRes;
}

//-------------------------------------------
// CAgent::CNotifySink methods
//-------------------------------------------

STDMETHODIMP CAgent::CNotifySink::Command(long dwCommandID, IUnknown * punkUserInput)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::ActivateInputState(long dwCharID, long bActivated)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::Restart()
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::Shutdown()
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::VisibleState(long dwCharID, long bVisible, long lCause)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::Click(long dwCharID, short fwKeys, long x, long y)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::DblClick(long dwCharID, short fwKeys, long x, long y)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::DragStart(long dwCharID, short fwKeys, long x, long y)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::DragComplete(long dwCharID, short fwKeys, long x, long y)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::RequestStart(long dwRequestID)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::RequestComplete(long dwRequestID, long hrStatus)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::BookMark(long dwBookMarkID)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::Idle(long dwCharID, long bStart)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::Move(long dwCharID, long x, long y, long lCause)
{
#if defined TIP || defined BALLOONOPTION || defined BALLOONWEB
	try 
	{
		POINT point = m_pAgent->GetPosition( dwCharID  );
		SIZE size = m_pAgent->GetSize( dwCharID  );
		CRect rectAgent ( point.x, point.y, point.x + size.cx, point.y + size.cy );
#ifdef TIP
		m_pAgent->GetTip( dwCharID )->Move ( rectAgent );
#endif
#ifdef BALLOONOPTION
		m_pAgent->GetOptionBalloon( dwCharID )->Move ( rectAgent );
#endif 
#ifdef BALLOONWEB
		m_pAgent->GetWebBalloon( dwCharID )->Move ( rectAgent );
#endif 
	}
	catch (int* pErr)
	{
		delete pErr;
	}
#endif
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::Size(long dwCharID, long lWidth, long lHeight)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::BalloonVisibleState(long dwCharID, long bVisible)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::HelpComplete(long dwCharID, long dwCommandID, long dwCause)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::ListeningState(long dwCharacterID, long bListenState, long dwCause)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::DefaultCharacterChange(BSTR bszGUID)
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::AgentPropertyChange()
{
	return NOERROR;
}


STDMETHODIMP CAgent::CNotifySink::ActiveClientChange(long dwCharID, long lStatus)
{
	return NOERROR;
}

//-------------------------------------------
// MyNotify
//-------------------------------------------

STDMETHODIMP CAgent::CNotifySink::OptionBalloon( long dwCharID, long lEvent )
{
	return NOERROR;
}

STDMETHODIMP CAgent::CNotifySink::WebBalloon( long dwCharID, long lEvent )
{
	return NOERROR;
}

//-------------------------------------------
// CALLBACKs
//-------------------------------------------

STDMETHODIMP CALLBACK CAgent::CNotifySink::OptionBalloonProc( long lEvent, long lAgent, long dwCharID )
{
	return ((CAgent*)lAgent)->m_pNotifySink->OptionBalloon( dwCharID, lEvent );
}
STDMETHODIMP CALLBACK CAgent::CNotifySink::WebBalloonProc( long lEvent, long lAgent, long dwCharID )
{
	return ((CAgent*)lAgent)->m_pNotifySink->WebBalloon( dwCharID, lEvent );
}