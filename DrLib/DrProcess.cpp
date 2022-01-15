/*
	process library

*/

#include	<DrPlatform.h>
#include	<DrTypeBase.h>
#include	<DrProcess.h>

#ifdef		LINUX_APP
	#include	<unistd.h>
	#include	<sched.h>
#endif

//====================================================================
//global var


//====================================================================
//functions

//--------------------------------------------------------------------
void		DrPrcSleep( __u32 A_Mili )
{
	#ifdef LINUX_APP
		if ( 1000 <= A_Mili )
		{
			sleep( (A_Mili / 1000) );
		}
		else if ( 0 != A_Mili )
		{
			usleep( (A_Mili * 1000) );
		}
		else
		{
			usleep( 1 );
		}

	#else
		Sleep( A_Mili );
	#endif
}
//--------------------------------------------------------------------
BOOL8		DrPrcThrdCreate( Ptr_DrPrcThrd A_PtrMng, Fnc_DrPrcThrdProc A_Proc, Def_DrPrcThrdPara A_Para )
{
	// local -------------------
		BOOL8		Tv_Result;
	// code --------------------
		//init
		Tv_Result		=	FALSE;

		#ifdef LINUX_APP
			if ( 0 <= pthread_create( &A_PtrMng->Hndl, NULL, A_Proc, A_Para ) )
			{
				Tv_Result		=	TRUE;
			}
		#else
			A_PtrMng->HndlThrd	=	CreateThread( NULL, 0, A_Proc, A_Para, 0, (LPDWORD)&A_PtrMng->ThrdId );
			if ( NULL != A_PtrMng->ThrdId )			
			{
				Tv_Result		=	TRUE;
			}
		#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
Def_DrPrcThrdRslt		DrPrcThrdRelease( Ptr_DrPrcThrd A_PtrMng )
{
	// local -------------------
		Def_DrPrcThrdRslt		Tv_Result;
	// code --------------------
		#ifdef LINUX_APP
			pthread_join( A_PtrMng->Hndl, &Tv_Result );
		#else
			Tv_Result	=	0;
			if ( WAIT_OBJECT_0 == WaitForSingleObject( A_PtrMng->HndlThrd, INFINITE ) )
			{
				GetExitCodeThread( A_PtrMng->HndlThrd, (LPDWORD)&Tv_Result );
				CloseHandle( A_PtrMng->HndlThrd );
			}
		#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrPrcThrdTerminate( Ptr_DrPrcThrd A_PtrMng )
{
	// local -------------------
		BOOL8		Tv_Result;
		#ifdef LINUX_APP
			Def_DrPrcThrdRslt		Tv_Rval;
		#endif
	// code --------------------
		//init
		Tv_Result		=	FALSE;

		#ifdef LINUX_APP
			pthread_cancel( A_PtrMng->Hndl );
			pthread_join( A_PtrMng->Hndl, &Tv_Rval );
			Tv_Result		=	TRUE;
		#else
			Tv_Result		=	TerminateThread( A_PtrMng->HndlThrd, 0 );
		#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
Def_DrPrcThrdId	DrPrcThrdGetIdByMng( Ptr_DrPrcThrd A_PtrMng )
{
		#ifdef LINUX_APP
			return	A_PtrMng->Hndl;
		#else
			return	A_PtrMng->ThrdId;
		#endif
}
//--------------------------------------------------------------------
Def_DrPrcThrdId	DrPrcThrdGetCurId( void )
{
		#ifdef LINUX_APP
			return	pthread_self();
		#else
			return	GetCurrentThreadId();
		#endif
}
//--------------------------------------------------------------------
BOOL8	DrPrcThrdIsEqual( Def_DrPrcThrdId A_ThrdA, Def_DrPrcThrdId A_ThrdB )
{
	// local -------------------
		BOOL8		Tv_Result;
	// code --------------------
		//init
		Tv_Result		=	FALSE;

		#ifdef LINUX_APP
			if ( 0 != pthread_equal( A_ThrdA, A_ThrdB ) )
			{
				Tv_Result		=	TRUE;
			}
		#else
			if ( A_ThrdA == A_ThrdB )
			{
				Tv_Result	=	TRUE;
			}
		#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrPrcCritInit( Ptr_DrPrcCritSct A_PtrCrit )
{
	// local -------------------
		BOOL8		Tv_Result;
	// code --------------------
		//init
		#ifdef LINUX_APP
			Tv_Result		=	FALSE;
			if ( 0 == pthread_mutex_init( (pthread_mutex_t*)A_PtrCrit, NULL ) )
			{
				Tv_Result		=	TRUE;
			}
		#else
			InitializeCriticalSection( (LPCRITICAL_SECTION)A_PtrCrit );
			Tv_Result		=	TRUE;
		#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrPrcCritDel( Ptr_DrPrcCritSct A_PtrCrit )
{
	// local -------------------
		BOOL8		Tv_Result;
	// code --------------------
		//init
		#ifdef LINUX_APP
			Tv_Result		=	FALSE;
			if ( 0 == pthread_mutex_destroy( (pthread_mutex_t*)A_PtrCrit ) )
			{
				Tv_Result	=	TRUE;
			}
		#else
			DeleteCriticalSection( (LPCRITICAL_SECTION)A_PtrCrit );
			Tv_Result	=	TRUE;
		#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrPrcCritEnter( Ptr_DrPrcCritSct A_PtrCrit )
{
	// local -------------------
		BOOL8		Tv_Result;
	// code --------------------
		#ifdef	LINUX_APP
			Tv_Result		=	FALSE;
			if ( 0 == pthread_mutex_lock( (pthread_mutex_t*)A_PtrCrit ) )
			{
				Tv_Result		=	TRUE;
			}
		#else
			EnterCriticalSection( (LPCRITICAL_SECTION)A_PtrCrit );
			Tv_Result		=	TRUE;
		#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrPrcCritTryEnter( Ptr_DrPrcCritSct A_PtrCrit )
{
	// local -------------------
		BOOL8		Tv_Result;
	// code --------------------
		Tv_Result		=	FALSE;
		#ifdef	LINUX_APP
			if ( 0 == pthread_mutex_trylock( (pthread_mutex_t*)A_PtrCrit ) )
			{
				Tv_Result		=	TRUE;
			}
		#else
			if ( TryEnterCriticalSection( (LPCRITICAL_SECTION)A_PtrCrit ) )
			{
				Tv_Result		=	TRUE;
			}
		#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrPrcCritLeave( Ptr_DrPrcCritSct A_PtrCrit )
{
	// local -------------------
		BOOL8		Tv_Result;
	// code --------------------
		#ifdef	LINUX_APP
			Tv_Result		=	FALSE;
			if ( 0 == pthread_mutex_unlock( (pthread_mutex_t*)A_PtrCrit ) )
			{
				Tv_Result		=	TRUE;
			}
		#else
			LeaveCriticalSection( (LPCRITICAL_SECTION)A_PtrCrit );
			Tv_Result		=	TRUE;
		#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
void DrPrcSrwLockInit(Ptr_DrPrcSrwLock A_PtrLock)
{
#ifdef LINUX_APP
	pthread_rwlock_init(A_PtrLock,NULL);
#else
	InitializeSRWLock(A_PtrLock);
#endif
}
//--------------------------------------------------------------------
void DrPrcSrwLockDel(Ptr_DrPrcSrwLock A_PtrLock)
{
#ifdef LINUX_APP
	pthread_rwlock_destroy(A_PtrLock);
#else

#endif
}
//--------------------------------------------------------------------
void DrPrcSrwLockEnterRead(Ptr_DrPrcSrwLock A_PtrLock)
{
#ifdef LINUX_APP
	pthread_rwlock_rdlock(A_PtrLock);
#else
	AcquireSRWLockShared(A_PtrLock);
#endif
}
//--------------------------------------------------------------------
void DrPrcSrwLockLeaveRead(Ptr_DrPrcSrwLock A_PtrLock)
{
#ifdef LINUX_APP
	pthread_rwlock_unlock(A_PtrLock);
#else
	ReleaseSRWLockShared(A_PtrLock);
#endif
}
//--------------------------------------------------------------------
void DrPrcSrwLockEnterWrite(Ptr_DrPrcSrwLock A_PtrLock)
{
#ifdef LINUX_APP
	pthread_rwlock_wrlock(A_PtrLock);
#else
	AcquireSRWLockExclusive(A_PtrLock);
#endif
}
//--------------------------------------------------------------------
void DrPrcSrwLockLeaveWrite(Ptr_DrPrcSrwLock A_PtrLock)
{
#ifdef LINUX_APP
	pthread_rwlock_unlock(A_PtrLock);
#else
	ReleaseSRWLockExclusive(A_PtrLock);
#endif
}
//--------------------------------------------------------------------

