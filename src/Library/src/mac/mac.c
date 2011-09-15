#define DBNTWIN32

#include <stdio.h>
#include <winsock2.h>
#include <sqlfront.h>
#include <sqldb.h>
#include <lm.h>

#include "../../h/mac.h"

//----------------------------------------------------------------------
//					 M  A  C     A  D  D  R  E  S  S
//---------------------------------------------------------------------- 

//
// Get mac address from system
//
BOOL
PeShield_GetMacAddressFromSystem(
	OUT LPSTR szMacAddress
	)
{
	PWKSTA_TRANSPORT_INFO_0	pwkti;
	DWORD					dwEntriesRead;
	DWORD					dwTotalEntries;
	PBYTE					pBuffer = NULL;
	BYTE					MACData[8];
	CHAR					MACAddr[LEN_MAC+1];

	// Enumerate all system mac address
	NET_API_STATUS dwStatus = NetWkstaTransportEnum(
		NULL,						// [in]  server name
		0,							// [in]  data structure to return
		&pBuffer,					// [out] pointer to buffer
		MAX_PREFERRED_LENGTH,		// [in]  maximum length
		&dwEntriesRead,				// [out] counter of elements actually enumerated
		&dwTotalEntries,			// [out] total number of elements that could be enumerated
		NULL);						// [in/out] resume handle
	if (dwStatus != NERR_Success) 
		return FALSE;

	if (dwEntriesRead <= 1)
	{
		NetApiBufferFree(pBuffer);
		return FALSE;
	}
	
	pwkti = (WKSTA_TRANSPORT_INFO_0 *)pBuffer;
	
	// Copy mac address got ( it's NULL in first index )
	swscanf(
		(wchar_t *)pwkti[1].wkti0_transport_address, 
		L"%2hx%2hx%2hx%2hx%2hx%2hx", 
		&MACData[0], &MACData[1], &MACData[2], &MACData[3], &MACData[4], &MACData[5]
		);
	
	// Change it to string and save
	sprintf(
		MACAddr, 
		"%02X%02X%02X%02X%02X%02X",
		MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]
		);
	CopyMemory(szMacAddress, MACAddr, sizeof(MACAddr));
	
	// Free system buffer
	dwStatus = NetApiBufferFree(pBuffer);
	if (dwStatus != NERR_Success)
		return FALSE;
	
	return TRUE;
}

#define ASCII_TO_HEX(c)	((c >= 'a') ? (c-'a'+10) : (c-'0'))

//
// Get mac address hash from database
//
BOOL
PeShield_GetMacHashFromDatabase(
	OUT PBYTE pBufMacHash,
	IN LPSTR szSerial
	)
{
	BOOL		bSucceeded		= FALSE;

	PLOGINREC	login;
	PDBPROCESS	dbproc;
	STATUS		status;		
	char		query[128]		= {0,};
	BYTE		pBufData[512]	= {0,};
	int			len, i;
	char		szHex[3];

	dbinit();
    dbsettime(10);

    if (dbsetmaxprocs(40) == SUCCEED)
	{
		login = dblogin();
		DBSETLUSER(login, PESHIELD_DATABASE_USER);
		DBSETLPWD (login, PESHIELD_DATABASE_PWD);
		DBSETLAPP (login, "pubs");
	}
	else
	{
		return FALSE;
	}

	// Write query sentence
	sprintf(query, "SELECT MAC1 FROM pubs..suc where serial = '%s' ", szSerial);
	
	// Connect to database 
	dbproc = dbopen(login, PESHIELD_DATABASE_NAME);
	
	// Send the query
	dbcmd(dbproc, query);
	
	// Execute the query
	dbsqlexec (dbproc);
	
	// Query saved mac address to db using serial
	if (dbresults(dbproc) == SUCCEED)
	{
		// Bind to database
		dbbind(dbproc, 1, NTBSTRINGBIND, 0, pBufData);
		
		// Get the answer
		status = dbnextrow(dbproc);

		// Did it get the answer ?
		if (status != NO_MORE_ROWS && status != FAIL)
		{
			// Is the answer not null ?
			len  = strlen((LPCSTR)pBufData) / 2;
			if (len > 0)
			{
				// Copy to the mac address hash to parameter
				for (i = 0; i < len; i++)
				{
					strncpy(szHex, pBufData + i*2, 2);
					szHex[2] = 0;
					
					pBufMacHash[i] = ASCII_TO_HEX(szHex[0]) * 16 + ASCII_TO_HEX(szHex[1]);		
				}
				
				// Set result as succeeded
				bSucceeded = TRUE;
			}
		}
	}
	
	// Disconnect from database
	dbfreebuf(dbproc);
	dbclose(dbproc);
	
	return bSucceeded;
}


