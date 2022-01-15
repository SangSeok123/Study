/*
	task base

*/

//====================================================================
//include
#include <DrPlatform.h>
#include <DrTypeBase.h>
#include <DrDebug.h>
#include <DrThrdBase.h>
#include <DrDumyTool.h>
#include <DrTimeTool.h>
#include <DrProcess.h>
#include <DrThrdObj.h>
#include <DrStrTool.h>

#ifdef LINUX_APP
	#undef _XOPEN_SOURCE
	#define _XOPEN_SOURCE	500
	#include <signal.h>
	#include <stdlib.h>
	#include <stdio.h>
#endif

//====================================================================
//local const


//====================================================================
//local type

//====================================================================
//global var

St_DrThrdEnv	V_DrThrdEnv	=	
{0,};

//====================================================================
//functions


//--------------------------------------------------------------------
__s32	DrThrdObjReg(void* A_Obj)
{
	__s32		Tv_Result;
	__u32		Tv_WkIdx;

	//init
	Tv_Result		=	-1;

	//check inited
	if(0 == V_DrThrdEnv.ObjCnt)
	{
		DrThrdInit();
	}
	//crit
	DrPrcCritEnter(&V_DrThrdEnv.BaseCrit);

	//find empty slot
	for(Tv_WkIdx=0; Tv_WkIdx < E_DrThrdObjMaxCnt; Tv_WkIdx++)
	{
		if(NULL == V_DrThrdEnv.ObjTbl[Tv_WkIdx].Obj)
		{
			V_DrThrdEnv.ObjTbl[Tv_WkIdx].Obj	=	A_Obj;
			Tv_Result		=	(__s32)Tv_WkIdx;
			V_DrThrdEnv.ObjCnt++;
			break;
		}
	}

	if(0 > Tv_Result)
	{
#ifdef LINUX_APP
		printf((char*)"[BUG!!] DrThrdObjReg - task slot is full!\n");
#else
		DbgMsgPrint((char*)"[BUG!!] DrThrdObjReg - task slot is full!\n");
#endif
	}

	//crit
	DrPrcCritLeave(&V_DrThrdEnv.BaseCrit);

	return	Tv_Result;

}
//--------------------------------------------------------------------
BOOL8	DrThrdObjUnreg(void* A_Obj)
{
	BOOL8		Tv_Result;
	__u32		Tv_WkIdx;

	//init
	Tv_Result		=	FALSE;

	if (0 == V_DrThrdEnv.ObjCnt)
	{
#ifdef LINUX_APP
		printf("[BUG!!] DrThrdObjUnreg - task slot is empty!\n");
#else
		DbgMsgPrint((char*)"[BUG!!] DrThrdObjUnreg - task slot is empty!\n");
#endif
		return	FALSE;
	}

	//crit
	DrPrcCritEnter(&V_DrThrdEnv.BaseCrit);

	//find empty slot
	for(Tv_WkIdx=0; Tv_WkIdx < E_DrThrdObjMaxCnt; Tv_WkIdx++)
	{
		if(A_Obj == V_DrThrdEnv.ObjTbl[Tv_WkIdx].Obj)
		{
			V_DrThrdEnv.ObjTbl[Tv_WkIdx].Obj					=	NULL;
			V_DrThrdEnv.ObjTbl[Tv_WkIdx].StrName[0]		=	0;
			V_DrThrdEnv.ObjTbl[Tv_WkIdx].ThrdId				=	0;
			Tv_Result		=	TRUE;
			V_DrThrdEnv.ObjCnt --;
			break;
		}
	}

	//crit
	DrPrcCritLeave(&V_DrThrdEnv.BaseCrit);

	if(0 == V_DrThrdEnv.ObjCnt)
	{
		DrThrdFinish();
	}

	return	Tv_Result;
}
//--------------------------------------------------------------------
void	DrThrdInit(void)
{
		DrDumyZeroMem(&V_DrThrdEnv, sizeof(V_DrThrdEnv));
		DrPrcCritInit(&V_DrThrdEnv.BaseCrit);
}
//--------------------------------------------------------------------
void	DrThrdFinish(void)
{
		DrPrcCritDel(&V_DrThrdEnv.BaseCrit);
}
//--------------------------------------------------------------------
void* DrThrdObjGetByName(char* A_StrName)
{
	void*	Tv_Result;
	__u32		Tv_WkIdx;

	//init
	Tv_Result		=	NULL;

	//critical section
	DrPrcCritEnter(&V_DrThrdEnv.BaseCrit);

	//find empty slot
	for(Tv_WkIdx=0; Tv_WkIdx < E_DrThrdObjMaxCnt; Tv_WkIdx++)
	{
		if(NULL != V_DrThrdEnv.ObjTbl[Tv_WkIdx].Obj)
		{
			if(0 == DrStrCmpAsc(A_StrName, V_DrThrdEnv.ObjTbl[Tv_WkIdx].StrName, TRUE, 63))
			{
				Tv_Result		=	V_DrThrdEnv.ObjTbl[Tv_WkIdx].Obj;
				break;
			}
		}
	}

	//critical section
	DrPrcCritLeave(&V_DrThrdEnv.BaseCrit);

	return	Tv_Result;
}
//--------------------------------------------------------------------
void* DrThrdObjFindByThrdId(Def_DrPrcThrdId A_ThrdId, BOOL8 A_IsThrdSafe)
{
	void*	Tv_Result;
	__u32		Tv_WkIdx;
	//init
	Tv_Result		=	NULL;

	//critical section
	if(A_IsThrdSafe)
	{
		DrPrcCritEnter(&V_DrThrdEnv.BaseCrit);
	}

	//find empty slot
	for(Tv_WkIdx=0; Tv_WkIdx < E_DrThrdObjMaxCnt; Tv_WkIdx++)
	{
		if(NULL != V_DrThrdEnv.ObjTbl[Tv_WkIdx].Obj)
		{
			if(A_ThrdId == V_DrThrdEnv.ObjTbl[Tv_WkIdx].ThrdId)
			{
				Tv_Result		=	V_DrThrdEnv.ObjTbl[Tv_WkIdx].Obj;
				break;
			}
		}
	}

	//critical section
	if(A_IsThrdSafe)
	{
		DrPrcCritLeave(&V_DrThrdEnv.BaseCrit);
	}

	return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8	DrThrdObjStepDoThis(void)
{
	BOOL8		Tv_Result;
	Def_DrPrcThrdId	Tv_ThrdId;
	Cls_DrThrdObj*	Tv_ObjTask;

	//init
	Tv_Result		=	FALSE;

	DrPrcCritEnter(&V_DrThrdEnv.BaseCrit);

	Tv_ThrdId		=	DrPrcThrdGetCurId();
	Tv_ObjTask	=	(Cls_DrThrdObj*)DrThrdObjFindByThrdId(Tv_ThrdId,FALSE);
	if(NULL != Tv_ObjTask)
	{
		Tv_Result	=	Tv_ObjTask->ThrdDoStep();
	}

	DrPrcCritLeave(&V_DrThrdEnv.BaseCrit);

	return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrThrdMsgCmdAlloc(void)
{
	__u32		Tv_Result;
	__u32		Tv_ByteIdx;
	__u32		Tv_BitIdx;
	__u8		Tv_BitMask;

	//init
	Tv_Result		=	0;

	DrPrcCritEnter(&V_DrThrdEnv.BaseCrit);

	//find
	for(Tv_ByteIdx=0; Tv_ByteIdx < E_DrThrdMsgCmdTblSize; Tv_ByteIdx++)
	{
		if(0xFF != V_DrThrdEnv.CmdAlocTbl[Tv_ByteIdx])
		{
			//find bit
			Tv_BitMask	=	1;
			for(Tv_BitIdx=0; Tv_BitIdx < 8; Tv_BitIdx++)
			{
				//check found
				if(0 == (Tv_BitMask & V_DrThrdEnv.CmdAlocTbl[Tv_ByteIdx]))
				{
					//allocate
					V_DrThrdEnv.CmdAlocTbl[Tv_ByteIdx]	=	V_DrThrdEnv.CmdAlocTbl[Tv_ByteIdx] | Tv_BitMask;
					Tv_Result	=	E_DrThrdMsgCmdAlloc + (Tv_ByteIdx * 8) + Tv_BitIdx;
					break;
				}
				//next
				Tv_BitMask	=	Tv_BitMask << 1;
			}
		}
		//check found
		if(0 != Tv_Result)
		{
			break;
		}
	}

	DrPrcCritLeave(&V_DrThrdEnv.BaseCrit);

	return	Tv_Result;
}
//--------------------------------------------------------------------
void	DrThrdMsgCmdFree(__u32 A_Cmd)
{
	// local -------------------
		__u32		Tv_ByteIdx;
		__u32		Tv_BitIdx;
		__u8		Tv_BitMask;
	// code --------------------
		//check able range
		if((E_DrThrdMsgCmdAlloc <= A_Cmd) && ((E_DrThrdMsgCmdAlloc + E_DrThrdMaxMsgCmdCnt) > A_Cmd))
		{
			//critical section enter
			DrPrcCritEnter(&V_DrThrdEnv.BaseCrit);

			//release
			Tv_BitIdx		=	A_Cmd - E_DrThrdMsgCmdAlloc;
			Tv_ByteIdx	=	Tv_BitIdx >> 3;
			Tv_BitIdx		=	Tv_BitIdx & 0x7;
			Tv_BitMask	=	1 << Tv_BitIdx;
			V_DrThrdEnv.CmdAlocTbl[Tv_ByteIdx]	=	V_DrThrdEnv.CmdAlocTbl[Tv_ByteIdx] & ~Tv_BitMask;

			//crit leave
			DrPrcCritLeave(&V_DrThrdEnv.BaseCrit);
		}
}
//--------------------------------------------------------------------
void	DrThrdMsgBroadCast(Ptr_DrThrdMsgHd A_PtrMsg, __u32 A_Size, BOOL8 A_IsFast)
{
	__u32		Tv_WkIdx;

	//crit
	DrPrcCritEnter(&V_DrThrdEnv.BaseCrit);

	//find exist item
	for(Tv_WkIdx=0; Tv_WkIdx < E_DrThrdObjMaxCnt; Tv_WkIdx++)
	{
		if(NULL != V_DrThrdEnv.ObjTbl[Tv_WkIdx].Obj)
		{
			((Cls_DrThrdObj*)V_DrThrdEnv.ObjTbl[Tv_WkIdx].Obj)->MsgPost(A_PtrMsg, A_Size, A_IsFast);
		}
	}

	//crit
	DrPrcCritLeave(&V_DrThrdEnv.BaseCrit);
}
//--------------------------------------------------------------------
void	DrThrdMsgSysErrNotify(__u32 A_ErrCode, __u32 A_ParaD1, __u32 A_ParaD2)
{
	// local -------------------
		St_DrThrdMsgSysErr	Tv_Msg;
	// code --------------------
		Tv_Msg.MsgHd.Cmd	=	E_DrThrdMsgSysErr;
		Tv_Msg.ErrCode		=	A_ErrCode;
		Tv_Msg.ParaD1			=	A_ParaD1;
		Tv_Msg.ParaD2			=	A_ParaD2;
		DrThrdMsgBroadCast((Ptr_DrThrdMsgHd)&Tv_Msg, sizeof(Tv_Msg));
}
//--------------------------------------------------------------------
__s32	DrThrdObjGetUnstable(char* A_StrRtName)
{
	__s32	Tv_Result;
	__u32	Tv_SvrIdx;

	Tv_Result	=	-1;

	for(Tv_SvrIdx = 0; Tv_SvrIdx < E_DrThrdObjMaxCnt; Tv_SvrIdx++)
	{
		if(NULL != V_DrThrdEnv.ObjTbl[Tv_SvrIdx].Obj)
		{
			if(V_DrThrdEnv.ObjTbl[Tv_SvrIdx].PrgsCnt == V_DrThrdEnv.ObjTbl[Tv_SvrIdx].PrgsChk)
			{
				// found
				Tv_Result		=	(__s32)Tv_SvrIdx;
				DrStrCopyAsc(A_StrRtName, V_DrThrdEnv.ObjTbl[Tv_SvrIdx].StrName);
				break;
			}
		}
	}

	return	Tv_Result;
}
//--------------------------------------------------------------------
