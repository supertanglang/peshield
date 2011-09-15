// Sink.h: interface for the CSink class.
//
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------
//			  P  E  S  H  I  E  L  D       A  G  E  N  T
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: HAM DON WOOK
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------


#if !defined(AFX_SINK_H__25321F40_C54D_4B39_A484_2C1C59A64566__INCLUDED_)
#define AFX_SINK_H__25321F40_C54D_4B39_A484_2C1C59A64566__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Agent.h"

class CSink : public CAgent::CNotifySink
{
public:
	STDMETHODIMP Command(long dwCommandID, IUnknown * punkUserInput);
	STDMETHODIMP ActivateInputState(long dwCharID, long bActivated);
	STDMETHODIMP VisibleState(long dwCharID, long bVisible, long lCause);
	STDMETHODIMP Click(long dwCharID, short fwKeys, long x, long y);
	STDMETHODIMP DblClick(long dwCharID, short fwKeys, long x, long y);
	STDMETHODIMP DragStart(long dwCharID, short fwKeys, long x, long y);
	STDMETHODIMP DragComplete(long dwCharID, short fwKeys, long x, long y);
	STDMETHODIMP RequestStart(long dwRequestID);
	STDMETHODIMP RequestComplete(long dwRequestID, long hrStatus);
	STDMETHODIMP BookMark(long dwBookMarkID);
	STDMETHODIMP Idle(long dwCharID, long bStart);
	STDMETHODIMP Move(long dwCharID, long x, long y, long lCause);
	STDMETHODIMP Size(long dwCharID, long lWidth, long lHeight);
	STDMETHODIMP BalloonVisibleState(long dwCharID, long bVisible);

	STDMETHODIMP HelpComplete(long dwCharID, long dwCommandID, long dwCause);
	STDMETHODIMP ListeningState(long dwCharacterID, long bListenState, long dwCause);
	STDMETHODIMP DefaultCharacterChange(BSTR bszGUID);
	STDMETHODIMP AgentPropertyChange();
	STDMETHODIMP ActiveClientChange(long dwCharID, long lStatus);

	STDMETHODIMP Tip( long dwCharID, long lEvent );
	STDMETHODIMP OptionBalloon( long dwCharID, long lEvent );
	STDMETHODIMP WebBalloon( long dwCharID, long lEvent );

	BOOL bChecked;
	CString strWeb;
	CSink();
	virtual ~CSink();
};

#endif // !defined(AFX_SINK_H__25321F40_C54D_4B39_A484_2C1C59A64566__INCLUDED_)
