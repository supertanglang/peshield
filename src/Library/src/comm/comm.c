#include "..\..\h\comm.h"

//----------------------------------------------------------------------
//                   M  E  T  A      D  A  T  A
//---------------------------------------------------------------------- 

//
// Get meta data from exe file
//
PPESHIELD_METADATA
PeShield_GetMetaData(
	IN CONST PBYTE pBufExe,
	IN DWORD cbExe
	)
{
	PPESHIELD_METADATA pMetaData;

	if (cbExe < SIZEOF_PESHIELD_METADATA)
		return NULL;
	
	pMetaData = (PPESHIELD_METADATA)(pBufExe + cbExe - SIZEOF_PESHIELD_METADATA);
	
	return (pMetaData->Signature == SENOL_SIGNATURE) ? pMetaData : NULL;
}

//
// Is meta data valid ?
//
BOOL
PeShield_IsMetaDataValid(
	IN PPESHIELD_METADATA pMetaData,
	IN DWORD cbExe
	)
{
	// Is meta data set ?
	if (pMetaData)
	{
		// Is loader offset zero ?
		if (pMetaData->SpyOffset == 0)
		{
			// Is exe size right ?
			if (cbExe == 
				pMetaData->SpyLength + 
				pMetaData->LoaderLength + 
				pMetaData->ExeLength + 
				SIZEOF_PESHIELD_METADATA)
			{
				// Is web address right ?
				if (strncmp(pMetaData->WebAddress, "http://", 7) == 0)
				{
					return TRUE;
				}
			}
		}
	}
	
	return FALSE;
}