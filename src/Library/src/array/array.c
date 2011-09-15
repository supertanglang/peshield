#include "../../h/array.h"

#include <stdio.h>

//----------------------------------------------------------------------
//			 G  L  O  B  A  L      V  A  R  I  A  B  L  E  S
//----------------------------------------------------------------------

//
// Initialization Flag
//
static BOOL		g_bInitArray;

//
// Node size
//
static DWORD	g_NodeSize;

//
// Array max length
//
static DWORD	g_MaxCount;

//
// Current array count
//
static DWORD	g_CurCount;

//
// Array Object
//
static PBYTE	g_Array;

//
// Index queue Object
//
static PDWORD	g_IndexQueue;
static DWORD	g_QueueFront;
static DWORD	g_QueueRear;

//
// Critical section
//
static CRITICAL_SECTION		g_CriticalSection;


//----------------------------------------------------------------------
//				I  N  I  T  I  A  L  I  Z  A  T  I  O  N
//----------------------------------------------------------------------

//
// Initialize Pe Shield Array data
//
BOOL
PeShield_ArrayStartup(
	IN CONST PVOID Array,
	IN DWORD Size,
	IN DWORD Count
	)
{
	BOOL	bInitialized = FALSE;
	DWORD	i;

	if (g_bInitArray)
	{
		return FALSE;
	}

	// Set node size
	g_NodeSize = Size;

	// Set max count
	g_MaxCount = Count;
	
	// Set current count
	g_CurCount = 0;
	
	// Allocate index queue
	g_IndexQueue = (PDWORD)malloc(sizeof(DWORD) * g_MaxCount);
	if (g_MaxCount)
	{
		// Put all index into queue
		for (i = 0; i < g_MaxCount; i++)
		{
			g_IndexQueue[i] = i;
		}

		// Set queue front and read to zero
		g_QueueFront = g_QueueRear = 0;

		// Set array object
		g_Array = Array;
		if (g_Array)
		{
			// Set array to zero
			ZeroMemory(g_Array, g_MaxCount * g_NodeSize);

			// Initialize critical section
			InitializeCriticalSection(&g_CriticalSection);

			// Set initialized
			g_bInitArray = bInitialized = TRUE;
		}
	}

	if (!bInitialized)
	{
		// Uninitialize
		g_bInitArray = TRUE;
		PeShield_ArrayCleanup();
	}

	return TRUE;
}

//
// Uninitialize Pe Shield Array data
//
VOID
PeShield_ArrayCleanup()
{
	if (g_bInitArray)
	{
		// Uninitialize critical section
		DeleteCriticalSection(&g_CriticalSection);

		// Free array
		g_Array = NULL;
		
		// Free index queue
		if (g_IndexQueue)
		{
			free(g_IndexQueue);
			g_IndexQueue = NULL;
		}

		// Set length and count to zero
		g_NodeSize = g_MaxCount = g_CurCount = g_QueueFront = g_QueueRear = 0;

		// Set uninitialized
		g_bInitArray = FALSE;
	}
}


//----------------------------------------------------------------------
//			 A  R  R  A  Y      M  A  N  A  G  E  M  E  N  T
//----------------------------------------------------------------------

// 
// Insert data, returns index
//
DWORD
PeShield_ArrayInsert(
	IN PVOID Data
	)
{
	DWORD Index;

	if (!g_bInitArray || PeShield_ArrayIsFull())
	{
		return 0xFFFFFFFF;
	}
	
	// Enter critical section
	EnterCriticalSection(&g_CriticalSection);

	// Get index from queue
	Index = g_IndexQueue[g_QueueRear];
	g_QueueRear = (g_QueueRear + 1) % g_MaxCount;

	// Copy data to array
	CopyMemory(&g_Array[g_NodeSize * Index], Data, g_NodeSize);

	// Increment current count
	g_CurCount++;

	// Leave critical section
	LeaveCriticalSection(&g_CriticalSection);

	return Index;
}

// 
// Remove data by index
//
VOID
PeShield_ArrayRemove(
	IN DWORD Index
	)
{
	if (!g_bInitArray || PeShield_ArrayIsEmpty())
	{
		return;
	}

	// Enter critical section
	EnterCriticalSection(&g_CriticalSection);

	// Set data to zero
	ZeroMemory(&g_Array[g_NodeSize * Index], g_NodeSize);
	
	// Put index to queue
	g_IndexQueue[g_QueueFront] = Index;
	g_QueueFront = (g_QueueFront + 1) % g_MaxCount;

	// Decrement current count
	g_CurCount--;

	// Leave critical section
	LeaveCriticalSection(&g_CriticalSection);
}

//
// Free all data 
//
VOID
PeShield_ArrayFree()
{
	DWORD i;

	if (!g_bInitArray)
	{
		return;
	}
	
	// Enter critical section
	EnterCriticalSection(&g_CriticalSection);

	// Put all index
	for (i = 0; i < g_MaxCount; i++)
	{
		g_IndexQueue[i] = i;
	}

	// Set queue front and read to zero
	g_QueueFront = g_QueueRear = 0;

	// Set array to zero
	ZeroMemory(g_Array, g_NodeSize * g_MaxCount);
	
	// Set current count to zero
	g_CurCount = 0;

	// Leave critical section
	LeaveCriticalSection(&g_CriticalSection);
}


//----------------------------------------------------------------------
//			A  R  R  A  Y      I  N  F  O  R  M  A  T  I  O  N
//----------------------------------------------------------------------

//
// Get array object
//
CONST PVOID
PeShield_ArrayObject()
{
	if (!g_bInitArray)
	{
		return NULL;
	}
	
	return g_Array;
}

//
// Get data by index
//
CONST PVOID
PeShield_ArrayData(
	IN DWORD index
	)
{
	if (!g_bInitArray)
	{
		return NULL;
	}

	return &g_Array[index * g_NodeSize];
}

//
// Get current count
//
DWORD
PeShield_ArrayCount()
{
	if (!g_bInitArray)
	{
		return 0xFFFFFFFF;
	}

	return g_CurCount;
}

//
// Is array full ?
//
BOOL
PeShield_ArrayIsFull()
{
	if (!g_bInitArray)
	{
		return FALSE;
	}

	return (g_CurCount >= g_MaxCount);
}

//
// Is array empty ?
//
BOOL
PeShield_ArrayIsEmpty()
{
	if (!g_bInitArray)
	{
		return TRUE;
	}

	return (!g_CurCount);
}
