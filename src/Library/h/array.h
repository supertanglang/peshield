//----------------------------------------------------------------------
//			     A  R  R  A  Y       L  I  B  R  A  R  Y
//----------------------------------------------------------------------
// Project		: Pe Shield
// Duration		: 23.OCT.2006 - 22.JAN.2007 
// Author		: Will
// Copyright	: Senol (t) 2006 - 2007 All Right Reserved				
//----------------------------------------------------------------------

#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <windows.h>

//----------------------------------------------------------------------
//				I  N  I  T  I  A  L  I  Z  A  T  I  O  N
//----------------------------------------------------------------------

//
// Initialize Pe Shield Array data
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_ArrayStartup(
	IN CONST PVOID Array,
	IN DWORD Size,
	IN DWORD Count
	);

//
// Uninitialize Pe Shield Array data
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID
PeShield_ArrayCleanup();


//----------------------------------------------------------------------
//			 A  R  R  A  Y      M  A  N  A  G  E  M  E  N  T
//----------------------------------------------------------------------

// 
// Insert data, returns index
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
DWORD
PeShield_ArrayInsert(
	IN PVOID Data
	);

// 
// Remove data by index
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID
PeShield_ArrayRemove(
	IN DWORD Index
	);

//
// Free all data 
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
VOID
PeShield_ArrayFree();


//----------------------------------------------------------------------
//			A  R  R  A  Y      I  N  F  O  R  M  A  T  I  O  N
//----------------------------------------------------------------------

//
// Get array object
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
CONST PVOID
PeShield_ArrayObject();

//
// Get data by index
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
CONST PVOID
PeShield_ArrayData(
	IN DWORD index
	);

//
// Get current count
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
DWORD
PeShield_ArrayCount();

//
// Is array full ?
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_ArrayIsFull();

//
// Is array empty ?
//
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
BOOL
PeShield_ArrayIsEmpty();


#endif // _ARRAY_H_