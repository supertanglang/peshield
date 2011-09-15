#include "PeShield.h"
#include "IoCtlCode.h"

#include <stdio.h>

//
// Remove optimize for debugging
//
#pragma optimize("", off)


//----------------------------------------------------------------------
//			 G  L  O  B  A  L      V  A  R  I  A  B  L  E  S
//----------------------------------------------------------------------

//
// Pe Shield Driver object
//
PDRIVER_OBJECT	PeShieldDriverObject;

//
// Pe Shield Loader id
//
DWORD	LoaderId;

//
// Pe Shield Sys on/off flag
//
BOOLEAN	PeShieldSysOn;

//
// Run Event data
//
HANDLE	RunRequestEventHandle;
HANDLE	RunCompleteEventHandle;
PKEVENT	RunRequestEvent;
PKEVENT	RunCompleteEvent;

//
// Run request data ( app readable )
//
PPESHIELD_RUN_REQUEST	RunRequest;
PMDL					RunRequestMdl;
PVOID					RunRequestUser;

//
// Process list data ( app writable )
//
PPESHIELD_PROCESS_INFO	ProcessList;
PMDL					ProcessListMdl;
PVOID					ProcessListUser;

//
// Process name offset
//
ULONG	ProcessNameOffset;

//
// Imported Native Api Service Table
//
__declspec(dllimport) ULONG	NtBuildNumber;
__declspec(dllimport) ServiceDescriptorTableEntry_t KeServiceDescriptorTable;
#define NT_SERVICE(Function)	KeServiceDescriptorTable.ServiceTableBase[*(PULONG)((PUCHAR)Function+1)]
#define NT_SERVICE_INDEX(Index)	KeServiceDescriptorTable.ServiceTableBase[Index]

//
// Old Native Api pointers
//
ZWCREATEPROCESSEX			OldZwCreateProcessEx;
ZWOPENPROCESS				OldZwOpenProcess;
ZWREADVIRTUALMEMORY			OldZwReadVirtualMemory;
ZWWRITEVIRTUALMEMORY		OldZwWriteVirtualMemory;
ZWQUERYSYSTEMINFORMATION	OldZwQuerySystemInformation;

// Create Process information data
LPSTR		CreateProcessName;
LPSTR		CreateCallerName;
FAST_MUTEX	CreateProcessMutex;

// Open Process information data
LPSTR		OpenProcessName;
LPSTR		OpenCallerName;
FAST_MUTEX	OpenProcessMutex;

// Read Process information data
LPSTR		ReadProcessName;
LPSTR		ReadCallerName;
FAST_MUTEX	ReadProcessMutex;

// Write Process information data
LPSTR		WriteProcessName;
LPSTR		WriteCallerName;
FAST_MUTEX	WriteProcessMutex;


//----------------------------------------------------------------------
//			D  I  S  P  A  T  C  H      R  O  U  T  I  N  E  S
//----------------------------------------------------------------------

//
// Driver entry point
//
NTSTATUS 
DriverEntry( 
	IN PDRIVER_OBJECT DriverObject, 
	IN PUNICODE_STRING RegistryPath 
	)
{
	NTSTATUS		ntStatus;
	UNICODE_STRING	NtDeviceName, Win32Name;
	PDEVICE_OBJECT	PeShieldDeviceObject;
	ULONG			i;

	UNICODE_STRING	RunEventName;
	
	// Debug
//	DbgBreakPoint();

	DbgPrint("PeShield.SYS: [Information] Entering DriverEntry\n");

	// Set Pe Shield Driver object
	PeShieldDriverObject = DriverObject;

	// Create device object name
    RtlInitUnicodeString(&NtDeviceName, PESHIELD_NT_DEVICE_NAME);
	ntStatus = IoCreateDevice(
		DriverObject,
		sizeof(DEVICE_EXTENSION),
		&NtDeviceName,
		FILE_DEVICE_PESHIELD,
		0,
		TRUE,
		&PeShieldDeviceObject
		);
	if (!NT_SUCCESS(ntStatus))
	{
		return ntStatus;
	}

	// Mark application signature on this device object ( for security )
    ((PDEVICE_EXTENSION)PeShieldDeviceObject->DeviceExtension)->Signature = SENOL_SIGNATURE;

	// Create symbolic link to communicate with application
	RtlInitUnicodeString(&Win32Name, PESHIELD_DOS_DEVICE_NAME);
    ntStatus = IoCreateSymbolicLink(&Win32Name, &NtDeviceName);
    if(!NT_SUCCESS(ntStatus))
	{
        IoDeleteDevice(PeShieldDeviceObject);
        return ntStatus;            
    }

	// Set dispatch routines
	for(i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) 
	{ 
        DriverObject->MajorFunction[i] = PeShield_DispatchRoutine;
    }

	// Set driver unload
	DriverObject->DriverUnload = PsShield_Unload;

	// Get process name offset
	ProcessNameOffset = PeShield_GetProcessNameOffset();
	
	// Open Run Request Event 
	RtlInitUnicodeString(&RunEventName, PESHIELD_RUN_REQUEST_KEVENT);
	RunRequestEvent = IoCreateNotificationEvent(&RunEventName, &RunRequestEventHandle);
	if (RunRequestEvent)
	{
		DbgPrint("PeShield.SYS: [Success] Run Request Event is opened\n");
	}
	else
	{
		DbgPrint("PeShield.SYS: [Error] Run Request Event is not opened\n");

		IoDeleteSymbolicLink(&Win32Name);
		IoDeleteDevice(PeShieldDeviceObject);

		return STATUS_UNSUCCESSFUL;
	}

	// Open Run Complete Event 
	RtlInitUnicodeString(&RunEventName, PESHIELD_RUN_COMPLETE_KEVENT);
	RunCompleteEvent = IoCreateNotificationEvent(&RunEventName, &RunCompleteEventHandle);
	if (RunCompleteEvent)
	{
		DbgPrint("PeShield.SYS: [Success] Run Complete Event is opened\n");
	}
	else
	{
		DbgPrint("PeShield.SYS: [Error] Run Complete Event is not opened\n");

		ZwClose(RunRequestEventHandle);
		IoDeleteSymbolicLink(&Win32Name);
		IoDeleteDevice(PeShieldDeviceObject);

		return STATUS_UNSUCCESSFUL;
	}

	// Initialize process fast mutex
	ExInitializeFastMutex(&CreateProcessMutex);
	ExInitializeFastMutex(&OpenProcessMutex);
	ExInitializeFastMutex(&ReadProcessMutex);
	ExInitializeFastMutex(&WriteProcessMutex);
	DbgPrint("PeShield.SYS: [Success] Process mutex is initialized\n");

	// Allocate process name buffer
	CreateProcessName	= ExAllocatePool(NonPagedPool, PAGE_ROUND_UP(NT_PROCNAMELEN));
	OpenProcessName		= ExAllocatePool(NonPagedPool, PAGE_ROUND_UP(NT_PROCNAMELEN));
	ReadProcessName		= ExAllocatePool(NonPagedPool, PAGE_ROUND_UP(NT_PROCNAMELEN));
	WriteProcessName	= ExAllocatePool(NonPagedPool, PAGE_ROUND_UP(NT_PROCNAMELEN));
	CreateCallerName	= ExAllocatePool(NonPagedPool, PAGE_ROUND_UP(NT_PROCNAMELEN));
	OpenCallerName		= ExAllocatePool(NonPagedPool, PAGE_ROUND_UP(NT_PROCNAMELEN));
	ReadCallerName		= ExAllocatePool(NonPagedPool, PAGE_ROUND_UP(NT_PROCNAMELEN));
	WriteCallerName		= ExAllocatePool(NonPagedPool, PAGE_ROUND_UP(NT_PROCNAMELEN));
	if (CreateProcessName && OpenProcessName && ReadProcessName && WriteProcessName &&
		OpenCallerName && ReadCallerName && WriteCallerName)
	{
		DbgPrint("PeShield.SYS: [Success] Process name buffer is allocated\n");
	}
	else
	{
		DbgPrint("PeShield.SYS: [Error] Process name buffer is not allocated\n");
	
		if (CreateProcessName)	ExFreePool(CreateProcessName);
		if (OpenProcessName)	ExFreePool(OpenProcessName);
		if (ReadProcessName)	ExFreePool(ReadProcessName);
		if (WriteProcessName)	ExFreePool(WriteProcessName);
		if (CreateCallerName)	ExFreePool(CreateCallerName);
		if (OpenCallerName)		ExFreePool(OpenCallerName);
		if (ReadCallerName)		ExFreePool(ReadCallerName);
		if (WriteCallerName)	ExFreePool(WriteCallerName);

		ZwClose(RunCompleteEvent);
		ZwClose(RunRequestEventHandle);
		IoDeleteSymbolicLink(&Win32Name);
		IoDeleteDevice(PeShieldDeviceObject);
		
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	DbgPrint("PeShield.SYS: [Information] Reterning DriverEntry\n");

	return ntStatus;
}

//
// Driver unload routine
//
VOID 
PsShield_Unload( 
    IN PDRIVER_OBJECT  DriverObject 
    )
{
	UNICODE_STRING	Win32Name;

	DbgPrint("PeShield.SYS: [Information] Entering Unload\n");
	
	// Free Create process name buffer
	if (CreateProcessName)	ExFreePool(CreateProcessName);
	if (OpenProcessName)	ExFreePool(OpenProcessName);
	if (ReadProcessName)	ExFreePool(ReadProcessName);
	if (WriteProcessName)	ExFreePool(WriteProcessName);
	if (CreateCallerName)	ExFreePool(CreateCallerName);
	if (OpenCallerName)		ExFreePool(OpenCallerName);
	if (ReadCallerName)		ExFreePool(ReadCallerName);
	if (WriteCallerName)	ExFreePool(WriteCallerName);
	DbgPrint("PeShield.SYS: [Success] Create process name buffer is freed\n");
	
	// Close Run Complete Event
	if (RunCompleteEventHandle)
	{
		ZwClose(RunCompleteEventHandle);
		RunCompleteEventHandle = NULL;
		DbgPrint("PeShield.SYS: [Success] Run Complete Event is closed\n");
	}

	// Close Run Request Event
	if (RunRequestEventHandle)
	{
		ZwClose(RunRequestEventHandle);
		RunRequestEventHandle = NULL;
		DbgPrint("PeShield.SYS: [Success] Run Request Event is closed\n");
	}
	
	// Delete symbolic link
    RtlInitUnicodeString(&Win32Name, PESHIELD_DOS_DEVICE_NAME);
    IoDeleteSymbolicLink(&Win32Name);

	// Delete PeShield application device object
	IoDeleteDevice(DriverObject->DeviceObject);

	DbgPrint("PeShield.SYS: [Information] Returning Unload\n");
}

//
// Driver dispatch routine
//
NTSTATUS
PeShield_DispatchRoutine(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
	)
{
	// Is irp from application ? 
	if (((PDEVICE_EXTENSION)DeviceObject->DeviceExtension)->Signature == SENOL_SIGNATURE)
	{
		// Yes, pass it to the device routine
		return PeShield_DeviceRoutine(DeviceObject, Irp);
	}
	else
	{	
		DbgPrint("PeShield.SYS: [Fail] Unknown Irp\n");

		// No, just complete irp
		Irp->IoStatus.Status = STATUS_SUCCESS;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);

		return Irp->IoStatus.Status;
	}
}

//
// Device dispatch routine
//
NTSTATUS
PeShield_DeviceRoutine(
	IN PDEVICE_OBJECT DeviceObject, 
    IN PIRP Irp
	)
{
	PIO_STACK_LOCATION  currentIrpStack;
    PVOID               InputBuffer;
    PVOID               OutputBuffer;
    ULONG               InputBufferLength;
    ULONG               OutputBufferLength;
    ULONG               IoControlCode;

	PESHIELD_START_RESULT	StartResult;
	SIZE_T					SharedSize;

	// Set io status to success
	Irp->IoStatus.Status      = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

	// Get current irp stack location
	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);

	// Get input/output buffer and length
	InputBuffer        = Irp->AssociatedIrp.SystemBuffer;
    InputBufferLength  = currentIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    OutputBuffer       = Irp->AssociatedIrp.SystemBuffer;
    OutputBufferLength = currentIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    IoControlCode      = currentIrpStack->Parameters.DeviceIoControl.IoControlCode;

	// Handling major function irp
	switch (currentIrpStack->MajorFunction)
	{
	case IRP_MJ_CREATE:

		DbgPrint("PeShield.SYS: [Information] IRP_MJ_CREATE ( Application )\n");
		break;

	case IRP_MJ_CLOSE:
		DbgPrint("PeShield.SYS: [Information] IRP_MJ_CLOSE ( Application )\n");
		break;

	case IRP_MJ_DEVICE_CONTROL:
		DbgPrint("PeShield.SYS: [Information] IRP_MJ_DEVICE_CONTROL ( Application )\n");

		// Get output buffer if its passed as an MDL
        if( Irp->MdlAddress )
		{
            OutputBuffer = MmGetSystemAddressForMdl( Irp->MdlAddress );
        }

		// Handling ioctl code
		switch (IoControlCode)
		{
		case IOCTL_PESHIELD_START:
			
			DbgPrint("PeShield.SYS: [Information] IOCTL code : Pe Shield Start\n");
			
			// Is buffer length okay ?
			if (InputBufferLength >= sizeof(LoaderId) && OutputBufferLength >= sizeof(StartResult))
			{
				// Get Pe Shield Loader id
				RtlCopyMemory(&LoaderId, InputBuffer, sizeof(LoaderId));
				
				// Is Pe Shield Sys already on ?
				if (PeShieldSysOn)
				{
					// Unhook Native Api
					PeShield_UnhookNativeApi();
					
					// Close Run Request shared memory
					PeShield_CloseSharedMemory(&RunRequest, &RunRequestMdl, &RunRequestUser);
					
					// Close Process List shared memory
					PeShield_CloseSharedMemory(&ProcessList, &ProcessListMdl, &ProcessListUser);

					DbgPrint("PeShield.SYS: [Success] Pe Shield Sys is restarted\n");
				}
				
				// Set Process List size
				SharedSize = sizeof(PESHIELD_PROCESS_INFO) * PESHIELD_MAX_PROCESS;
				
				// Create Process List shared memory
				StartResult.ProcessList =
					PeShield_CreateSharedMemory(&ProcessList, &ProcessListMdl, &ProcessListUser, &SharedSize);
				if (StartResult.ProcessList)
				{
					DbgPrint("PeShield.SYS: [Success] Process List is created (%d bytes)\n", SharedSize);
					
					// Set Process List size
					SharedSize = sizeof(PESHIELD_RUN_REQUEST);
					
					// Create Run Request shared memory
					StartResult.RunRequest =
						PeShield_CreateSharedMemory(&RunRequest, &RunRequestMdl, &RunRequestUser, &SharedSize);
					
					if (StartResult.RunRequest)
					{
						DbgPrint("PeShield.SYS: [Success] Run Request is created (%d bytes)\n", SharedSize);
						
						// Set output buffer
						RtlCopyMemory(OutputBuffer, &StartResult, sizeof(StartResult));
						
						// Hook Native Api
						PeShield_HookNativeApi();
						DbgPrint("PeShield.SYS: [Success] Native Api are hooked\n");
						
						// Set status information
						Irp->IoStatus.Information = sizeof(StartResult);
						
						// Set Pe Shield Sys on
						PeShieldSysOn = TRUE;
					}
					else
					{
						DbgPrint("PeShield.SYS: [Error] Run Request is not created\n");
						Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					}
				}
				else
				{
					DbgPrint("PeShield.SYS: [Error] Process List is not created\n");
					Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
				}
				
				// Is operation failed ?
				if (!NT_SUCCESS(Irp->IoStatus.Status))
				{
					// Unhook Native Api
					PeShield_UnhookNativeApi();
					
					// Close Run Request shared memory
					PeShield_CloseSharedMemory(&RunRequest, &RunRequestMdl, &RunRequestUser);
					
					// Close Process List shared memory
					PeShield_CloseSharedMemory(&ProcessList, &ProcessListMdl, &ProcessListUser);
					
					// Set Pe Shield Loader id to zero
					LoaderId = 0;
				}
			}
			else
			{
				Irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
			}

			break;

		case IOCTL_PESHIELD_STOP:

			DbgPrint("PeShield.SYS: [Information] IOCTL code : Pe Shield Stop\n");

			// Is Pe Shield Sys off ?
			if (!PeShieldSysOn)
			{
				Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
				break;
			}

			// Set Pe Shield Sys off
			PeShieldSysOn = FALSE;

			// Unhook Native Api
			PeShield_UnhookNativeApi();
			DbgPrint("PeShield.SYS: [Success] Native Api are unhooked\n");
				
			// Close Run Request shared memory
			PeShield_CloseSharedMemory(&RunRequest, &RunRequestMdl, &RunRequestUser);
			DbgPrint("PeShield.SYS: [Success] Run Request is closed\n");
					
			// Close Process List shared memory
			PeShield_CloseSharedMemory(&ProcessList, &ProcessListMdl, &ProcessListUser);
			DbgPrint("PeShield.SYS: [Success] Process List is closed\n");

			// Set Pe Shield Loader id to zero
			LoaderId = 0;

			break;

		default:

			DbgPrint("PeShield.SYS: [Error] Unknown IOCTL code (%x)\n", IoControlCode);
			Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;

			break;
		}

		break;
	}

    // Complete irp
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}


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
	)
{
	PVOID	buffer;
	PMDL	mdl;
	PVOID	userVA;
	SIZE_T	size;

	// Round up size by page size
	size = PAGE_ROUND_UP(*Size);

	// Allocate buffer to share with the context
	buffer = ExAllocatePool(NonPagedPool, size);
	if (!buffer)
	{
		return NULL;
	}

	// Allocate and initialize an MDL that describes the buffer
	mdl = IoAllocateMdl(buffer, size, FALSE, FALSE, NULL);
	if (!mdl)
	{
		ExFreePool(buffer);
		return NULL;
	}

	// Finish building the MDL - Fill in the "page portion"
	MmBuildMdlForNonPagedPool(mdl);

	// Map the buffer into user space
	userVA = MmMapLockedPages(mdl, UserMode);

	// Store away both the mapped VA and the MDL address, so that
	// later we can call MmUnmapLockedPages(StoredUserVA, StoredMdl)
	if (StoredPointer)	
	{
		*StoredPointer = buffer;
	}
	if (StoredMdl)
	{
		*StoredMdl = mdl;
	}
	if (StoredUserVA)
	{	
		*StoredUserVA = userVA;
	}
	*Size = size;

	return userVA;
}

//
// Close and unmap shared memory
// ( * must be called in context that wants to shared the memory )
//
VOID
PeShield_CloseSharedMemory(
	IN OUT PVOID* StoredPointer OPTIONAL,
	IN OUT PVOID* StoredMdl OPTIONAL,
	IN OUT PVOID* StoredUserVA OPTIONAL
	)
{
	if (StoredMdl && *StoredMdl && StoredUserVA && *StoredUserVA)
	{
		MmUnmapLockedPages(*StoredUserVA, *StoredMdl);
		IoFreeMdl(*StoredMdl);
		*StoredMdl = NULL;
	}

	if (StoredPointer && *StoredPointer)
	{
		ExFreePool(*StoredPointer);
		*StoredPointer = NULL;
	}
}


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
    )
{
    PEPROCESS       curproc;
    int             i;

    curproc = PsGetCurrentProcess();

    // Scan for 12KB, hoping the KPEB never grows that big!
    for (i = 0; i < 3*PAGE_SIZE; i++) 
	{ 
        if (!strncmp(SYSNAME, (LPSTR)curproc + i, strlen(SYSNAME))) 
		{
            return i;
        }
    }

    return 0;
}

//
// Get current process name
//
LPCSTR
PeShield_GetCurrentProcessName(
	OUT LPSTR Name
	)
{
	PEPROCESS       curproc;
	LPSTR			nameptr;
	
	if (ProcessNameOffset) 
	{
		curproc	= PsGetCurrentProcess();
		nameptr	= (LPSTR)curproc + ProcessNameOffset;
		strncpy(Name, nameptr, NT_PROCNAMELEN);
		Name[NT_PROCNAMELEN] = 0;
	
		return Name;
	} 

	return NULL;
}

//
// Get file name from its path
//
VOID
PeShield_GetFileName(
	OUT LPSTR Name,
	IN LPCSTR Path
	)
{
	LPSTR	LastSlash;
	
	LastSlash = strrchr(Path, '\\');
	if (LastSlash)
	{
		strcpy(Name, LastSlash+1);
	}
	else
	{
		strcpy(Name, "???");
	}
}

//
// Find managed process information index in Process List
//
BOOLEAN
PeShield_FindManagedProcess(
	OUT PDWORD ProcessIndex OPTIONAL,
	IN LPCSTR ProcessName
	)
{
	DWORD	i;
	CHAR	FileName[_MAX_FNAME+_MAX_EXT];

	for (i = 0; i < PESHIELD_MAX_PROCESS; i++)
	{
		// Search same process name that is already processing ( means certified )
		if (ProcessList[i].RefCount > 0)
		{
			// Get file name from path
			PeShield_GetFileName(FileName, ProcessList[i].ExePath);

			if (strncmp(FileName, ProcessName, strlen(ProcessName)) == 0)
			{
				if (ProcessIndex)
				{
					*ProcessIndex = i;
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}


//----------------------------------------------------------------------
//					A  P  I      H  O  O  K  I  N  G
//----------------------------------------------------------------------

//
// Hook Native Api
//
VOID
PeShield_HookNativeApi()
{
	DbgPrint("PeShield.SYS: [Information] Entering hook Native Api\n");

	// Save old Native Api
	OldZwCreateProcessEx		= (ZWCREATEPROCESSEX)(NT_SERVICE_INDEX(ZwCreateProcessEx));
	OldZwOpenProcess			= (ZWOPENPROCESS)(NT_SERVICE(ZwOpenProcess));
	OldZwReadVirtualMemory		= (ZWREADVIRTUALMEMORY)(NT_SERVICE_INDEX(ZwReadVirtualMemory));
	OldZwWriteVirtualMemory		= (ZWWRITEVIRTUALMEMORY)(NT_SERVICE_INDEX(ZwWriteVirtualMemory));
	OldZwQuerySystemInformation	= (ZWQUERYSYSTEMINFORMATION)(NT_SERVICE(ZwQuerySystemInformation));

	// Disable system interrupt
	_asm
	{
		CLI
		MOV	EAX, CR0	
		AND EAX, NOT 10000H 
		MOV	CR0, EAX
	}

	// Set new Native Api
	(ZWCREATEPROCESSEX)(NT_SERVICE_INDEX(ZwCreateProcessEx))			= PeShield_ZwCreateProcessEx;
	(ZWOPENPROCESS)(NT_SERVICE(ZwOpenProcess))							= PeShield_ZwOpenProcess;
	(ZWREADVIRTUALMEMORY)(NT_SERVICE_INDEX(ZwReadVirtualMemory))		= PeShield_ZwReadVirtualMemory;
	(ZWWRITEVIRTUALMEMORY)(NT_SERVICE_INDEX(ZwWriteVirtualMemory))		= PeShield_ZwWriteVirtualMemory;
	(ZWQUERYSYSTEMINFORMATION)(NT_SERVICE(ZwQuerySystemInformation))	= PeShield_ZwQuerySystemInformation;

	// Enable system interrupt
	_asm
	{
		MOV	EAX, CR0		
		OR	EAX, 10000H		
		MOV	CR0, EAX			
		STI	
	}

	DbgPrint("PeShield.SYS: [Information] Returning hook Native Api\n");
}

//
// Unhook Native Api
//
VOID
PeShield_UnhookNativeApi()
{
	DbgPrint("PeShield.SYS: [Information] Entering unhook Native Api\n");

	// Disable system interrupt
	_asm
	{
		CLI
		MOV	EAX, CR0	
		AND EAX, NOT 10000H 
		MOV	CR0, EAX
	}

	// Set old Native Api back
	(ZWCREATEPROCESSEX)(NT_SERVICE_INDEX(ZwCreateProcessEx))			= OldZwCreateProcessEx;
	(ZWOPENPROCESS)(NT_SERVICE(ZwOpenProcess))							= OldZwOpenProcess;
	(ZWREADVIRTUALMEMORY)(NT_SERVICE_INDEX(ZwReadVirtualMemory))		= OldZwReadVirtualMemory;
	(ZWWRITEVIRTUALMEMORY)(NT_SERVICE_INDEX(ZwWriteVirtualMemory))		= OldZwWriteVirtualMemory;
	(ZWQUERYSYSTEMINFORMATION)(NT_SERVICE(ZwQuerySystemInformation))	= OldZwQuerySystemInformation;

	// Enable system interrupt
	_asm
	{
		MOV	EAX, CR0		
		OR	EAX, 10000H		
		MOV	CR0, EAX			
		STI	
	}

	DbgPrint("PeShield.SYS: [Information] Returning unhook Native Api\n");
}


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
	)
{
	NTSTATUS					ntStatus;
	PEPROCESS					Process;
	DWORD						ProcessIndex;
	PROCESS_BASIC_INFORMATION	BasicInformation;
	
	// Call old ZwCreateProcessEx
	ntStatus = ((ZWCREATEPROCESSEX)OldZwCreateProcessEx)(
		ProcessHandle,
		DesiredAccess,
		ObjectAttributes,
		ParentProcessHandle,
		InheritObjectTable,
		SectionHandle,
		DebugPort,
		ExceptionPort, 
		Unknown
		);
	
	// Is old ZwCreateProcessEx succeeded ?
	if (NT_SUCCESS(ntStatus))
	{
		// Is this necessary to hook ?
		if (PeShieldSysOn && (DWORD)PsGetCurrentProcessId() != LoaderId)
		{
			if (NT_SUCCESS(
				ObReferenceObjectByHandle(
				*ProcessHandle,
				PROCESS_ALL_ACCESS,
				NULL,
				KernelMode,
				(PVOID *)&Process,
				NULL)))
			{	
				// Acquire fast mutex
				ExAcquireFastMutex(&CreateProcessMutex);
				
				// Get process name
				strncpy(CreateProcessName, ((PCHAR)Process) + ProcessNameOffset, NT_PROCNAMELEN);
				CreateProcessName[NT_PROCNAMELEN] = 0;
				
				// Get caller name
				PeShield_GetCurrentProcessName(CreateCallerName);
				
				// Is caller explorer or the process ?
				if (strcmp(CreateCallerName, EXPLORERNAME) == 0 || 
					strcmp(CreateCallerName, CreateProcessName) == 0)
				{	
					// Is this process managed ?
					if (PeShield_FindManagedProcess(&ProcessIndex, CreateProcessName))
					{
						// Get process id
						if (NT_SUCCESS(
							ZwQueryInformationProcess(
							*ProcessHandle,
							ProcessBasicInformation,
							&BasicInformation,
							sizeof(BasicInformation),
							NULL
							)))
						{
							// Set Run Request
							RunRequest->IsSucceeded		= FALSE;
							RunRequest->ProcessId		= BasicInformation.UniqueProcessId;
							RunRequest->ProcessIndex	= ProcessIndex;
							
							DbgPrint("PeShield.SYS: [Information] ZwCreateProcessEx "
								"< Caller = %s, Name = %s, Id = %d, Index = %d, Count = %d >\n", 
								CreateCallerName, CreateProcessName, 
								RunRequest->ProcessId, RunRequest->ProcessIndex, 
								ProcessList[ProcessIndex].RefCount
								);
							
							// Set Run Request Event
							KeSetEvent(RunRequestEvent, IO_NO_INCREMENT, FALSE);
							
							DbgPrint("PeShield.SYS: [Information] Set Run Request Event, waiting for Run Complete Event\n");
							
							// Wait for Run Complete Event
							KeWaitForSingleObject(
								RunCompleteEvent, 
								Executive, 
								KernelMode,
								FALSE,
								NULL
								);
							
							// Reset Run Complete Event for the next time
							KeClearEvent(RunCompleteEvent);
							
							DbgPrint("PeShield.SYS: [Information] Run Complete Event is released\n");
							
							// Is Run Request failed ?
							if (!RunRequest->IsSucceeded)
							{
								ntStatus = STATUS_UNSUCCESSFUL;
							}
						}
						else
						{
							ntStatus = STATUS_UNSUCCESSFUL;
						}
					}
				}
				
				// Release fast mutex
				ExReleaseFastMutex(&CreateProcessMutex);
				
				ObDereferenceObject(Process);
			}
		}
	}
	
	return ntStatus;
}

//
// ZeOpenProcess
//
NTSTATUS PeShield_ZwOpenProcess( 
	OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN PCLIENT_ID ClientId OPTIONAL
	)
{
	NTSTATUS	ntStatus;
	PEPROCESS	Process;
	DWORD		ProcessIndex;
	
	// Call old ZwOpenProcess
	ntStatus = ((ZWOPENPROCESS)OldZwOpenProcess)(
		ProcessHandle,
		DesiredAccess,
		ObjectAttributes,
		ClientId
		);

	// Is old ZwOpenProcess succeeded ?
	if (NT_SUCCESS(ntStatus))
	{
		// Is process handle all accessible ?
		if (DesiredAccess == PROCESS_ALL_ACCESS)
		{
			// Is this necessary to hook ?
			if (PeShieldSysOn && (DWORD)PsGetCurrentProcessId() != LoaderId)
			{		
				if (NT_SUCCESS(
					ObReferenceObjectByHandle(
					*ProcessHandle,
					PROCESS_ALL_ACCESS,
					NULL,
					KernelMode,
					(PVOID *)&Process,
					NULL)))
				{	
					// Acquire fast mutex
					ExAcquireFastMutex(&OpenProcessMutex);

					// Get process name
					strncpy(OpenProcessName, ((PCHAR)Process) + ProcessNameOffset, NT_PROCNAMELEN);
					OpenProcessName[NT_PROCNAMELEN] = 0;
					
					// Get caller name
					PeShield_GetCurrentProcessName(OpenCallerName);

					// Is caller not explorer or task manager ?
					if (strcmp(OpenCallerName, EXPLORERNAME) != 0 && 
						strcmp(OpenCallerName, TASKMGRNAME) != 0)
					{
						// Is caller not the process ?
						if (strcmp(OpenCallerName, OpenProcessName) != 0)
						{
							// Is this process managed ?
							if (PeShield_FindManagedProcess(&ProcessIndex, OpenProcessName))
							{
								DbgPrint("PeShield.SYS: [Information] ZwOpenProcess "
									"< Caller = %s, Name = %s, Index = %d, Count = %d >\n", 
									OpenCallerName, OpenProcessName, 
									RunRequest->ProcessIndex, ProcessList[ProcessIndex].RefCount
									);
								
								// Close process handle
								ZwClose(ProcessHandle);
								ProcessHandle = NULL;
								
								// Set status to invalid handle
								ntStatus = STATUS_INVALID_HANDLE;
							}
						}
					}
					
					// Release fast mutex
					ExReleaseFastMutex(&OpenProcessMutex);

					ObDereferenceObject(Process);
				}
			}
		}
	}

	return	ntStatus;
}

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
	)
{
	NTSTATUS	ntStatus;
	PEPROCESS	Process;
	DWORD		ProcessIndex;


	// Is read process request ?
	if ((BytesToRead != 0x50) && 
		(BaseAddress != (void *)0x7FFDC00C) && 
		(BaseAddress != (void *)0x7FFDB00C) && 
		(BaseAddress != (void *)0x1A1EA4) && 
		(BaseAddress != (void *)0x20840))
	{
		// Is this necessary to hook ?
		if (PeShieldSysOn && (DWORD)PsGetCurrentProcessId() != LoaderId)
		{		
			if (NT_SUCCESS(
				ObReferenceObjectByHandle(
				hProcess,
				PROCESS_ALL_ACCESS,
				NULL,
				KernelMode,
				(PVOID *)&Process,
				NULL)))
			{	
				// Acquire fast mutex
			//	ExAcquireFastMutex(&ReadProcessMutex);
				
				// Get process name
				strncpy(ReadProcessName, ((PCHAR)Process) + ProcessNameOffset, NT_PROCNAMELEN);
				ReadProcessName[NT_PROCNAMELEN] = 0;
				
				// Get caller name
				PeShield_GetCurrentProcessName(ReadCallerName);
				
				// Is caller not explorer ?
				if (strcmp(OpenCallerName, EXPLORERNAME) != 0 && 
					strcmp(OpenCallerName, TASKMGRNAME) != 0)
				{
					// Is caller not the process ?
					if (strcmp(OpenCallerName, OpenProcessName) != 0)
					{
						// Is this process managed ?
						if (PeShield_FindManagedProcess(&ProcessIndex, ReadProcessName))
						{
							DbgPrint("PeShield.SYS: [Information] ZwReadVirtualMemory "
								"< Caller = %s, Name = %s, Index = %d, Count = %d >\n", 
								ReadCallerName, ReadProcessName, 
								RunRequest->ProcessIndex, ProcessList[ProcessIndex].RefCount
								);
							
							// return invalid handle status
							return STATUS_INVALID_HANDLE;
						}
					}
				}

				// Release fast mutex
			//	ExReleaseFastMutex(&ReadProcessMutex);
				
				ObDereferenceObject(Process);
			}
		}
	}
	
	// Call old ZwReadVirtualMemory
	ntStatus = ((ZWREADVIRTUALMEMORY)OldZwReadVirtualMemory)(
		hProcess,
		BaseAddress,
		Buffer,
		BytesToRead,
		BytesRead
		);
	
	return	ntStatus;
}

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
	)
{
	NTSTATUS	ntStatus;
	PEPROCESS	Process;
	DWORD		ProcessIndex;
	
	// Is this necessary to hook ?
	if (PeShieldSysOn && (DWORD)PsGetCurrentProcessId() != LoaderId)
	{		
		if (NT_SUCCESS(
			ObReferenceObjectByHandle(
			hProcess,
			PROCESS_ALL_ACCESS,
			NULL,
			KernelMode,
			(PVOID *)&Process,
			NULL)))
		{	
			// Acquire fast mutex
		//	ExAcquireFastMutex(&WriteProcessMutex);
			
			// Get process name
			strncpy(WriteProcessName, ((PCHAR)Process) + ProcessNameOffset, NT_PROCNAMELEN);
			WriteProcessName[NT_PROCNAMELEN] = 0;
			
			// Get caller name
			PeShield_GetCurrentProcessName(WriteCallerName);
			
			// Is caller not explorer ?
			if (strcmp(OpenCallerName, EXPLORERNAME) != 0 && 
				strcmp(OpenCallerName, TASKMGRNAME) != 0)
			{
				// Is caller not the process ?
				if (strcmp(OpenCallerName, OpenProcessName) != 0)
				{
					// Is this process managed ?
					if (PeShield_FindManagedProcess(&ProcessIndex, WriteProcessName))
					{
						DbgPrint("PeShield.SYS: [Information] ZwWriteVirtualMemory "
							"< Caller = %s, Name = %s, Index = %d, Count = %d >\n", 
							WriteCallerName, WriteProcessName, 
							RunRequest->ProcessIndex, ProcessList[ProcessIndex].RefCount
							);
						
						// return invalid handle status
						return STATUS_SUCCESS;
					}
				}
			}

			// Release fast mutex
		//	ExReleaseFastMutex(&WriteProcessMutex);
			
			ObDereferenceObject(Process);
		}
	}
	
	// Call old ZwWriteVirtualMemory
	ntStatus = ((ZWWRITEVIRTUALMEMORY)OldZwWriteVirtualMemory)(
		hProcess,
		BaseAddress,
		Buffer,
		BytesToWrite,
		BytesWritten
		);
	
	return	ntStatus;
}

//
// ZwQuerySystemInformation
// 
NTSTATUS 
PeShield_ZwQuerySystemInformation(
	IN ULONG SystemInformationClass,
	IN PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength
	)
{
	NTSTATUS			ntStatus;
	PSYSTEM_PROCESSES	curr;
	PSYSTEM_PROCESSES	prev;
		
	// Call old ZwQuerySystemInformation
	ntStatus = ((ZWQUERYSYSTEMINFORMATION)OldZwQuerySystemInformation)(
		SystemInformationClass,
		SystemInformation,
		SystemInformationLength,
		ReturnLength);
	
	// Is old ZwQuerySystemInformation succeeded ?
	if (NT_SUCCESS(ntStatus))
	{
		if (5 == SystemInformationClass)
		{
			curr = (PSYSTEM_PROCESSES)SystemInformation;
			prev = NULL;
			
			while (curr)
			{
				// Is process id the same with Pe Shield Loader id ?
				if (curr->ProcessId == LoaderId)
				{
					// Is current process not the header ?
					if(prev)
					{
						// Pick current entry out
						if(curr->NextEntryDelta)
						{
							prev->NextEntryDelta += curr->NextEntryDelta;
						}
						else
						{
							prev->NextEntryDelta = 0;
						}
					}
					else
					{
						// Pick header entry out
						if(curr->NextEntryDelta)
						{
							(char *)SystemInformation += curr->NextEntryDelta;
						}
						else
						{
							// Set system information to NULL
							SystemInformation = NULL;
						}
					}

				//	DbgPrint("PeShield.SYS: Pe Shield Loader process is invisible\n");
				}
				
				// Save current process
				prev = curr;
				
				// Move to next
				if (curr->NextEntryDelta) 
				{
					(PBYTE)curr += curr->NextEntryDelta;
				}
				else 
				{
					curr = NULL;
				}
			}
		}
	}

	return ntStatus;
}