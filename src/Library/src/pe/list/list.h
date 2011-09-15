#ifndef _LIST_H_
#define _LIST_H_

#include <windows.h>

//
// Pe Shield list node structure
//
typedef struct _PESHIELD_LISTNODE
{
	PVOID						Data;
	struct _PESHIELD_LISTNODE*	Next;
}PESHIELD_LISTNODE, *PPESHIELD_LISTNODE;


//----------------------------------------------------------------------
//				I  N  I  T  I  A  L  I  Z  A  T  I  O  N
//----------------------------------------------------------------------

//
// Initialize Pe Shield list data
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_ListStartup();

//
// Uninitialize Pe Shield list data
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID
PeShield_ListCleanup();

//----------------------------------------------------------------------
//							   P  U  S  H
//----------------------------------------------------------------------

//
// Push a node back
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_ListPushBack(
	IN PVOID Data
	);

//
// Push a node front
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL 
PeShield_ListPushFront(
	IN PVOID Data
	);


//----------------------------------------------------------------------
//								 P  O  P
//----------------------------------------------------------------------

//
// Pop a node back
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
PVOID
PeShield_ListPopBack();

//
// Pop a node front
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
PVOID
PeShield_ListPopFront();

//
// Pop all nodes
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID
PeShield_ListPopAll();


//----------------------------------------------------------------------
//								 G  E  T
//----------------------------------------------------------------------

// 
// Get a node back
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
PVOID
PeShield_ListGetBack();

//
// Get a bide front
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
PVOID
PeShield_ListGetFront();


//----------------------------------------------------------------------
//					 I  N  F  O  R  M  A  T  I  O  N
//----------------------------------------------------------------------

//
// Get curretn node count
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
DWORD
PeShield_ListGetCount();

//
// Is list empty ?
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_ListIsEmpty();


//----------------------------------------------------------------------
//							 D  E  B  U  G
//----------------------------------------------------------------------

#ifdef _DEBUG

//
// Print all nodes in console window
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_ListPrintf();

#endif // _DEBUG


#endif // _LIST_H_