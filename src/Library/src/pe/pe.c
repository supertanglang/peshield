#include "list\list.h"

#include "..\..\h\pe.h"

//----------------------------------------------------------------------
//			P  E     H  E  A  D  E  R     S  U  P  P  O  R  T
//----------------------------------------------------------------------

//
// Get pe headers from pe image
//
BOOL 
PeShield_GetPeHdrs(
	IN CONST PBYTE pBufPe, 
	OUT PIMAGE_DOS_HEADER* ppDosHdr OPTIONAL,
	OUT PIMAGE_NT_HEADERS* ppNtHdrs OPTIONAL,
	OUT PIMAGE_SECTION_HEADER* ppSecHdrs OPTIONAL,
	OUT PBYTE* ppSections OPTIONAL
	)
{
	PIMAGE_DOS_HEADER pDosHdr_t;
	PIMAGE_NT_HEADERS pNtHdrs_t;
	
	// Dos header
	pDosHdr_t		= (PIMAGE_DOS_HEADER)pBufPe;
	
	// NTt Header
	pNtHdrs_t		= (PIMAGE_NT_HEADERS)(pBufPe + pDosHdr_t->e_lfanew);
	
	// Is pe image ?
	if (pDosHdr_t->e_magic == IMAGE_DOS_SIGNATURE && pNtHdrs_t->Signature == IMAGE_NT_SIGNATURE)
	{
		// Set dos header
		if (ppDosHdr)
		{
			*ppDosHdr	= pDosHdr_t;
		}

		// Set nt headers 
		if (ppNtHdrs)
		{
			*ppNtHdrs	= pNtHdrs_t;
		}	
	
		// Set section headers
		if (ppSecHdrs)
		{
			*ppSecHdrs	= (PIMAGE_SECTION_HEADER)(pNtHdrs_t + 1);
		}

		// Set sections
		if (ppSections)
		{
			*ppSections	= ((PBYTE)pBufPe) + IMAGE_SIZEOF_HEADERS;
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//
// Get section information
//
BOOL
PeShield_GetSectionInformation(
	IN CONST PBYTE pBufPe,
	IN LPCTSTR szSectionName,
	OUT DWORD* pOffset OPTIONAL,
	OUT DWORD* pLength OPTIONAL,
	OUT DWORD* pRvAddr OPTIONAL,
	OUT DWORD* pRvSize OPTIONAL
	)
{
	BOOL					bExist		= FALSE;
	PIMAGE_SECTION_HEADER	pSecHdrs	= NULL;
	PIMAGE_SECTION_HEADER	pSecTgt		= NULL;
	CHAR					szName[8];

	// Get pe header
	if (PeShield_GetPeHdrs(
		pBufPe, 
		NULL, 
		NULL, 
		&pSecHdrs, 
		NULL
		))
	{
		// Is this build as unicode version ?
#ifdef UNICODE
		// Change unicode section name to ansi one 
		WideCharToMultiByte(
			CP_ACP, 
			0, 
			szName, 
			-1, 
			szSectionName, 
			MAX_URL, 
			0,
			0);
#else
		// Just copy it
		strcpy(szName, szSectionName);
#endif // UNICODE

		// Find the section by name
		pSecTgt = pSecHdrs;
		while (strncmp((LPCSTR)pSecTgt->Name, szName, 8) != 0 && pSecTgt->Name[0])
		{
			pSecTgt++;
		}

		// Is it found ?
		if (pSecTgt->Name[0])
		{
			// Set offset
			if (pOffset)
			{
				*pOffset = pSecTgt->PointerToRawData;
			}

			// Set length
			if (pLength)
			{
				*pLength = pSecTgt->SizeOfRawData;
			}

			// Set relative virtual address
			if (pRvAddr)
			{
				*pRvAddr = pSecTgt->VirtualAddress;
			}

			// Set relarive virtual size
			if (pRvSize)
			{
				*pRvSize = pSecTgt->Misc.VirtualSize;
			}

			bExist = TRUE;	
		}
	}

	return bExist;
}


//----------------------------------------------------------------------
//				I  M  A  G  E     C  H  E  C  K  I  N  G   
//----------------------------------------------------------------------

//
// Is image executable ?
//
BOOL
PeShield_IsImageExecutable(
	IN CONST PBYTE pBufImage
	)
{
	PIMAGE_DOS_HEADER		pDosHdr		= NULL;
	PIMAGE_NT_HEADERS		pNtHdrs		= NULL;
	
	// Get pe headers
	if (PeShield_GetPeHdrs(
		pBufImage, 
		&pDosHdr, 
		&pNtHdrs, 
		NULL, 
		NULL
		))
	{
		// Is image flags set as executable ?
		if ((pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) && 
			!(pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_DLL))
		{
			return TRUE;
		}
	}

	return FALSE;
}


//----------------------------------------------------------------------
//						  H  O  O  K  I  N  G
//----------------------------------------------------------------------

//
// Hook source resource to target ( target must not have resource ) 
//
BOOL
PeShield_HookResource(
	OUT PBYTE pBufTgt,
	IN CONST PBYTE pBufSrc,
	IN DWORD cbTgt
	)
{
	PIMAGE_NT_HEADERS				pNtHdrsTgt			= NULL;
	PIMAGE_SECTION_HEADER			pSecHdrsTgt			= NULL;
	PBYTE							pSectionsTgt		= NULL;
	PIMAGE_DATA_DIRECTORY			pResDatDirTgt		= NULL;
	PIMAGE_SECTION_HEADER			pLastSecHdrTgt		= NULL;
	PIMAGE_SECTION_HEADER			pResSecHdrTgt		= NULL;
	PBYTE							pResSectionTgt		= NULL;
	
	PIMAGE_NT_HEADERS				pNtHdrsSrc			= NULL;
	PIMAGE_SECTION_HEADER			pSecHdrsSrc			= NULL;
	PBYTE							pSectionsSrc		= NULL;
	PIMAGE_DATA_DIRECTORY			pResDatDirSrc		= NULL;
	PIMAGE_SECTION_HEADER			pResSecHdrSrc		= NULL;
	PBYTE							pResSectionSrc		= NULL;
	
	PIMAGE_RESOURCE_DIRECTORY		pResDir				= NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY	pResDirEntry		= NULL;
	PIMAGE_RESOURCE_DATA_ENTRY		pResDatEntry		= NULL;
	DWORD							cbResSection		= 0;
	DWORD							PointerToResData	= 0;
	DWORD							nResDataDelta		= 0;
	DWORD							cbResData			= 0;
	WORD							NumberOfEntries		= 0;
	int								i					= 0;

	DWORD							SectionAlignment	= 0;
	DWORD							cbResAligned		= 0;
	DWORD							cbLastAligned		= 0;

	BOOL							bDataEntry			= FALSE;
	PIMAGE_RESOURCE_DATA_ENTRY		pResDatEntryTemp	= NULL;

	// Get source pe headers
	if (!PeShield_GetPeHdrs(
		pBufSrc,
		NULL,
		&pNtHdrsSrc,
		&pSecHdrsSrc,
		&pSectionsSrc
		))
	{
		return FALSE;
	}
	
	// Get source resouce data directory entry
	pResDatDirSrc = &pNtHdrsSrc->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];

	// Does source have resource section ?
	if (!pResDatDirSrc->Size && !pResDatDirSrc->VirtualAddress)
	{
		return FALSE;
	}

	// Find source resource section header
	pResSecHdrSrc = pSecHdrsSrc;
	while (strcmp((const char*)pResSecHdrSrc->Name, ".rsrc") != 0)
	{
		pResSecHdrSrc++;
	}

	// Get source resource section
	pResSectionSrc = pBufSrc + pResSecHdrSrc->PointerToRawData;
	
	// Get source resource section size
	cbResSection = pResSecHdrSrc->SizeOfRawData;

	// ========================================================================================
		
	// Get target pe headers
	if (!PeShield_GetPeHdrs(
		pBufTgt,
		NULL,
		&pNtHdrsTgt,
		&pSecHdrsTgt,
		&pSectionsTgt
		))
	{
		return FALSE;
	}

	// Get target resource data directory entry
	pResDatDirTgt = &pNtHdrsTgt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];

	// Does target resource have resource section ?
	if (pResDatDirTgt->Size || pResDatDirTgt->VirtualAddress)
	{
		return FALSE;
	}

	// Find the place to copy new resource section header 
	pResSecHdrTgt = pSecHdrsTgt;
	while (pResSecHdrTgt->Name[0] != 0)
	{
		pResSecHdrTgt++;
	}

	// Get last section header
	pLastSecHdrTgt = pResSecHdrTgt - 1;

	// Get the place to copy new resource section ( at the end of sections )
	pResSectionTgt = pBufTgt + cbTgt;
	
	// ========================================================================================
	
	// Copy source resource section to target
	CopyMemory(
		pResSectionTgt,
		pResSectionSrc,
		cbResSection);
	
	// Copy source resource data directory entry to target
	CopyMemory(
		pResDatDirTgt,
		pResDatDirSrc,
		sizeof(IMAGE_DATA_DIRECTORY));
	
	// Copy source resource section header to target
	CopyMemory(
		pResSecHdrTgt,
		pResSecHdrSrc,
		IMAGE_SIZEOF_SECTION_HEADER);
	
	// ========================================================================================
	
	// Add target section number
	pNtHdrsTgt->FileHeader.NumberOfSections ++;
	
	// Change target image size information
	SectionAlignment = pNtHdrsTgt->OptionalHeader.SectionAlignment;
	cbResAligned = pResSecHdrTgt->Misc.VirtualSize + 
		((SectionAlignment - pResSecHdrTgt->Misc.VirtualSize % SectionAlignment) % SectionAlignment);
	if (cbResAligned % SectionAlignment)
	{
		return FALSE;
	}
	pNtHdrsTgt->OptionalHeader.SizeOfInitializedData	+= cbResAligned;
	pNtHdrsTgt->OptionalHeader.SizeOfImage				+= cbResAligned;
	
	// Change target resource data directory entry
	cbLastAligned = pLastSecHdrTgt->Misc.VirtualSize + 
		((SectionAlignment - pLastSecHdrTgt->Misc.VirtualSize % SectionAlignment) % SectionAlignment);
	if (cbLastAligned % SectionAlignment)
	{
		return FALSE;
	}
	pResDatDirTgt->VirtualAddress = pLastSecHdrTgt->VirtualAddress + cbLastAligned;
	
	// Change target resource section header
	pResSecHdrTgt->PointerToRawData	= cbTgt;
	pResSecHdrTgt->VirtualAddress	= pResDatDirTgt->VirtualAddress;
	
	// ========================================================================================
	
	// Initialize Pe Shield list
	if (!PeShield_ListStartup())
	{
		return FALSE;
	}
	
	// Get root resource directory
	pResDir	= (PIMAGE_RESOURCE_DIRECTORY)pResSectionTgt;
	
	while (TRUE)
	{
		// Get directory entry number
		NumberOfEntries = pResDir->NumberOfNamedEntries + pResDir->NumberOfIdEntries;
		
		// Get directory entry
		pResDirEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDir + 1);
		for (i = 0; i < NumberOfEntries; i++)
		{	
			bDataEntry = FALSE;
			
			// Is next data directory ?
			if (!pResDirEntry->DataIsDirectory)
			{
				bDataEntry = TRUE;
				
				// Get data directory
				pResDatEntry = (PIMAGE_RESOURCE_DATA_ENTRY)(pResSectionTgt + pResDirEntry->OffsetToData);
				
				// Put data entry into list
				if (!PeShield_ListPushBack(pResDatEntry))
				{
					// Uninitialize Pe Shield list data
					PeShield_ListCleanup();

					return FALSE;
				}
			}
			
			pResDirEntry ++;
		}
		
		// Is this just before data entry ?
		if (bDataEntry)
		{
			// Is data entry ?
			if (((PIMAGE_RESOURCE_DATA_ENTRY)pResDirEntry)->Reserved == 0)
			{
				break;
			}
		}
		
		// Get next directory entry
		pResDir = (PIMAGE_RESOURCE_DIRECTORY)pResDirEntry;
	}
		
	// Loop by when the list is empty
	while (!PeShield_ListIsEmpty())
	{
		// Get data entry from list
		pResDatEntry = (PIMAGE_RESOURCE_DATA_ENTRY)PeShield_ListPopFront();
		
		// Get resource data offset from data entry
		PointerToResData =	pResSecHdrTgt->PointerToRawData +
			pResDatEntry->OffsetToData - 
			pResSecHdrSrc->VirtualAddress;
		
		// Is list empty ?
		if (!PeShield_ListIsEmpty())
		{
			pResDatEntryTemp = (PIMAGE_RESOURCE_DATA_ENTRY)PeShield_ListGetFront();
		}

		// Change data entry rva
		pResDatEntry->OffsetToData = 
			pResSecHdrTgt->VirtualAddress - 
			pResSecHdrTgt->PointerToRawData +
			PointerToResData ;
	}

	// Uninitialize Pe Shield list data
	PeShield_ListCleanup();
		
	return TRUE;
}