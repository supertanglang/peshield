//----------------------------------------------------------------------
//				P  E  S  H  I  E  L  D       T  O  O  L 
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _TOOL_H_
#define _TOOL_H_

//----------------------------------------------------------------------
//				I  N  I  T  I  A  L  I  Z  A  T  I  O  N
//---------------------------------------------------------------------- 

//
// Initialize Pe Shield Tool data
//
BOOL
PeShield_ToolStartup(
	IN HINSTANCE hInstance
	);

//
// Uninitialize Pe Shield Tool data
//
VOID
PeShield_ToolCleanup();


//----------------------------------------------------------------------
//					  E  N  C  R  Y  P  T  I  O  N
//----------------------------------------------------------------------

//
// Encryption Result
//
typedef enum {
	ENC_SUCCEEDED,				// Success		: Encryption succeeded
	ENC_ALREADY_ENCRYPTED,		// Fail			: File is already encrypted
	ENC_INVALID_WEB,			// Fail			: Invalid web address
	ENC_NOT_EXECUTABLE,			// Fail			: File is not executable
	ENC_NO_TXT_SECTION,			// Fail			: File has no text section
	ENC_NOT_INITIALIZED,		// Error		: Data is not initialized
	ENC_MAPPING_FAILED,			// Error		: File mapping failed
	ENC_RES_HOOKING_FAILED,		// Error		: Resource hooking failed
}ENCRYPT_RESULT, *PENCRYPT_RESULT;

//
// Encrypt exe file
//
ENCRYPT_RESULT
PeShield_EncryptExe(
	IN LPCTSTR szExePath,
	IN LPCTSTR szWebAddress,
	IN DWORD cbFrag,
	IN HWND hProgress OPTIONAL,
	IN HWND hList OPTIONAL
	);

//
// Decryption Result
//
typedef enum {
	DEC_SUCCEEDED,				// Success		: Decryption succeeded
	DEC_NOT_ENCRYPTED,			// Fail			: File is not encrypted
	DEC_NOT_EXECUTABLE,			// Fail			: File is not executable
	DEC_NOT_INITIALIZED,		// Error		: Data is not initialized
	DEC_MAPPING_FAILED,			// Error		: File mapping failed
	DEC_TRUNCATE_FAILED,		// Error		: File truncating failed
	DEC_NO_TXT_SECTION,			// Error		: File has no text section
}DECRYPT_RESULT, *PDECRYPT_RESULT;

//
// Decrypt exe file
//
DECRYPT_RESULT
PeShield_DecryptExe(
	IN LPCTSTR szExePath,
	IN DWORD cbFrag,
	IN HWND hProgress OPTIONAL,
	IN HWND hList OPTIONAL
	);


//----------------------------------------------------------------------
//						C  H  E  C  K  I  N  G
//----------------------------------------------------------------------

//
// Encryption Checking Result
//
typedef enum {
	CHK_ENCRYPTED,			// Information	: File is encrypted
	CHK_INVALID_METADATA,	// Information	: Invalid meta data
	CHK_INVALID_SIZE,		// Information	: Invalid encrypted file size
	CHK_NOT_EXECUTABLE,		// Fail			: File is not executable
	CHK_NOT_INITIALIZED,	// Error		: Data is not initialized
	CHK_MAPPING_FAILED		// Error		: File mapping failed
}CHECK_RESULT, *PCHECK_RESULT;

//
// Is exe encrypted ?
//
CHECK_RESULT
PeShield_IsExeEncrypted(
	IN LPCTSTR szExePath
	);

//
// Is file executable exe ?
//
BOOL 
PeShield_IsFileExecutable(
	IN LPCTSTR szFilePath
	);


#endif // _TOOL_H_