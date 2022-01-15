/*
 time Tools

 linux - nt time query
	38year = 13879day = 1,198,368,000 sec = 0x476DA500
	bacause Nttimebase = 0x2FD8A1380ULL - 0x476DA500 = 0x2B61C6E80

 */


//====================================================================
// uses
#include	<DrPlatform.h>

#ifdef	LINUX_APP
	#include	<sys/time.h>
	#include	<unistd.h>
	#include	<fcntl.h>
	#include	<sys/stat.h>
	#include	<sys/ioctl.h>
	#include	<time.h>
	#include	<linux/rtc.h>
	#include	<sys/times.h>
#else
	#include	<winternl.h>

#if 0
	typedef	NTSTATUS	(__stdcall *Fnc_NtQuerySystemTime)	( PLARGE_INTEGER SystemTime );
#endif

#endif

#include <DrTypeBase.h>
#include <DrMath.h>
#include <DrTimeTool.h>
#include <DrDumyTool.h>
#include <DrThrdBase.h>


//====================================================================
//local const

#define	E_DrTimeModeFileTime	/* file time mode */

//#define E_DrTimeNttimeBase	0x2B61C6E80ULL
#define E_DrTimeNttimeBase	0x2B6109100ULL

#ifdef DR_TICK_MUL
#define E_DrTimeTickMul			DR_TICK_MUL
#else
#define E_DrTimeTickMul			10
#endif


//====================================================================
//local type

typedef	struct St_DrTimeSmItm
{
	__u32	Low;
	__u32	High;
}	*Ptr_DrTimeSmItm;

typedef	struct	St_DrTimeLimDh		//time limit day - hour
{
	__u8		Hour;					// hour
	__u8		Week;					// week th
	__u8		Day;					// day
	__u8		Month;				// month
}	*Ptr_DrTimeLimDh;

typedef	struct St_DrTimeMode 
{
	BOOL8	IsSmUse;
	BOOL8	IsSmDst;

	struct St_DrTimeLimDh	SmStTime;		// DST start time
	struct St_DrTimeLimDh	SmEdTime;		// DST end time

}	*Ptr_DrTimeMode;

typedef struct St_DrTimeDstRtl
{
	BOOL8	IsSm;
	__u8	_rsvd0;
	__u8	_rsvd1;
	__u8	_rsvd2;
	__u32	TimeUpdt;
	St_DrTimeSmItm	SmTbl[E_DrTimeSmTblCnt];		// DST check table
}	*Ptr_DrTimeDstRtl;

typedef struct St_DrTimeRtl
{
	St_DrTimeDstRtl	DstRtl;
}	*Ptr_DrTimeRtl;

//====================================================================
//local function

void	DrTimeDstRtlBuild( Def_DrTime A_BaseTime, struct St_DrTimeDstRtl* A_PtrRtRtl );
Def_DrTime	DrTimeGetByOs( __u32* A_PtrRtMili );
void	DrTimeDstRtlUpdtSm( struct St_DrTimeDstRtl* A_PtrRtRtl );

//====================================================================
//var
	__u8	V_DrTimeDayOfMonTbl[12]			=	{31,28,31,30,31,30,31,31,30,31,30,31};		// day count table
	__u16	V_DrTimeDaySumOfMonTbl[12]	=	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };		// day count table

	Def_DrTime	V_DrTimeSecOfMonTbl[12]	=	
	{31*E_DrTimeSecOfDay,28*E_DrTimeSecOfDay,31*E_DrTimeSecOfDay,30*E_DrTimeSecOfDay,31*E_DrTimeSecOfDay,30*E_DrTimeSecOfDay,
	31*E_DrTimeSecOfDay,31*E_DrTimeSecOfDay,30*E_DrTimeSecOfDay,31*E_DrTimeSecOfDay,30*E_DrTimeSecOfDay,31*E_DrTimeSecOfDay};
#ifndef	LINUX_APP

#ifndef E_DrTimeModeFileTime
		BOOL8	V_DrTimeIsGetTimeInited	=	FALSE;
		//expansion function
		Fnc_NtQuerySystemTime	V_DrTimeFncNtQuerySystemTime = NULL;
		__u64	V_DrTimeTimeInited	=	E_DrTimeNttimeBase;
#endif

#ifdef LINUX_EMU
		__u32	V_DrTimeEmuGap	=	0;
#endif

#endif

#if defined(LINUX_APP) || defined(LINUX_EMU)
	struct St_DrTimeMode V_DrTimeMode	=	{ FALSE, FALSE };

	struct St_DrTimeRtl V_DrTimeRtl;
#endif

//====================================================================
//--------------------------------------------------------------------
BOOL8		DrTimeIsLeapYear( __u16 A_Year )
{
	// local -------------------
		BOOL8		Tv_Result;
	// code --------------------
		//init
		Tv_Result		=	FALSE;
		if ( 0 == (A_Year & 3) )
		{
			Tv_Result		=	TRUE;
		}
		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32		DrTimeDayCntBySysTime( Def_DrTime A_Time )
{
		return	A_Time / E_DrTimeSecOfDay;
}
//--------------------------------------------------------------------
Def_DrTime	DrTimeTimeToSysTime( Def_DrTime A_Time, BOOL8 A_IsSm )
{
	// local -------------------
		Def_DrTime	Tv_Result;
	// code --------------------
		Tv_Result	=	A_Time;
		if ( A_IsSm )
		{
			Tv_Result	=	Tv_Result - E_DrTimeSecOfHour;
		}
		return	Tv_Result;
}
//--------------------------------------------------------------------
Def_DrTime	DrTimeSysTimeToTime( Def_DrTime A_SysTime, BOOL8* A_PtrRtIsSm )
{
	// local -------------------
		Def_DrTime	Tv_Result;
	// code --------------------
		Tv_Result			=	A_SysTime;
		*A_PtrRtIsSm	=	FALSE;
		//check summer time
		if ( DrTimeIsSmTime(Tv_Result) )
		{
			*A_PtrRtIsSm	=	TRUE;
			Tv_Result			=	Tv_Result + E_DrTimeSecOfHour;
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrTimeIsSmTime( Def_DrTime A_SysTime )
{
	// local -------------------
		BOOL8		Tv_Result;
#if defined(LINUX_APP) || defined(LINUX_EMU)
		__u32		Tv_WkIdx;
		struct St_DrTimeDstRtl	Tv_DstRtl;
#else
		ULARGE_INTEGER	Tv_Tm64;
		_FILETIME		Tv_TmFl;
		_FILETIME		Tv_TmLfl;
		SYSTEMTIME	Tv_TmSys;
		SYSTEMTIME	Tv_TmLc;
		SYSTEMTIME	Tv_TmZlc;
#endif
	// code --------------------
		//init
		Tv_Result	=	FALSE;
		
#if defined(LINUX_APP) || defined(LINUX_EMU)
		if ( V_DrTimeMode.IsSmUse )
		{
			// check in hour range
			if ( (A_SysTime >= V_DrTimeRtl.DstRtl.TimeUpdt) && (A_SysTime < (V_DrTimeRtl.DstRtl.TimeUpdt + E_DrTimeSecOfHour) ) )
			{
				Tv_Result	=	V_DrTimeRtl.DstRtl.IsSm;
			}
			else
			{
				// check cached summer time
				if ( (A_SysTime >= V_DrTimeRtl.DstRtl.SmTbl[0].Low) && (A_SysTime <= V_DrTimeRtl.DstRtl.SmTbl[E_DrTimeSmTblCnt-1].High) )
				{
					// check by cached
					for ( Tv_WkIdx=0;Tv_WkIdx < E_DrTimeSmTblCnt;Tv_WkIdx++ )
					{
						if ( (V_DrTimeRtl.DstRtl.SmTbl[Tv_WkIdx].Low <= A_SysTime) && (V_DrTimeRtl.DstRtl.SmTbl[Tv_WkIdx].High > A_SysTime ) )
						{
							//in summer time
							Tv_Result	=	TRUE;
							break;
						}
					}
				}
				else
				{
					// make 
					DrTimeDstRtlBuild( A_SysTime, &Tv_DstRtl );
					Tv_Result	=	Tv_DstRtl.IsSm;
				}

			}
		}
#else
		Tv_Tm64.QuadPart	=	(((ULONGLONG)A_SysTime) + E_DrTimeNttimeBase) * 10000000;
		Tv_TmLfl.dwLowDateTime		=	Tv_Tm64.LowPart;
		Tv_TmLfl.dwHighDateTime		=	Tv_Tm64.HighPart;

		if(FileTimeToSystemTime(&Tv_TmLfl,&Tv_TmLc))
		{
			if(LocalFileTimeToFileTime(&Tv_TmLfl, &Tv_TmFl))
			{
				if(FileTimeToSystemTime(&Tv_TmFl, &Tv_TmSys))
				{
					if(SystemTimeToTzSpecificLocalTime(NULL, &Tv_TmSys, &Tv_TmZlc))
					{
						if((Tv_TmZlc.wHour != Tv_TmLc.wHour) || (Tv_TmZlc.wMinute != Tv_TmLc.wMinute))
						{
							Tv_Result	=	TRUE;
						}
					}
				}
			}
		}

#endif
		
		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrTimeDayCntByDate( __u16 A_Year, __u8 A_Month, __u8 A_Day )
{
	// local -------------------
		__u32			Tv_Result;
		__u32			Tv_Year;
	// code --------------------
		//calculate day count
		//calculate year
		Tv_Year		=	A_Year - E_DrTimeBaseYear;
		Tv_Result	=	(Tv_Year * 365);
		if(2 < Tv_Year)
		{
			Tv_Result	=	Tv_Result + ((Tv_Year - 2) >> 2);
		}
		
		if(0 != (0x03 & A_Year))
		{
			Tv_Result ++;
		}

		//calculate month
		if ( (1 <= A_Month) && (12 >= A_Month) )
		{
			Tv_Result		=	Tv_Result + (__u32)	V_DrTimeDaySumOfMonTbl[A_Month - 1];
		}
		//check leap
		if ( (2 < A_Month ) && DrTimeIsLeapYear( A_Year ) )
		{
			Tv_Result ++;
		}

		//add day
		Tv_Result	=	Tv_Result + A_Day - 1;
		
		return	Tv_Result;
}
//--------------------------------------------------------------------
Def_DrTime	DrTimeEnc(	__u16 A_Year, __u8 A_Month, __u8 A_Day, 
												__u8 A_Hour, __u8 A_Min, __u8 A_Sec/*, BOOL8* A_PtrRtIsSm*/ )
{
	// local -------------------
		Def_DrTime		Tv_SysTime;
#if defined(LINUX_APP) || defined(LINUX_EMU)
		__u32			Tv_Days;
#else
		SYSTEMTIME	Tv_TmSys;
		_FILETIME		Tv_TmFl;
		ULARGE_INTEGER	Tv_Tm64;
		__u64				Tv_NtMs;
#endif
	// code --------------------
#if defined(LINUX_APP) || defined(LINUX_EMU)
		//calculate day count
		Tv_Days		=	DrTimeDayCntByDate( A_Year, A_Month, A_Day );
		Tv_SysTime	=	(Tv_Days * E_DrTimeSecOfDay) + (A_Hour * E_DrTimeSecOfHour ) + (A_Min * E_DrTimeSecOfMin) + A_Sec;
#else
		Tv_SysTime				=	0;

		Tv_TmSys.wYear		=	A_Year;
		Tv_TmSys.wMonth		=	A_Month;
		Tv_TmSys.wDay			=	A_Day;
		Tv_TmSys.wDayOfWeek	=	0;
		Tv_TmSys.wHour		=	A_Hour;
		Tv_TmSys.wMinute	=	A_Min;
		Tv_TmSys.wSecond	=	A_Sec;
		Tv_TmSys.wMilliseconds	=	0;

		if(SystemTimeToFileTime(&Tv_TmSys, &Tv_TmFl))
		{
			Tv_Tm64.LowPart		=	Tv_TmFl.dwLowDateTime;
			Tv_Tm64.HighPart	=	Tv_TmFl.dwHighDateTime;
			Tv_NtMs			=	(Tv_Tm64.QuadPart / 10000);
			Tv_SysTime	=	(Def_DrTime)((Tv_NtMs / 1000) - E_DrTimeNttimeBase);
		}

#endif

		return	Tv_SysTime;
}
//--------------------------------------------------------------------
void		DrTimeDec(	Def_DrTime A_Time, 
										__u16* A_PtrRtYear, __u8* A_PtrRtMonth, __u8* A_PtrRtDay,	
										__u8* A_PtrRtHour, __u8* A_PtrRtMin, __u8* A_PtrRtSec )
{
	// local -------------------
#if defined(LINUX_APP) || defined(LINUX_EMU)
		Def_DrTime	Tv_WkIdx;
		__u32	Tv_Remain;
		BOOL8	Tv_IsLeap;
		__u32	Tv_Tmp;
#else
		SYSTEMTIME	Tv_TmSys;
		_FILETIME		Tv_TmFl;
		_FILETIME		Tv_TmLfl;
		SYSTEMTIME	Tv_TmLc;
		ULARGE_INTEGER	Tv_Tm64;
#endif
	// code --------------------
		//init
		*A_PtrRtYear	=	E_DrTimeBaseYear;
		*A_PtrRtMonth	=	1;
		*A_PtrRtDay		=	1;
		*A_PtrRtHour	=	0;
		*A_PtrRtMin		=	0;
		*A_PtrRtSec		=	0;

#if defined(LINUX_APP) || defined(LINUX_EMU)

		// year
		Tv_IsLeap	=	FALSE;
		if((E_DrTimeSecOfYear * 2) > A_Time)
		{
			Tv_WkIdx	=	0;
			Tv_Remain	=	A_Time;
			if(E_DrTimeSecOfYear < A_Time)
			{
				Tv_WkIdx ++;
				Tv_Remain	=	Tv_Remain - E_DrTimeSecOfYear;
			}
			*A_PtrRtYear	=	E_DrTimeBaseYear + (__u16)Tv_WkIdx;
		}
		else
		{
			Tv_IsLeap	=	TRUE;
			Tv_Remain	=	A_Time - (E_DrTimeSecOfYear * 2);
			Tv_WkIdx	=	((Tv_Remain / E_DrTimeSecOf4year) * 4) + 2;
			Tv_Remain	=	Tv_Remain % E_DrTimeSecOf4year;
			if(E_DrTimeSecOfLeafYear <= Tv_Remain)
			{
				Tv_WkIdx ++;
				Tv_Remain	=	Tv_Remain - E_DrTimeSecOfLeafYear;
				Tv_IsLeap	=	FALSE;

				if(E_DrTimeSecOfYear <= Tv_Remain)
				{
					Tv_WkIdx ++;
					Tv_Remain	=	Tv_Remain - E_DrTimeSecOfYear;
				}
				if(E_DrTimeSecOfYear <= Tv_Remain)
				{
					Tv_WkIdx ++;
					Tv_Remain	=	Tv_Remain - E_DrTimeSecOfYear;
				}
			}
			*A_PtrRtYear	=	E_DrTimeBaseYear + (__u16)Tv_WkIdx;
		}

		// month
		for(Tv_WkIdx=0; Tv_WkIdx < 12; Tv_WkIdx++)
		{
			Tv_Tmp	=	V_DrTimeSecOfMonTbl[Tv_WkIdx];
			if(1 == Tv_WkIdx)
			{
				if(Tv_IsLeap)
				{
					Tv_Tmp	=	Tv_Tmp + E_DrTimeSecOfDay;
				}
			}
			//check month over
			if(Tv_Remain < Tv_Tmp)
			{
				break;
			}
			//next
			Tv_Remain	=	Tv_Remain - Tv_Tmp;
			(*A_PtrRtMonth)++;
		}

		// day
		Tv_Tmp		=	Tv_Remain / E_DrTimeSecOfDay;
		*A_PtrRtDay = (__u8)(Tv_Tmp + 1);
		Tv_Remain	=	Tv_Remain % E_DrTimeSecOfDay;

		// hour
		Tv_Tmp				=	Tv_Remain / E_DrTimeSecOfHour;
		Tv_Remain			=	Tv_Remain %E_DrTimeSecOfHour;
		*A_PtrRtHour	=	(__u8)Tv_Tmp;

		//min
		Tv_Tmp				=	Tv_Remain / E_DrTimeSecOfMin;
		*A_PtrRtMin		=	(__u8)Tv_Tmp;
		Tv_Remain			=	Tv_Remain % E_DrTimeSecOfMin;

		//calculate sec
		*A_PtrRtSec	=	(__u8)Tv_Remain;

#else
		Tv_Tm64.QuadPart	=	(((ULONGLONG)A_Time) + E_DrTimeNttimeBase) * 10000000;
		Tv_TmLfl.dwLowDateTime	=	Tv_Tm64.LowPart;
		Tv_TmLfl.dwHighDateTime	=	Tv_Tm64.HighPart;
		if(LocalFileTimeToFileTime(&Tv_TmLfl,&Tv_TmFl))
		{
			if(FileTimeToSystemTime(&Tv_TmFl, &Tv_TmSys))
			{
				if(SystemTimeToTzSpecificLocalTime(NULL, &Tv_TmSys, &Tv_TmLc))
				{
					// update
					*A_PtrRtYear	=	Tv_TmLc.wYear;
					*A_PtrRtMonth	=	(__u8)Tv_TmLc.wMonth;
					*A_PtrRtDay		=	(__u8)Tv_TmLc.wDay;
					*A_PtrRtHour	=	(__u8)Tv_TmLc.wHour;
					*A_PtrRtMin		=	(__u8)Tv_TmLc.wMinute;
					*A_PtrRtSec		=	(__u8)Tv_TmLc.wSecond;
				}
			}
		}

#endif

}
//--------------------------------------------------------------------
void		DrTimeDec(Def_DrTime A_Time, Ptr_DrDateTime A_PtrRtDateTime)
{
	DrTimeDec(A_Time, &A_PtrRtDateTime->Year, &A_PtrRtDateTime->Month, &A_PtrRtDateTime->Day,
		&A_PtrRtDateTime->Hour, &A_PtrRtDateTime->Min, &A_PtrRtDateTime->Sec);
	A_PtrRtDateTime->_rsvd0	=	0;
}
//--------------------------------------------------------------------
__u8		DrTimeGetWeekByTime( Def_DrTime A_Time )
{
	// local -------------------
		__u32		Tv_Result;
#if defined(LINUX_APP) || defined(LINUX_EMU)
		__u32		Tv_DayCnt;
#else
		SYSTEMTIME	Tv_TmSys;
		_FILETIME		Tv_TmFl;
		_FILETIME		Tv_TmLfl;
		SYSTEMTIME	Tv_TmLc;
		ULARGE_INTEGER	Tv_Tm64;
#endif
	// code --------------------
#if defined(LINUX_APP) || defined(LINUX_EMU)
		Tv_DayCnt	=	DrTimeDayCntBySysTime( A_Time );
		Tv_Result	=	(Tv_DayCnt + E_DrTimeBaseWeek) % 7;
#else
		Tv_Result	=	0;

		Tv_Tm64.QuadPart	=	(((ULONGLONG)A_Time) + E_DrTimeNttimeBase) * 10000000;
		Tv_TmLfl.dwLowDateTime		=	Tv_Tm64.LowPart;
		Tv_TmLfl.dwHighDateTime		=	Tv_Tm64.HighPart;
		if(LocalFileTimeToFileTime(&Tv_TmLfl,&Tv_TmFl))
		{
			if(FileTimeToSystemTime(&Tv_TmFl, &Tv_TmSys))
			{
				if(SystemTimeToTzSpecificLocalTime(NULL, &Tv_TmSys, &Tv_TmLc))
				{
					// update
					Tv_Result	=	(__u32)Tv_TmLc.wDayOfWeek;
				}
			}
		}

#endif
		
		return	(__u8)Tv_Result;
}
//--------------------------------------------------------------------
__u8		DrTimeGetWeekByDate( __u16 A_Year, __u8 A_Month, __u8 A_Day )
{
	// local -------------------
		__u32		Tv_Result;
#if defined(LINUX_APP) || defined(LINUX_EMU)
		__u32		Tv_DayCnt;
#else
		SYSTEMTIME	Tv_TmSys;
		//_FILETIME		Tv_TmFl;
		_FILETIME		Tv_TmLfl;
		SYSTEMTIME	Tv_TmLc;
#endif
	// code --------------------
#if defined(LINUX_APP) || defined(LINUX_EMU)
		Tv_DayCnt	=	DrTimeDayCntByDate( A_Year, A_Month, A_Day );
		Tv_Result	=	(Tv_DayCnt + E_DrTimeBaseWeek) % 7;
#else
		Tv_TmLc.wYear				=	A_Year;
		Tv_TmLc.wMonth			=	A_Month;
		Tv_TmLc.wDay				=	A_Day;
		Tv_TmLc.wDayOfWeek	=	0;
		Tv_TmLc.wHour				=	0;
		Tv_TmLc.wMinute			=	0;
		Tv_TmLc.wSecond			=	0;
		Tv_TmLc.wMilliseconds	=	0;

		if(SystemTimeToFileTime(&Tv_TmLc, &Tv_TmLfl))
		{
			if(FileTimeToSystemTime(&Tv_TmLfl, &Tv_TmSys))
			{
				Tv_Result	=	Tv_TmSys.wDayOfWeek;
			}

			/*
			if(LocalFileTimeToFileTime(&Tv_TmLfl, &Tv_TmFl))
			{
				if(FileTimeToSystemTime(&Tv_TmFl, &Tv_TmSys))
				{
					if(SystemTimeToTzSpecificLocalTime(NULL,&Tv_TmSys,&Tv_TmLc))
					{
						Tv_Result	=	Tv_TmLc.wDayOfWeek;
					}
				}
			}
			*/
		}

#endif

		return	(__u8)Tv_Result;
}
//--------------------------------------------------------------------
Def_DrTime	DrTimeCvtGtodToDrtime( __u32 A_Sec, BOOL8* A_PtrRtIsSm )
{
	// local -------------------
	// code --------------------
		return	DrTimeSysTimeToTime(A_Sec, A_PtrRtIsSm);
}
//--------------------------------------------------------------------
Def_DrTime	DrTimeCvtGtodToSysTime( __u32 A_Sec )
{
		return	A_Sec;
}
//--------------------------------------------------------------------
__u32		DrTimeCvtDrtimeToGtod( Def_DrTime A_Time, BOOL8 A_IsSm )
{
	// local -------------------
	// code --------------------
		return	DrTimeTimeToSysTime( A_Time, A_IsSm );
}
//--------------------------------------------------------------------
__u32		DrTimeCvtSysTimeToGtod( Def_DrTime A_Time )
{
		return	A_Time;
}
//--------------------------------------------------------------------
Def_DrTime	DrTimeGet( __u32* A_PtrRtMili, BOOL8* A_PtrRtIsSm )
{
	// local -------------------
		
#if defined(LINUX_APP) || defined(LINUX_EMU)
		Def_DrTime	Tv_Result;
#else
		Def_DrTime	Tv_Result;
		_FILETIME		Tv_TmFl;
		_FILETIME		Tv_TmLfl;
		SYSTEMTIME	Tv_TmSys;
		SYSTEMTIME	Tv_TmLc;
		SYSTEMTIME	Tv_TmZlc;
		ULARGE_INTEGER	Tv_Tm64;
		__u64				Tv_NtMs;
#endif
	// code --------------------
#if defined(LINUX_APP) || defined(LINUX_EMU)
		//init
		Tv_Result	=	DrTimeSysTimeGet( A_PtrRtMili );
		Tv_Result	=	DrTimeSysTimeToTime( Tv_Result, A_PtrRtIsSm );
#else
		*A_PtrRtIsSm	=	FALSE;

		GetSystemTimeAsFileTime(&Tv_TmFl);
		FileTimeToLocalFileTime(&Tv_TmFl, &Tv_TmLfl);
		Tv_Tm64.LowPart		=	Tv_TmLfl.dwLowDateTime;
		Tv_Tm64.HighPart	=	Tv_TmLfl.dwHighDateTime;
		Tv_NtMs			=	(Tv_Tm64.QuadPart / 10000);
		Tv_Result		=	(Def_DrTime)((Tv_NtMs / 1000) - E_DrTimeNttimeBase);

		*A_PtrRtMili	=	(__u32)(Tv_NtMs % 1000);

		if(FileTimeToSystemTime(&Tv_TmLfl, &Tv_TmLc))
		{
			if(FileTimeToSystemTime(&Tv_TmFl, &Tv_TmSys))
			{
				if(SystemTimeToTzSpecificLocalTime(NULL, &Tv_TmSys, &Tv_TmZlc))
				{
					if((Tv_TmZlc.wHour != Tv_TmLc.wHour) || (Tv_TmZlc.wMinute != Tv_TmLc.wMinute))
					{
						*A_PtrRtIsSm	=	TRUE;
					}
				}
			}
		}

#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
Def_DrTime	DrTimeSysTimeGet( __u32* A_PtrRtMili )
{
	// local -------------------
		Def_DrTime	Tv_Result;
	// code --------------------
		// time get
		Tv_Result	=	DrTimeGetByOs( A_PtrRtMili );

#if defined(LINUX_APP) || defined(LINUX_EMU)
		if ( V_DrTimeMode.IsSmUse )
		{
			// check out of hour range
			if ( (Tv_Result < V_DrTimeRtl.DstRtl.TimeUpdt) || (Tv_Result >= (V_DrTimeRtl.DstRtl.TimeUpdt + E_DrTimeSecOfHour )) )
			{
				// update new hour
				V_DrTimeRtl.DstRtl.TimeUpdt	=	Tv_Result - (Tv_Result % E_DrTimeSecOfHour);
				if ( (Tv_Result >= V_DrTimeRtl.DstRtl.SmTbl[0].Low) && (Tv_Result <= V_DrTimeRtl.DstRtl.SmTbl[E_DrTimeSmTblCnt-1].High) )
				{
					DrTimeDstRtlUpdtSm( &V_DrTimeRtl.DstRtl );
				}
				else
				{
					// update table
					DrTimeDstRtlBuild( V_DrTimeRtl.DstRtl.TimeUpdt, &V_DrTimeRtl.DstRtl );
				}
			}
		}
#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
#ifndef	LINUX_APP

#if 0

NTSTATUS	NTAPI	NtQuerySystemTime( PLARGE_INTEGER SystemTime )
{
	// local -------------------
		NTSTATUS	Tv_Result;
	// code --------------------
		//init
		Tv_Result		=	STATUS_ACCESS_VIOLATION;
		if ( NULL != V_DrTimeFncNtQuerySystemTime )
		{
			Tv_Result		=	V_DrTimeFncNtQuerySystemTime( SystemTime );
		}

		return	Tv_Result;
}

#endif

#endif
//--------------------------------------------------------------------
__u8		DrTimeGetMonthMaxDay( __u16 A_Year, __u8 A_Month )
{
	// local -------------------
		__u8		Tv_Result;
	// code --------------------
		Tv_Result		=	31;
		//check month range
		if ( (1 <= A_Month) && (12 >= A_Month) )
		{
			Tv_Result		=	V_DrTimeDayOfMonTbl[A_Month - 1];
			if ( 2 == A_Month )
			{
				if ( DrTimeIsLeapYear( A_Year ) )
				{
					Tv_Result ++;
				}
			}
		}
		
		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32		DrTimeGetTick( void )
{
	// local -------------------
#ifdef LINUX_APP
		struct tms	Tv_Tms;
#endif
	// code --------------------
#ifdef LINUX_APP
		return	(__u32)times(&Tv_Tms);
#else
		return	GetTickCount();
#endif
}
//--------------------------------------------------------------------
__u32		DrTimeTickDifMs( __u32 A_Old, __u32 A_Near )
{
	// local -------------------
		__u32		Tv_Result;
	// code --------------------
		//init
		#ifdef LINUX_APP
			//Tv_Result		=	((A_Near - A_Old) * 1000) / sysconf(_SC_CLK_TCK);
#if (1 == E_DrTimeTickMul)
		Tv_Result		=	(A_Near - A_Old);
#else
		Tv_Result		=	(A_Near - A_Old) * E_DrTimeTickMul;
#endif
			
		#else
			Tv_Result		=	(A_Near - A_Old);
		#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32		DrTimeMsToTick( __u32 A_Ms )
{
#ifdef LINUX_APP
		//return	(A_Ms * sysconf(_SC_CLK_TCK) ) / 1000;
#if (1 == E_DrTimeTickMul)
		return	A_Ms;
#else
	return	A_Ms / E_DrTimeTickMul;
#endif

#else
		return	A_Ms;
#endif
}
//--------------------------------------------------------------------
__u32		DrTimeTickToMs( __u32 A_Tick )
{
#ifdef LINUX_APP
		//return	(A_Tick * 1000) / sysconf(_SC_CLK_TCK);
		return	A_Tick * E_DrTimeTickMul;
#else
		return	A_Tick;
#endif
}
//--------------------------------------------------------------------
void	DrTimeSetDstTime( BOOL8 A_IsAble, __u8 A_StMon, __u8 A_StWkTh, __u8 A_StWkDay, __u8 A_StHour, __u8 A_EndMon, __u8 A_EndWkTh, __u8 A_EndWkDay, __u8 A_EndHour )
{
	// local -------------------
#if defined(LINUX_APP) || defined(LINUX_EMU)
		__u32	Tv_Mili;
		Def_DrTime	Tv_Time;
#endif
	// code --------------------

#if defined(LINUX_APP) || defined(LINUX_EMU)
		V_DrTimeMode.IsSmUse	=	A_IsAble;
		if ( V_DrTimeMode.IsSmUse )
		{
			Tv_Time	=	DrTimeGetByOs( &Tv_Mili );
			V_DrTimeRtl.DstRtl.TimeUpdt	=	Tv_Time - (Tv_Time % E_DrTimeSecOfHour);

			V_DrTimeMode.IsSmDst	=	TRUE;

			V_DrTimeMode.SmStTime.Month	=	A_StMon;
			V_DrTimeMode.SmStTime.Week	=	A_StWkTh;	
			V_DrTimeMode.SmStTime.Day		=	A_StWkDay;
			V_DrTimeMode.SmStTime.Hour	=	A_StHour;
			V_DrTimeMode.SmEdTime.Month	=	A_EndMon;
			V_DrTimeMode.SmEdTime.Week	=	A_EndWkTh;
			V_DrTimeMode.SmEdTime.Day		=	A_EndWkDay;
			V_DrTimeMode.SmEdTime.Hour	=	A_EndHour;

			DrTimeDstRtlBuild( V_DrTimeRtl.DstRtl.TimeUpdt, &V_DrTimeRtl.DstRtl );

		}
#endif

}
//--------------------------------------------------------------------
void	DrTimeSetSummerTime( BOOL8 A_IsAble, __u8 A_StMon, __u8 A_StDay, __u8 A_StHour, __u8 A_EndMon, __u8 A_EndDay, __u8 A_EndHour )
{
	// local -------------------
#if defined(LINUX_APP) || defined(LINUX_EMU)
		__u32	Tv_Mili;
		Def_DrTime	Tv_Time;
#endif
	// code --------------------
#if defined(LINUX_APP) || defined(LINUX_EMU)
		V_DrTimeMode.IsSmUse	=	(BOOL8)A_IsAble;
		if ( V_DrTimeMode.IsSmUse )
		{
			Tv_Time	=	DrTimeGetByOs( &Tv_Mili );
			V_DrTimeRtl.DstRtl.TimeUpdt	=	Tv_Time - (Tv_Time % E_DrTimeSecOfHour);

			V_DrTimeMode.IsSmDst	=	FALSE;

			V_DrTimeMode.SmStTime.Month	=	A_StMon;
			V_DrTimeMode.SmStTime.Day		=	A_StDay;
			V_DrTimeMode.SmStTime.Hour	=	A_StHour;
			V_DrTimeMode.SmEdTime.Month	=	A_EndMon;
			V_DrTimeMode.SmEdTime.Day		=	A_EndDay;
			V_DrTimeMode.SmEdTime.Hour	=	A_EndHour;

			DrTimeDstRtlBuild( V_DrTimeRtl.DstRtl.TimeUpdt, &V_DrTimeRtl.DstRtl );
		}
#endif
}
//--------------------------------------------------------------------
void		DrTimeSet( Def_DrTime A_Time, __u32 A_Fcc )
{
	// local -------------------
			Def_DrTime	Tv_SysTime;			
			BOOL8	Tv_IsSm;
	// code --------------------
			Tv_IsSm			=	DrTimeIsSmTime( A_Time );
			Tv_SysTime	=	DrTimeTimeToSysTime( A_Time, Tv_IsSm );
			DrTimeSysTimeSet( Tv_SysTime, A_Fcc );
}
//--------------------------------------------------------------------
void		DrTimeSysTimeSet( Def_DrTime A_Time, __u32 A_Fcc )
{
	// local -------------------
#ifdef LINUX_APP
		struct timeval	Tv_Gtod;	
#else

#if 1
		_FILETIME	Tv_TmFl;
		ULARGE_INTEGER	Tv_Tm64;
#else
		LARGE_INTEGER	Tv_NtTime;
		HINSTANCE	Tv_HinstNtdll;
#endif

		__u64		Tv_NtMs;
		__u32		Tv_Sec;
#endif
		St_DrThrdMsgTimeUpdt	Tv_Msg;
	// code --------------------
#ifdef LINUX_APP
		//set time
		Tv_Gtod.tv_usec		=	0;
		Tv_Gtod.tv_sec		=	A_Time;
		settimeofday( &Tv_Gtod, NULL );
#else

#ifdef E_DrTimeModeFileTime

		// check initialized
		GetSystemTimeAsFileTime(&Tv_TmFl);
		Tv_Tm64.LowPart		=	Tv_TmFl.dwLowDateTime;
		Tv_Tm64.HighPart	=	Tv_TmFl.dwHighDateTime;
		Tv_NtMs			=	(Tv_Tm64.QuadPart / 10000);
		Tv_Sec		=	(Def_DrTime)(Tv_NtMs / 1000);	
#else

		if ( !V_DrTimeIsGetTimeInited )
		{
			V_DrTimeTimeInited	=	E_DrTimeNttimeBase;
			V_DrTimeIsGetTimeInited	=	TRUE;
			//load API
			Tv_HinstNtdll		=	LoadLibraryA("Ntdll.dll"); 
			if ( NULL != Tv_HinstNtdll )
			{
				V_DrTimeFncNtQuerySystemTime	=	(Fnc_NtQuerySystemTime)GetProcAddress(Tv_HinstNtdll, "NtQuerySystemTime" ); 
			}
		}
		NtQuerySystemTime( &Tv_NtTime );
		Tv_NtMs		=	(Tv_NtTime.QuadPart / 10000);
		Tv_Sec		=	(Def_DrTime)(((Tv_NtMs / 1000) - V_DrTimeTimeInited) + 0x7E90);	// GMT korea = +9
#endif

#if defined(LINUX_APP) || defined(LINUX_EMU)
		V_DrTimeEmuGap	=	Tv_Sec - A_Time;
#endif

#endif

#if defined(LINUX_APP) || defined(LINUX_EMU)
		// update init
		if ( V_DrTimeMode.IsSmUse )
		{
			V_DrTimeRtl.DstRtl.TimeUpdt	=	A_Time - (A_Time % E_DrTimeSecOfHour);
			DrTimeDstRtlBuild( V_DrTimeRtl.DstRtl.TimeUpdt, &V_DrTimeRtl.DstRtl );
		}
#endif

		// notify
		Tv_Msg.MsgHd.Cmd	=	E_DrThrdMsgTimeUpdt;
		Tv_Msg.Fcc				=	A_Fcc;
		Tv_Msg.Time				=	A_Time;
		DrThrdMsgBroadCast( (Ptr_DrThrdMsgHd)&Tv_Msg, sizeof(Tv_Msg) );
}
//--------------------------------------------------------------------
void	DrTimeDstRtlBuild( Def_DrTime A_BaseTime, struct St_DrTimeDstRtl* A_PtrRtRtl )
{
	// local -------------------
#if defined(LINUX_APP) || defined(LINUX_EMU)
		__u16	Tv_Yr;
		__u8	Tv_Mo;
		__u8	Tv_Dy;
		__u8	Tv_Hr;
		__u8	Tv_Mi;
		__u8	Tv_Sc;
		__u16	Tv_AddYear;
		__u8	Tv_WkIdx;
		__u16	Tv_Year;
		__u32	Tv_Days;
		__u8	Tv_LastDay;
		__u8	Tv_Week;
		__u32	Tv_StDays;
		__u32	Tv_EdDays;
#endif
	// code --------------------
#if defined(LINUX_APP) || defined(LINUX_EMU)
		// last year make
		if ( V_DrTimeMode.IsSmUse )
		{
			// get base time
			DrTimeDec( A_BaseTime, &Tv_Yr, &Tv_Mo, &Tv_Dy, &Tv_Hr, &Tv_Mi, &Tv_Sc );
			if ( V_DrTimeMode.IsSmDst )
			{
				// relative DST
				// low
				// calculate date
				for ( Tv_WkIdx=0;Tv_WkIdx < E_DrTimeSmTblCnt;Tv_WkIdx++ )
				{
					Tv_Year	=	Tv_WkIdx + Tv_Yr - 1;
					if ( 0 == V_DrTimeMode.SmStTime.Week )
					{
						// last week
						Tv_LastDay	=	DrTimeGetMonthMaxDay( Tv_Year, V_DrTimeMode.SmStTime.Month );
						Tv_StDays	=	DrTimeDayCntByDate( Tv_Year, V_DrTimeMode.SmStTime.Month, Tv_LastDay );
						Tv_Week	=	DrTimeGetWeekByDate( Tv_Year, V_DrTimeMode.SmStTime.Month, Tv_LastDay );
						if ( Tv_Week < V_DrTimeMode.SmStTime.Day )
						{
							Tv_StDays	=	Tv_StDays - ( (7 + (__u32)Tv_Week) - (__u32)V_DrTimeMode.SmStTime.Day );
						}
						else
						{
							Tv_StDays	=	Tv_StDays - ( (__u32)Tv_Week - (__u32)V_DrTimeMode.SmStTime.Day );
						}
					}
					else
					{
						// normal
						Tv_StDays	=	DrTimeDayCntByDate( Tv_Year, V_DrTimeMode.SmStTime.Month, 1 );
						Tv_Week	=	DrTimeGetWeekByDate( Tv_Year, V_DrTimeMode.SmStTime.Month, 1 );
						if ( Tv_Week > V_DrTimeMode.SmStTime.Day )
						{
							Tv_StDays	= Tv_StDays + 7;
						}
						Tv_StDays	=	Tv_StDays + ( 7 * ( (__u32)V_DrTimeMode.SmStTime.Week - 1 ) ) + (__u32)V_DrTimeMode.SmStTime.Day - (__u32)Tv_Week;
					}
					A_PtrRtRtl->SmTbl[Tv_WkIdx].Low	=	(Tv_StDays * E_DrTimeSecOfDay) + ((__u32)V_DrTimeMode.SmStTime.Hour * E_DrTimeSecOfHour );
				}

				// high
				Tv_Year	=	Tv_Yr - 1;
				if ( V_DrTimeMode.SmStTime.Month > V_DrTimeMode.SmEdTime.Month )
				{
					Tv_Year ++;
				}
				for ( Tv_WkIdx=0;Tv_WkIdx < E_DrTimeSmTblCnt;Tv_WkIdx++ )
				{
					if ( 0 == V_DrTimeMode.SmEdTime.Week )
					{
						// last week
						Tv_LastDay	=	DrTimeGetMonthMaxDay( Tv_Year, V_DrTimeMode.SmEdTime.Month );
						Tv_EdDays	=	DrTimeDayCntByDate( Tv_Year, V_DrTimeMode.SmEdTime.Month, Tv_LastDay );
						Tv_Week	=	DrTimeGetWeekByDate( Tv_Year, V_DrTimeMode.SmEdTime.Month, Tv_LastDay );
						if ( Tv_Week < V_DrTimeMode.SmEdTime.Day )
						{
							Tv_EdDays	=	Tv_EdDays - ( (7 + (__u32)Tv_Week) - (__u32)V_DrTimeMode.SmEdTime.Day );
						}
						else
						{
							Tv_EdDays	=	Tv_EdDays - ( (__u32)Tv_Week - (__u32)V_DrTimeMode.SmEdTime.Day );
						}
					}
					else
					{
						// normal
						Tv_EdDays	=	DrTimeDayCntByDate( Tv_Year, V_DrTimeMode.SmEdTime.Month, 1 );
						Tv_Week	=	DrTimeGetWeekByDate( Tv_Year, V_DrTimeMode.SmEdTime.Month, 1 );
						if ( Tv_Week > V_DrTimeMode.SmEdTime.Day )
						{
							Tv_EdDays	= Tv_EdDays + 7;
						}
						Tv_EdDays	=	Tv_EdDays + ( 7 * ( (__u32)V_DrTimeMode.SmEdTime.Week - 1 ) ) + (__u32)V_DrTimeMode.SmEdTime.Day - (__u32)Tv_Week;
					}
					A_PtrRtRtl->SmTbl[Tv_WkIdx].High	=	(Tv_EdDays * E_DrTimeSecOfDay) + ((__u32)V_DrTimeMode.SmEdTime.Hour * E_DrTimeSecOfHour );

					// next
					Tv_Year ++;
				}
			}
			else
			{
				// absoluteness DST
				Tv_AddYear	=	0;
				if (	(V_DrTimeMode.SmStTime.Month > V_DrTimeMode.SmEdTime.Month) || 
							((V_DrTimeMode.SmStTime.Month == V_DrTimeMode.SmEdTime.Month) && (V_DrTimeMode.SmStTime.Day > V_DrTimeMode.SmEdTime.Day)) || 
							( (V_DrTimeMode.SmStTime.Month == V_DrTimeMode.SmEdTime.Month) && (V_DrTimeMode.SmStTime.Day == V_DrTimeMode.SmEdTime.Day) && (V_DrTimeMode.SmStTime.Hour > V_DrTimeMode.SmEdTime.Hour) ) )
				{
					Tv_AddYear	=	1;
				}
				for ( Tv_WkIdx=0;Tv_WkIdx < E_DrTimeSmTblCnt;Tv_WkIdx++ )
				{
					Tv_Year	=	Tv_WkIdx + Tv_Yr - 1;
					Tv_Days	=	DrTimeDayCntByDate( Tv_Year, V_DrTimeMode.SmStTime.Month, V_DrTimeMode.SmStTime.Day );
					A_PtrRtRtl->SmTbl[Tv_WkIdx].Low	=	(Tv_Days * E_DrTimeSecOfDay) + ((__u32)V_DrTimeMode.SmStTime.Hour * E_DrTimeSecOfHour );
					Tv_Year	=	Tv_Year + Tv_AddYear;
					Tv_Days	=	DrTimeDayCntByDate( Tv_Year, V_DrTimeMode.SmEdTime.Month, V_DrTimeMode.SmEdTime.Day );
					
					A_PtrRtRtl->SmTbl[Tv_WkIdx].High	=	(Tv_Days * E_DrTimeSecOfDay) + ((__u32)V_DrTimeMode.SmEdTime.Hour * E_DrTimeSecOfHour ) 
						- 1;
				}
			}

			DrTimeDstRtlUpdtSm( A_PtrRtRtl );
		}
#endif
}
//--------------------------------------------------------------------
Def_DrTime	DrTimeGetByOs( __u32* A_PtrRtMili )
{
	// local -------------------
		Def_DrTime	Tv_Result;
#ifdef	LINUX_APP
		struct timeval	Tv_Gtod;
#else

#if 1
		_FILETIME	Tv_TmFl;
		_FILETIME	Tv_TmLfl;
		ULARGE_INTEGER	Tv_Tm64;
#else
		LARGE_INTEGER	Tv_NtTime;
		HINSTANCE	Tv_HinstNtdll;
#endif // 0

		__u64		Tv_NtMs;

#endif
	// code --------------------
		//init
#ifdef	LINUX_APP
		gettimeofday( &Tv_Gtod, NULL );
		Tv_Result	=	Tv_Gtod.tv_sec;
		*A_PtrRtMili	=	Tv_Gtod.tv_usec >> 10;
#else

#ifdef E_DrTimeModeFileTime

		GetSystemTimeAsFileTime(&Tv_TmFl);
		FileTimeToLocalFileTime(&Tv_TmFl,&Tv_TmLfl);
		Tv_Tm64.LowPart		=	Tv_TmLfl.dwLowDateTime;
		Tv_Tm64.HighPart	=	Tv_TmLfl.dwHighDateTime;
		Tv_NtMs			=	(Tv_Tm64.QuadPart / 10000);
		Tv_Result		=	(Def_DrTime)((Tv_NtMs / 1000) - E_DrTimeNttimeBase);
#else
		if(!V_DrTimeIsGetTimeInited)
		{
			V_DrTimeTimeInited	=	E_DrTimeNttimeBase;
			V_DrTimeIsGetTimeInited	=	TRUE;
			//load API
			Tv_HinstNtdll		=	LoadLibraryA("Ntdll.dll");
			if(NULL != Tv_HinstNtdll)
			{
				V_DrTimeFncNtQuerySystemTime	=	(Fnc_NtQuerySystemTime)GetProcAddress(Tv_HinstNtdll, "NtQuerySystemTime");
			}
		}
		NtQuerySystemTime(&Tv_NtTime);
		Tv_NtMs			=	(Tv_NtTime.QuadPart / 10000);
		Tv_Result		=	(Def_DrTime)(((Tv_NtMs / 1000) - V_DrTimeTimeInited) + 0x7E90) - V_DrTimeEmuGap;		// GMT + 9

#endif // 0

		*A_PtrRtMili	=	(__u32)(Tv_NtMs % 1000);

#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
void	DrTimeDstRtlUpdtSm( struct St_DrTimeDstRtl* A_PtrRtRtl )
{
	// local -------------------
#if defined(LINUX_APP) || defined(LINUX_EMU)
		__u8	Tv_WkIdx;
#endif
	// code --------------------
#if defined(LINUX_APP) || defined(LINUX_EMU)
		// update summer time status
		A_PtrRtRtl->IsSm	=	FALSE;
		for ( Tv_WkIdx=0;Tv_WkIdx < E_DrTimeSmTblCnt;Tv_WkIdx++ )
		{
			if ( (A_PtrRtRtl->SmTbl[Tv_WkIdx].Low <= A_PtrRtRtl->TimeUpdt ) && (A_PtrRtRtl->SmTbl[Tv_WkIdx].High > A_PtrRtRtl->TimeUpdt ) )
			{
				//in summer time
				A_PtrRtRtl->IsSm	=	TRUE;
				break;
			}
		}
#endif
}
//--------------------------------------------------------------------
