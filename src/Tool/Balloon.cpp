// NotifySink.cpp: implementation of the CNotifySink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atlbase.h>
#include "Agent.h"


BOOL CAgent::CBalloon::GetEnabled()
{
	BOOL bEnabled;
	CheckAndThrow( m_pBalloon->GetEnabled( (long*)&bEnabled ) );

	return bEnabled;
}

long CAgent::CBalloon::GetNumLines()
{
	long lLines;
	CheckAndThrow( m_pBalloon->GetNumLines( &lLines ) );

	return lLines;
}

long CAgent::CBalloon::GetNumCharsPerLine()
{
	long lChars;
	CheckAndThrow( m_pBalloon->GetNumCharsPerLine( &lChars ) );

	return lChars;
}

void CAgent::CBalloon::SetNumLines ( long lLines )
{
	CheckAndThrow( m_pBalloon->SetNumLines( lLines ) );
}

void CAgent::CBalloon::SetNumCharsPerLine ( long lChars )
{
	CheckAndThrow( m_pBalloon->SetNumCharsPerLine( lChars ) );
}

//-------------------------------------------
// Font and colors
//-------------------------------------------

CString CAgent::CBalloon::GetFontName ()
{
	CComBSTR bstrFont;
	CheckAndThrow( m_pBalloon->GetFontName( &bstrFont ) );
	
	return (CString)bstrFont;
}

void CAgent::CBalloon::SetFontName ( CString strFontName )
{
	CheckAndThrow( m_pBalloon->SetFontName( CComBSTR( strFontName ) ) );
}

long CAgent::CBalloon::GetFontSize ()
{
	long lSize;
	CheckAndThrow( m_pBalloon->GetFontSize( &lSize ) );
	
	return lSize;
}

void CAgent::CBalloon::SetFontSize ( long lSize )
{
	CheckAndThrow( m_pBalloon->SetFontSize( lSize ) );
}

short CAgent::CBalloon::GetFontCharSet ()
{
	short sCharSet;
	CheckAndThrow( m_pBalloon->GetFontCharSet( &sCharSet ) );
	
	return sCharSet;
}

void CAgent::CBalloon::SetFontCharSet ( short sCharSet )
{
	CheckAndThrow( m_pBalloon->SetFontSize( sCharSet ) );
}

BOOL CAgent::CBalloon::GetFontBold ()
{
	BOOL bBold;
	CheckAndThrow( m_pBalloon->GetFontBold( (long*)&bBold ) );
	
	return bBold;
}

BOOL CAgent::CBalloon::GetFontItalic ()
{
	BOOL bItalic;
	CheckAndThrow( m_pBalloon->GetFontItalic( (long*)&bItalic ) );
	
	return bItalic;
}

BOOL CAgent::CBalloon::GetFontStrikethru ()
{
	BOOL bStrikethru;
	CheckAndThrow( m_pBalloon->GetFontStrikethru( (long*)&bStrikethru ) );
	
	return bStrikethru;
}

BOOL CAgent::CBalloon::GetFontUnderline ()
{
	BOOL bUnderline;
	CheckAndThrow( m_pBalloon->GetFontUnderline( (long*)&bUnderline ) );
	
	return bUnderline;
}

//-------------------------------------------
// Colors
//-------------------------------------------

COLORREF CAgent::CBalloon::GetForeColor ()
{
	COLORREF crFore;
	CheckAndThrow( m_pBalloon->GetForeColor( (long*)&crFore ) );
	
	return crFore;
}

COLORREF CAgent::CBalloon::GetBackColor ()
{
	COLORREF crBack;
	CheckAndThrow( m_pBalloon->GetBackColor( (long*)&crBack ) );
	
	return crBack;
}

COLORREF CAgent::CBalloon::GetBorderColor ()
{
	COLORREF crBorder;
	CheckAndThrow( m_pBalloon->GetBorderColor( (long*)&crBorder ) );
	
	return crBorder;
}

BOOL CAgent::CBalloon::GetVisible ( )
{
	BOOL bVisible;
	CheckAndThrow( m_pBalloon->GetVisible( (long*)&bVisible ) );
	
	return bVisible;
}

void CAgent::CBalloon::SetVisible ( BOOL bVisible )
{
	CheckAndThrow( m_pBalloon->SetVisible( bVisible ) );
}

DWORD CAgent::CBalloon::GetStyle ( )
{
	DWORD dwStyle;
	CheckAndThrow( m_pBalloon->GetStyle( (long*)&dwStyle ) );
	
	return dwStyle;
}

void CAgent::CBalloon::SetStyle ( DWORD dwStyle )
{
	CheckAndThrow( m_pBalloon->SetStyle( dwStyle ) );
}