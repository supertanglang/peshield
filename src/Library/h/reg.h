//----------------------------------------------------------------------
//			 R  E  G  I  S  T  R  Y      L  I  B  R  A  R  Y
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _REG_H_
#define _REG_H_

#include <windows.h>

#ifndef _T
#define _T(csz)	TEXT(csz)
#endif

//
// Pe Shield Regstry Key
//
#define PESHIELD_REGKEY			_T("SOFTWARE\\Senol\\PeShield")
#define PESHIELD_REGKEY_LOADER	_T("Loader")

//
// Length and Max Size
//
#define MAX_REG			255


//----------------------------------------------------------------------
//       R  E  G  I  S  T  R  Y      M  A  N  A  G  E  M  E  N  T
//---------------------------------------------------------------------- 

//
// Get Pe Shield Subkey
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID 
PeShield_RegGetSubKey(
	OUT LPTSTR szRegKey, 
	IN LPCTSTR szSubkey
	);

//
// Get Pe Shield Subkey Handle
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
HKEY 
PeShield_RegGetSubKeyHandle(
	IN LPCTSTR szSubkey
	);

//
// Does Pe Shield registry key exist ?
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_RegDoesKeyExist(
	IN LPCTSTR szSubkey
	);

//
// Create Pe Shield registry key
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_RegCreateKey(
	IN LPCTSTR szSubkey
	);

//
// Set Pe Shield registry value
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL 
PeShield_RegSetValue(
	IN LPCTSTR szSubkey,
	IN LPCVOID pBufValue, 
	IN DWORD cbValue,
	IN LPCTSTR szValueName OPTIONAL,
	IN DWORD dwType
	);

//
// Get Pe Shield registry value
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_RegGetValue(
	IN LPCTSTR szSubkey, 
	OUT PDWORD pcbValue,
	OUT PVOID pBufValue OPTIONAL,
	IN LPCTSTR szValueName OPTIONAL,
	OUT PDWORD pdwType OPTIONAL);

// 
// Delete Pe Shield registry key
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_RegDeleteKey(
	IN LPCTSTR szSubkey);


#endif // _REG_H_