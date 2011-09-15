/*************** Header files *********************************************/
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include "des_in.h"
#include "des.h"

/*************** Definitions / Macros  ************************************/
#define KILO	1024
#define MEGA	(KILO*KILO)

/*************** Constant (Error Code) ************************************/
#define CTR_USAGE_ERROR		0x2001
#define CTR_KEYFILE_ERROR	0x2002

/*************** Global Variables *****************************************/
char	Help[] = "\
Usage1 : -[T/S]   (Test Value/Test Speed)\n\
Usage2 : -[E/D] -[ECB/CBC/OFB/CFB/ECBPAD/CBCPAD/OFBPAD/CFBPAD] infile outfile\n\
            (file 'key.dat' contains UserKey and IV)\n";

/*************************************************************************/
typedef struct{
	DWORD		ErrorCode;
	BYTE		Message[32];
} ERROR_MESSAGE;

ERROR_MESSAGE	ErrorMessage[] = {
	{CTR_FATAL_ERROR,		"CTR_FATAL_ERROR"},
	{CTR_INVALID_USERKEYLEN,"CTR_INVALID_USERKEYLEN"},
	{CTR_PAD_CHECK_ERROR,	"CTR_PAD_CHECK_ERROR"},
	{CTR_DATA_LEN_ERROR,	"CTR_DATA_LEN_ERROR"},
	{CTR_CIPHER_LEN_ERROR,	"CTR_CIPHER_LEN_ERROR"},
	{CTR_USAGE_ERROR,		"CTR_USAGE_ERROR"},
	{CTR_KEYFILE_ERROR,		"CTR_KEYFILE_ERROR"},
	{0, ""},
};

/**************************************************************************
*
*/
void	Error(
		DWORD	ErrorCode,
		char	*Message)
{
	DWORD	i;

	for( i=0; ErrorMessage[i].ErrorCode!=0; i++)
		if( ErrorMessage[i].ErrorCode==ErrorCode )	break;

	printf("ERROR(%s) :::: %s\n", ErrorMessage[i].Message, Message);
	if( ErrorCode==CTR_USAGE_ERROR )	printf("\n%s", Help);
	exit(1);
}

/**************************************************************************
*
*/

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
	)
{
	BYTE*	SrcData = new BYTE[Frag], * DstData = new BYTE[Frag];
	DWORD	PadType;
	DWORD	UKLen, IVLen, SrcLen, DstLen, Written, Read, Rest;
	RET_VAL	ret;
	DES_ALG_INFO	AlgInfo;
	
	UKLen	= DES_USER_KEY_LEN;
	IVLen	= DES_BLOCK_LEN;
	
	PadType	= AI_NO_PADDING;
	
	DES_SetAlgInfo(ModeType, PadType, IV, &AlgInfo);
	ret = DES_KeySchedule(UserKey, UKLen, &AlgInfo);
	if( ret!=CTR_SUCCESS )	
	{
		delete []SrcData;
		delete []DstData;
		Error(ret, "DES_KeySchedule() returns.");
	}

	if (hProgress)
	{
		SendMessage(hProgress, PBM_SETRANGE32, Size, 0);
		SendMessage(hProgress, PBM_SETPOS, 0, 0);
	}

	if( EncType==AI_ENC ) {		//	Encryption
		ret = DES_EncInit(&AlgInfo);
		if( ret!=CTR_SUCCESS )	
		{
			delete []SrcData;
			delete []DstData;
			Error(ret, "DES_EncInit() returns.");
		}

		Written	= 0;
		Read	= 0;
		
		for(  ;  ;  ) 
		{
			if (Read >= Size)			break;
			
			if (Size - Read < Frag)	SrcLen = Size - Read;
			else						SrcLen = Frag;

		
			memcpy(SrcData, Source + Read, SrcLen);
			Read += SrcLen;
		
			DstLen = Frag;
			ret = DES_EncUpdate(&AlgInfo, SrcData, SrcLen, DstData, &DstLen);
			if( ret!=CTR_SUCCESS )
			{
				delete []SrcData;
				delete []DstData;
				Error(ret, "DES_EncUpdate() returns.");
			}
		
			memcpy(Target + Written, DstData, DstLen);
			Written += DstLen;

			if (hProgress) 
				SendMessage(hProgress, PBM_SETPOS, Written, 0);
		}
		
		Rest = Size - Written;
		if (Rest > 0)
		{
			memcpy(Target + Written, Source + Read - Rest, Rest);
			Written += Rest;

			if (hProgress) 
				SendMessage(hProgress, PBM_SETPOS, Written, 0);
		}

		if (Written != Read)
		{
			delete []SrcData;
			delete []DstData;
			Error(ret, "Written and read size don't match.");
		}
	}
	else if( EncType==AI_DEC ) {					//	Decryption
		ret = DES_DecInit(&AlgInfo);
		if( ret!=CTR_SUCCESS )	
		{
			delete []SrcData;
			delete []DstData;
			Error(ret, "DES_DecInit() returns.");
		}

		Written	= 0;
		Read	= 0;
		for(  ;  ;  ) 
		{
			if (Read >= Size)			break;
			
			if (Size - Read < Frag)	SrcLen = Size - Read;
			else						SrcLen = Frag;
			
			memcpy(SrcData, Source + Read, SrcLen);
			Read += SrcLen;								
		
			DstLen = Frag;
			ret = DES_DecUpdate(&AlgInfo, SrcData, SrcLen, DstData, &DstLen);
			if( ret!=CTR_SUCCESS )	
			{
				delete []SrcData;
				delete []DstData;
				Error(ret, "DES_DecUpdate() returns.");
			}

			memcpy(Target + Written, DstData, DstLen);
			Written += DstLen;

			if (hProgress) 
				SendMessage(hProgress, PBM_SETPOS, Written, 0);
		}
		
		Rest = Size - Written;
		if (Rest > 0)
		{
			memcpy(Target + Written, Source + Read - Rest, Rest);
			Written += Rest;

			if (hProgress) 
				SendMessage(hProgress, PBM_SETPOS, Written, 0);
		}
		
		if (Written != Read)
		{
			delete []SrcData;
			delete []DstData;
			Error(ret, "Written and read size don't match.");
		}
	}
	else
	{
		delete []SrcData;
		delete []DstData;
		Error(ret, "Invalid EncType.");
	}

	delete []SrcData;
	delete []DstData;
}

/*************** END OF FILE **********************************************/
