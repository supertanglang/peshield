#include "..\..\h\io.h"

#include <tchar.h>

//----------------------------------------------------------------------
//      F  I  L  E       I / O        M  A  N  A  G  E  M  E  N  T
//---------------------------------------------------------------------- 

//
// Create file mapping, get file and mapping handle, buffer pointer
//
BOOL
PeShield_CreateFileMapping(
	OUT HANDLE* phFile,
	OUT HANDLE* phMap,
	OUT PBYTE* ppBuf,
	OUT DWORD* pcbSize OPTIONAL,
	IN LPCTSTR szPath,
	IN DWORD cbMapSize,
	IN DWORD dwAccess,
	IN DWORD dwShare,
	IN DWORD dwCreate
	)
{
	HANDLE	hFile;
	HANDLE	hMap;
	PVOID	pBuf;
	DWORD	cbSize;

	hFile = CreateFile(
		szPath,
		dwAccess,
		dwShare,
		NULL,
		dwCreate,
		0,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	
	hMap = CreateFileMapping(
		hFile, 
		NULL,
		(dwAccess & GENERIC_WRITE) ? PAGE_READWRITE : PAGE_READONLY,
		0,
		cbMapSize,
		NULL);
	if (!hMap)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	pBuf = MapViewOfFile(
		hMap,
		(dwAccess & GENERIC_WRITE) ? FILE_MAP_WRITE : FILE_MAP_READ,
		0, 
		0, 
		cbMapSize);
	if (!pBuf) 
	{
		CloseHandle(hMap);
		CloseHandle(hFile);
		return FALSE;
	}
	
	if (pcbSize)
	{
		cbSize = GetFileSize(hFile, NULL);
		if (cbSize == 0xFFFFFFFF && GetLastError() != NO_ERROR)
		{
			UnmapViewOfFile(pBuf);
			CloseHandle(hMap);
			CloseHandle(hFile);
			
			return FALSE;
		}

		*pcbSize	= cbSize;	
	}

	*phFile		= hFile;
	*phMap		= hMap;
	*ppBuf		= (PBYTE)pBuf;

	return TRUE;
}

//
// Close file and mapping handle and set them to zero
//
VOID
PeShield_CloseFileMapping(
	IN OUT HANDLE* phFile OPTIONAL,
	IN OUT HANDLE* phMap OPTIONAL,
	IN OUT PBYTE* ppBuf OPTIONAL,
	IN OUT DWORD* pcbSize OPTIONAL
	)
{	
	if (pcbSize && *pcbSize)
	{
		*pcbSize = 0;
	}
	
	if (ppBuf && *ppBuf)
	{
		UnmapViewOfFile(*ppBuf);
		*ppBuf = NULL;
	}	
	if (phMap && *phMap)
	{
		CloseHandle(*phMap);
		*phMap = NULL;
	}
	if (phFile && *phFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(*phFile);
		*phFile = INVALID_HANDLE_VALUE;
	} 
}

//
// Truncate file
//
BOOL
PeShield_TruncateFile(
	IN LPCTSTR szPath,
	IN DWORD cbSize,
	IN DWORD dwMethod
	)
{
	BOOL	bSucceeded	= FALSE;
	
	HANDLE	hFile; 
	DWORD	dwCurPointer;
	DWORD	dwResult;

	// Create file handle
	hFile = CreateFile(
		szPath, 
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
		);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		// Save current file pointer
		dwCurPointer = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
		if (dwCurPointer != 0xFFFFFFFF || 
			(dwCurPointer == 0xFFFFFFFF && GetLastError() == NO_ERROR))
		{
			// Set new file pointer
			dwResult = SetFilePointer(hFile, cbSize, NULL, dwMethod);
			if (dwResult != 0xFFFFFFFF || 
				(dwResult == 0xFFFFFFFF && GetLastError() == NO_ERROR))
			{
				// Set EOF on it
				if (SetEndOfFile(hFile))
				{
					// Set saved file pointer back
					dwResult = SetFilePointer(hFile, dwCurPointer, NULL, FILE_BEGIN);
					if (dwResult != 0xFFFFFFFF || 
						(dwResult == 0xFFFFFFFF && GetLastError() == NO_ERROR))
					{
						bSucceeded = TRUE;
					}
				}
			}
		}

		// Close file handle
		CloseHandle(hFile);
	}
	
	return bSucceeded;
}

//
// Hide File
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_HideFile(
	IN LPCTSTR szPath,
	IN BOOL bHide
	)
{
	return SetFileAttributes(
		szPath, 
		(bHide) ? 
		FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN : 
		FILE_ATTRIBUTE_NORMAL
		);
}


//----------------------------------------------------------------------
//               P  A  T  H      A N D      N  A  M  E
//---------------------------------------------------------------------- 

//
// Get file name with extension from full path
//
VOID
PeShield_GetFileDirectory(
	OUT LPTSTR szDirectory, 
	IN LPCTSTR szPath
	)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	
	_tsplitpath(
		szPath,
		drive,
		dir,
		fname,
		ext);
	
	lstrcpy(szDirectory, drive);
	lstrcat(szDirectory, dir);
	szDirectory[lstrlen(szDirectory) - 1] = '\0';
}

//
// Get file directory from full path
//
VOID
PeShield_GetFileName(
	OUT LPTSTR szName, 
	IN LPCTSTR szPath
	)
{
	TCHAR drive[_MAX_DRIVE+1];
	TCHAR dir[_MAX_DIR+1];
	TCHAR fname[_MAX_FNAME+1];
	TCHAR ext[_MAX_EXT+1];
	
	_tsplitpath(
		szPath,
		drive,
		dir,
		fname,
		ext);
	
	lstrcpy(szName, fname);
}

//
// Get file extension from full path
//
VOID
PeShield_GetFileExt(
	OUT LPTSTR szExt,
	IN LPCTSTR szPath
)
{
	TCHAR drive[_MAX_DRIVE+1];
	TCHAR dir[_MAX_DIR+1];
	TCHAR fname[_MAX_FNAME+1];
	TCHAR ext[_MAX_EXT+1];
	
	_tsplitpath(
		szPath,
		drive,
		dir,
		fname,
		ext);
	
	lstrcpy(szExt, ext + 1);
}
