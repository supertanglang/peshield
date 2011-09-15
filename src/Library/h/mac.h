//----------------------------------------------------------------------
//		M  A  C      A  D  D  R  E  S  S      L  I  B  R  A  R  Y
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _MAC_H_
#define _MAC_H_

#include <windows.h>

#ifndef _T
#define _T(csz)	TEXT(csz)
#endif

// Pe Shield Database
#define PESHIELD_DATABASE_NAME  _T("203.247.221.245")
#define PESHIELD_DATABASE_USER  _T("sa")
#define PESHIELD_DATABASE_PWD	_T("6561")

//
// Length and Max Size
//
#define LEN_MAC			12
#define LEN_HASH		20


//----------------------------------------------------------------------
//					 M  A  C     A  D  D  R  E  S  S
//---------------------------------------------------------------------- 

//
// Get mac address from system
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_GetMacAddressFromSystem(
	OUT LPSTR szMacAddress
	);

//
// Get mac address hash from database
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_GetMacHashFromDatabase(
	OUT PBYTE pBufMacHash,
	IN LPSTR szSerial
	);


#endif // _MAC_H_