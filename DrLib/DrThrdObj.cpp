/*
	task server

*/

//====================================================================
//include
#include <DrPlatform.h>
#include <DrTypeBase.h>
#include <DrThrdObj.h>
#include <DrDumyTool.h>
#include <DrTimeTool.h>
#include <DrStrTool.h>
#include <DrDebug.h>

#ifdef LINUX_APP
#include <sys/signal.h>
#else
#include <stdlib.h>
#endif

//====================================================================
//local const

//====================================================================
//local type

//====================================================================
//global var

extern	St_DrThrdEnv	V_DrThrdEnv;

//====================================================================
//functions

Def_DrPrcThrdRslt	Def_DrPrcThrdClcb	DrThrdProc(Def_DrPrcThrdPara A_Para);

//--------------------------------------------------------------------
Cls_DrThrdObj::Cls_DrThrdObj(char* A_StrName, BOOL8 A_IsNewThrd)
{

	//init
	DrDumyZeroMem(&m_ThrdRtl, sizeof(m_ThrdRtl));

	m_ThrdRtl.IsAble			=	FALSE;
	m_ThrdRtl.IsRun		=	FALSE;
	m_ThrdRtl.IsReqQuit		=	FALSE;
	
	m_ThrdRtl.IsNewThrd		=	A_IsNewThrd;
	m_ThrdRtl.MsgPumpCnt	=	E_DrThrdDfltMsgPump;

	//make critical section
	DrPrcCritInit(&m_ThrdRtl.Crit);

	// init message queue
	DrBufVqInit((Ptr_DrBufVarQue)m_ThrdRtl.MsgBuf, E_DrThrdMsgBufSize, 4);
	//register
	m_ThrdRtl.SvrId	=	DrThrdObjReg(this);

	if(0 <= m_ThrdRtl.SvrId)
	{
		// set name
		DrStrCopyAsc(m_ThrdRtl.StrName, A_StrName, 63);
		DrStrCopyAsc(V_DrThrdEnv.ObjTbl[m_ThrdRtl.SvrId].StrName, A_StrName, 63);

		m_ThrdRtl.RestPeriod	=	E_DrThrdDfltRestPeriod;
		m_ThrdRtl.RestPrgs		=	0;

		//check create thread
		if(m_ThrdRtl.IsNewThrd)
		{
			DrPrcThrdCreate(&m_ThrdRtl.ThrdMng, DrThrdProc, (Def_DrPrcThrdPara)this);
		}
		else
		{
			m_ThrdRtl.IsAble = TRUE;
			m_ThrdRtl.IsRun = TRUE;

			V_DrThrdEnv.ObjTbl[m_ThrdRtl.SvrId].ThrdId	=	DrPrcThrdGetCurId();
		}

	}

	// regist command message
	m_ThrdRtl.CmdBase = DrThrdMsgCmdAlloc();

	
}
//--------------------------------------------------------------------
Cls_DrThrdObj::~Cls_DrThrdObj()
{
	//stop thread
	ThrdSafeQuit();

	//releas critical
	DrPrcCritDel(&m_ThrdRtl.Crit);

	DrThrdMsgCmdFree(m_ThrdRtl.CmdBase);

	//unregister
	DrThrdObjUnreg(this);

}
//--------------------------------------------------------------------
Def_DrPrcThrdRslt	Def_DrPrcThrdClcb	DrThrdProc(Def_DrPrcThrdPara A_Para)
{
	Cls_DrThrdObj*	Tv_ThrdObj;
	Tv_ThrdObj		=	(Cls_DrThrdObj*)A_Para;
	Tv_ThrdObj->RtlRun();

	return	0;
}
//--------------------------------------------------------------------
BOOL8	Cls_DrThrdObj::RtlMsgProc(Ptr_DrThrdMsgHd A_PtrMsg )
{
		return	FALSE;
}
//--------------------------------------------------------------------
BOOL8	Cls_DrThrdObj::RtlBkgProc( void )
{
		return	FALSE;
}
//--------------------------------------------------------------------
void	Cls_DrThrdObj::RtlTimer( __u32 A_Hndl )
{
}
//--------------------------------------------------------------------
Def_DrErrCode	Cls_DrThrdObj::LcProcMsg(Ptr_DrThrdMsgHd A_PtrMsg)
{
	Def_DrErrCode		Tv_Result;

	//init
	Tv_Result		=	E_DrErrNotFound;

	if (RtlMsgProc(A_PtrMsg))
	{
		Tv_Result		=	E_DrErrNone;
	}

	//mark finish code
	if (NULL != A_PtrMsg->PtrRtl)
	{
		if (E_DrErrNone == Tv_Result)
		{
			A_PtrMsg->PtrRtl->Stat	=	E_DrThrdMsgFcodeOk;		//mark finish
		}
		else
		{
			A_PtrMsg->PtrRtl->Stat	=	E_DrThrdMsgFcodeNoCli;		//not exist client
		}
	}

	return	Tv_Result;

}
//--------------------------------------------------------------------
BOOL8	Cls_DrThrdObj::LcProcTimer(void)
{
	BOOL8		Tv_Result;
	__u32		Tv_Tick;
	__u32		Tv_TmIdx;

	//init
	Tv_Result		=	FALSE;

	Tv_Tick		=	DrTimeGetTick();
	for (Tv_TmIdx=0; Tv_TmIdx < E_DrThrdMaxTimerCnt; Tv_TmIdx++)
	{
		// check use
		if (m_ThrdRtl.TimerTbl[Tv_TmIdx].IsUse)
		{
			// check period
			if (m_ThrdRtl.TimerTbl[Tv_TmIdx].Priod <= (Tv_Tick - m_ThrdRtl.TimerTbl[Tv_TmIdx].TickLast))
			{
				// run timer
				RtlTimer(Tv_TmIdx);
				Tv_Result	=	TRUE;

				//update time
				m_ThrdRtl.TimerTbl[Tv_TmIdx].TickLast	=	Tv_Tick;
			}
		}
	}

	return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8	Cls_DrThrdObj::RegistTimer(__u32 A_Priod, __u32* A_PtrRtHndl)
{
	BOOL8	Tv_Result;
	__u32	Tv_TmrIdx;

	// check leagal thread
	if (0 > m_ThrdRtl.SvrId)
	{
		return	FALSE;
	}

	Tv_Result	=	FALSE;

	// find empty timer slot
	for (Tv_TmrIdx = 0; Tv_TmrIdx < E_DrThrdMaxTimerCnt; Tv_TmrIdx++)
	{
		if (!m_ThrdRtl.TimerTbl[Tv_TmrIdx].IsUse)
		{
			// found
			m_ThrdRtl.TimerTbl[Tv_TmrIdx].Priod			=	DrTimeMsToTick(A_Priod);
			m_ThrdRtl.TimerTbl[Tv_TmrIdx].TickLast	=	DrTimeGetTick();
			m_ThrdRtl.TimerTbl[Tv_TmrIdx].IsUse			=	TRUE;

			*A_PtrRtHndl	=	Tv_TmrIdx;
			break;
		}
	}

	return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8	Cls_DrThrdObj::UnregistTimer(__u32 A_Hndl)
{
	if (E_DrThrdMaxTimerCnt <= A_Hndl)
	{
		return	FALSE;
	}
	// check exist slot
	if (!m_ThrdRtl.TimerTbl[A_Hndl].IsUse)
	{
		return	FALSE;
	}

	m_ThrdRtl.TimerTbl[A_Hndl].IsUse			=	FALSE;

	return	TRUE;
}
//--------------------------------------------------------------------
void Cls_DrThrdObj::ThrdSafeInit(void)
{
	m_ThrdRtl.IsAble = TRUE;
}
//--------------------------------------------------------------------
void	Cls_DrThrdObj::ThrdSafeQuit(void)
{

	if(m_ThrdRtl.IsNewThrd && m_ThrdRtl.IsAble)
	{
		ThrdReqQuit();

		while(m_ThrdRtl.IsRun)
		{
			DrPrcSleep(10);
		}
		DrPrcThrdRelease(&m_ThrdRtl.ThrdMng);
		m_ThrdRtl.IsAble	=	FALSE;
	}

}
//--------------------------------------------------------------------
char*	Cls_DrThrdObj::ThrdNameGet(void)
{

	char*	Tv_Result;

	Tv_Result	=	NULL;

	if (0 <= m_ThrdRtl.SvrId)
	{
		Tv_Result	=	V_DrThrdEnv.ObjTbl[m_ThrdRtl.SvrId].StrName;
	}

	return	Tv_Result;

}
//--------------------------------------------------------------------
Def_DrPrcThrdId	Cls_DrThrdObj::ThreadId(void)
{
	Def_DrPrcThrdId	Tv_Result;

	Tv_Result	=	0;

	if (0 <= m_ThrdRtl.SvrId)
	{
		Tv_Result	=	V_DrThrdEnv.ObjTbl[m_ThrdRtl.SvrId].ThrdId;
	}

	return	Tv_Result;

}
//--------------------------------------------------------------------
BOOL8	Cls_DrThrdObj::IsThrdRun(void)
{
	return	m_ThrdRtl.IsRun;
}
//--------------------------------------------------------------------
BOOL8	Cls_DrThrdObj::ThrdDoStep(void)
{
	BOOL8		Tv_Result;

	// check registered
	if (0 > m_ThrdRtl.SvrId)
	{
		return	FALSE;
	}

	//init
	Tv_Result		=	FALSE;
	//message
	if (RtlMsgPump())
	{
		Tv_Result	=	TRUE;
	}

	//background
	if(RtlBkgProc())
	{
		Tv_Result		=	TRUE;
	}

	//timer
	if (LcProcTimer())
	{
		Tv_Result		=	TRUE;
	}

	// update step
	V_DrThrdEnv.ObjTbl[m_ThrdRtl.SvrId].PrgsCnt ++;

	return	Tv_Result;

}
//--------------------------------------------------------------------
void	Cls_DrThrdObj::ThrdRun(void)
{
	BOOL8		Tv_IsDone;

	if (m_ThrdRtl.IsAble)
	{
		m_ThrdRtl.IsRun		=	TRUE;

		while (!m_ThrdRtl.IsReqQuit)
		{
			//process
			Tv_IsDone		=	ThrdDoStep();

			if (Tv_IsDone)
			{
				if (0 < m_ThrdRtl.RestPeriod)
				{
					m_ThrdRtl.RestPrgs ++;
					if (m_ThrdRtl.RestPrgs >= m_ThrdRtl.RestPeriod)
					{
						DrPrcSleep(10);
						m_ThrdRtl.RestPrgs	=	0;
					}
				}
			}
			else
			{
				DrPrcSleep(10);
				m_ThrdRtl.RestPrgs	=	0;
			}
		}
		//mark finish
		m_ThrdRtl.IsRun		=	FALSE;
	}

}
//--------------------------------------------------------------------
void	Cls_DrThrdObj::ThrdRestPeriodSet(__s32 A_Period)
{
	m_ThrdRtl.RestPeriod	=	A_Period;
	m_ThrdRtl.RestPrgs		=	0;
}
//--------------------------------------------------------------------
void	Cls_DrThrdObj::ThrdReqQuit(void)
{
	m_ThrdRtl.IsReqQuit	=	TRUE;
}
//--------------------------------------------------------------------
void	Cls_DrThrdObj::MsgPumpCntSet(__u32 A_Cnt)
{
	if (0 != A_Cnt)
	{
		m_ThrdRtl.MsgPumpCnt	=	A_Cnt;
	}
}
//--------------------------------------------------------------------
Def_DrErrCode	Cls_DrThrdObj::MsgSend(Ptr_DrThrdMsgHd A_PtrMsg, __u32 A_Size)
{
	Cls_DrThrdObj*	Tv_TskNow;
	Def_DrPrcThrdId	Tv_ThrdId;
	St_DrThrdMsgRtl	Tv_Mrtl;
	Ptr_DrThrdMsgHd	Tv_PtrMsg;

	if (0 > m_ThrdRtl.SvrId)
	{
		DbgMsgPrint((char*)"Cls_DrThrdObj::MsgSend - [BUG!] task not registered!\n");
		return	E_DrErrInitFail;
	}

	if (E_DrThrdMsgMaxSize < A_Size)
	{
		DbgMsgPrint((char*)"Cls_DrThrdObj::MsgSend - [BUG!] message size (%d) over \n", A_Size);
		return	E_DrErrBadParam;
	}

	// init
	A_PtrMsg->Size	=	A_Size;
	Tv_Mrtl.Stat		=	E_DrThrdMsgFcodeNone;

	// get my thread
	Tv_ThrdId		=	DrPrcThrdGetCurId();

	// check my task
	if (Tv_ThrdId == V_DrThrdEnv.ObjTbl[m_ThrdRtl.SvrId].ThrdId)
	{
		//direct process
		A_PtrMsg->PtrRtl	=	NULL;
		return	LcProcMsg(A_PtrMsg);
	}

	// far task
	Tv_TskNow		=	(Cls_DrThrdObj*)DrThrdObjFindByThrdId(Tv_ThrdId);

	//critical
	Tv_PtrMsg		=	NULL;
	if (DrPrcCritEnter(&m_ThrdRtl.Crit))
	{
		//allocate queue
		Tv_PtrMsg	=	(Ptr_DrThrdMsgHd)DrBufVqGetInPtr((Ptr_DrBufVarQue)m_ThrdRtl.MsgBuf, A_Size);
		if (NULL != Tv_PtrMsg)
		{
			DrDumyCopyMem((void*)Tv_PtrMsg, (void*)A_PtrMsg, A_Size);
			Tv_PtrMsg->PtrRtl		=	&Tv_Mrtl;
			DrBufVqIn((Ptr_DrBufVarQue)m_ThrdRtl.MsgBuf);
		}
		DrPrcCritLeave(&m_ThrdRtl.Crit);
	}	//if(DrPrcCritEnter(&m_ThrdRtl.Crit))

	// check message allocate fail
	if (NULL == Tv_PtrMsg)
	{
		return	E_DrErrBufFull;
	}

	//wait result
	while (E_DrThrdMsgFcodeNone == Tv_Mrtl.Stat)
	{
		if (NULL != Tv_TskNow)
		{
			if (!Tv_TskNow->RtlMsgPump())
			{
				DrPrcSleep(10);
			}
		}

		// check finish
		if (m_ThrdRtl.IsReqQuit)
		{
			break;
		}
	}

	// check fail
	if (E_DrThrdMsgFcodeOk != Tv_Mrtl.Stat)
	{
		return	E_DrErrBusy;
	}

	return	E_DrErrNone;

}
//--------------------------------------------------------------------
Def_DrErrCode	Cls_DrThrdObj::MsgPost(Ptr_DrThrdMsgHd A_PtrMsg, __u32 A_Size, BOOL8 A_IsFast, BOOL8 A_ShowMsg)
{
	Def_DrErrCode		Tv_Result;
	Def_DrPrcThrdId	Tv_ThrdId;
	Ptr_DrThrdMsgHd	Tv_PtrMsg;
	BOOL8		Tv_IsDirect;

	// check registered
	if (0 > m_ThrdRtl.SvrId)
	{
		DbgMsgPrint((char*)"Cls_DrThrdObj::MsgPost - task not registered!\n");
		return	E_DrErrInitFail;
	}

	//init
	Tv_Result		=	E_DrErrFail;

	A_PtrMsg->Size	=	A_Size;
	//check direct message
	Tv_IsDirect	=	FALSE;
	//check thread
	if (A_IsFast)
	{
		Tv_ThrdId		=	DrPrcThrdGetCurId();
		if (DrPrcThrdIsEqual(Tv_ThrdId, V_DrThrdEnv.ObjTbl[m_ThrdRtl.SvrId].ThrdId))
		{
			Tv_IsDirect		=	TRUE;
		}
	}

	//not need wait
	A_PtrMsg->PtrRtl	=	NULL;
	//check direct mode
	if (Tv_IsDirect)
	{
		//direct process
		Tv_Result		=	LcProcMsg(A_PtrMsg);
	}
	else
	{
		//post 
		//check message size
		if (E_DrThrdMsgMaxSize >= A_Size)
		{
			//critical
			if (DrPrcCritEnter(&m_ThrdRtl.Crit))
			{
				//allocate queue
				Tv_PtrMsg	=	(Ptr_DrThrdMsgHd)DrBufVqGetInPtr((Ptr_DrBufVarQue)m_ThrdRtl.MsgBuf, A_Size);
				if (NULL != Tv_PtrMsg)
				{
					DrDumyCopyMem((void*)Tv_PtrMsg, (void*)A_PtrMsg, A_Size);
					DrBufVqIn((Ptr_DrBufVarQue)m_ThrdRtl.MsgBuf);
					Tv_Result		=	E_DrErrNone;
				}
				else
				{
					if (A_ShowMsg)
					{
						DbgMsgPrint((char*)"Cls_DrThrdObj::MsgPost - Task %s msg %d buffer full\n",
							V_DrThrdEnv.ObjTbl[m_ThrdRtl.SvrId].StrName, A_PtrMsg->Cmd);
					}
				}
				DrPrcCritLeave(&m_ThrdRtl.Crit);
			}

		}
		else
		{
			if (A_ShowMsg)
			{
				DbgMsgPrint((char*)"Cls_DrThrdObj::MsgPost - Bad size message\n");
			}
		}
	}

	return	Tv_Result;

}
//--------------------------------------------------------------------
BOOL8	Cls_DrThrdObj::MsgExPost(__u32 A_Cmd, void* A_PtrData, __u32 A_SizeData)
{
	BOOL8		Tv_Result;
	Ptr_DrThrdMsgHd	Tv_PtrMsg;
	__u32		Tv_Size;
	void*		Tv_PtrPara;

		//init
	Tv_Result		=	FALSE;

	//post 
	//check message size
	if (E_DrThrdMsgMaxSize >= (A_SizeData + sizeof(St_DrThrdMsgHd)))
	{

		//critical
		if (DrPrcCritEnter(&m_ThrdRtl.Crit))
		{
			Tv_Size	=	A_SizeData + sizeof(St_DrThrdMsgHd);
			//allocate queue
			Tv_PtrMsg	=	(Ptr_DrThrdMsgHd)DrBufVqGetInPtr((Ptr_DrBufVarQue)m_ThrdRtl.MsgBuf, Tv_Size);
			if (NULL != Tv_PtrMsg)
			{
				//init head
				Tv_PtrMsg->Cmd		=	A_Cmd;
				Tv_PtrMsg->Size		=	Tv_Size;
				Tv_PtrMsg->PtrRtl	=	NULL;
				if (0 != A_SizeData)
				{
					Tv_PtrPara	=	(void*)((Def_DrCalPtr)Tv_PtrMsg + sizeof(St_DrThrdMsgHd));
					DrDumyCopyMem(Tv_PtrPara, (void*)A_PtrData, A_SizeData);
				}
				DrBufVqIn((Ptr_DrBufVarQue)m_ThrdRtl.MsgBuf);
				Tv_Result		=	TRUE;
			}
			else
			{
				DbgMsgPrint((char*)"Cls_DrThrdObj::MsgExPost - Message buffer full\n");
			}
			DrPrcCritLeave(&m_ThrdRtl.Crit);
		}
	}
	else
	{
		DbgMsgPrint((char*)"Cls_DrThrdObj::MsgExPost - Bad size message\n");
	}

	return	Tv_Result;

}
//--------------------------------------------------------------------
void	Cls_DrThrdObj::RtlRun(void)
{
	__u32 Tv_WkIdx;
	if (0 > m_ThrdRtl.SvrId)
	{
		DbgMsgPrint((char*)"Cls_DrThrdSvr::RtlRun - task not registered!\n");
		return;
	}
	//LINUX signal block
#ifdef LINUX_APP
	sigset(SIGPIPE, SIG_IGN);
#endif
	V_DrThrdEnv.ObjTbl[m_ThrdRtl.SvrId].ThrdId	=	DrPrcThrdGetCurId();
	//m_ThrdRtl.IsAble		=	TRUE;
	Tv_WkIdx = 0;
	while(!m_ThrdRtl.IsAble)
	{
		DrPrcSleep(100);
		Tv_WkIdx ++;
		if(50 < Tv_WkIdx)
		{
			DbgMsgPrint((char*)"[THRD] %s safe init fail!\n",m_ThrdRtl.StrName);
			break;
		}
	}

	// event create
	EvtThrdCreate();

	ThrdRun();

	// event destroy
	EvtThrdDestroy();

}
//--------------------------------------------------------------------
BOOL8	Cls_DrThrdObj::RtlMsgPump(void)
{
	BOOL8		Tv_Result;
	__u32		Tv_WkIdx;
	Ptr_DrThrdMsgHd	Tv_PtrMsg;
	__u8		Tv_MsgBuf[E_DrThrdMsgMaxSize];

	Tv_Result	=	FALSE;

	for (Tv_WkIdx = 0; Tv_WkIdx < m_ThrdRtl.MsgPumpCnt; Tv_WkIdx++)
	{
		Tv_PtrMsg	=	(Ptr_DrThrdMsgHd)DrBufVqGetOutPtr((Ptr_DrBufVarQue)m_ThrdRtl.MsgBuf);
		if (NULL == Tv_PtrMsg)
		{
			break;
		}
		if (E_DrThrdMsgMaxSize < Tv_PtrMsg->Size)
		{
			DbgMsgPrint((char*)"Cls_DrThrdObj::RtlMsgPump: message size over! - %d\n", Tv_PtrMsg->Size);
			DrBufVqOut((Ptr_DrBufVarQue)m_ThrdRtl.MsgBuf);		//release message
			continue;
		}
		DrDumyCopyMem(Tv_MsgBuf, Tv_PtrMsg, Tv_PtrMsg->Size);
		DrBufVqOut((Ptr_DrBufVarQue)m_ThrdRtl.MsgBuf);		//release message

		if (E_DrErrNone == LcProcMsg((Ptr_DrThrdMsgHd)Tv_MsgBuf))
		{
			Tv_Result		=	TRUE;
		}
	}

	return	Tv_Result;

}
//--------------------------------------------------------------------
void	Cls_DrThrdObj::EvtThrdCreate(void)
{

}
//--------------------------------------------------------------------
void	Cls_DrThrdObj::EvtThrdDestroy(void)
{

}
//--------------------------------------------------------------------

