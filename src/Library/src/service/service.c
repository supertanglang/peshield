#include "../../h/service.h"

#include <shlobj.h>
#include <tlhelp32.h>

//----------------------------------------------------------------------
//				  C  E  R  T  I  F  I  C  A  T  I  O  N
//----------------------------------------------------------------------

//
// Is exe web-certified ?
//
BOOL
PeShield_IsExeWebCertified(
	IN PPESHIELD_METADATA pMetaData
	)
{
	int i;

	// Is certified flag true ?
	if (!pMetaData->Certified)
	{
		return FALSE;
	}
	
	// Is serial full ascii ?
	for (i = 0; i < LEN_SERIAL; i++)
	{
		if (pMetaData->Serial[i] < 0 || pMetaData->Serial[i] > 127)
		{
			return FALSE;
		}
	}

	// Is serial null terminated ?
	if (pMetaData->Serial[LEN_SERIAL] != 0)
	{
		return FALSE;
	}
		
	return TRUE;
}


//----------------------------------------------------------------------
//			      U  S  E  R       S  U  P  P  O  R  T
//---------------------------------------------------------------------- 

//
// Connect to web site
//
BOOL
PeShield_ConnectToWebSite(
	IN LPCSTR szWebAddress
	)
{
	TCHAR szCmdLine[MAX_PATH+MAX_URL+1];
	PROCESS_INFORMATION	pi	= {0,};
	STARTUPINFO si			= {0,};
	
	// Set web address as a command line
	wsprintf(szCmdLine, _T("explorer.exe %s"), szWebAddress);
	
	// Run explorer
	si.cb = sizeof(si);
	if (!CreateProcess(
		NULL,
		szCmdLine,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi))
	{
		return FALSE;
	}

	// Close process and thread handle
	if (pi.hProcess)
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	return TRUE;
}


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
	)
{
	HANDLE			hSnapShot;	
	PROCESSENTRY32	Entry;
	BOOL			hRes;
	HANDLE			hProcess;

	//Get the snapshot of the system
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

	//Get first process
	Entry.dwSize = sizeof(Entry);
	Process32First (hSnapShot, &Entry);

	while (TRUE)
	{
		hRes = Process32Next(hSnapShot, &Entry);
		if (!hRes)
		{
			break;
		}

		// Is this process named the same ?
		if (lstrcmp(Entry.szExeFile, szName) == 0)
		{
			// Open process handle
			hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, Entry.th32ProcessID);
			if (hProcess)
			{
				// Terminate process
				TerminateProcess(hProcess, 1);

				// Close process handle
				CloseHandle(hProcess);
			}
		}
	}
}


//----------------------------------------------------------------------
//			   F  I  L  E      M  A  N  A  G  E  M  E  N  T
//----------------------------------------------------------------------

//
// Delete file itself
//
BOOL
PeShield_DeleteSelf(
	IN LPCTSTR szPath
	)
{
	SHELLEXECUTEINFO sei;
	
	TCHAR szShort [MAX_PATH];
    TCHAR szComspec[MAX_PATH];
    TCHAR szParams [MAX_PATH];
	
	// Get short path
	if (!GetShortPathName(szPath, szShort, MAX_PATH))
	{
		return FALSE;
	}
	
	// Get COMSPEC environment value
	if (!GetEnvironmentVariable(_T("COMSPEC"), szComspec, MAX_PATH))
	{
		return FALSE;
	}
	
	// Write command lines
	lstrcpy(szParams, "/c del ");
	lstrcat(szParams, szShort);
	lstrcat(szParams, " > nul");
	
	sei.cbSize       = sizeof(sei);
	sei.hwnd         = 0;
	sei.lpVerb       = "Open";
	sei.lpFile       = szComspec;
	sei.lpParameters = szParams;
	sei.lpDirectory  = 0;
	sei.nShow        = SW_HIDE;
	sei.fMask        = SEE_MASK_NOCLOSEPROCESS;
	
	// Set priority to realtime
	SetPriorityClass(GetCurrentProcess(),
		REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(),
		THREAD_PRIORITY_TIME_CRITICAL);
	
	// Shell command lines
	if (!ShellExecuteEx(&sei))
	{
		// Set priority to normal back
		SetPriorityClass(GetCurrentProcess(),
			NORMAL_PRIORITY_CLASS);
		SetThreadPriority(GetCurrentThread(),
			THREAD_PRIORITY_NORMAL);
		
		return FALSE;
	}
	
	// Set priority to idle
	SetPriorityClass(sei.hProcess, IDLE_PRIORITY_CLASS);
	SetProcessPriorityBoost(sei.hProcess, TRUE);
	
	// Run command lines ( transaction )
	SHChangeNotify(SHCNE_DELETE, SHCNF_PATH, szShort, 0);

	return TRUE;
}

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
	)
{
	LPTSTR	slash;
	TCHAR	hex[3];
	int		i, len, dir;	
	
	slash = strrchr(szFilePath, '\\');
	if (!slash)
	{
		return FALSE;
	}

	slash++;
	len = lstrlen(slash);
	dir = (slash - szFilePath) / sizeof(TCHAR);

	strncpy(szSecuredPath, szFilePath, dir);
	szSecuredPath[dir] = 0;

	for (i = 0; i < len; i++)
	{
		wsprintf(hex, _T("%02hx"), slash[i]);
		lstrcat(szSecuredPath, hex);
	}

	return TRUE;
}