#include "..\..\h\crypto.h"

#include "des\des.h"
#include "sha1\sha1.h"

//----------------------------------------------------------------------
//			   D  E  S       E  N  C  R  Y  P  T  I  O  N
//---------------------------------------------------------------------- 

//
// Des Encryption Keys
//
static BYTE	UserKey[DES_USER_KEY_LEN]	= { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
static BYTE	IV[DES_BLOCK_LEN]			= { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };

//
// Encrypt by des algorithm ( target size is the same with source size )
//
VOID
PeShield_EncryptDes(
	IN OUT PBYTE pBuffer,
	IN DWORD cbBuffer,
	IN DWORD cbFrag,
	IN HWND hProgress OPTIONAL
	)
{
	DES_Crypt(
		AI_ENC,
		AI_CBC,
		UserKey,
		IV,
		cbFrag,
		hProgress,
		pBuffer,
		pBuffer,
		cbBuffer);
}

//
// Decrypt by des algorithm ( target size is the same with source size )
//
VOID
PeShield_DecryptDes(
	IN OUT PBYTE pBuffer,
	IN DWORD cbBuffer,
	IN DWORD cbFrag,
	IN HWND hProgress OPTIONAL
	)
{
	DES_Crypt(
		AI_DEC,
		AI_CBC,
		UserKey,
		IV,
		cbFrag,
		hProgress,
		pBuffer,
		pBuffer,
		cbBuffer);
}


//----------------------------------------------------------------------
//			  S  H  A  1       E  N  C  R  Y  P  T  I  O  N
//---------------------------------------------------------------------- 

//
// Get sha1 hash
//
VOID
PeShield_GetSha1Hash(
	OUT PBYTE pBufTgt,
	IN CONST PBYTE pBufSrc,
	IN DWORD cbSrc
	)
{
	EncryptSHA1(
		pBufTgt,
		pBufSrc,
		cbSrc
		);
}

