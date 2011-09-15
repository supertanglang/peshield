#include "../../h/reg.h"

//----------------------------------------------------------------------
//       R  E  G  I  S  T  R  Y      M  A  N  A  G  E  M  E  N  T
//---------------------------------------------------------------------- 

//
// Get Pe Shield Subkey
//
VOID 
PeShield_RegGetSubKey(
	OUT LPTSTR szRegKey, 
	IN LPCTSTR szSubkey
	)
{
	lstrcpy(szRegKey, PESHIELD_REGKEY);
	if (szSubkey)
	{
		lstrcat(szRegKey, _T("\\"));
		lstrcat(szRegKey, szSubkey);
	}
}

//
// Get Pe Shield Subkey Handle
//
HKEY 
PeShield_RegGetSubKeyHandle(
	IN LPCTSTR szSubkey
	)
{
	TCHAR	szRegKey[MAX_REG];
	LONG	lResult;	
	HKEY	hKey;
	
	PeShield_RegGetSubKey(szRegKey, szSubkey);
	
	lResult = RegOpenKey(
		HKEY_LOCAL_MACHINE,
		szRegKey,
		&hKey);
	if (lResult != NO_ERROR)
		return NULL;
	
	return hKey;
}

//
// Does Pe Shield registry key exist ?
//
BOOL
PeShield_RegDoesKeyExist(
	IN LPCTSTR szSubkey
	)
{
	TCHAR	szRegKey[MAX_REG];
	HKEY	hKey;
	LONG	lResult;

	PeShield_RegGetSubKey(szRegKey, szSubkey);
	
	lResult = RegOpenKey(
		HKEY_LOCAL_MACHINE, 
		szRegKey, 
		&hKey);
	
	if (lResult == ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return TRUE;
	}	
	else 
		return FALSE;
}

//
// Create Pe Shield registry key
//
BOOL
PeShield_RegCreateKey(
	IN LPCTSTR szSubkey
	)
{
	TCHAR	szRegKey[MAX_REG];
	HKEY	hKey;
	LONG	lResult; 

	PeShield_RegGetSubKey(szRegKey, szSubkey);
	
	lResult = RegCreateKey(
		HKEY_LOCAL_MACHINE, 
		szRegKey, 
		&hKey);
	RegCloseKey(hKey);

	return (lResult == ERROR_SUCCESS);
}

//
// Set Pe Shield registry value
//
BOOL 
PeShield_RegSetValue(
	IN LPCTSTR szSubkey,
	IN LPCVOID pBufValue, 
	IN DWORD cbValue,
	IN LPCTSTR szValueName OPTIONAL,
	IN DWORD dwType
	)
{
	HKEY	hKey;
	LONG	lResult;

	hKey = PeShield_RegGetSubKeyHandle(szSubkey);
	
	lResult = RegSetValueEx(
		hKey, 
		szValueName, 
		0,
		dwType, 
		(CONST PBYTE)pBufValue,
		cbValue);

	return (lResult == ERROR_SUCCESS);
}

//
// Get Pe Shield registry value
//
BOOL
PeShield_RegGetValue(
	IN LPCTSTR szSubkey, 
	OUT PDWORD pcbValue,
	OUT PVOID pBufValue OPTIONAL,
	IN LPCTSTR szValueName OPTIONAL,
	OUT PDWORD pdwType OPTIONAL)
{
	HKEY	hKey;
	LONG	lResult;

	hKey = PeShield_RegGetSubKeyHandle(szSubkey);
	
	lResult = RegQueryValueEx(
		hKey,
		szValueName,
		NULL,
		pdwType,
		(PBYTE)pBufValue,
		pcbValue);
	RegCloseKey(hKey);
	
	return (lResult == ERROR_SUCCESS);
}

// 
// Delete Pe Shield registry key
//
BOOL
PeShield_RegDeleteKey(
	IN LPCTSTR szSubkey)
{
	TCHAR	szRegKey[MAX_REG];
	LONG	lResult;
	
	PeShield_RegGetSubKey(szRegKey, szSubkey);
	
	lResult = RegDeleteKey(HKEY_LOCAL_MACHINE, szRegKey);
	
	return (lResult == ERROR_SUCCESS);
}

