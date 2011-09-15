//----------------------------------------------------------------------
//				P  E  S  H  I  E  L  D       S  P  Y
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _D_H_
#define _D_H_

#ifdef UNICODE
#error Pe Shield Spy doesn't support unicode !
#endif // UNICODE

//----------------------------------------------------------------------
//				I  N  I  T  I  A  L  I  Z  A  T  I  O  N
//---------------------------------------------------------------------- 

//
// Initialize Pe Shield Spy data
//
BOOL
PeShield_SpyStartup(
	IN HINSTANCE hInstance
	);

//
// Uninitialize Pe Shield Spy data
//
VOID
PeShield_SpyCleanup();


//----------------------------------------------------------------------
//				L  O  A  D  E  R	   W  I  N  D  O  W
//----------------------------------------------------------------------

//
// Load Pe Shield Loader
//
BOOL
PeShield_LoadLoader(
	OUT HWND* phWndLoader
	);

//
// Send WM_PESHIELD_CREATE to Loader
//
BOOL 
PeShield_SendCreate(
	IN HWND hWndLoader,
	IN LPCSTR lpCmdLine
	);


//----------------------------------------------------------------------
//				  S  T  R  I  N  G       T  A  B  L  E
//----------------------------------------------------------------------

#define IDS_NO_CREATE_SEND	"Pe Shield Loader���� �޼��� ���ۿ� �����߽��ϴ�."
#define IDS_NO_LOADER_LOAD	"Pe Shield Loader�� �ε����� ���߽��ϴ�."
#define IDS_NO_STARTUP		"Pe Shield Spy�� �ε����� ���߽��ϴ�."

#endif // _D_H_