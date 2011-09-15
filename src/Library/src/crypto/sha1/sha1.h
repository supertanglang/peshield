#ifndef _SHA1_H_
#define _SHA1_H_

#include <windows.h>

#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID
EncryptSHA1(
	OUT PBYTE pBufTgt,
	IN CONST PBYTE pBufSrc,
	IN DWORD cbSrc);

#endif // _SHA1_H_