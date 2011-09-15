//----------------------------------------------------------------------
//			   S  E  R  V  I  C  E      L  I  B  R  A  R  Y
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _SERVICE_H_
#define _SERVICE_H_

#include "comm.h"

#include <windows.h>

#ifndef _T
#define _T(csz)	TEXT(csz)
#endif

//----------------------------------------------------------------------
//				  C  E  R  T  I  F  I  C  A  T  I  O  N
//----------------------------------------------------------------------

//
// Is exe certified ?
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_IsExeWebCertified(
	IN PPESHIELD_METADATA pMetaData
	);


//----------------------------------------------------------------------
//			      U  S  E  R       S  U  P  P  O  R  T
//---------------------------------------------------------------------- 

//
// Connect to web site
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_ConnectToWebSite(
	IN LPCSTR szWebAddress
	);


//----------------------------------------------------------------------
//			   P  R  O  C  E  S  S      S  U  P  P  O  R  T
//----------------------------------------------------------------------

//
// Terminate all processes by name
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID
PeShield_TerminateProcesses(
	IN LPCTSTR szName
	);


//----------------------------------------------------------------------
//			   F  I  L  E      M  A  N  A  G  E  M  E  N  T
//----------------------------------------------------------------------

//
// Delete file itself
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_DeleteSelf(
	IN LPCTSTR szPath
	);

// 
// Get secured file name
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_GetSecuredPath(
	OUT LPTSTR szSecuredPath,
	IN LPCTSTR szFilePath
);


#endif // _SERVICE_H_