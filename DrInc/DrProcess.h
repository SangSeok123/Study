/*
	linux on win32 emulation
*/

//--------------------------------------------------------------------
//Enviroment conditional
#ifndef	_DrProcess
#define	_DrProcess

//====================================================================
// uses

#include <DrPlatform.h>

#ifdef LINUX_APP
	#include	<pthread.h>
#endif
	
//====================================================================
//constance


//====================================================================
//global macro

//====================================================================
//global type
#ifdef LINUX_APP
	#define Def_DrPrcThrdId		pthread_t
	typedef pthread_mutex_t		St_DrPrcCritSct ,*Ptr_DrPrcCritSct;
	typedef pthread_rwlock_t St_DrPrcSrwLock, * Ptr_DrPrcSrwLock;
#else
	#define Def_DrPrcThrdId		__u32
	typedef CRITICAL_SECTION	St_DrPrcCritSct ,*Ptr_DrPrcCritSct;
	typedef SRWLOCK St_DrPrcSrwLock, * Ptr_DrPrcSrwLock;
#endif

typedef	struct St_DrPrcThrd 
{
	#ifdef LINUX_APP
		pthread_t		Hndl;
	#else
		HANDLE		HndlThrd;
		__u32			ThrdId;
	#endif
}	*Ptr_DrPrcThrd;

#ifdef LINUX_APP
	#define		Def_DrPrcThrdRslt		void*
	#define		Def_DrPrcThrdClcb		
	#define		Def_DrPrcThrdPara		void*
	typedef		void*	(*Fnc_DrPrcThrdProc)( void* A_Para );
#else
	#define		Def_DrPrcThrdRslt		DWORD	
	#define		Def_DrPrcThrdClcb		WINAPI	
	#define		Def_DrPrcThrdPara		LPVOID
	typedef		DWORD (WINAPI *Fnc_DrPrcThrdProc)( LPVOID A_Para );
#endif

//====================================================================
//class

//====================================================================
//global function

void		DrPrcSleep( __u32 A_Mili );

BOOL8		DrPrcThrdCreate( Ptr_DrPrcThrd A_PtrMng, Fnc_DrPrcThrdProc A_Proc, Def_DrPrcThrdPara A_Para );
Def_DrPrcThrdRslt		DrPrcThrdRelease( Ptr_DrPrcThrd A_PtrMng );
BOOL8		DrPrcThrdTerminate( Ptr_DrPrcThrd A_PtrMng );

Def_DrPrcThrdId	DrPrcThrdGetIdByMng( Ptr_DrPrcThrd A_PtrMng );
Def_DrPrcThrdId	DrPrcThrdGetCurId( void );
BOOL8		DrPrcThrdIsEqual( Def_DrPrcThrdId A_ThrdA, Def_DrPrcThrdId A_ThrdB );

BOOL8		DrPrcCritInit( Ptr_DrPrcCritSct A_PtrCrit );
BOOL8		DrPrcCritDel( Ptr_DrPrcCritSct A_PtrCrit );
BOOL8		DrPrcCritEnter( Ptr_DrPrcCritSct A_PtrCrit );
BOOL8		DrPrcCritTryEnter( Ptr_DrPrcCritSct A_PtrCrit );
BOOL8		DrPrcCritLeave( Ptr_DrPrcCritSct A_PtrCrit );

void DrPrcSrwLockInit(Ptr_DrPrcSrwLock A_PtrLock);
void DrPrcSrwLockDel(Ptr_DrPrcSrwLock A_PtrLock);
void DrPrcSrwLockEnterRead(Ptr_DrPrcSrwLock A_PtrLock);
void DrPrcSrwLockLeaveRead(Ptr_DrPrcSrwLock A_PtrLock);
void DrPrcSrwLockEnterWrite(Ptr_DrPrcSrwLock A_PtrLock);
void DrPrcSrwLockLeaveWrite(Ptr_DrPrcSrwLock A_PtrLock);


//====================================================================
//global variable

//====================================================================

//out Enviroment conditional
#endif


