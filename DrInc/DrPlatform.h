/*
	Linux on Win32 header

*/

//--------------------------------------------------------------------
//Environment conditional
#ifndef	_EPC_DrPlatform
#define	_EPC_DrPlatform

//====================================================================
// uses

#ifndef LINUX_APP

#if 1

#ifdef DEBUG

#define	new	DEBUG_NEW

#endif

#endif

#if 0

#ifdef _CRTDBG_MAP_ALLOC
	#include <stdlib.h> 
	#include <crtdbg.h>
	#define DEBUG_NEW new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif

#endif

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#endif

#endif

#define	DR_TASKMSG_DEBUG

//====================================================================
//constance

#ifdef	LINUX_APP
	#ifdef	WIN32
		#define		__GNUC__	4
		#define		__GNUC_MINOR__	1
		#define		_GNU_SOURCE
		#define		__extension__
		#define		__USE_BSD

		#define		__signed__	signed
		#define		__attribute__( A_Para )		
		#define		__inline__
		#define		inline
		#define		__value
		#define		__asm__( A_Para )
		#define		__volatile__
		#define		__builtin_expect
		#define		__typeof__( A_Type )	A_Type	
		#define		__const	const

		#define		__STDC__
	#else
		#ifdef LINUX_EMU
			#define		inline
		#endif
	#endif		//ifdef		WIN32

#else

#ifdef WIN32

#endif

#ifdef _WIN64

#define	DR_64BIT

#endif

#ifdef DR_MFC_USE
#include <afx.h>
#else
	#include <windows.h>
#endif

#endif


//====================================================================
//global macro

//====================================================================
//global type


//====================================================================
//global function

//====================================================================
//out Environment conditional

#endif		//#ifndef	_EPC_DrPlatform

