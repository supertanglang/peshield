//----------------------------------------------------------------------
//					  P  E       L  I  B  R  A  R  Y
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _PE_H_
#define _PE_H_

#include <windows.h>

//----------------------------------------------------------------------
//			P  E     H  E  A  D  E  R     S  U  P  P  O  R  T
//----------------------------------------------------------------------

//
// Pe header size
//
#define	IMAGE_SIZEOF_HEADERS	4096

//
// Get pe headers from pe image
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL 
PeShield_GetPeHdrs(
	IN CONST PBYTE pBufPe, 
	OUT PIMAGE_DOS_HEADER* ppDosHdr OPTIONAL,
	OUT PIMAGE_NT_HEADERS* ppNtHdrs OPTIONAL,
	OUT PIMAGE_SECTION_HEADER* ppSecHdrs OPTIONAL,
	OUT PBYTE* ppSections OPTIONAL
	);

//
// Get resource section size
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_GetSectionInformation(
	IN CONST PBYTE pBufPe,
	IN LPCTSTR szSectionName,
	OUT DWORD* pOffset OPTIONAL,
	OUT DWORD* pLength OPTIONAL,
	OUT DWORD* pRvAddr OPTIONAL,
	OUT DWORD* pRvSize OPTIONAL
	);


//----------------------------------------------------------------------
//				I  M  A  G  E     C  H  E  C  K  I  N  G   
//----------------------------------------------------------------------

//
// Is image executable ?
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_IsImageExecutable(
	IN CONST PBYTE pBufImage
	);


//----------------------------------------------------------------------
//						  H  O  O  K  I  N  G
//----------------------------------------------------------------------

//
// Hook source resource to target ( target must not have resource ) 
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_HookResource(
	OUT PBYTE pBufTgt,
	IN CONST PBYTE pBufSrc,
	IN DWORD cbTgt
	);


#endif // _PE_H_