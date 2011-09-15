#include "sha1.h"
#include "sha1_in.h"

extern "C"
VOID
EncryptSHA1(
	OUT PBYTE pBufTgt,
	IN CONST PBYTE pBufSrc,
	IN DWORD cbSrc)
{
	CSHA1 sha1;
	
	sha1.Reset();
	sha1.Update((UINT_8 *)pBufSrc, cbSrc);
	sha1.Final();
	
	sha1.GetHash((UINT_8 *)pBufTgt);
}