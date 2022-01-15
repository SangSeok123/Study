/*
	time tool 
	uses
		DrTypeBase.h

		predefine
			DR_TICK_MUL - mili tick multiplier - default = 10  ( jiffies * 10 = mili tick )
			
*/

//--------------------------------------------------------------------
//Environment conditional
#ifndef	_EPC_DrTimeTool
#define	_EPC_DrTimeTool

//====================================================================
// uses

#include	<DrTypeBase.h>
#ifdef LINUX_APP
	#ifndef LINUX_DRV
		#include	<time.h>
	#endif
#endif
	
//====================================================================
//constance
#define		E_DrTimeHourOfDay		24
#define		E_DrTimeMinOfDay		(24*60)
#define		E_DrTimeMinOfHour		60
#define		E_DrTimeSecOfDay		(24*60*60) //86400
#define		E_DrTimeSecOfHour		(60*60)
#define		E_DrTimeSecOfMin		60
#define		E_DrTimeSecOfLeafYear	(366 * E_DrTimeSecOfDay)	//31622400
#define		E_DrTimeSecOfYear		(365 * E_DrTimeSecOfDay)	//31536000
#define		E_DrTimeSecOf4year	( (366 + (365*3)) * E_DrTimeSecOfDay )	//126230400
#define		E_DrTimeDayOf4year	( 366 + (365*3) )

#define		E_DrTimeMiliOfSec		1000
#define		E_DrTimeMiliOfMin		(E_DrTimeMiliOfSec * 60)
#define		E_DrTimeMiliOfHour	(E_DrTimeMiliOfMin * 60)

#define		E_DrTimeDayOfWeek		7
#define		E_DrTimeMonthOfYear 12

//week
#define		E_DrTimeWeekSun			0
#define		E_DrTimeWeekMon			1
#define		E_DrTimeWeekTue			2
#define		E_DrTimeWeekWed			3
#define		E_DrTimeWeekThu			4
#define		E_DrTimeWeekFri			5
#define		E_DrTimeWeekSat			6

#define		E_DrTimeFindRngIn				(1 << 0)		/* not use */
#define		E_DrTimeFindRngDreate		(1 << 1)		/* not use */
#define		E_DrTimeFindRngSmall		(1 << 2)		/* not use */

#define		E_DrTimeBaseYear				1970		/* linux base time */
#define		E_DrTimeMaxYear					2038

#define	E_DrTimeLnxGap				0				/* LINUX time gap */
#define	E_DrTimeWinGap				0				/* window time gap */

#define	E_DrTimeSmTblCnt			3

#define	E_DrTimeBaseWeek			E_DrTimeWeekThu

#define E_DrTimeFcc						Mac_DrMakeFcc('G','R','T','M')		/* FCC code for message notify  */

//====================================================================
//global macro

//====================================================================
//global type

//====================================================================
//class

//====================================================================
//global function
BOOL8		DrTimeIsLeapYear( __u16 A_Year );		//check leap year validate 2001 ~ 2099

BOOL8		DrTimeIsSmTime( Def_DrTime A_SysTime );
Def_DrTime	DrTimeEnc(	__u16 A_Year, __u8 A_Month, __u8 A_Day, 
												__u8 A_Hour, __u8 A_Min, __u8 A_Sec /*, BOOL8* A_PtrRtIsSm*/ );

void		DrTimeDec(	Def_DrTime A_Time, 
										__u16* A_PtrRtYear, __u8* A_PtrRtMonth, __u8* A_PtrRtDay,	
										__u8* A_PtrRtHour, __u8* A_PtrRtMin, __u8* A_PtrRtSec  );
void		DrTimeDec(Def_DrTime A_Time, Ptr_DrDateTime A_PtrRtDateTime);

__u8		DrTimeGetWeekByTime( Def_DrTime A_Time );
__u8		DrTimeGetWeekByDate( __u16 A_Year, __u8 A_Month, __u8 A_Day );

Def_DrTime	DrTimeCvtGtodToDrtime( __u32 A_Sec, BOOL8* A_PtrRtIsSm );
__u32		DrTimeCvtDrtimeToGtod( Def_DrTime A_Time, BOOL8 A_IsSm );
Def_DrTime	DrTimeCvtGtodToSysTime( __u32 A_Sec );
__u32		DrTimeCvtSysTimeToGtod( Def_DrTime A_Time );
Def_DrTime	DrTimeSysTimeGet( __u32* A_PtrRtMili );

Def_DrTime	DrTimeTimeToSysTime( Def_DrTime A_Time, BOOL8 A_IsSm );
Def_DrTime	DrTimeSysTimeToTime( Def_DrTime A_SysTime, BOOL8* A_PtrRtIsSm );
Def_DrTime	DrTimeGet( __u32* A_PtrRtMili, BOOL8* A_PtrRtIsSm );
void				DrTimeSetSummerTime( BOOL8 A_IsAble, __u8 A_StMon, __u8 A_StDay, __u8 A_StHour, __u8 A_EndMon, __u8 A_EndDay, __u8 A_EndHour );
void				DrTimeSetDstTime( BOOL8 A_IsAble, __u8 A_StMon, __u8 A_StWkTh, __u8 A_StWkDay, __u8 A_StHour, __u8 A_EndMon, __u8 A_EndWkTh, __u8 A_EndWkDay, __u8 A_EndHour );

__u8		DrTimeGetMonthMaxDay( __u16 A_Year, __u8 A_Month );

__u32		DrTimeGetTick( void );
__u32		DrTimeTickDifMs( __u32 A_Old, __u32 A_Near );
__u32		DrTimeMsToTick( __u32 A_Ms );
__u32		DrTimeTickToMs( __u32 A_Tick );

#ifdef __cplusplus
void		DrTimeSet( Def_DrTime A_Time, __u32 A_Fcc = E_DrTimeFcc );
void		DrTimeSysTimeSet( Def_DrTime A_Time, __u32 A_Fcc = E_DrTimeFcc );
#else
void		DrTimeSet( Def_DrTime A_Time, __u32 A_Fcc );
void		DrTimeSysTimeSet( Def_DrTime A_Time, __u32 A_Fcc );
#endif

__u32		DrTimeDayCntBySysTime( Def_DrTime A_Time );
__u32		DrTimeDayCntByDate( __u16 A_Year, __u8 A_Month, __u8 A_Day );

//====================================================================
//out Environment conditional
#endif
