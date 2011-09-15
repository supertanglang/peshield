//----------------------------------------------------------------------
//		P  E  S  H  I  E  L  D      I  O      C  O  N  T  R  O  L
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _IOCTLCODE_H_
#define _IOCTLCODE_H_

//----------------------------------------------------------------------
//		  D  R  I  V  E  R      I  N  F  O  R  M  A  T  I  O  N
//----------------------------------------------------------------------

//
// Pe Shield Driver device type ( 0x8000 - 0xFFFF )
//
#define FILE_DEVICE_PESHIELD	0x9000

//
// Pe Shield Driver name
//
#define DRIVERNAME	"PESHIELD"

//
// Pe Shield Driver NT device name
//
#define PESHIELD_NT_DEVICE_NAME		L"\\Device\\PeShield"

//
// Pe Shield Driver device name
//
#define PESHIELD_DOS_DEVICE_NAME	L"\\DosDevices\\PeShield"


//----------------------------------------------------------------------
//				S  H  A  R  E  D      O  B  J  E  C  T  S
//----------------------------------------------------------------------

//
// Pe Shield Process Information ( sys readable )
//
#define PESHIELD_MAX_PROCESS	30
typedef struct _PESHIELD_PROCESS_INFO {
	CHAR	ExePath[MAX_PATH+1];
	DWORD	RefCount;
	PVOID	TxtAddr;
	DWORD	TxtSize;
} PESHIELD_PROCESS_INFO, *PPESHIELD_PROCESS_INFO;

//
// Pe Shield Run Event
//
#define PESHIELD_RUN_REQUEST_EVENT		_T("Senol.PeShield.Run.Request.Event")
#define PESHIELD_RUN_COMPLETE_EVENT		_T("Senol.PeShield.Run.Complete.Event")
#define PESHIELD_RUN_REQUEST_KEVENT		L"\\BaseNamedObjects\\Senol.PeShield.Run.Request.Event"
#define PESHIELD_RUN_COMPLETE_KEVENT	L"\\BaseNamedObjects\\Senol.PeShield.Run.Complete.Event"

//
// Pe Shield Run Request ( sys writable )
//
typedef struct _PESHIELD_RUN_REQUEST {
	BOOLEAN		IsSucceeded;
	DWORD		ProcessId;
	DWORD		ProcessIndex;
} PESHIELD_RUN_REQUEST, *PPESHIELD_RUN_REQUEST;


//----------------------------------------------------------------------
//		   I  O      D  E  V  I  C  E      C  O  N  T  R  O  L
//----------------------------------------------------------------------

//
// Io control code base ( 0x800 - 0xFFF )
//
#define PESHIELD_IOCTL_BASE		0x800

//
// Pe Shield Driver device io control code
//
#define IOCTL_PESHIELD_START	\
				CTL_CODE(FILE_DEVICE_PESHIELD, PESHIELD_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PESHIELD_STOP		\
				CTL_CODE(FILE_DEVICE_PESHIELD, PESHIELD_IOCTL_BASE+1, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Pe Shield Start Result
//
typedef struct _PESHIELD_START_RESULT {
	PPESHIELD_PROCESS_INFO	ProcessList;
	PPESHIELD_RUN_REQUEST	RunRequest;
} PESHIELD_START_RESULT, *PPESHIELD_START_RESULT;


#endif // _IOCTLCODE_H_