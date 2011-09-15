//----------------------------------------------------------------------
//				C  O  M  M  O  N      L  I  B  R  A  R  Y
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _COMM_H_
#define _COMM_H_

#include <windows.h>

#ifndef _T
#define _T(csz)	TEXT(csz)
#endif

//
// Senol Signature
//
#define SENOL_SIGNATURE	0x4E53	// SN

//
// Length and Max Size
//
#define MAX_MESSAGE		512
#define MAX_URL			255
#define LEN_SERIAL		5
#define LEN_MAC			12
#define LEN_HASH		20

//
// Pe Shield Window
//
#define PESHIELD_LOADER_WNDCLASS	_T("Senol.PeShield.Loader.Class")
#define PESHIELD_LOADER_WINDOW		_T("Senol.PeShield.Loader.Window")
#define WM_PESHIELD_CREATE			(WM_USER + 100)

//
// Pe Shield File
//
#define PESHIELD_SPY_FILE		_T("d")
#define PESHIELD_LOADER_FILE	_T("m")

//
// Common Kernel Objects
#define PESHIELD_CREATE_SECTION	_T("Senol.PeShield.Create.Section")
#define PESHIELD_CREATE_MUTEX	_T("Senol.PeShield.Create.Mutex")


//----------------------------------------------------------------------
//                   M  E  T  A      D  A  T  A
//---------------------------------------------------------------------- 

//
// Pe Shield Meta Data Structure
//
#pragma pack(1)
typedef struct _PESHIELD_METADATA {
	WORD	Signature;
	BOOL	Certified;
	
	DWORD	SpyOffset;
	DWORD	SpyLength;

	DWORD	LoaderOffset;
	DWORD	LoaderLength;

	DWORD	ExeOffset;
	DWORD	ExeLength;

	CHAR	Serial[LEN_SERIAL+1];
	CHAR	WebAddress[MAX_URL+1];
} PESHIELD_METADATA, *PPESHIELD_METADATA;
#pragma pack()

//
// Size of Meta Data Structure
//
#define SIZEOF_PESHIELD_METADATA	sizeof(PESHIELD_METADATA)

//
// Get meta data from exe file
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
PPESHIELD_METADATA
PeShield_GetMetaData(
	IN CONST PBYTE pBufExe,
	IN DWORD cbExe
	);

//
// Is meta data valid ?
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_IsMetaDataValid(
	IN PPESHIELD_METADATA pMetaData,
	IN DWORD cbExe
	);


//----------------------------------------------------------------------
//				  C  O  M  M  O  N       T  Y  P  E  S
//---------------------------------------------------------------------- 

//
// What is Pe Shield Function result in ?
//
#define PESHIELD_SUCCESS(status)	(status == 0)
#define PESHIELD_FAIL(status)		(status > 0)
#define PESHIELD_ERROR(status)		(status < 0)

//
// Pe Shield Create Request
//
typedef struct _PESHIELD_CREATE_REQUEST {
	CHAR	ExePath[MAX_PATH+1];
	CHAR	CmdLine[MAX_PATH+1];
} PESHIELD_CREATE_REQUEST, *PPESHIELD_CREATE_REQUEST;


#endif // _COMM_H_