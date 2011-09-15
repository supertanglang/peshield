//----------------------------------------------------------------------
//				 D  R  I  V  E  R      L  I  B  R  A  R  Y
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _SYS_H_
#define _SYS_H_

#include <windows.h>

#ifndef _T
#define _T(csz)	TEXT(csz)
#endif

//
// Pe Shield File
//
#define	PESHIELD_SYS_FILE		_T("PeShield.sys")
#define	PESHIELD_SYS_NAME		_T("PeShield")


//----------------------------------------------------------------------
//				D  R  I  V  E  R      S  U  P  P  O  R  T
//---------------------------------------------------------------------- 

//
// Load device driver
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL 
PeShield_LoadDriver(
	OUT HANDLE* phSys,
	IN HINSTANCE hInstance,
	IN WORD wSysResId,
	IN LPCTSTR szSysResType
	);

//
// Unload device driver
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID 
PeShield_UnloadDriver(
	IN OUT HANDLE* phSys OPTIONAL
	);


#endif // _SYS_H_