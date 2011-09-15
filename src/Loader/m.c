#include "../Library/h/comm.h"
#include "../Library/h/io.h"
#include "../Library/h/pe.h"
#include "../Library/h/crypto.h"
#include "../Library/h/mac.h"
#include "../Library/h/sys.h"
#include "../Library/h/array.h"
#include "../Library/h/service.h"
#include "../Sys/IoCtlCode.h"

#include "m.h"

#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <winioctl.h>
#include <io.h>
#include "resource.h"

//----------------------------------------------------------------------
//			 				D  E  B  U  G
//----------------------------------------------------------------------

//
// Debug options
//
#define _PESHIELD_LOG			0	// log all process
#define _PESHIELD_NO_CERTIFY	0	// exclude certification
#define _PESHIELD_NO_DECRYPT	0	// exclude decryption

//
// Log file information
//
#if _PESHIELD_LOG
#define	PESHIELD_LOG_FILE	"PeShield_Loader.log"
static HANDLE	g_hFileLog = INVALID_HANDLE_VALUE;
static CHAR		g_szLogPath[MAX_PATH+1];
static DWORD	g_cbWrittenLog;
static CHAR		g_szLog[1024];
#endif // _PESHIELD_LOG

//
// Log short cut
//
#if _PESHIELD_LOG
#define PESHIELD_LOG(sz)	if (g_hFileLog != INVALID_HANDLE_VALUE)	{									\
							strcpy(g_szLog, sz); strcat(g_szLog, "\r\n");								\
							WriteFile(g_hFileLog, g_szLog, strlen(g_szLog), &g_cbWrittenLog, NULL); }	
#else
#define PESHIELD_LOG(sz)
#endif // _PESHIELD_LOG


//----------------------------------------------------------------------
//			 G  L  O  B  A  L     V  A  R  I  A  B  L  E  S
//----------------------------------------------------------------------

//
// Pe Shield Loader data
//
static HINSTANCE	g_hInstance;
static HANDLE		g_hWndLoader;
static CHAR			g_szLoaderPath[MAX_PATH+1];

//
// Pe Shield Loader Initialization Flag
//
static BOOL			g_bInitLoader;

//
// WinSock2 data
//
static WSADATA		g_wsa;

//
// Create Request data
//
static HANDLE	g_hMutexCreate;
static HANDLE	g_hSectionCreate;
static PPESHIELD_CREATE_REQUEST	g_CreateRequest;

//
// Run Request thread data
//
static HANDLE	g_hThreadRunRequest;
static BOOL		g_bRunRequestThread;

//
// Run Event data
//
static HANDLE	g_hEventRunRequest;
static HANDLE	g_hEventRunComplete;

//
// Run request data
//
static PPESHIELD_RUN_REQUEST	g_RunRequest;

//
// Pe Shield Driver data
//
static HANDLE	g_hSys = INVALID_HANDLE_VALUE;

//
// Message for user
//
static CHAR	g_szMessage[MAX_MESSAGE+1];


//----------------------------------------------------------------------
//					      W  I  N  D  O  W  S
//----------------------------------------------------------------------

//
// Short cuts for message box
//
#define MESSAGE_SUCCESS(id)	if (g_hInstance && LoadString(g_hInstance, id, g_szMessage, MAX_MESSAGE)){	\
							MessageBox(HWND_DESKTOP, g_szMessage, "Pe Shield Loader", MB_ICONINFORMATION);}
#define MESSAGE_FAIL(id)	if (g_hInstance && LoadString(g_hInstance, id, g_szMessage, MAX_MESSAGE)){	\
							MessageBox(HWND_DESKTOP, g_szMessage, "Pe Shield Loader", MB_ICONWARNING);}
#define MESSAGE_TEST(id)	if (g_hInstance && LoadString(g_hInstance, id, g_szMessage, MAX_MESSAGE)){	\
							MessageBox(HWND_DESKTOP, g_szMessage, "Pe Shield Loader", MB_OK);}
#ifdef _DEBUG
#define MESSAGE_ERROR(id)	if (g_hInstance && LoadString(g_hInstance, id, g_szMessage, MAX_MESSAGE)){	\
							MessageBox(HWND_DESKTOP, g_szMessage, "Pe Shield Loader", MB_ICONERROR);}
#else
#define MESSAGE_ERROR(id)
#endif // _DEBUG

//
// Windows application entry point
//
INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, INT nCmdShow)
{
	WNDCLASS	WndClass = {0};
	HWND		hWnd;	
	MSG			Message;
	
	// Initialize Pe Shield Loader data
	if (!PeShield_LoaderStartup(hInstance))
	{
		MESSAGE_ERROR(IDS_NO_STARTUP);
		return 1;
	}

	// Register window class
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)PeShield_WindowProc;
	WndClass.lpszClassName = PESHIELD_LOADER_WNDCLASS;
	if (RegisterClass(&WndClass))
	{
		// Create window
		hWnd = CreateWindow(
			PESHIELD_LOADER_WNDCLASS, 
			PESHIELD_LOADER_WINDOW, 
			WS_OVERLAPPEDWINDOW, 
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
			NULL,
			(HMENU)NULL,
			hInstance, 
			NULL
			);
		if (!hWnd)
		{
			Message.wParam = 1;
			PESHIELD_LOG("Fail to create window.");
			goto Cleanup;
		}
	}
	else
	{
		Message.wParam = 1;
		PESHIELD_LOG("Fail to register window class.");
		goto Cleanup;
	}

	// Message loop
	while(GetMessage(&Message, 0, 0, 0)) 
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

Cleanup:

	// Uninitialize Pe Shield Loader data
	PeShield_LoaderCleanup();

	return Message.wParam;
}

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
	)
{
	switch(iMessage)
	{
	// Window is created	
	case WM_CREATE:

		// Set global window handle
		g_hWndLoader = hWnd;

		return (PeShield_OnCreate()) ? 0 : 1;

	// Pe Shield process creation request
	case WM_PESHIELD_CREATE:

		return (PeShield_OnPeShieldCreate()) ? 0 : 1;

	// Window is destroyed
	case WM_DESTROY:

		PeShield_OnDestroy();

		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}

//
// Message handling for WM_CREATE
//
BOOL
PeShield_OnCreate(
	VOID
	)
{
	BOOL bSucceeded = FALSE;

	PESHIELD_LOG("Information : Window message = WM_CREATE");
	
	// Set Run Request thread on
	g_bRunRequestThread = TRUE;
	
	// Begin Run Request thread
	g_hThreadRunRequest = (HANDLE)_beginthreadex(NULL, 0, PeShield_RunRequestThreadProc, NULL, 0, NULL);
	if (g_hThreadRunRequest)
	{
		PESHIELD_LOG("Success : Run Request thread is begun.");
		
		// Set success
		bSucceeded = TRUE;
	}
	else
	{
		PESHIELD_LOG("Error : Run Request thread is not begun.");
	}	
	
	// Is succeeded ?
	if (!bSucceeded)
	{
		// Close all data
		PeShield_OnDestroy();
	}

	return bSucceeded;
}

//
// Message handling for  WM_DESTROY
//
VOID
PeShield_OnDestroy(
	VOID
	)
{
	DWORD	i;
	PPESHIELD_PROCESS_INFO	ProcessInfo;
			
			
	PESHIELD_LOG("Information : Window message = WM_DESTROY");

	// Set Run Request thread off
	g_bRunRequestThread = FALSE;

	// Wait for Run Request thread's end	
	if (g_hThreadRunRequest)
	{
		// Set Run Request Event to finish thread
		if (g_hEventRunRequest)
		{
			SetEvent(g_hEventRunRequest);
			PESHIELD_LOG("Success : Run Request Event is signaled to finish thread");
		}
		
		PESHIELD_LOG("Information : Wait for Run Request Thread's end");
		WaitForSingleObject(g_hThreadRunRequest, INFINITE);
		
		// Close Run Request thread
		CloseHandle(g_hThreadRunRequest);
		g_hThreadRunRequest = NULL;
		PESHIELD_LOG("Success : Run Request thread is closed.");
	}

	// Clear all processes remain just in case
	for (i = 0; i < PESHIELD_MAX_PROCESS; i++)
	{
		ProcessInfo = PeShield_ArrayData(i);
		if (ProcessInfo->RefCount > 0)
		{
			// Clear process information
			PeShield_ClearProcessInfo(i);
		}
	}
}

//
// Message handling for WM_PESHIELD_CREATE
//
BOOL
PeShield_OnPeShieldCreate(
	VOID
	)
{
	PESHIELD_PROCESS_INFO	NewProcess;
	PPESHIELD_CREATE_PARAM	CreateParam;
	DWORD					ProcessIndex;

	PESHIELD_LOG("Information : Window message = WM_PESHIELD_CREATE");

	// Allocate create thread parameter
	CreateParam = (PPESHIELD_CREATE_PARAM)malloc(sizeof(PESHIELD_CREATE_PARAM));
	if (CreateParam)
	{
		PESHIELD_LOG("Success : Create thread parameter is allocated.");
		
		// Is Process List full ?
		if (PeShield_ArrayIsFull())
		{
			PESHIELD_LOG("Fail : Process List is full.");
			
			// Free create thread parameter
			free(CreateParam);
			
			// Notice Process List is full
			MESSAGE_FAIL(IDS_PROCESS_FULL);
			
			return FALSE;
		}
		
		// Set new process information
		strcpy(NewProcess.ExePath, g_CreateRequest->ExePath);
		NewProcess.RefCount = 0;	// reference count is zero
		
		// Insert new process information into Process List
		ProcessIndex = PeShield_ArrayInsert(&NewProcess);
		PESHIELD_LOG("Success : New Process information is inserted.");
		
		// Set create thread parameter
		CreateParam->ProcessIndex	= ProcessIndex;
		strcpy(CreateParam->CmdLine, g_CreateRequest->CmdLine);
		PESHIELD_LOG("Success : Create parameter is set.");
		
		// Begin create thread ( close the handle no need )
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, PeShield_CreateThreadProc, CreateParam, 0, NULL));
	}
	else
	{
		PESHIELD_LOG("Error : Create thread parameter is not allocated.");
	}
	
	return TRUE;
}


//----------------------------------------------------------------------
//				I  N  I  T  I  A  L  I  Z  A  T  I  O  N
//----------------------------------------------------------------------

//
// Initialize Pe Shield Loader data
//
BOOL
PeShield_LoaderStartup(
	IN HINSTANCE hInstance
	)
{
	BOOL	bInitialized = FALSE;

#if _PESHIELD_LOG

	CHAR	szLoaderDirectory[MAX_PATH+1];

#endif // _PESHIELD_LOG

	DWORD					IoCtlResult;
	DWORD					dwLoaderId;
	PESHIELD_START_RESULT	StartResult;

	// Is already initialized ?
	if (g_bInitLoader)
	{
		return FALSE;
	}

	// Set global Loader instance
	g_hInstance = hInstance;

	// Do not allow more than single process
	if (FindWindow(PESHIELD_LOADER_WNDCLASS, PESHIELD_LOADER_WINDOW))
	{
		return FALSE;
	}

	// Do not allow Windows version below NT
	if (GetVersion() >= 0x80000000)
	{
		return FALSE;
	}

#if _PESHIELD_LOG

	// Get Loader path and directory
	if (!GetLoaderFileName((HLoader)hInstance, g_szLoaderPath, MAX_PATH))
	{
		return FALSE;
	}
	PeShield_GetFileDirectory(szLoaderDirectory, g_szLoaderPath);

	// Make log path
	sprintf(g_szLogPath, "%s\\%s", szLoaderDirectory, PESHIELD_LOG_FILE);

	// Create log file
	g_hFileLog = CreateFile(
		g_szLogPath,
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		0,
		NULL
		);
	if (g_hFileLog == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

#endif // _PESHIELD_LOG
	
	PESHIELD_LOG("Information : Pe Shield Loader start up.");

	// Initialize WinSock 2
	if (WSAStartup(MAKEWORD(2, 2), &g_wsa) == 0)
	{
		PESHIELD_LOG("Success : WinSock 2 is initialized.");
		
		// Create Run Request Event
		g_hEventRunRequest = CreateEvent(NULL, TRUE, FALSE, PESHIELD_RUN_REQUEST_EVENT);
		if (g_hEventRunRequest)
		{
			PESHIELD_LOG("Success : Run Request Event is created.");
			
			// Create Run Complete Event
			g_hEventRunComplete = CreateEvent(NULL, TRUE, FALSE, PESHIELD_RUN_COMPLETE_EVENT);
			if (g_hEventRunComplete)
			{			
				PESHIELD_LOG("Success : Run Complete Event is created.");

				// Load Pe Shield Sys
				if (PeShield_LoadDriver(&g_hSys, g_hInstance, IDR_PESHIELD_SYS, "SYS"))
				{
					PESHIELD_LOG("Success : Pe Shield Sys is loaded.");
					
					// Get Pe Shield Loader process id
					dwLoaderId = GetCurrentProcessId();

					// Start Pe Shield Sys
					DeviceIoControl(
						g_hSys,
						IOCTL_PESHIELD_START,
						&dwLoaderId,
						sizeof(dwLoaderId),
						&StartResult,
						sizeof(StartResult),
						&IoCtlResult,
						NULL
						);
					if (IoCtlResult == sizeof(StartResult))
					{
						PESHIELD_LOG("Success : Pe Shield Sys is started.");
						
						// Set Run Request
						g_RunRequest = StartResult.RunRequest;
						
						// Initialize Pe Shield Array data
						if (PeShield_ArrayStartup(
							StartResult.ProcessList, 
							sizeof(PESHIELD_PROCESS_INFO),
							PESHIELD_MAX_PROCESS
							))
						{
							PESHIELD_LOG("Success : Pe Shield Array data is initialized.");
							
							// Create Loader mutex
							g_hMutexCreate = CreateMutex(NULL, FALSE, PESHIELD_CREATE_MUTEX);
							if (g_hMutexCreate)
							{
								PESHIELD_LOG("Success : Create mutex is created.");
								
								// Create Loader section
								g_hSectionCreate = CreateFileMapping(
									INVALID_HANDLE_VALUE, 
									NULL,
									PAGE_READWRITE,
									0,
									sizeof(PESHIELD_CREATE_REQUEST),
									PESHIELD_CREATE_SECTION
									);
								if (g_hSectionCreate)
								{
									PESHIELD_LOG("Success : Create section is created.");
									
									// Map Loader section view
									g_CreateRequest = (PPESHIELD_CREATE_REQUEST)MapViewOfFile(
										g_hSectionCreate,
										FILE_MAP_READ,
										0,
										0,
										sizeof(PESHIELD_CREATE_REQUEST)
										);
									if (g_CreateRequest)
									{
										PESHIELD_LOG("Success : Create section is mapped.");
										
										// Set success
										g_bInitLoader = bInitialized = TRUE;
										PESHIELD_LOG("Success : Pe Shield Loader data is initialized.");
									}
									else
									{
										PESHIELD_LOG("Error : Create section is not mapped.");
									}
								}
								else
								{
									PESHIELD_LOG("Error : Create section is not created.");
								}
							}
							else
							{
								PESHIELD_LOG("Error : Create mutex is not created.");
							}
						}
						else
						{
							PESHIELD_LOG("Error : Pe Shield Array data is not initialized.");
						}
					}
					else
					{
						PESHIELD_LOG("Error : Pe Shield Sys is not started.");
					}
				}
				else
				{
					PESHIELD_LOG("Error : Pe Shield Sys is not loaded.");
				}
			}
			else
			{
				PESHIELD_LOG("Error : Run Complete Event is not created.");
			}
		}
		else
		{
			PESHIELD_LOG("Error : Run Request Event is not created.");
		}
	}
	else
	{
		PESHIELD_LOG("Error : WinSock 2 is not initialized.");
	}

	// Is it initialized ?
	if (!bInitialized)
	{
		// No, uninitialize it
		g_bInitLoader = TRUE;
		PeShield_LoaderCleanup();
	}
	
	return bInitialized;
}

//
// Uninitialize Pe Shield Loader data
//
VOID
PeShield_LoaderCleanup()
{
	DWORD	IoCtlResult;

	if (g_bInitLoader)
	{
		PESHIELD_LOG("Information : Pe Shield Loader clean up.");
	
		// Close create section map
		if (g_CreateRequest)
		{
			UnmapViewOfFile(g_CreateRequest);
			g_CreateRequest = NULL;
			PESHIELD_LOG("Success : Create section is unmapped.");
		}

		// Close create section
		if (g_hSectionCreate)
		{
			CloseHandle(g_hSectionCreate);
			g_hSectionCreate = NULL;
			PESHIELD_LOG("Success : Create section is closed.");	
		}

		// Close create mutex
		if (g_hMutexCreate)
		{
			CloseHandle(g_hMutexCreate);
			g_hMutexCreate = NULL;
			PESHIELD_LOG("Success : Create mutex is closed.");
		}

		// Uninitialize Pe Shield Array data
		PeShield_ArrayCleanup();
		
		// Set Run Request to NULL
		g_RunRequest = NULL;

		// Is Pe Shield Sys loaded ?
		if (g_hSys != INVALID_HANDLE_VALUE)
		{
			// Stop Sys
			DeviceIoControl(
				g_hSys,
				IOCTL_PESHIELD_STOP,
				NULL,
				0,
				NULL,
				0,
				&IoCtlResult,
				NULL
				);
			PESHIELD_LOG("Success : Pe Shield Sys is stopped.");

			// Unload driver
			PeShield_UnloadDriver(&g_hSys);
			PESHIELD_LOG("Success : Pe Shield Sys is unloaded.");
		}

		// Close Run Complete Event
		if (g_hEventRunComplete)
		{
			CloseHandle(g_hEventRunComplete);
			g_hEventRunComplete = NULL;
			PESHIELD_LOG("Success : Run Complete Event is closed.");
		}

		// Close Run Request Event
		if (g_hEventRunRequest)
		{
			CloseHandle(g_hEventRunRequest);
			g_hEventRunRequest = NULL;
			PESHIELD_LOG("Success : Run Request Event is closed.");
		}

		// Uninitialize WinSock 2
		if (g_wsa.wVersion)
		{
			WSACleanup();
			ZeroMemory(&g_wsa, sizeof(g_wsa));
			PESHIELD_LOG("Success : WinSock 2 is uninitialized.");
		}

		// Delete Pe Shield Loader file itself
		if (PeShield_DeleteSelf(g_szLoaderPath))
		{
			PESHIELD_LOG("Success : Pe Shield Loader file is deleted.");
		}
		else
		{
			PESHIELD_LOG("Fail : Pe Shield Loader file is not deleted.");
		}

#if _PESHIELD_LOG	

		// Close log file
		if (g_hFileLog != INVALID_HANDLE_VALUE)  
		{
			CloseHandle(g_hFileLog);
			g_hFileLog = INVALID_HANDLE_VALUE;
		}

#endif // _PESHIELD_LOG
		
		// Set uninitialized
		g_bInitLoader = FALSE;
		PESHIELD_LOG("Success : Pe Shield Loader data is uninitialized.");
	}
}


//----------------------------------------------------------------------
//				  C  E  R  T  I  F  I  C  A  T  I  O  N
//----------------------------------------------------------------------

//
// Certify exe
//
CERTIFY_STATUS
PeShield_CertifyExe(
	IN PPESHIELD_METADATA pMetaData
	)
{
	CERTIFY_STATUS	CertifyStatus;

#if _PESHIELD_NO_CERTIFY

	CertifyStatus = CERTIFY_SUCCEEDED;

#else

	CHAR	szSysMacAddr[LEN_MAC+1];
	BYTE	pBufSysMacHash[LEN_HASH];
	BYTE	pBufDbMacHash[LEN_HASH];

	// Is exe web-certified ?
	if (PeShield_IsExeWebCertified(pMetaData))
	{
		PESHIELD_LOG("Success : Exe is certified.");
		
		// Get system mac address
		if (PeShield_GetMacAddressFromSystem(szSysMacAddr))
		{
			PESHIELD_LOG("Success : System mac address is got.");
			
			// Get system mac hash
			PeShield_GetSha1Hash(
				pBufSysMacHash, 
				szSysMacAddr,
				LEN_MAC
				);
			
			// Get database mac hash
			if (PeShield_GetMacHashFromDatabase(pBufDbMacHash, pMetaData->Serial))
			{
				PESHIELD_LOG("Success : Database mac hash is not got.");
				
				// Is two mac hashes the same ?
				if (memcmp(pBufSysMacHash, pBufDbMacHash, LEN_HASH) == 0)
				{
					PESHIELD_LOG("Success : Two mac hashes are the same.");
					
					// Set success
					CertifyStatus = CERTIFY_SUCCEEDED;
					PESHIELD_LOG("Success : Certification is succeeded.");
				}
				else
				{
					// Set unmatched mac hashes
					CertifyStatus = CERTIFY_MACHASH_UNMATCHED;
					PESHIELD_LOG("Fail : Two mac hashes are not the same.");
				}
			}
			else
			{
				// Set database off
				CertifyStatus = CERTIFY_DATABASE_OFFLINE;
				PESHIELD_LOG("Fail : Database mac hash is not got.");	
			}
		}
		else
		{
			// Set no system mac address
			CertifyStatus = CERTIFY_NO_SYSTEM_MAC;
			PESHIELD_LOG("Fail : System mac address is not got.");
		}
	}
	else
	{
		// Set no web certification
		CertifyStatus = CERTIFY_NO_WEB_CERTIFIED;
		PESHIELD_LOG("Fail : Exe is not certified.");
	}
	
	// Is operation not succeeded ?
	if (!PESHIELD_SUCCESS(CertifyStatus))
	{
		// Set exe to no-certified ( canceled to map exe only readable )
	//	pMetaData->Certified = FALSE;
	//	ZeroMemory(pMetaData->Serial, LEN_SERIAL+1);
	}

#endif // _PESHIELD_NO_CERTIFY

	return CertifyStatus;
}


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
	)
{
	BOOL	bSucceeded	= FALSE;

	HANDLE	hFileClone	= INVALID_HANDLE_VALUE;
	HANDLE	hMapClone	= NULL;
	PBYTE	pBufClone	= NULL;
	DWORD	cbClone		= 0;
	DWORD	cbExe		= 0;

	CHAR				szSecuredPath[MAX_PATH+1];
	CHAR				szCommandLine[MAX_PATH+MAX_PATH+1];
	STARTUPINFO			Startup;
	PROCESS_INFORMATION	Process;

	// Sleep for ZwCreateProcess()
	Sleep(150);

	// Get secured path
	if (PeShield_GetSecuredPath(szSecuredPath, szExePath))
	{
		PESHIELD_LOG("Success : Secured path is made.");

		// Close exe mapping to rename
		PeShield_CloseFileMapping(
			hFileExe,
			hMapExe,
			pBufExe,
			NULL
			);
		
		// Hide exe file
		if (PeShield_HideFile(szExePath, TRUE))
		{
			PESHIELD_LOG("Success : Exe file is hidden.");
		}
		else
		{
			PESHIELD_LOG("Error : Exe file is not hidden.");
		}

		// Delete secured file just in case
		DeleteFile(szSecuredPath);

		// Rename exe to secured name for clone
		if (rename(szExePath, szSecuredPath) == 0)
		{
			PESHIELD_LOG("Success : Exe file is renamed.");

			// Create exe mapping again to copy
			if (PeShield_CreateFileMapping(
				hFileExe,
				hMapExe,
				pBufExe,
				&cbExe,
				szSecuredPath,	// it's renamed
				0,
				GENERIC_READ,	// |GENERIC_WRITE
				FILE_SHARE_READ,
				OPEN_EXISTING
				))
			{
				PESHIELD_LOG("Success : Exe mapping is created again.");

				// Get metadata
				*pMetaData = PeShield_GetMetaData(*pBufExe, cbExe);
				PESHIELD_LOG("Success : Metadata is got again.");

				// Create new clone mapping
				cbClone = (*pMetaData)->ExeLength;
				if (PeShield_CreateFileMapping(
					&hFileClone,
					&hMapClone,
					&pBufClone,
					0,
					szExePath,
					cbClone,
					GENERIC_READ|GENERIC_WRITE,
					FILE_SHARE_READ,
					CREATE_NEW
					))
				{
					PESHIELD_LOG("Success : Clone mapping is created.");

					// Copy exe to clone
					CopyMemory(
						pBufClone,
						*pBufExe + (*pMetaData)->ExeOffset, 
						cbClone
						);
					
					// Close clone mapping for 
					PeShield_CloseFileMapping(
						&hFileClone,
						&hMapClone,
						&pBufClone,
						&cbClone
						);
					
					// Make command line
					wsprintf(szCommandLine, "%s %s", szExePath, szCmdLine);
					
					// Create clone process ( with suspend flag )
					ZeroMemory(&Startup, sizeof(Startup));
					Startup.cb = sizeof(Startup);
					if (CreateProcess(
						NULL,
						szCommandLine,
						NULL,
						NULL,
						FALSE,
						CREATE_SUSPENDED,
						NULL,
						NULL,
						&Startup,
						&Process
						))
					{
						PESHIELD_LOG("Success : Process is created.");
						
						// Set process id, thread handle
						*dwProcessId	= Process.dwProcessId;
						*hThread		= Process.hThread;
						
						// Close process handle not necessary
						CloseHandle(Process.hProcess);
						
						// Set success
						bSucceeded = TRUE;
					}
					else
					{
						PESHIELD_LOG("Error : Process is not created.");
					}
				}	
				else
				{
					PESHIELD_LOG("Error : Clone mapping is not created.");
				}
			}
			else
			{
				PESHIELD_LOG("Error : Exe mapping is not created again.");
			}
		}
		else
		{
			PESHIELD_LOG("Error : Exe file is not renamed.");
		}
	}
	else
	{
		PESHIELD_LOG("Error : Secured path is not made.");
	}

	return bSucceeded;
}

//
// Decrypt process
//
BOOL
PeShield_DecryptProcess(
	IN DWORD dwProcessId,
	IN PVOID pTxtAddr,
	IN DWORD cbTxtSize
	)
{
	BOOL	bSucceeded	= FALSE;

#if _PESHIELD_NO_DECRYPT
	
	bSucceeded	= TRUE;

#else

	HANDLE	hProcess;
	PBYTE	TxtSection;
	
	// Open all accessible process handle
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (hProcess)
	{
		PESHIELD_LOG("Success : All accessible process handle is opened.");
		
		// Allocate .txt section buffer
		TxtSection = (PBYTE)malloc(cbTxtSize);
		if (TxtSection)
		{
			PESHIELD_LOG("Success : .text section buffer is allocated.");
			
			// Read .txt section from process
			if (ReadProcessMemory(
				hProcess,
				pTxtAddr,
				TxtSection,
				cbTxtSize,
				NULL
				))
			{
				// Decrypt part of .text section
				PeShield_DecryptDes(
					TxtSection,
					cbTxtSize,
					PESHIELD_FRAGSIZE,
					NULL
					);
				
				// Write decrypted .text section
				if (WriteProcessMemory(
					hProcess,
					pTxtAddr,
					TxtSection,
					cbTxtSize,
					NULL
					))
				{
					PESHIELD_LOG("Success : .text section is written.");
					
					bSucceeded = TRUE;
				}
				else
				{
					PESHIELD_LOG("Error : .text section is not written.");
				}
				
				// Free .text section buffer
				free(TxtSection);
				PESHIELD_LOG("Success : .text section buffer is freed");
			}
		}
		else
		{
			PESHIELD_LOG("Error : .text section buffer is not allocated.");
		}
		
		// Close process handle
		CloseHandle(hProcess);
		PESHIELD_LOG("Success : All accessible process handle is closed.");
	}
	else
	{
		PESHIELD_LOG("Error : All accessible process handle is not opened.");
	}
	
#endif // _PESHIELD_NO_DECRYPT

	return bSucceeded;
}

//
// Try clearing process information
//
VOID
PeShield_ClearProcessInfo(
	IN DWORD ProcessIndex
	)
{
	PPESHIELD_PROCESS_INFO	ProcessInfo;
	CHAR	szSecuredPath[MAX_PATH+1];
	LPSTR	ProcessName;

	// Get process information
	ProcessInfo = PeShield_ArrayData(ProcessIndex);
	
	// Does process information exist ?
	if (ProcessInfo)
	{
		// Is process reference count zero ?
		if (ProcessInfo->RefCount <= 0)
		{
			// Sleep for ZwTerminateProcess
			Sleep(150);
			
			// Get process name
			ProcessName = strrchr(ProcessInfo->ExePath, '\\');
			ProcessName++;
			
			// Terminate processes named the same
			PeShield_TerminateProcesses(ProcessName);
			
			// Make secured path
			PeShield_GetSecuredPath(szSecuredPath, ProcessInfo->ExePath);
			PESHIELD_LOG("Success : Secured path is made.");
			
			// Does secured path exist ?
			if (_access(szSecuredPath, 0) == 0)
			{	
				// Delete clone file
				if (DeleteFile(ProcessInfo->ExePath))
				{
					PESHIELD_LOG("Success : Clone file is deleted.");
					
					// Rename exe to non-secured name back
					if (rename(szSecuredPath, ProcessInfo->ExePath) == 0)
					{
						PESHIELD_LOG("Success : Exe file is renamed back.");
					}
					else
					{
						PESHIELD_LOG("Error : Exe file is renamed back.");
					}
					
					// Show hidden exe file
					if (PeShield_HideFile(ProcessInfo->ExePath, FALSE))
					{
						PESHIELD_LOG("Success : Exe file is shown.");
					}
					else
					{
						PESHIELD_LOG("Error : Exe file is not shown.");
					}
				}
			}
			
			// Remove process information from Process List
			PeShield_ArrayRemove(ProcessIndex);
			PESHIELD_LOG("Success : Process information is removed.");
			
			// Is Process List empty ? ( means no more processes are managed )
			if (PeShield_ArrayIsEmpty())
			{
				PESHIELD_LOG("Information : Process List is empty.");
				
				// Post WM_DESTROY message
				PostMessage(g_hWndLoader, WM_DESTROY, 0, 0);
			}
			else
			{
				PESHIELD_LOG("Error : Clone file is not deleted.");
			}
		}
	}
}


//----------------------------------------------------------------------
//			 T  H  R  E  A  D      P  R  O  C  E  D  U  R  E
//----------------------------------------------------------------------

//
// Create thread procedure
//
UINT
WINAPI
PeShield_CreateThreadProc(
	IN PVOID Param
	)
{
	UINT	nRetCode	= 1;

	PPESHIELD_CREATE_PARAM	CreateParam;
	PPESHIELD_PROCESS_INFO	ProcessInfo;

	HANDLE	hFileExe	= INVALID_HANDLE_VALUE;
	HANDLE	hMapExe		= NULL;
	PBYTE	pBufExe		= NULL;
	DWORD	cbExe		= 0;

	PPESHIELD_METADATA	pMetaData;
	CERTIFY_STATUS		CertifyStatus;

	DWORD	dwProcessId;
	HANDLE	hThread;

	PIMAGE_NT_HEADERS	NtHdrs;
	DWORD				TxtAddr;
	DWORD				TxtSize;

	PESHIELD_LOG("Information : Create thread starts.");

	// Sleep for spy Loader's end
	Sleep(100);

	// Get create param
	CreateParam = (PPESHIELD_CREATE_PARAM)Param;

	// Get process information
	ProcessInfo = (PPESHIELD_PROCESS_INFO)PeShield_ArrayData(CreateParam->ProcessIndex);
	if (ProcessInfo)
	{
		PESHIELD_LOG("Success : Process information is in.");

		// Create exe mapping
		if (PeShield_CreateFileMapping(
			&hFileExe,
			&hMapExe,
			&pBufExe,
			&cbExe,
			ProcessInfo->ExePath,
			0,
			GENERIC_READ,	// |GENERIC_WRITE
			FILE_SHARE_READ,
			OPEN_EXISTING
			))
		{
			PESHIELD_LOG("Success : Exe mapping is created.");

			// Get metadata
			pMetaData = PeShield_GetMetaData(pBufExe, cbExe);
			
			// Is metadata valid ?
			if (PeShield_IsMetaDataValid(pMetaData, cbExe))
			{
				PESHIELD_LOG("Success : Metadata is valid.");

				// Certify exe
				CertifyStatus = PeShield_CertifyExe(pMetaData);
				if (PESHIELD_SUCCESS(CertifyStatus))
				{
					// Create first process
					if (PeShield_CreateFirstProcess(
						&hFileExe,
						&hMapExe,
						&pBufExe,
						&pMetaData,
						ProcessInfo->ExePath,
						CreateParam->CmdLine,
						&dwProcessId,
						&hThread
						))
					{
						PESHIELD_LOG("Success : First process is created.");
						
						// Increment process reference count ( means a process is created )
						InterlockedIncrement(&ProcessInfo->RefCount);
						
						// Get nt headers from exe
						if (PeShield_GetPeHdrs(pBufExe + pMetaData->ExeOffset, NULL, &NtHdrs, NULL, NULL))
						{
							PESHIELD_LOG("Success : Nt header is got.");
							
							// Get .text section information
							if (PeShield_GetSectionInformation(
								pBufExe + pMetaData->ExeOffset, 
								".text", 
								NULL, &TxtSize, &TxtAddr, NULL) || 
								PeShield_GetSectionInformation(
								pBufExe + pMetaData->ExeOffset,
								"CODE", 
								NULL, &TxtSize, &TxtAddr, NULL) || 
								PeShield_GetSectionInformation(
								pBufExe + pMetaData->ExeOffset, 
								"UPX1", 
								NULL, &TxtSize, &TxtAddr, NULL
								))
							{
								PESHIELD_LOG("Success : .text section information is got.");
								
								// Save .text section information in process information
								ProcessInfo->TxtAddr = (PVOID)(NtHdrs->OptionalHeader.ImageBase + TxtAddr);
								ProcessInfo->TxtSize = TxtSize;
								
								// Decrypt process
								if (PeShield_DecryptProcess(
									dwProcessId, 
									ProcessInfo->TxtAddr, 
									ProcessInfo->TxtSize
									))
								{
									PESHIELD_LOG("Success : Process is decrypted.");
									
									// Resume decrypted process
									if (ResumeThread(hThread) != 0xFFFFFFFF)
									{
										PESHIELD_LOG("Success : Thread starts to run.");
										
										// Wait for running process
										PESHIELD_LOG("Information : Waiting for running process.");
										WaitForSingleObject(hThread, INFINITE);
										
										// Set success
										nRetCode = 0;
										PESHIELD_LOG("Information : Process is successfully finished");
									}
									else
									{
										PESHIELD_LOG("Error : Thread doesn't start.");
									}
								}
								else
								{
									PESHIELD_LOG("Error : Process is not decrypted.");
								}
							}
							else
							{
								PESHIELD_LOG("Error : .txt section information is not got.");
							}
						}
						else
						{
							PESHIELD_LOG("Error : Nt header is not got.");
						}

						// Decrement process reference count
						InterlockedDecrement(&ProcessInfo->RefCount);

						// Close process handle
						CloseHandle(hThread);
					}
					else
					{
						PESHIELD_LOG("Error : First process is not created.");
					}
				}
				else
				{
					// Reflection by certification status
					switch (CertifyStatus)
					{
					case CERTIFY_NO_WEB_CERTIFIED:
				
						// Notice that it's not web-certified yet
						MESSAGE_FAIL(IDS_NO_WEB_CERTIFIED);

						// Connect to web site
						PeShield_ConnectToWebSite(pMetaData->WebAddress);

						break;

					case CERTIFY_NO_SYSTEM_MAC:		

						// Notice the failure to get mac address
						MESSAGE_FAIL(IDS_NO_SYSTEM_MAC);

						break;

					case CERTIFY_DATABASE_OFFLINE:	
						
						// Notice that database is not connectable
						MESSAGE_FAIL(IDS_DATABASE_OFFLINE);

						break;

					case CERTIFY_MACHASH_UNMATCHED:	
						
						// Notice that mac hashes are not the same
						MESSAGE_FAIL(IDS_MACHASH_UNMATCHED);

						// Connect to web site
						PeShield_ConnectToWebSite(pMetaData->WebAddress);

						break;

					default:
						PESHIELD_LOG("Error : Unknown certificaion status.");
						break;
					}
				}
			}
			else
			{
				PESHIELD_LOG("Fail : Metadata is invalid.");

				// Notice that it's invalid meta data
				MESSAGE_FAIL(IDS_INVALID_METADATA);
			}

			// Close exe mapping
			PeShield_CloseFileMapping(
				&hFileExe,
				&hMapExe,
				&pBufExe,
				&cbExe
				);
			PESHIELD_LOG("Success : Exe mapping is closed.");
		}
		else
		{
			PESHIELD_LOG("Error : Exe mapping is not created.");
		}
	}
	else
	{
		PESHIELD_LOG("Error : Process information is not in.");
	}

	// Clear process information
	PeShield_ClearProcessInfo(CreateParam->ProcessIndex);	
	PESHIELD_LOG("Information : Process information is cleared.");

	// Free create param
	free(CreateParam);

	PESHIELD_LOG("Information : Create thread ends.");

	return nRetCode;
}

//
// Run Request thread procedure
//
UINT
WINAPI
PeShield_RunRequestThreadProc(
	IN PVOID Param
	)
{	
	UINT	nRetCode	= 1;

	PPESHIELD_PROCESS_INFO	ProcessInfo;
	PPESHIELD_RUN_PARAM		RunParam;

	PESHIELD_LOG("Information : Run Request thread starts.");

	// Is Run Request thread on ?
	while (g_bRunRequestThread)
	{
		// Wait for Run Request Event ( from sys )
		PESHIELD_LOG("Information : Waiting for Run Request Event.");
		WaitForSingleObject(g_hEventRunRequest, INFINITE);

		// Reset Run Request Event for the next
		ResetEvent(g_hEventRunRequest);

		PESHIELD_LOG("Information : Run Request Event is released.");

		// Is Run Requset thread off ?
		if (!g_bRunRequestThread)
		{
			break;
		}

		// Get process info
		ProcessInfo = PeShield_ArrayData(g_RunRequest->ProcessIndex);
		if (ProcessInfo)
		{
			PESHIELD_LOG("Success : Process information is got.");

			// Increment process reference count
			InterlockedIncrement(&ProcessInfo->RefCount);
			
			// Decrypt process
			if (PeShield_DecryptProcess(
				g_RunRequest->ProcessId, 
				ProcessInfo->TxtAddr, 
				ProcessInfo->TxtSize
				))
			{
				PESHIELD_LOG("Success : Process is decrypted.");
				
				// Allocate Run thread parameter
				RunParam = (PPESHIELD_RUN_PARAM)malloc(sizeof(PESHIELD_RUN_PARAM));
				if (RunParam)
				{
					PESHIELD_LOG("Success : Run thread parameter is allocated.");

					// Set Run thread parameter
					RunParam->ProcessIndex	= g_RunRequest->ProcessIndex;
					RunParam->ProcessId		= g_RunRequest->ProcessId;

					// Begin Run thread
					CloseHandle((HANDLE)_beginthreadex(NULL, 0, PeShield_RunThreadProc, RunParam, 0, NULL));
					
					// Set success
					g_RunRequest->IsSucceeded = TRUE;
					nRetCode = 0;
					PESHIELD_LOG("Success : Run Request is succeeded.");
				}
				else
				{
					PESHIELD_LOG("Error : Run thread parameter is not allocated.");
				}
			}
			else
			{
				PESHIELD_LOG("Error : Process is not decrypted.");
			}

			// Is operation succeeded ?
			if (!g_RunRequest->IsSucceeded)
			{
				// Decrement process reference count
				InterlockedDecrement(&ProcessInfo->RefCount);
			}
		}
		else
		{
			PESHIELD_LOG("Error : Process information is not got.");
		}

		// Set Run Complete Event ( to sys )
		SetEvent(g_hEventRunComplete);
	}

	PESHIELD_LOG("Information : Run Request thread ends.");

	return nRetCode;
}

//
// Run thread procedure
//
UINT
WINAPI
PeShield_RunThreadProc(
	IN PVOID Param
	)
{
	UINT	nRetCode	= 1;

	PPESHIELD_PROCESS_INFO	ProcessInfo;
	PPESHIELD_RUN_PARAM		RunParam;

	HANDLE	hProcess;
	
	PESHIELD_LOG("Information : Run thread starts.");

	// Sleep for ZwResumeThread()
	Sleep(100);

	// Get Run thread parameter
	RunParam = (PPESHIELD_RUN_PARAM)Param;

	// Get process information
	ProcessInfo = (PPESHIELD_PROCESS_INFO)PeShield_ArrayData(RunParam->ProcessIndex);
	if (ProcessInfo)
	{
		PESHIELD_LOG("Success : Process information is got.");

		// Open waitable process handle 
		hProcess = OpenProcess(SYNCHRONIZE, FALSE, RunParam->ProcessId);
		if (hProcess)
		{
			PESHIELD_LOG("Success : Waitable process handle is opened.");

			// Waiting for process
			PESHIELD_LOG("Success : Waiting for process.");
			WaitForSingleObject(hProcess, INFINITE);
			
			// Close process handle
			CloseHandle(hProcess);
			PESHIELD_LOG("Success : Waitable process handle is closed.");
			
			// Set success
			nRetCode = 0;
			PESHIELD_LOG("Information : Process is successfully finished");
		}
		else
		{
			PESHIELD_LOG("Error : Waitable process handle is not opened.");
		}
	}
	else
	{
		PESHIELD_LOG("Error : Process information is not got.");
	}

	// Decrement process reference count
	InterlockedDecrement(&ProcessInfo->RefCount);

	// Clear process information
	PeShield_ClearProcessInfo(RunParam->ProcessIndex);

	// Free Run thread parameter
	free(RunParam);

	PESHIELD_LOG("Information : Run thread ends.");

	return nRetCode;
}