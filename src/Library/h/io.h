//----------------------------------------------------------------------
//			 F  I  L  E       I  O       L  I  B  R  A  R  Y
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _IO_H_
#define _IO_H_

#include <windows.h>

//----------------------------------------------------------------------
//      F  I  L  E       I / O        M  A  N  A  G  E  M  E  N  T
//---------------------------------------------------------------------- 

//
// Create file mapping, get file and mapping handle, buffer pointer
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
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
	);

//
// Close file and mapping handle and set them to zero
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID
PeShield_CloseFileMapping(
	IN OUT HANDLE* phFile OPTIONAL,
	IN OUT HANDLE* phMap OPTIONAL,
	IN OUT PBYTE* ppBuf OPTIONAL,
	IN OUT DWORD* pcbSize OPTIONAL
	);

//
// Truncate file
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_TruncateFile(
	IN LPCTSTR szPath,
	IN DWORD cbSize,
	IN DWORD dwMethod
	);

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
	);


//----------------------------------------------------------------------
//               P  A  T  H      A N D      N  A  M  E
//---------------------------------------------------------------------- 

//
// Get file name with extension from full path
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID
PeShield_GetFileDirectory(
	OUT LPTSTR szDirectory, 
	IN LPCTSTR szPath
	);

//
// Get file directory from full path
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID
PeShield_GetFileName(
	OUT LPTSTR szName, 
	IN LPCTSTR szPath
	);

//
// Get file extension from full path
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID
PeShield_GetFileExt(
	OUT LPTSTR szExt,
	IN LPCTSTR szPath
);


#endif // _IO_H_
