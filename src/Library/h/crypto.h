//----------------------------------------------------------------------
//				C  R  Y  P  T  O      L  I  B  R  A  R  Y
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <windows.h>

//----------------------------------------------------------------------
//			   D  E  S       E  N  C  R  Y  P  T  I  O  N
//---------------------------------------------------------------------- 

//
// Default fragment size
// 
#define PESHIELD_FRAGSIZE	1024

//
// Encrypt by des algorithm ( target size is the same with source size )
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplusVOID
VOID
PeShield_EncryptDes(
	IN OUT PBYTE pBuffer,
	IN DWORD cbBuffer,
	IN DWORD cbFrag,
	IN HWND hProgress OPTIONAL
	);

//
// Decrypt by des algorithm ( target size is the same with source size )
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplusVOID
VOID
PeShield_DecryptDes(
	IN OUT PBYTE pBuffer,
	IN DWORD cbBuffer,
	IN DWORD cbFrag,
	IN HWND hProgress OPTIONAL
	);


//----------------------------------------------------------------------
//			  S  H  A  1       E  N  C  R  Y  P  T  I  O  N
//---------------------------------------------------------------------- 

//
// Get sha1 hash
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplusVOID
VOID
PeShield_GetSha1Hash(
	OUT PBYTE pBufTgt,
	IN CONST PBYTE pBufSrc,
	IN DWORD cbSrc
	);


#endif // _CRYPTO_H_