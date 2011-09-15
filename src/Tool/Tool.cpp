#include "stdafx.h"

#include <tchar.h>

#include "../Library/h/comm.h"
#include "../Library/h/io.h"
#include "../Library/h/pe.h"
#include "../Library/h/crypto.h"

#include "Tool.h"

//----------------------------------------------------------------------
//			 				D  E  B  U  G
//----------------------------------------------------------------------

//
// No Encryption/Decryption
//
#define PESHIELD_NO_CRYPTO 0


//
// Log short cut
//
#define PESHIELD_LOG(h, sz) if (h) SendMessage(h, LB_ADDSTRING, 0, (LPARAM)sz)

//----------------------------------------------------------------------
//			 G  L  O  B  A  L     V  A  R  I  A  B  L  E  S
//----------------------------------------------------------------------

//
// Pe Shield Tool Initialization Flag
//
static BOOL		g_bInitTool;

//
// Spy file information
//
static TCHAR	g_szSpyPath[MAX_PATH+1];
static HANDLE	g_hFileSpy = INVALID_HANDLE_VALUE;
static HANDLE	g_hMapSpy;
static PBYTE	g_pBufSpy;
static DWORD	g_cbSpy;

//
// Loader file information
//
static TCHAR	g_szLoaderPath[MAX_PATH+1];
static HANDLE	g_hFileLoader = INVALID_HANDLE_VALUE;
static HANDLE	g_hMapLoader;
static PBYTE	g_pBufLoader;
static DWORD	g_cbLoader;

//----------------------------------------------------------------------
//				I  N  I  T  I  A  L  I  Z  A  T  I  O  N
//---------------------------------------------------------------------- 

//
// Initialize Pe Shield Tool data
//
BOOL
PeShield_ToolStartup(
	IN HINSTANCE hInstance
	)
{
	BOOL	bInitialized = FALSE;

	TCHAR	szToolPath[MAX_PATH+1];
	TCHAR	szToolDirectory[MAX_PATH+1];
	
	// Is tool initialized?
	if (!g_bInitTool)
	{
		// Get Pe Shield Tool directory
		if (GetModuleFileName((HMODULE)hInstance, szToolPath, MAX_PATH))
		{
			PeShield_GetFileDirectory(szToolDirectory, szToolPath);
			
			// Make spy and module path
			wsprintf(g_szSpyPath, _T("%s\\%s"), szToolDirectory, PESHIELD_SPY_FILE);
			wsprintf(g_szLoaderPath, _T("%s\\%s"), szToolDirectory, PESHIELD_LOADER_FILE);
			
			// Create spy mapping
			if (PeShield_CreateFileMapping(
				&g_hFileSpy,
				&g_hMapSpy,
				&g_pBufSpy,
				&g_cbSpy,
				g_szSpyPath,
				0,
				GENERIC_READ,
				0,
				OPEN_EXISTING
				))
			{
				// Create module mapping
				if (PeShield_CreateFileMapping(
					&g_hFileLoader,
					&g_hMapLoader,
					&g_pBufLoader,
					&g_cbLoader,
					g_szLoaderPath,
					0,
					GENERIC_READ,
					0,
					OPEN_EXISTING
					))
				{
					// Set initialization flag to true
					g_bInitTool = bInitialized = TRUE;
				}
				else
				{
					// Close spy mapping just in case
					PeShield_CloseFileMapping(
						&g_hFileSpy,
						&g_hMapSpy,
						&g_pBufSpy,
						&g_cbSpy
						);
				}
			}
		}
	}
	
	return bInitialized;
}

//
// Uninitialize Pe Shield Tool data
//
VOID
PeShield_ToolCleanup()
{
	// Is tool initialized?
	if (g_bInitTool)
	{
		// Set module size to zero
		g_cbLoader = 0;
		
		// Close module mapping
		PeShield_CloseFileMapping(
			&g_hFileLoader,
			&g_hMapLoader,
			&g_pBufLoader,
			&g_cbLoader
			);
		
		// Close spy mapping
		PeShield_CloseFileMapping(
			&g_hFileSpy,
			&g_hMapSpy,
			&g_pBufSpy,
			&g_cbSpy
			);

		// Set module path to zero
		ZeroMemory(g_szLoaderPath, sizeof(g_szLoaderPath));

		// Set spy path to zero
		ZeroMemory(g_szSpyPath, sizeof(g_szSpyPath));
		
		// Set initialization flag to false
		g_bInitTool = FALSE;
	}
}


//----------------------------------------------------------------------
//					  E  N  C  R  Y  P  T  I  O  N
//----------------------------------------------------------------------

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
	)
{
	ENCRYPT_RESULT		EncryptResult;
	CHECK_RESULT		CheckResult;
	
	HANDLE				hFileExe	= INVALID_HANDLE_VALUE;
	HANDLE				hMapExe		= NULL;
	PBYTE				pBufExe		= NULL;
	DWORD				cbExe		= 0;
	
	PESHIELD_METADATA	MetaData;
	
	DWORD				cbEncrypted;
	DWORD				cbResLength;
	DWORD				dwTxtOffset;
	DWORD				cbTxtLength;

	TCHAR				szExeName[_MAX_FNAME+1];
	TCHAR				szLog[256];

	if (hList)
	{
		PeShield_GetFileName(szExeName, szExePath);
		wsprintf(szLog, _T("Information : Encryption starts (%s)."), szExeName); 
		PESHIELD_LOG(hList, szLog);
	}

	// Is Pe Shield Tool data initialized ?
	if (g_bInitTool)
	{
		PESHIELD_LOG(hList, _T("Success : Pe Shield Tool data is initialized."));
	}
	else
	{
		PESHIELD_LOG(hList, _T("Error : Pe Shield Tool data is not initialized."));

		return ENC_NOT_INITIALIZED;
	}
	
	// Is web address alright ?
	if (strncmp(szWebAddress, "http://", 7) == 0)
	{
		PESHIELD_LOG(hList, _T("Success : Web address is valid."));

		// Is file executable exe ?
		if (PeShield_IsFileExecutable(szExePath))
		{
			PESHIELD_LOG(hList, _T("Success : File is executable."));

			// Check exe
			CheckResult = PeShield_IsExeEncrypted(szExePath);

			// Is exe encrypted ?
			if (CheckResult != CHK_ENCRYPTED)
			{
				PESHIELD_LOG(hList, _T("Success : File is not encrypted yet."));

				// Create exe mapping ( first )
				if (PeShield_CreateFileMapping(
					&hFileExe,
					&hMapExe,
					&pBufExe,
					&cbExe,
					szExePath,
					0,
					GENERIC_READ,
					FILE_SHARE_READ|FILE_SHARE_WRITE,
					OPEN_EXISTING
					))
				{
					PESHIELD_LOG(hList, _T("Success : File mapping is created. (first)"));

					// Set meta data ( doesn't care resource hooking )
					ZeroMemory(&MetaData, SIZEOF_PESHIELD_METADATA);
					MetaData.Signature		= SENOL_SIGNATURE;
					MetaData.SpyOffset	= 0;
					MetaData.SpyLength	= g_cbSpy;
					MetaData.LoaderOffset	= MetaData.SpyOffset + MetaData.SpyLength;
					MetaData.LoaderLength	= g_cbLoader;
					MetaData.ExeOffset		= MetaData.LoaderOffset + MetaData.LoaderLength;
					MetaData.ExeLength		= cbExe;
					
					PESHIELD_LOG(hList, _T("Success : Meta data is set."));

					// Is this build as unicode version ?
#ifdef UNICODE
					// Change unicode web address to ansi one 
					WideCharToMultiByte(
						CP_ACP, 
						0, 
						MetaData.WebAddress, 
						-1, 
						szWebAddress, 
						MAX_URL, 
						0,
						0);
#else
					// Just copy it
					strcpy(MetaData.WebAddress, szWebAddress);
#endif // UNICODE

					// Does exe have .rsrc section ?
					if (PeShield_GetSectionInformation(
						pBufExe,
						_T(".rsrc"),
						NULL,
						&cbResLength,
						NULL,
						NULL
						))
					{
						PESHIELD_LOG(hList, _T("Information : File has .rsrc section."));

						// Change meta data for resource hooking
						MetaData.SpyLength	+= cbResLength;
						MetaData.LoaderOffset	= MetaData.SpyOffset + MetaData.SpyLength;
						MetaData.ExeOffset		= MetaData.LoaderOffset + MetaData.LoaderLength;
					}
					
					// Close exe mapping to re-map it with encrypted size ( first )
					PeShield_CloseFileMapping(
						&hFileExe,
						&hMapExe,
						&pBufExe,
						NULL	// except exe size
						);

					PESHIELD_LOG(hList, _T("Success : File mapping is closed to re-map. (first)"));
					
					// Set encrypted exe size
					cbEncrypted = 
						MetaData.SpyLength + 
						MetaData.LoaderLength + 
						MetaData.ExeLength + 
						SIZEOF_PESHIELD_METADATA;
					
					// Create exe mapping again with encrypted size ( second )
					if (PeShield_CreateFileMapping(
						&hFileExe,
						&hMapExe,
						&pBufExe,
						NULL,
						szExePath,
						cbEncrypted,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ,
						OPEN_EXISTING
						))
					{
						PESHIELD_LOG(hList, _T("Success : File mapping is created. (second)"));
						
						// Move exe image to modified offset		
						MoveMemory(
							pBufExe + MetaData.ExeOffset,		// |       |       |  exe  |       |
							pBufExe,							// |  exe  |       |       |       |
							MetaData.ExeLength
							);
						
						PESHIELD_LOG(hList, _T("Success : Exe image is moved to modified offset."));
						
						// Copy spy image to modified offset
						CopyMemory(
							pBufExe + MetaData.SpyOffset,		// |  d  | |       |  exe  |       |
							g_pBufSpy,						// |  d  |
							g_cbSpy	// non hooked size
							);
						
						PESHIELD_LOG(hList, _T("Success : Spy image is copied to modified offset."));
						
						// Copy module image to modified offset
						CopyMemory(
							pBufExe + MetaData.LoaderOffset,	// |  d  | |   m   |  exe  |       |
							g_pBufLoader,						//         |   m   |
							MetaData.LoaderLength
							);
						
						PESHIELD_LOG(hList, _T("Success : Loader image is copied to modified offset."));
						
						// Copy meta data to the end offset
						CopyMemory(
							pBufExe + cbEncrypted - SIZEOF_PESHIELD_METADATA,	// |  d  | |   m   |  exe  | meta  |
							&MetaData,											//                         | meta  |
							SIZEOF_PESHIELD_METADATA
							);
						
						PESHIELD_LOG(hList, _T("Success : Meta data is copied to the end offset."));
						
						// Is resource hooking necessary ?
						if (MetaData.SpyLength > g_cbSpy)
						{
							// Hook exe resource to spy
							if (PeShield_HookResource(
								pBufExe + MetaData.SpyOffset,
								pBufExe + MetaData.ExeOffset,
								g_cbSpy	// non hooked size
								))
							{
								PESHIELD_LOG(hList, _T("Success : File resource is hooked to spy image."));
							}
							else
							{
								PESHIELD_LOG(hList, _T("Error : File resource is not hooked."));
								
								EncryptResult = ENC_RES_HOOKING_FAILED;
							}
						}
						
						if (EncryptResult != ENC_RES_HOOKING_FAILED)
						{
							// Get .text section information
							if (PeShield_GetSectionInformation(
								pBufExe + MetaData.ExeOffset,
								_T(".text"), &dwTxtOffset, &cbTxtLength,
								NULL, NULL) || 
								PeShield_GetSectionInformation(
								pBufExe + MetaData.ExeOffset,
								_T("CODE"), &dwTxtOffset, &cbTxtLength,
								NULL, NULL) ||
								PeShield_GetSectionInformation(
								pBufExe + MetaData.ExeOffset,
								_T("UPX1"), &dwTxtOffset, &cbTxtLength,
								NULL, NULL)
								)
							{
								PESHIELD_LOG(hList, _T("Success : .text section offset and length are got."));
								
								PESHIELD_LOG(hList, _T("Information : Des encrytion starts."));
#if !PESHIELD_NO_CRYPTO
								// Encrypt .text section
								PeShield_EncryptDes(
									pBufExe + MetaData.ExeOffset + dwTxtOffset,
									cbTxtLength,
									cbFrag,
									hProgress
									);
#endif // !PESHIELD_NO_CRYPTO
								PESHIELD_LOG(hList, _T("Success : Des encrytion succeeded."));
								
								// Success
								EncryptResult = ENC_SUCCEEDED;
								
								PESHIELD_LOG(hList, _T("Success : Encryption succeeded."));
							}
							else
							{
								PESHIELD_LOG(hList, _T("Fail : File has no .text section."));
								
								EncryptResult = ENC_NO_TXT_SECTION;
							}
						}

						// Close exe mapping for re-map it with encrypted size ( second )
						PeShield_CloseFileMapping(
							&hFileExe,
							&hMapExe,
							&pBufExe,
							&cbExe
							);

						PESHIELD_LOG(hList, _T("Success : File mapping is closed. (second)"));
					}
					else
					{
						EncryptResult = ENC_MAPPING_FAILED;

						PESHIELD_LOG(hList, _T("Error : File mapping is failed. (second)"));
					}
				}
				else
				{
					EncryptResult = ENC_MAPPING_FAILED;

					PESHIELD_LOG(hList, _T("Error : File mapping is failed. (first)"));
				}
			}
			else if (CheckResult == CHK_MAPPING_FAILED)
			{
				EncryptResult = ENC_MAPPING_FAILED;

				PESHIELD_LOG(hList, _T("Error : File mapping is failed."));
			}
			else
			{
				EncryptResult = ENC_ALREADY_ENCRYPTED;

				PESHIELD_LOG(hList, _T("Fail : File is already encrypted."));
			}
		}
		else
		{
			EncryptResult = ENC_NOT_EXECUTABLE;

			PESHIELD_LOG(hList, _T("Fail : File is not executable."));
		}
	}
	else
	{
		EncryptResult = ENC_INVALID_WEB;

		PESHIELD_LOG(hList, _T("Fail : Web address is invalid."));
	}

	if (hList)
	{
		wsprintf(szLog, _T("Information : Encryption ends (%s)"), szExeName); 
		PESHIELD_LOG(hList, szLog);
		PESHIELD_LOG(hList, _T("-----------------------------------------------------------"));
	}

	return EncryptResult;
}

//
// Decrypt exe file
//
DECRYPT_RESULT
PeShield_DecryptExe(
	IN LPCTSTR szExePath,
	IN DWORD cbFrag,
	IN HWND hProgress OPTIONAL,
	IN HWND hList OPTIONAL
	)
{
	DECRYPT_RESULT		DecryptResult;
	CHECK_RESULT		CheckResult;

	HANDLE				hFileExe	= INVALID_HANDLE_VALUE;
	HANDLE				hMapExe		= NULL;
	PBYTE				pBufExe		= NULL;
	DWORD				cbEncrypted	= 0;

	PPESHIELD_METADATA	pMetaData;
	DWORD				cbExe		= 0;

	DWORD				dwTxtOffset;
	DWORD				cbTxtLength;

	TCHAR				szExeName[_MAX_FNAME+1];
	TCHAR				szLog[256];

	if (hList)
	{
		PeShield_GetFileName(szExeName, szExePath);
		wsprintf(szLog, _T("Information : Decryption starts (%s)."), szExeName); 
		PESHIELD_LOG(hList, szLog);
	}

	// Is Pe Shield Tool data initialized ?
	if (g_bInitTool)
	{
		PESHIELD_LOG(hList, _T("Success : Pe Shield Tool data is initialized."));
	}
	else
	{
		PESHIELD_LOG(hList, _T("Error : Pe Shield Tool data is not initialized."));

		return DEC_NOT_INITIALIZED;
	}

	// Is file executable exe ?
	if (PeShield_IsFileExecutable(szExePath))
	{
		PESHIELD_LOG(hList, _T("Success : File is executable."));

		// Check exe
		CheckResult = PeShield_IsExeEncrypted(szExePath);

		// Is exe encrypted ?
		if (CheckResult == CHK_ENCRYPTED)
		{
			PESHIELD_LOG(hList, _T("Success : File is encrypted."));

			// Create exe mapping ( first )
			if (PeShield_CreateFileMapping(
				&hFileExe,
				&hMapExe,
				&pBufExe,
				&cbEncrypted,	// Encrypted size
				szExePath,
				0,
				GENERIC_READ|GENERIC_WRITE,
				FILE_SHARE_READ,
				OPEN_EXISTING
				))
			{
				PESHIELD_LOG(hList, _T("Success : File mapping is created. (first)"));

				// Get meta data
				pMetaData = PeShield_GetMetaData(pBufExe, cbEncrypted);
				
				PESHIELD_LOG(hList, _T("Success : Meta data is got."));

				// Move exe back from modified offset	
				MoveMemory(
					pBufExe,							// |  exe  |       |       |       |
					pBufExe + pMetaData->ExeOffset,		// |       |       |  exe  |       |
					pMetaData->ExeLength
					);

				PESHIELD_LOG(hList, _T("Success : Exe image is moved back from modified offset."));
				
				// Save exe size to truncate file
				cbExe = pMetaData->ExeLength;
				
				// Close exe mapping to truncate file ( first )
				PeShield_CloseFileMapping(
					&hFileExe,
					&hMapExe,
					&pBufExe,
					NULL
					);

				PESHIELD_LOG(hList, _T("Success : File mapping is closed to truncate. (first)"));

				// Truncate file except exe
				if (PeShield_TruncateFile(
					szExePath,
					cbExe,
					FILE_BEGIN
					))
				{
					PESHIELD_LOG(hList, _T("Success : File is truncated."));

					// Create exe mapping again for des decryption ( second )
					if (PeShield_CreateFileMapping(
						&hFileExe,
						&hMapExe,
						&pBufExe,
						NULL,
						szExePath,
						0,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ,
						OPEN_EXISTING
						))
					{
						PESHIELD_LOG(hList, _T("Success : File mapping is created. (second)"));

						// Get .text section information
						if (PeShield_GetSectionInformation(
							pBufExe, _T(".text"), 
							&dwTxtOffset, &cbTxtLength,
							NULL, NULL) || 
							PeShield_GetSectionInformation(
							pBufExe, _T("CODE"),
							&dwTxtOffset, &cbTxtLength,
							NULL, NULL) || 
							PeShield_GetSectionInformation(
							pBufExe, _T("UPX1"),
							&dwTxtOffset, &cbTxtLength,
							NULL, NULL))
						{
							PESHIELD_LOG(hList, _T("Success : .text section offset and length are got."));

							PESHIELD_LOG(hList, _T("Information : Des decrytion starts."));
#if !PESHIELD_NO_CRYPTO
							// Decrypt .text section
							PeShield_DecryptDes(
								pBufExe + dwTxtOffset,
								cbTxtLength,
								cbFrag,
								hProgress
								);
#endif // !PESHIELD_NO_CRYPTO
							PESHIELD_LOG(hList, _T("Success : Des decrytion succeeded."));

							// Success
							DecryptResult = DEC_SUCCEEDED;
						
							PESHIELD_LOG(hList, _T("Success : Decryption succeeded."));
						}
						else
						{
							DecryptResult = DEC_NO_TXT_SECTION;

							PESHIELD_LOG(hList, _T("Error : File has no .text section."));
						}
					}
					else
					{
						DecryptResult = DEC_MAPPING_FAILED;

						PESHIELD_LOG(hList, _T("Error : File mapping is failed. (second)"));
					}
				}
				else
				{
					DecryptResult = DEC_TRUNCATE_FAILED;

					PESHIELD_LOG(hList, _T("Error : File is not truncated."));
				}
				
				// Close exe mapping
				PeShield_CloseFileMapping(
					&hFileExe,
					&hMapExe,
					&pBufExe,
					&cbEncrypted
					);

				PESHIELD_LOG(hList, _T("Success : File mapping is closed. (first|second)"));
			}
			else
			{
				DecryptResult = DEC_MAPPING_FAILED;

				PESHIELD_LOG(hList, _T("Error : File mapping is failed. (first)"));
			}
		}
		else if (CheckResult == CHK_MAPPING_FAILED)
		{
			DecryptResult = DEC_MAPPING_FAILED;

			PESHIELD_LOG(hList, _T("Error : File mapping is failed."));
		}
		else
		{
			DecryptResult = DEC_NOT_ENCRYPTED;

			PESHIELD_LOG(hList, _T("Fail : File is not encrypted yet."));
		}
	}
	else
	{
		DecryptResult = DEC_NOT_EXECUTABLE;

		PESHIELD_LOG(hList, _T("Fail : File is not executable."));
	}

	if (hList)
	{
		wsprintf(szLog, _T("Information : Decryption ends (%s)"), szExeName); 
		PESHIELD_LOG(hList, szLog);
		PESHIELD_LOG(hList, _T("-----------------------------------------------------------"));
	}

	return DecryptResult;
}


//----------------------------------------------------------------------
//						C  H  E  C  K  I  N  G
//----------------------------------------------------------------------

//
// Is exe encrypted ?
//
CHECK_RESULT
PeShield_IsExeEncrypted(
	IN LPCTSTR szExePath
	)
{
	CHECK_RESULT	CheckResult;

	HANDLE			hFileExe	= INVALID_HANDLE_VALUE;
	HANDLE			hMapExe		= NULL;
	PBYTE			pBufExe		= NULL;
	DWORD			cbExe		= 0;

	PPESHIELD_METADATA	pMetaData;

	// Is Pe Shield Tool data initialized ?
	if (!g_bInitTool)
	{
		return CHK_NOT_INITIALIZED;
	}

	// Is file executable
	if (PeShield_IsFileExecutable(szExePath))
	{
		// Create file mapping
		if (PeShield_CreateFileMapping(
			&hFileExe,
			&hMapExe,
			&pBufExe,
			&cbExe,
			szExePath,
			0,
			GENERIC_READ,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			OPEN_EXISTING
			))
		{
			// Is exe size larger than pe header + module + meta data size ?
			if (cbExe > IMAGE_SIZEOF_HEADERS + g_cbLoader + SIZEOF_PESHIELD_METADATA)
			{
				// Get meta data
				pMetaData = PeShield_GetMetaData(pBufExe, cbExe);

				// Is meta data valid ? 
				if (PeShield_IsMetaDataValid(pMetaData, cbExe))
				{
					CheckResult = CHK_ENCRYPTED;
				}
				else
				{
					CheckResult = CHK_INVALID_METADATA;
				}
			}
			else
			{
				CheckResult = CHK_INVALID_SIZE;
			}

			// Close exe mapping
			PeShield_CloseFileMapping(
				&hFileExe,
				&hMapExe,
				&pBufExe,
				&cbExe
				);
		}
		else
		{
			CheckResult = CHK_MAPPING_FAILED;
		}
	}
	else
	{
		CheckResult = CHK_NOT_EXECUTABLE;
	}

	return CheckResult;
}

//
// Is file executable exe ?
//
BOOL 
PeShield_IsFileExecutable(
	IN LPCTSTR szFilePath
	)
{
	BOOL	bExecutable	= FALSE;

	TCHAR	szExt[_MAX_EXT];

	HANDLE	hFile	= INVALID_HANDLE_VALUE;
	HANDLE	hMap	= NULL;
	PBYTE	pBuf	= NULL;

	// Is Pe Shield Tool data initialized ?
	if (!g_bInitTool)
	{
		return FALSE;
	}

	// Is extension exe ?
	PeShield_GetFileExt(szExt, szFilePath);
	if (_tcsicmp(szExt, _T("EXE")) == 0)
	{
		// Create module mapping
		if (PeShield_CreateFileMapping(
			&hFile,
			&hMap,
			&pBuf,
			NULL,
			szFilePath,
			0,
			GENERIC_READ,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			OPEN_EXISTING
			))
		{
			// Is image executable ?
			if (PeShield_IsImageExecutable(pBuf))
			{
				bExecutable = TRUE;
			}
			
			// Close file mapping
			PeShield_CloseFileMapping(
				&hFile,
				&hMap,
				&pBuf,
				NULL
				);
		}
	}

	return bExecutable;
}