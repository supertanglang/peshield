//----------------------------------------------------------------------
//			  P  E  S  H  I  E  L  D       D  R  I  V  E  R
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _PESHIELD_H_
#define _PESHIELD_H_

#include <ntddk.h>

//----------------------------------------------------------------------
//					D  E  F  I  N  I  T  I  O  N  S
//----------------------------------------------------------------------

//
// Type definitions
//
typedef unsigned long	DWORD, *PDWORD;
typedef char			CHAR, *LPSTR;
typedef unsigned char	BYTE, *PBYTE;
typedef HANDLE			HMODULE;

//
// Max path length
//
#define MAX_PATH	260
#define _MAX_DRIVE	3
#define _MAX_DIR	256
#define _MAX_FNAME	256
#define _MAX_EXT	256

//
// Page size
//
#define PAGE_SIZE			0x1000
#define PAGE_ROUND_DOWN(x)	(((ULONG_PTR)x)&(~(PAGE_SIZE-1)))
#define PAGE_ROUND_UP(x)	((((ULONG_PTR)x)%PAGE_SIZE) ? ((((ULONG_PTR)x)&(~(PAGE_SIZE-1)))+PAGE_SIZE) : ((ULONG_PTR)x))

//
// System process name
//
#define SYSNAME			"System"
#define EXPLORERNAME	"explorer.exe"
#define TASKMGRNAME		"taskmgr.exe"

//
// Max length of NT process name
//
#define NT_PROCNAMELEN  16


//----------------------------------------------------------------------
//				   D  R  I  V  E  R      T  Y  P  E  S
//----------------------------------------------------------------------

//
// Senol application signature
// 
#define SENOL_SIGNATURE	0x4E53	// SN

//
// Device extension
//
typedef struct _DEVICE_EXTENSION {
	unsigned short	Signature;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

//
// System threads structure
// 
typedef struct _SYSTEM_THREADS {
	LARGE_INTEGER		KernelTime;
	LARGE_INTEGER		UserTime;
	LARGE_INTEGER		CreateTime;
	ULONG				WaitTime;
	PVOID				StartAddress;
	CLIENT_ID			ClientIs;
	KPRIORITY			Priority;
	KPRIORITY			BasePriority;
	ULONG				ContextSwitchCount;
	ULONG				ThreadState;
	KWAIT_REASON		WaitReason;
} SYSTEM_THREADS, *PSYSTEM_THREADS;

//
// System processes structure
//
typedef struct _SYSTEM_PROCESSES {
	ULONG				NextEntryDelta;
	ULONG				ThreadCount;
	ULONG				Reserved[6];
	LARGE_INTEGER		CreateTime;
	LARGE_INTEGER		UserTime;
	LARGE_INTEGER		KernelTime;
	UNICODE_STRING		ProcessName;
	KPRIORITY			BasePriority;
	ULONG				ProcessId;
	ULONG				InheritedFromProcessId;
	ULONG				HandleCount;
	ULONG				Reserved2[2];
	VM_COUNTERS			VmCounters;
	IO_COUNTERS			IoCounters; //windows 2000 only
	struct _SYSTEM_THREADS		Threads[1];
} SYSTEM_PROCESSES, *PSYSTEM_PROCESSES;


//----------------------------------------------------------------------
//			D  I  S  P  A  T  C  H      R  O  U  T  I  N  E  S
//----------------------------------------------------------------------

//
// Driver unload routine
//
VOID 
PsShield_Unload( 
    IN PDRIVER_OBJECT  DriverObject 
    );

//
// Driver dispatch routine
//
NTSTATUS
PeShield_DispatchRoutine(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
	);

//
// Device dispatch routine
//
NTSTATUS
PeShield_DeviceRoutine(
	IN PDEVICE_OBJECT DeviceObject, 
    IN PIRP Irp
	);


//----------------------------------------------------------------------
//				 S  H  A  R  E  D      M  E  M  O  R  Y
//----------------------------------------------------------------------

//
// Create and map shared memory 
// ( * must be called in context that wants to shared the memory )
//
PVOID
PeShield_CreateSharedMemory(
	OUT PVOID* StoredPointer OPTIONAL,
	OUT PVOID* StoredMdl OPTIONAL,
	OUT	PVOID* StoredUserVA OPTIONAL,
	IN OUT SIZE_T* Size
	);

//
// Close and unmap shared memory
//
VOID
PeShield_CloseSharedMemory(
	IN OUT PVOID* StoredPointer OPTIONAL,
	IN OUT PVOID* StoredMdl OPTIONAL,
	IN OUT PVOID* StoredUserVA OPTIONAL
	);


//----------------------------------------------------------------------
//		 P  R  O  C  E  S  S      I  N  F  O  R  M  A  T  I  O  N
//----------------------------------------------------------------------

//
// Get process name offset 
// ( * this function must be called in driver entry )
//
ULONG 
PeShield_GetProcessNameOffset(
    VOID
    );

//
// Get current process name
//
LPCSTR 
PeShield_GetCurrentProcessName(
	OUT LPSTR Name
	);

//
// Get file name from its path
//
VOID
PeShield_GetFileName(
	OUT LPSTR Name,
	IN LPCSTR Path
	);

//
// Find process index in Process List
//
BOOLEAN
PeShield_FindManagedProcess(
	OUT PDWORD ProcessIndex OPTIONAL,
	IN LPCSTR ProcessName
	);


//----------------------------------------------------------------------
//					A  P  I      H  O  O  K  I  N  G
//----------------------------------------------------------------------

//
// Native Api Server Descriptor Entry
//
#pragma pack(1)
typedef struct ServiceDescriptorEntry {
	unsigned int *ServiceTableBase;
	unsigned int *ServiceCounterTableBase; 
	unsigned int NumberOfServices;
	unsigned char *ParamTableBase;
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t;
#pragma pack()

//
// Native Api Service Table
//
typedef struct _SRVTABLE {
	PVOID           *ServiceTable;
	ULONG           LowCall;        
	ULONG           HiCall;
	PVOID			*ArgTable;
} SRVTABLE, *PSRVTABLE;

//
// Hook Native Api
//
VOID
PeShield_HookNativeApi();

//
// Unhook Native Api
//
VOID
PeShield_UnhookNativeApi();


//----------------------------------------------------------------------
//		  N  A  T  I  V  E      A  P  I      D  E  C  L  A  R  E
//----------------------------------------------------------------------

//
// ZwCreateProcessEx
//
typedef NTSTATUS (NTAPI *ZWCREATEPROCESSEX)(
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN HANDLE ParentProcessHandle,
	IN BOOLEAN InheritObjectTable,
	IN HANDLE SectionHandle,
	IN HANDLE DebugPort,
	IN HANDLE ExceptionPort, 
	IN HANDLE Unknown
	);

//
// ZwOpenProcess
//
typedef NTSTATUS (NTAPI *ZWOPENPROCESS)(
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN PCLIENT_ID ClientId OPTIONAL
	);

//
// ZwReadVirtualMemory
//
typedef NTSTATUS (NTAPI *ZWREADVIRTUALMEMORY)(
	IN HANDLE hProcess,
	IN PVOID BaseAddress,
	IN PVOID Buffer,
	IN ULONG BytesToRead,
	OUT PULONG BytesRead
	);

//
// ZwWriteVirtualMemory
//
typedef NTSTATUS (NTAPI *ZWWRITEVIRTUALMEMORY)(
	IN HANDLE hProcess,
	IN PVOID BaseAddress,
	IN PVOID Buffer,
	IN ULONG BytesToWrite,
	OUT PULONG BytesWritten
	);

//
// ZwQuerySystemInformation
//
typedef NTSTATUS (NTAPI *ZWQUERYSYSTEMINFORMATION)(
    IN ULONG SystemInformationClass,
	IN PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength
	);


//----------------------------------------------------------------------
//				O  L  D      N  A  T  I  V  E      A  P  I
//----------------------------------------------------------------------

//
// ZwCreateProcessEx
//
#define ZwCreateProcessEx		0x30

//
// ZwOpenProcess
//
NTSYSAPI
NTSTATUS
NTAPI
ZwOpenProcess( 
	OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN PCLIENT_ID ClientId OPTIONAL
	);

//
// ZwReadVirtualMemory
//
#define ZwReadVirtualMemory		0xBA

//
// ZwWriteVirtualMemory
//
#define ZwWriteVirtualMemory	0x115

//
// ZwQuerySystemInformation
//
NTSYSAPI
NTSTATUS
NTAPI 
ZwQuerySystemInformation(
    IN ULONG SystemInformationClass,
	IN PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength
	);

//
// ZwQueryInformationProcess
// 
NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationProcess(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
 	);


//----------------------------------------------------------------------
//				N  E  W      N  A  T  I  V  E      A  P  I
//----------------------------------------------------------------------

//
// ZwCreateProcessEx
//
NTSTATUS 
PeShield_ZwCreateProcessEx(
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN HANDLE ParentProcessHandle,
	IN BOOLEAN InheritObjectTable,
	IN HANDLE SectionHandle,
	IN HANDLE DebugPort,
	IN HANDLE ExceptionPort, 
	IN HANDLE Unknown	
	);

//
// ZwOpenProcess
//
NTSTATUS 
PeShield_ZwOpenProcess( 
	OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN PCLIENT_ID ClientId OPTIONAL
	);

//
// ZwReadVirtualMemory
//
NTSTATUS 
PeShield_ZwReadVirtualMemory(
	IN HANDLE hProcess,
	IN PVOID BaseAddress,
	IN PVOID Buffer,
	IN ULONG BytesToRead,
	OUT PULONG BytesRead
	);

//
// ZwWriteVirtualMemory
//
NTSTATUS
PeShield_ZwWriteVirtualMemory(
	IN HANDLE hProcess,
	IN PVOID BaseAddress,
	IN PVOID Buffer,
	IN ULONG BytesToWrite,
	OUT PULONG BytesWritten
	);

//
// ZwQuerySystemInformation
// 
NTSTATUS 
PeShield_ZwQuerySystemInformation(
	IN ULONG SystemInformationClass,
	IN PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength
	);


#endif // _PESHIELD_H_