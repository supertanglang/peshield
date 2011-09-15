#ifndef _DES_H_
#define _DES_H_

#include <windows.h>

#define DES_BLOCK_LEN		8	//	in BYTEs
#define DES_USER_KEY_LEN	8	//	in BYTEs

// EncType
#define AI_ENC	0
#define AI_DEC	1

// ModeType
#define AI_ECB	1
#define AI_CBC	2
#define AI_OFB	3
#define AI_CFB	4

//
// Des encryption
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID
DES_Crypt(
	IN DWORD EncType,
	IN DWORD ModeType,
	IN BYTE* UserKey,
	IN BYTE* IV,
	IN DWORD Frag,
	IN HWND hProgress OPTIONAL,
	OUT BYTE* Target,		
	IN CONST BYTE* Source,	
	IN DWORD Size
	);

#endif // _DES_H_