/*
 configuration - Fds1640

 */

//====================================================================
// uses
#include	<DrPlatform.h>

#include	<DrTypeBase.h>
#include	<DrTimeTool.h>
#include	<DrDumyTool.h>
#include	<DrStrTool.h>

#include	<DrDebug.h>
#include <DrError.h>

//====================================================================
//local const

#define	E_DrDebugErrStrCnt			20

//====================================================================
//local type

typedef struct St_DrDebugErrStrTbl
{
	__u32	Code;
	char	Str[28];
}	*Ptr_DrDebugErrStrTbl;

//====================================================================
//local function

//====================================================================
//var
Def_StrShort	V_DbgStrDumpMsg;

char	V_DbgStrErrUnknown[24]	=	"Unknown error code.";

St_DrDebugErrStrTbl	V_DbgStrErrTbl[E_DrDebugErrStrCnt]	=
{
	{E_DrErrNone,"Error none."},
	{E_DrErrBadParam,"Bad parametter."},
	{E_DrErrReqSendFail,"Send fail."},
	{E_DrErrObjectCreateFail,"Object create fail."},
	{E_DrErrDataBroken,"Data broken."},
	{E_DrErrBufFull,"Buffer full."},
	{E_DrErrInitFail,"Initialize fail."},
	{E_DrErrEndOfData,"Arrival end of data."},
	{E_DrErrBusy,"Busy."},
	{E_DrErrFail,"Fail."},
	{E_DrErrBadIo,"Bad input output operation."},
	{E_DrErrNotFound,"Not found."},
	{E_DrErrUserCancel,"User cancel."},
	{E_DrErrBadPasswd,"Bad password input."},
	{E_DrErrPermisDenied,"Permission denied."},
	{E_DrErrAlreadyExist,"Already exist."},
	{E_DrErrWaiting,"Waiting."},
	{E_DrErrOutOfRange,"Out of range."},
	{E_DrErrDataRemain,"Data remain."},
	{E_DrErrDataRemain,"Data remain."},

};


//====================================================================
//--------------------------------------------------------------------
void	DbgMemDump( void* A_Ptr, unsigned int A_Size, unsigned int A_Unit )
{
	// local -------------------
		__u32		Tv_LastSize;
		void*		Tv_PtrWk;
		__u32		Tv_WkIdx;
		__u32		Tv_WkVal;
	// code --------------------
		//print head
		DbgMsgPrint((char*)"Dump - Addr : 0x" );
		DrStrDwordToHexAsc( V_DbgStrDumpMsg, (__u32)(Def_DrCalPtr)A_Ptr, 8, TRUE );
		DbgMsgPrint( "%s", V_DbgStrDumpMsg );
		DbgMsgPrint((char*)"\n" );

		Tv_LastSize		=	A_Size;
		Tv_PtrWk			=	A_Ptr;
		while ( 0 != Tv_LastSize )
		{
			//print address
			DrStrDwordToHexAsc(V_DbgStrDumpMsg, (__u32)(Def_DrCalPtr)Tv_PtrWk, 8, TRUE);
			DbgMsgPrint( "%s", V_DbgStrDumpMsg );
			DbgMsgPrint((char*)" : " );

			//prite contents
			switch ( A_Unit )
			{
				case E_DbgDumpUnitWord:
					for (Tv_WkIdx = 0;Tv_WkIdx < 8;Tv_WkIdx++)
					{
						if ( 2 <= Tv_LastSize )
						{
							Tv_WkVal	=	*((__u16*)Tv_PtrWk);
							DrStrDwordToHexAsc( V_DbgStrDumpMsg, Tv_WkVal, 4, TRUE );
							DbgMsgPrint( "%s", V_DbgStrDumpMsg );
						}
						else
						{
							Tv_LastSize		=	0;
							break;
						}
						//next
						Tv_LastSize		=	Tv_LastSize - 2;
						Tv_PtrWk			=	(void*)((Def_DrCalPtr)Tv_PtrWk + 2);
						DbgMsgPrint((char*)" " );
					}
					break;
				case E_DbgDumpUnitDword:
					for (Tv_WkIdx = 0;Tv_WkIdx < 4;Tv_WkIdx++)
					{
						if ( 4 <= Tv_LastSize )
						{
							Tv_WkVal	=	*((__u32*)Tv_PtrWk);
							DrStrDwordToHexAsc( V_DbgStrDumpMsg, Tv_WkVal, 8, TRUE );
							DbgMsgPrint( "%s",V_DbgStrDumpMsg );
						}
						else
						{
							Tv_LastSize		=	0;
							break;
						}
						//next
						Tv_LastSize		=	Tv_LastSize - 4;
						Tv_PtrWk			=	(void*)( (Def_DrCalPtr)Tv_PtrWk + 4 );
						DbgMsgPrint((char*)" " );
					}
					break;
				default:
					for (Tv_WkIdx = 0;Tv_WkIdx < 16;Tv_WkIdx++)
					{
						if ( 1 <= Tv_LastSize )
						{
							Tv_WkVal	=	*((__u8*)Tv_PtrWk);
							DrStrDwordToHexAsc( V_DbgStrDumpMsg, Tv_WkVal, 2, TRUE );
							DbgMsgPrint( "%s", V_DbgStrDumpMsg );
						}
						else
						{
							Tv_LastSize		=	0;
							break;
						}
						//next
						Tv_LastSize		=	Tv_LastSize - 1;
						Tv_PtrWk			=	(void*)( (Def_DrCalPtr)Tv_PtrWk + 1 );
						DbgMsgPrint((char*)" " );
					}
			    break;
			}

			//go next line
			DbgMsgPrint((char*)"\n" );
		}

}
//--------------------------------------------------------------------
char* DbgErrCodeToStr(unsigned int A_ErrCode)
{
	__u32	Tv_WkIdx;

	for( Tv_WkIdx = 0; Tv_WkIdx < E_DrDebugErrStrCnt; Tv_WkIdx++)
	{
		if(V_DbgStrErrTbl[Tv_WkIdx].Code == (__u32)A_ErrCode)
		{
			// found
			return	V_DbgStrErrTbl[Tv_WkIdx].Str;
		}
	}
	
	return	V_DbgStrErrUnknown;
}
//--------------------------------------------------------------------
