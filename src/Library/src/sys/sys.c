#include "../../h/sys.h"

//----------------------------------------------------------------------
//				D  R  I  V  E  R      S  U  P  P  O  R  T
//---------------------------------------------------------------------- 

//
// Get driver file path 
//
BOOL 
PeShield_GetDriverPath(
	OUT LPTSTR szSysPath
	)
{
	TCHAR	systemRoot[MAX_PATH+1];

	// Get system root directory
	if (!GetEnvironmentVariable(_T("SYSTEMROOT"), systemRoot, sizeof(systemRoot))) 
	{
		return FALSE;
	}
	
	wsprintf(szSysPath, _T("%s\\system32\\drivers\\%s"), systemRoot, PESHIELD_SYS_FILE);

	return TRUE;
}

//
// Install driver service
//
BOOL 
PeShield_InstallDriverService(
	IN SC_HANDLE SchSCManager, 
	IN LPCTSTR DriverName, 
	IN LPCTSTR ServiceExe
	)
{
    SC_HANDLE  schService;

    schService = CreateService(
		SchSCManager,			// SCManager database
        DriverName,				// name of service
        DriverName,				// name to display
        SERVICE_ALL_ACCESS,		// desired access
        SERVICE_KERNEL_DRIVER,	// service type
        SERVICE_DEMAND_START,	// start type
        SERVICE_ERROR_NORMAL,	// error control type
        ServiceExe,				// service's binary
        NULL,					// no load ordering group
        NULL,					// no tag identifier
        NULL,					// no dependencies
        NULL,					// LocalSystem account
        NULL					// no password
        );
    if (!schService)
	{
        return FALSE;
	}

    CloseServiceHandle(schService);

    return TRUE;
}

//
// Uninstall driver service
//
BOOL 
PeShield_UninstallDriverService( 
	IN SC_HANDLE SchSCManager, 
	IN LPCTSTR DriverName
	)
{
    SC_HANDLE  schService;
    BOOL       ret;

    schService = OpenService( 
		SchSCManager,
        DriverName,
        SERVICE_ALL_ACCESS
        );

    if (!schService)
	{
        return FALSE;
	}

    ret = DeleteService(schService);

    CloseServiceHandle(schService);

    return ret;
}

// Start driver service
BOOL 
PeShield_StartDriverService( 
	IN SC_HANDLE SchSCManager, 
	IN LPCTSTR DriverName 
	)
{
    SC_HANDLE  schService;
    BOOL       ret;

    schService = OpenService( 
		SchSCManager,
        DriverName,
        SERVICE_ALL_ACCESS
        );
    if (!schService)
	{
        return FALSE;
	}

    ret = StartService(schService, 0, NULL)
       || GetLastError() == ERROR_SERVICE_ALREADY_RUNNING 
	   || GetLastError() == ERROR_SERVICE_DISABLED;

    CloseServiceHandle(schService);

    return ret;
}

//
// Stop driver service
//
BOOL 
PeShield_StopDriverService( 
	IN SC_HANDLE SchSCManager, 
	IN LPCTSTR DriverName 
	)
{
    SC_HANDLE       schService;
    BOOL            ret;
    SERVICE_STATUS  serviceStatus;

    schService = OpenService(SchSCManager, DriverName, SERVICE_ALL_ACCESS);
    if (!schService)
	{
        return FALSE;
	}

    ret = ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus);

    CloseServiceHandle(schService);

    return ret;
}

//
// Open driver handle
//
BOOL 
PeShield_OpenDriver( 
	IN LPCTSTR DriverName, 
	OUT HANDLE* phSys OPTIONAL
	)
{
    TCHAR    completeDeviceName[64];
    HANDLE   hDevice;

  	if (GetVersion() & 0xFF >= 5) 
	{
		wsprintf(completeDeviceName, _T("\\\\.\\Global\\%s"), DriverName);

	} 
	else 
	{
		wsprintf(completeDeviceName, _T("\\\\.\\%s"), DriverName);
	}

    hDevice = CreateFile(
		completeDeviceName,
        GENERIC_READ|GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );
    if (hDevice == INVALID_HANDLE_VALUE)
	{
        return FALSE;
	}

	// If user wants handle, give it to them.  Otherwise, just close it.
	if (phSys)
	{
		*phSys = hDevice;
	}
	else
	{
	    CloseHandle(hDevice);
	}

    return TRUE;
}

//
// Load driver
//
BOOL 
PeShield_LoadDriver( 
	OUT HANDLE* phSys,
	IN HINSTANCE hInstance,
	IN WORD wSysResId,
	IN LPCTSTR szSysResType
	)
{
	SC_HANDLE	schSCManager;
	BOOL		bSucceeded = FALSE;
	
	SC_HANDLE	schService;
	TCHAR		szSysPath[MAX_PATH+1];
	HANDLE		hFileSys = INVALID_HANDLE_VALUE;
	DWORD		cbWritten;
	
	HRSRC		hResSys;
	HGLOBAL		hGResSys;
	DWORD		cbSys;
	LPVOID		pBufSys;

	// Get driver path
	if (!PeShield_GetDriverPath(szSysPath))
	{
		return FALSE;
	}

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	schService = OpenService( 
		schSCManager,
        PESHIELD_SYS_NAME,
        SERVICE_ALL_ACCESS
        );
	if (schService)
	{
		bSucceeded = TRUE;
	}
	else 
   	{
		// Find driver from resource
		hResSys = FindResource((HMODULE)hInstance, MAKEINTRESOURCE(wSysResId), szSysResType);
		if (hResSys)
		{
			// Load driver from resource
			hGResSys = LoadResource((HMODULE)hInstance, hResSys);	
			if (hGResSys)
			{
				// Get sys size and buffer
				cbSys	= SizeofResource((HMODULE)hInstance, hResSys);
				pBufSys	= LockResource(hGResSys);
				
				// Create new sys file
				hFileSys = CreateFile(
					szSysPath,
					GENERIC_READ|GENERIC_WRITE,
					0,
					NULL,
					CREATE_ALWAYS,
					0,
					NULL
					);
				if (hFileSys != INVALID_HANDLE_VALUE)
				{
					// Write sys file
					if (WriteFile(
						hFileSys,
						pBufSys,
						cbSys,
						&cbWritten,
						NULL
						))
					{
						CloseHandle(hFileSys);
						hFileSys = INVALID_HANDLE_VALUE;
						
						// Ignore success of installation: it may already be installed.
						PeShield_InstallDriverService(schSCManager, PESHIELD_SYS_NAME, szSysPath);
						
						// Ignore success of start: it may already be started.
						PeShield_StartDriverService(schSCManager, PESHIELD_SYS_NAME);

						bSucceeded = TRUE;
					}
				}
			}
		}
	}
	
	// Is operation succeeded ?
	if (bSucceeded)
	{
		// Do make sure we can open it.
		bSucceeded = PeShield_OpenDriver(PESHIELD_SYS_NAME, phSys);
		if (!bSucceeded)
		{
			// Uninstall driver service
			PeShield_UninstallDriverService(schSCManager, PESHIELD_SYS_NAME);

			// Close driver service to do operation again
			CloseServiceHandle(schService);
			schService = NULL;

			// Do whole operation one more time
			bSucceeded = PeShield_LoadDriver(phSys, hInstance, wSysResId, szSysResType);
		}
	}

	if (hFileSys != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFileSys);
	}
	
	if (schService)
	{
		CloseServiceHandle(schService);
	}
	
	if (schSCManager)
	{
		CloseServiceHandle(schSCManager);
	}
	
	return bSucceeded;
}

//
// Unload driver
//
VOID 
PeShield_UnloadDriver(
	IN OUT HANDLE* phSys OPTIONAL
	)
{
	SC_HANDLE	schSCManager;

	TCHAR		szSysPath[MAX_PATH+1];

	if (phSys && *phSys != INVALID_HANDLE_VALUE)
	{
		CloseHandle(*phSys);
		*phSys = INVALID_HANDLE_VALUE;
	}

	schSCManager = OpenSCManager(	
		NULL,                 // machine (NULL == local)
        NULL,                 // database (NULL == default)
		SC_MANAGER_ALL_ACCESS // access required
		);

	PeShield_StopDriverService(schSCManager, PESHIELD_SYS_NAME);
	PeShield_UninstallDriverService(schSCManager, PESHIELD_SYS_NAME);
	 
	CloseServiceHandle(schSCManager);

	// Get driver path
	PeShield_GetDriverPath(szSysPath);

	// Delete driver file
	DeleteFile(szSysPath);
}
