//----------------------------------------------------------------------
//			  P  E  S  H  I  E  L  D       L  O  A  D  E  R
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _M_H_
#define _M_H_

#ifdef UNICODE
#error Pe Shield Loader doesn't support unicode !
#endif // UNICODE


//----------------------------------------------------------------------
//					      W  I  N  D  O  W  S
//----------------------------------------------------------------------

//
// Window procedure
//
LRESULT 
CALLBACK 
PeShield_WindowProc(
   IN HWND hWnd, 
   IN UINT iMessage, 
   IN WPARAM wParam, 
   IN LPARAM lParam
   );

//
// Message handling for WM_CREATE
//
BOOL
PeShield_OnCreate(
	VOID
	);

//
// Message handling for  WM_DESTROY
//
VOID
PeShield_OnDestroy(
	VOID
	);

//
// Message handling for  WM_PESHIELD_CREATE
//
BOOL
PeShield_OnPeShieldCreate(
	VOID
	);


//----------------------------------------------------------------------
//				I  N  I  T  I  A  L  I  Z  A  T  I  O  N
//---------------------------------------------------------------------- 

//
// Initialize Pe Shield Loader data
//
BOOL
PeShield_LoaderStartup(
	IN HINSTANCE hInstance
	);

//
// Uninitialize Pe Shield Loader data
//
VOID
PeShield_LoaderCleanup(
	VOID
	);


//----------------------------------------------------------------------
//				  C  E  R  T  I  F  I  C  A  T  I  O  N
//----------------------------------------------------------------------

//
// Certification status
//
typedef enum {
	CERTIFY_SUCCEEDED			= 0,	// Success	: File is certified
	CERTIFY_NO_WEB_CERTIFIED	= 1,	// Fail		: File is not web-certified
	CERTIFY_NO_SYSTEM_MAC		= 2,	// Fail		: System max address is not got
	CERTIFY_DATABASE_OFFLINE	= 3,	// Fail		: Database is off-line
	CERTIFY_MACHASH_UNMATCHED	= 4,	// Fail		: Mac address hashes don't match
}CERTIFY_STATUS, *PCERTIFY_STATUS;

//
// Certify exe
//
CERTIFY_STATUS
PeShield_CertifyResult(
	IN PPESHIELD_METADATA pMetaData
	);


//----------------------------------------------------------------------
//						  P  R  O  C  E  S  S
//----------------------------------------------------------------------

//
// Create first process
//
BOOL
PeShield_CreateFirstProcess(
	IN OUT HANDLE* hFileExe,
	IN OUT HANDLE* hMapExe,
	IN OUT PBYTE* pBufExe,
	IN OUT PPESHIELD_METADATA* pMetaData,
	IN LPCSTR szExePath,
	IN LPCSTR szCmdLine,
	OUT PDWORD dwProcessId,
	OUT HANDLE* hThread
	);

//
// Decrypt process
//
BOOL
PeShield_DecryptProcess(
	IN DWORD dwProcessId,
	IN PVOID pTxtAddr,
	IN DWORD cbTxtSize
	);

//
// Clear process information
//
VOID
PeShield_ClearProcessInfo(
	IN DWORD ProcessIndex
	);


//----------------------------------------------------------------------
//			T  H  R  E  A  D      M  A  N  A  G  E  M  E  N  T
//----------------------------------------------------------------------

//
// Create thread parameter
//
typedef struct _PESHIELD_CREATE_PARAM {
	DWORD	ProcessIndex;
	CHAR	CmdLine[MAX_PATH+1];
} PESHIELD_CREATE_PARAM, *PPESHIELD_CREATE_PARAM;

//
// Run thread parameter
//
typedef struct _PESHIELD_RUN_PARAM {
	DWORD	ProcessIndex;
	DWORD	ProcessId;
} PESHIELD_RUN_PARAM, *PPESHIELD_RUN_PARAM;

//
// Create thread procedure
//
UINT
WINAPI
PeShield_CreateThreadProc(
	IN PVOID Param
	);

//
// Run Request thread procedure
//
UINT
WINAPI
PeShield_RunRequestThreadProc(
	IN PVOID Param
	);

//
// Run thread procedure
//
UINT
WINAPI
PeShield_RunThreadProc(
	IN PVOID Param
	);


#endif // _M_H_