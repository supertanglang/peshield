#include "../Library/h/comm.h"
#include "../Library/h/io.h"
#include "../Library/h/reg.h"

#include "d.h"

#include <windows.h>
#include <stdio.h>
#include <io.h>

//----------------------------------------------------------------------
//			 				D  E  B  U  G
//----------------------------------------------------------------------

//
// Debug option
//
#define _PESHIELD_LOG	0	// log all process

//
// Log file information
//
#define	PESHIELD_LOG_FILE	"PeShield_Spy.log"
static HANDLE	g_hFileLog = INVALID_HANDLE_VALUE;
static CHAR		g_szLogPath[MAX_PATH+1];
static DWORD	g_cbWrittenLog;

//
// Log short cut
//
#define PESHIELD_LOG(sz)	if (g_hFileLog != INVALID_HANDLE_VALUE)	\
							WriteFile(g_hFileLog, sz"\r\n", strlen(sz)+2, &g_cbWrittenLog, NULL)


//----------------------------------------------------------------------
//			 G  L  O  B  A  L     V  A  R  I  A  B  L  E  S
//----------------------------------------------------------------------

//
// Pe Shield Spy Initialization Flag
//
static BOOL		g_bInitSpy;

//
// Exe file information
//
static CHAR		g_szExePath[MAX_PATH+1];


//----------------------------------------------------------------------
//					      W  I  N  M  A  I  N
//----------------------------------------------------------------------

//
// Short cuts for message box
//
#define MESSAGE_SUCCESS(id)	MessageBox(HWND_DESKTOP, id, "Pe Shield Spy", MB_ICONINFORMATION)
#define MESSAGE_FAIL(id)	MessageBox(HWND_DESKTOP, id, "Pe Shield Spy", MB_ICONWARNING)
#define MESSAGE_TEST(id)	MessageBox(HWND_DESKTOP, id, "Pe Shield Spy", MB_OK)
#ifdef _DEBUG
#define MESSAGE_ERROR(id)	MessageBox(HWND_DESKTOP, id, "Pe Shield Spy", MB_ICONERROR)
#else
#define MESSAGE_ERROR(id)
#endif // _DEBUG

//
// Application entry point
//
int WINAPI WinMain(
	HINSTANCE hInstance,	// handle to current instance
	HINSTANCE hPrevInstance,	// handle to previous instance
	LPSTR lpCmdLine,		// pointer to command line
	int nCmdShow			// show state of window
)
{
	int		nRetCode	= 1;

	HWND	hWndLoader;

	// Initialize Pe Shield Spy data
	if (PeShield_SpyStartup(hInstance))
	{
		// Load Pe Shield Loader
		if (PeShield_LoadLoader(&hWndLoader))
		{
			// Send WM_PESHIELD_CREATE to module window
			if (PeShield_SendCreate(hWndLoader, lpCmdLine))
			{
				nRetCode = 0;
			}
			else
			{
				MESSAGE_ERROR(IDS_NO_CREATE_SEND);
			}
		}
		else
		{
			MESSAGE_ERROR(IDS_NO_LOADER_LOAD);
		}

		// Uninitialize Pe Shield Spy data
		PeShield_SpyCleanup();
	}
	else
	{
		MESSAGE_ERROR(IDS_NO_STARTUP);
	}
	
	return nRetCode;
}


//----------------------------------------------------------------------
//				I  N  I  T  I  A  L  I  Z  A  T  I  O  N
//---------------------------------------------------------------------- 

//
// Initialize Pe Shield Spy data
//
BOOL
PeShield_SpyStartup(
	IN HINSTANCE hInstance
	)
{
	BOOL	bInitialized = FALSE;

#if _PESHIELD_LOG
	CHAR	szExeDirectory[MAX_PATH+1];
#endif // _PESHIELD_LOG
	CHAR	szExeName[_MAX_FNAME+1];

	DWORD	cbRegValue;
	CHAR	szRegSubkey[MAX_REG+1];
	CHAR	szRegValue[MAX_PATH+1];	

	if (g_bInitSpy)
	{
		return FALSE;
	}

	// Get exe path itself
	if (!GetModuleFileName((HMODULE)hInstance, g_szExePath, MAX_PATH))
	{
		return FALSE;		
	}

#if _PESHIELD_LOG
	// Make log path
	PeShield_GetFileDirectory(szExeDirectory, g_szExePath);
	sprintf(g_szLogPath, "%s\\(%d)%s", szExeDirectory, GetCurrentProcessId(), PESHIELD_LOG_FILE);

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

	PESHIELD_LOG("Information : Pe Shield Spy start up.");

	// Get exe name
	PeShield_GetFileName(szExeName, g_szExePath);	

	// Make PeShield Loader subkey
	wsprintf(szRegSubkey, "%s\\%s", PESHIELD_REGKEY_LOADER, szExeName);

	// Does registry key exist ?
	if (!PeShield_RegDoesKeyExist(szRegSubkey))
	{
		PESHIELD_LOG("Information : Registry key doesn't exist.");

		// No, Create registry key
		if (PeShield_RegCreateKey(szRegSubkey))
		{
			PESHIELD_LOG("Success : Registry key is created.");

			// Set exe path in the key
			if (PeShield_RegSetValue(
				szRegSubkey,
				g_szExePath,
				strlen(g_szExePath),
				NULL,
				REG_SZ
				))
			{				
				PESHIELD_LOG("Success : Registry value is set.");

				g_bInitSpy = bInitialized = TRUE;				
				PESHIELD_LOG("Success : Pe Shield Spy data is initialized.");
			}
			else
			{
				PESHIELD_LOG("Error : Registry value is not set.");
			}
		}
		else
		{
			PESHIELD_LOG("Error : Registry key is not created.");
		}
	}
	else
	{
		PESHIELD_LOG("Information : Registry key exists.");

		// Yes, Get saved path from registry
		if (PeShield_RegGetValue(
			szRegSubkey,
			&cbRegValue,
			szRegValue,
			NULL,
			NULL
			))
		{
			// But is the value the same with real path ?
			if (strcmp(szRegValue, g_szExePath) == 0)
			{
				PESHIELD_LOG("Information : Registry value and exe path match.");

				g_bInitSpy = bInitialized = TRUE;				
				PESHIELD_LOG("Success : Pe Shield Spy data is initialized.");
			}
			else
			{
				PESHIELD_LOG("Information : Registry value and exe path don't match.");

				// No, Overwrite the path
				if (PeShield_RegSetValue(
					szRegSubkey,
					g_szExePath,
					strlen(g_szExePath),
					NULL,
					REG_SZ
					))
				{
					PESHIELD_LOG("Success : New registry value is set.");

					g_bInitSpy = bInitialized = TRUE;				
					PESHIELD_LOG("Success : Pe Shield Spy data is initialized.");
				}
				else
				{
					PESHIELD_LOG("Success : New registry value is not set.");
				}
			}
		}
		else
		{
			PESHIELD_LOG("Error : Registry value is not accessible.");
		}
	}

#if _PESHIELD_LOG
	// Is initialized ?
	if (!bInitialized)
	{
		// Close log file
		if (g_hFileLog != INVALID_HANDLE_VALUE)  
		{
			CloseHandle(g_hFileLog);
			g_hFileLog = INVALID_HANDLE_VALUE;
		}
	}
#endif // _PESHIELD_LOG

	return TRUE;
}

//
// Uninitialize Pe Shield Spy data
//
VOID
PeShield_SpyCleanup()
{
	PESHIELD_LOG("Information : Pe Shield Spy clean up.");

	if (g_bInitSpy)
	{		
#if _PESHIELD_LOG
		// Close log file
		if (g_hFileLog != INVALID_HANDLE_VALUE)  
		{
			CloseHandle(g_hFileLog);
			g_hFileLog = INVALID_HANDLE_VALUE;
		}
#endif // _PESHIELD_LOG
		
		// Set uninitialized
		g_bInitSpy = FALSE;
		PESHIELD_LOG("Success : Pe Shield Spy data is uninitialized.");
	}
}


//----------------------------------------------------------------------
//				L  O  A  D  E  R	   W  I  N  D  O  W
//----------------------------------------------------------------------

BOOL
PeShield_LoadLoader(
	OUT HWND* phWndLoader
	)
{
	BOOL	bLoaded = FALSE;
	
	HWND	hWndLoader;
	
	CHAR	szSysPath[MAX_PATH+1];
	CHAR	szLoaderPath[MAX_PATH+1];
	
	HANDLE	hFileLoader	= INVALID_HANDLE_VALUE;
	HANDLE	hMapLoader	= NULL;
	PBYTE	pBufLoader	= NULL;
	
	HANDLE	hFileExe	= INVALID_HANDLE_VALUE;
	HANDLE	hMapExe		= NULL;
	PBYTE	pBufExe		= NULL;
	DWORD	cbExe		= 0;
	
	PPESHIELD_METADATA		pMetaData;
	
	STARTUPINFO				StartupInfo = {0};
	PROCESS_INFORMATION		ProcessInfo = {0};
	
	if (!g_bInitSpy)
	{
		return FALSE;
	}

	// Find module hwnd
	hWndLoader = FindWindow(PESHIELD_LOADER_WNDCLASS, PESHIELD_LOADER_WINDOW);
	
	// Does window not exist ?
	if (!hWndLoader)
	{
		PESHIELD_LOG("Information : Pe Shield Loader window is not found.");

		// Create exe mapping
		if (PeShield_CreateFileMapping(
			&hFileExe,
			&hMapExe,
			&pBufExe,
			&cbExe,
			g_szExePath,
			0,
			GENERIC_READ,
			0,	// doesn't share, it's dangerous someone access exe during this process
			OPEN_EXISTING
			))
		{
			PESHIELD_LOG("Success : Exe mapping is created.");

			// Get meta data
			pMetaData = PeShield_GetMetaData(pBufExe, cbExe);
			
			// Is meta data valid ? 
			if (PeShield_IsMetaDataValid(pMetaData, cbExe))
			{
				PESHIELD_LOG("Success : Metadata is valid.");

				// Get system directory
				if (GetSystemDirectory(szSysPath, MAX_PATH))
				{
					PESHIELD_LOG("Success : System directory is got.");

					// Make module path
					sprintf(szLoaderPath, "%s\\%s", szSysPath, PESHIELD_LOADER_FILE);
					
					// Create new module mapping
					if (PeShield_CreateFileMapping(
						&hFileLoader,
						&hMapLoader,
						&pBufLoader,
						NULL,
						szLoaderPath,
						pMetaData->LoaderLength,
						GENERIC_READ|GENERIC_WRITE,
						0,
						CREATE_ALWAYS	// overwrite if the file exists
						))
					{
						PESHIELD_LOG("Success : Loader mapping is created.");

						// Copy module file
						CopyMemory(
							pBufLoader,
							pBufExe + pMetaData->LoaderOffset,
							pMetaData->LoaderLength
							);

						PESHIELD_LOG("Success : Loader is copied into system directory.");
						
						// Close module mapping
						PeShield_CloseFileMapping(
							&hFileLoader,
							&hMapLoader,
							&pBufLoader,
							NULL
							);

						PESHIELD_LOG("Success : Loader mapping is closed.");
						
						// Create and run module process
						StartupInfo.cb = sizeof(StartupInfo);
						if (CreateProcess(
							szLoaderPath,
							NULL,
							NULL,
							NULL,
							FALSE,
							0,
							NULL,
							NULL,
							&StartupInfo,
							&ProcessInfo
							))
						{
							PESHIELD_LOG("Success : Loader process is created.");
								
							// Wait for module process' initialization
							WaitForInputIdle(ProcessInfo.hProcess, INFINITE);
							
							// Find module hwnd again
							hWndLoader = FindWindow(PESHIELD_LOADER_WNDCLASS, PESHIELD_LOADER_WINDOW);
							if (hWndLoader)
							{
								bLoaded = TRUE;
								PESHIELD_LOG("Success : Pe Shield Loader window is found.");
							}
							else
							{
								PESHIELD_LOG("Error : Pe Shield Loader window is not found.");
							}
							
							// Close process handle
							CloseHandle(ProcessInfo.hThread);
							CloseHandle(ProcessInfo.hProcess);
						}
						else
						{
							PESHIELD_LOG("Error : Loader process is not created.");
						}
					}
					else
					{
						PESHIELD_LOG("Error : Loader mapping is not created.");
					}
				}
				else
				{
					PESHIELD_LOG("Error : System directory is not got.");
				}
			}
			else
			{
				PESHIELD_LOG("Fail : Metadata is invalid.");
			}
			
			// Close exe mapping
			PeShield_CloseFileMapping(
				&hFileExe,
				&hMapExe,
				&pBufExe,
				&cbExe
				);
		}
		else
		{
			PESHIELD_LOG("Error : Exe mapping is not created.");
		}
	}
	else
	{
		bLoaded = TRUE;
		PESHIELD_LOG("Success : Pe Shield Loader window is found.");
	}

	// Is all loaded ?
	if (bLoaded)
	{
		// Set input window handle
		*phWndLoader = hWndLoader;
	}

	return bLoaded;
}

//
// Send WM_PESHIELD_CREATE to Loader
//
BOOL 
PeShield_SendCreate(
	IN HWND hWndLoader,
	IN LPCSTR lpCmdLine
	)
{	
	BOOL		bSucceeded = FALSE;

	HANDLE		hMutexCreate;
	HANDLE		hSectionCreate;
	PPESHIELD_CREATE_REQUEST	CreateRequest;

	if (!g_bInitSpy)
	{
		return FALSE;
	}
	
	// Open create mutex
	hMutexCreate = OpenMutex(SYNCHRONIZE, FALSE, PESHIELD_CREATE_MUTEX);
	if (hMutexCreate)
	{
		PESHIELD_LOG("Success : Create mutex is opened.");
	
		// Wait for the mutex
		PESHIELD_LOG("Information : Wait for create mutex.");
		if (WaitForSingleObject(hMutexCreate, INFINITE) != WAIT_FAILED)
		{
			// Open create section
			hSectionCreate = OpenFileMapping(FILE_MAP_WRITE, FALSE, PESHIELD_CREATE_SECTION);
			if (hSectionCreate)
			{
				PESHIELD_LOG("Success : Create section is opened.");

				// Map create section view
				CreateRequest = (PPESHIELD_CREATE_REQUEST)MapViewOfFile(
					hSectionCreate, 
					FILE_MAP_WRITE,
					0,
					0,
					sizeof(PESHIELD_CREATE_REQUEST)
					);
				if (CreateRequest)
				{
					// Set create request
					strcpy(CreateRequest->ExePath, g_szExePath);
					strcpy(CreateRequest->CmdLine, lpCmdLine);

					// Send WM_PESHIELD_CREATE to Pe Shield Loader
					if (SendMessage(hWndLoader, WM_PESHIELD_CREATE, 0, 0) == 0)
					{
						// Set success
						bSucceeded = TRUE;

						PESHIELD_LOG("Success : WM_PESHIELD_CREATE handling succeeded.");
					}
					else
					{
						PESHIELD_LOG("Fail : WM_PESHIELD_CREATE handling failed.");
					}

					// Unmap create section view
					UnmapViewOfFile(CreateRequest);
					PESHIELD_LOG("Success : Create section view is unmapped.");
				}
				else
				{
					PESHIELD_LOG("Error : Create section is not opened.");
				}

				CloseHandle(hSectionCreate);
				PESHIELD_LOG("Success : Create section is closed.");
			}
			else
			{
				PESHIELD_LOG("Error : Create section is not opened.");
			}

			// Release create mutex
			ReleaseMutex(hMutexCreate);
			PESHIELD_LOG("Success : Create mutex is released.");
		}
		else
		{
			PESHIELD_LOG("Error : Create mutex is not waitable.");
		}

		// Close create mutex
		CloseHandle(hMutexCreate);
		PESHIELD_LOG("Success : Create mutex is closed.");
	}
	else
	{
		PESHIELD_LOG("Error : Create mutex is not opened.");
	}

	return bSucceeded;
}