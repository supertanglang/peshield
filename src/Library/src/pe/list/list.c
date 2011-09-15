#include "list.h"

//----------------------------------------------------------------------
//			 G  L  O  B  A  L      V  A  R  I  A  B  L  E  S
//----------------------------------------------------------------------

//
// List head node
//
static PPESHIELD_LISTNODE	g_Head;

//
// List count
//
static DWORD				g_Count;


//----------------------------------------------------------------------
//				I  N  I  T  I  A  L  I  Z  A  T  I  O  N
//----------------------------------------------------------------------

//
// Initialize list data
//
BOOL
PeShield_ListStartup()
{
	// Allocate head node
	g_Head = (PPESHIELD_LISTNODE)malloc(sizeof(PESHIELD_LISTNODE));
	if (!g_Head)
	{
		return FALSE;
	}

	// Set head values to zero ( tail is null )
	g_Head->Data = NULL;
	g_Head->Next = NULL;

	return TRUE;
}

//
// Uninitialize list data
//
VOID
PeShield_ListCleanup()
{
	// Get rid of all nodes
	PeShield_ListPopAll();

	// Free head node
	if (g_Head)
	{
		free(g_Head);
		g_Head = NULL;
	}
}


//----------------------------------------------------------------------
//							   P  U  S  H
//----------------------------------------------------------------------

//
// Push a node back
//
BOOL
PeShield_ListPushBack(
	IN PVOID Data
	)
{
	PPESHIELD_LISTNODE	newNode;
	PPESHIELD_LISTNODE	curNode;

	// Is list initialized ?
	if (!g_Head)
	{
		return FALSE;
	}

	// Allocate new node
	newNode = (PPESHIELD_LISTNODE)malloc(sizeof(PESHIELD_LISTNODE));
	if (!newNode)
	{
		return FALSE;
	}

	// Find place just before tail
	curNode = g_Head;
	while (curNode->Next)
	{
		// Move to next
		curNode = curNode->Next;
	}

	// Set data and link it next to head
	newNode->Data = Data;
	newNode->Next = curNode->Next;
	curNode->Next = newNode;

	// Add list count
	g_Count++;

	return TRUE;
}

//
// Push a node front
//
BOOL 
PeShield_ListPushFront(
	IN PVOID Data
	)
{
	PPESHIELD_LISTNODE	newNode;

	// Is list initialized ?
	if (!g_Head)
	{
		return FALSE;
	}

	// Allocate new node
	newNode = (PPESHIELD_LISTNODE)malloc(sizeof(PESHIELD_LISTNODE));
	if (!newNode)
	{
		return FALSE;
	}

	// Set data and link it next to head
	newNode->Data = Data;
	newNode->Next = g_Head->Next;
	g_Head->Next = newNode;

	// Add list count
	g_Count++;

	return TRUE;
}


//----------------------------------------------------------------------
//								 P  O  P
//----------------------------------------------------------------------

//
// Pop a node back
//
PVOID
PeShield_ListPopBack()
{
	PPESHIELD_LISTNODE	popNode;
	PPESHIELD_LISTNODE	prevNode;
	PVOID				popData;

	// Is list initialized ?
	if (!g_Head || !g_Count)
	{
		return NULL;
	}

	// Find place just before tail
	popNode = g_Head;
	while (popNode->Next)
	{
		// Save current position
		prevNode = popNode;

		// Move to next
		popNode = popNode->Next;
	}

	// Save data to return
	popData = popNode->Data;

	// Change link
	prevNode->Next = popNode->Next;

	// Free node
	free(popNode);

	// Subtract list count
	g_Count--;

	return popData;
}

//
// Pop a node front
//
PVOID
PeShield_ListPopFront()
{
	PPESHIELD_LISTNODE	popNode;
	PVOID				popData;

	// Is list initialized ?
	if (!g_Head || !g_Count)
	{
		return NULL;
	}
	
	// Get node to pop
	popNode = g_Head->Next;

	// Save data to return
	popData = popNode->Data;

	// Change link
	g_Head->Next = popNode->Next;

	// Free node
	free(popNode);

	// Subtract list count
	g_Count--;

	return popData;
}

//
// Pop all nodes
//
VOID
PeShield_ListPopAll()
{
	PPESHIELD_LISTNODE	popNode;
	PPESHIELD_LISTNODE	savedNode;

	// Is list initialized ?
	if (!g_Head || !g_Count)
	{
		return;
	}

	// Free all nodes by touching tail
	popNode = g_Head->Next;
	while (popNode)
	{
		// Save current position
		savedNode = popNode;

		// Move to next
		popNode = popNode->Next;

		// Free saved node
		free(savedNode);
	}

	// Set head node next pointer
	g_Head->Next = NULL;

	// Set list count to zero
	g_Count = 0;
}


//----------------------------------------------------------------------
//								 G  E  T
//----------------------------------------------------------------------

// 
// Get a node back
//
PVOID
PeShield_ListGetBack()
{
	PPESHIELD_LISTNODE	getNode;

	// Is list initialized ?
	if (!g_Head || !g_Count)
	{
		return NULL;
	}

	// Find place just before tail
	getNode = g_Head;
	while (getNode->Next)
	{
		// Move to next
		getNode = getNode->Next;
	}

	return getNode->Data;	
}

//
// Get a bide front
//
PVOID
PeShield_ListGetFront()
{
	PPESHIELD_LISTNODE	getNode;

	// Is list initialized ?
	if (!g_Head || !g_Count)
	{
		return NULL;
	}

	// Get node to get
	getNode = g_Head->Next;

	return getNode->Data;
}


//----------------------------------------------------------------------
//					 I  N  F  O  R  M  A  T  I  O  N
//----------------------------------------------------------------------

//
// Get curretn node count
//
DWORD
PeShield_ListGetCount()
{
	return g_Count;
}

//
// Is list empty ?
//
BOOL
PeShield_ListIsEmpty()
{
	// Is list initialized ?
	if (!g_Head)
	{
		return TRUE;
	}

	return (!g_Count);
}


//----------------------------------------------------------------------
//							 D  E  B  U  G
//----------------------------------------------------------------------

#ifdef _DEBUG

#include <stdio.h>

//
// Print all nodes in console window
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_ListPrintf()
{
	PPESHIELD_LISTNODE	curNode;

	// Is list initialized ?
	if (!g_Head)
	{
		return FALSE;
	}

	// Print list count
	printf("[ Pe Shield List ] current count : %d\n", g_Count);

	// Print all nodes
	curNode = g_Head->Next;
	while (curNode)
	{
		// Print data
		printf("%08X\n", curNode->Data);

		// Move to next
		curNode = curNode->Next;
	}

	return TRUE;
}

#endif // _DEBUG
