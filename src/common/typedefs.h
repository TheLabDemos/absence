#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

typedef char int8;
typedef short int16;
typedef long int32;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

#ifdef _MSC_VER
typedef __int64 int64;
typedef unsigned __int64 uint64;
#else
typedef unsigned long long uint64;
typedef long long int64;
#endif	// _MSC_VER

typedef uint8	BYTE;
typedef uint16	WORD;
typedef uint32	DWORD;

typedef uint8	byte;
typedef uint16	word;
typedef uint32	dword;
typedef uint64	qword;

// Define NULL pointer value 
#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif	//__cplusplus
#endif	//NULL

#endif	//_TYPEDEFS_H_
