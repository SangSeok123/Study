/*
	Task client
*/

//--------------------------------------------------------------------
//Enviroment conditional
#ifndef	_EPC_DrThrdObj
#define	_EPC_DrThrdObj

//====================================================================
// uses
#include <DrPlatform.h>
#include <DrTypeBase.h>
#include <DrProcess.h>
#include <DrThrdBase.h>
#include <DrBufTool.h>

//====================================================================
//constance

#define E_DrThrdMsgBufSize						0x10000
#define E_DrThrdMsgMaxSize						128

#define E_DrThrdMaxTimerCnt						16

#define	E_DrThrdDfltMsgPump						2

#define	E_DrThrdDfltRestPeriod				100


//====================================================================
//global macro

#define	DR_THRD_MSG_DEF		virtual	BOOL8	RtlMsgProc( Ptr_DrThrdMsgHd A_PtrMsg )

#define	DR_THRD_MSG_BEGIN(classname)	BOOL8 classname::RtlMsgProc(Ptr_DrThrdMsgHd A_PtrMsg) { BOOL8 Tv_Result = FALSE; if (0 == A_PtrMsg->Cmd){return FALSE;}
#define	DR_THRD_MSG_INHERIT(classname,parentclass)	BOOL8 classname::RtlMsgProc(Ptr_DrThrdMsgHd A_PtrMsg) { BOOL8 Tv_Result = FALSE; if (parentclass::RtlMsgProc(A_PtrMsg)){return TRUE;}
#define	DR_THRD_MSG_END	return Tv_Result;}

#define	DR_THRD_MPROC_CMDID(cmdid,procfunc,stmsg)	else if((m_ThrdRtl.CmdBase == A_PtrMsg->Cmd) && (cmdid == ((Ptr_DrThrdMsgCmdHd)A_PtrMsg)->CmdId)) \
	{	if (NULL == ((Ptr_DrThrdMsgCmdHd)A_PtrMsg)->PtrCaller) { procfunc((stmsg*)A_PtrMsg); } \
	else { procfunc((stmsg*)(((Ptr_DrThrdMsgCmdHd)A_PtrMsg)->PtrCaller)); } }
#define	DR_THRD_MPROC_NP_CMDID(cmdid,procfunc)	else if((m_ThrdRtl.CmdBase == A_PtrMsg->Cmd) && (cmdid == ((Ptr_DrThrdMsgCmdHd)A_PtrMsg)->CmdId)) { procfunc(); }
#define	DR_THRD_MPROC_MSGID(msgid,procfunc,stmsg)	else if(msgid == A_PtrMsg->Cmd) \
	{	procfunc((stmsg*)A_PtrMsg); }



#define	DR_THRD_DEF_CMDID(funcname,stmsg)  Def_DrErrCode funcname(stmsg* A_PtrMsg)
#define	DR_THRD_DEF_NP_CMDID(funcname)  Def_DrErrCode funcname(void)

#define	DR_THRD_IMP_SEND_CMDID(classname,funcname,cmdid,stmsg)  Def_DrErrCode classname::funcname(stmsg* A_PtrMsg)\
	{St_DrThrdMsgCmdHd	Tv_Msg;\
	Tv_Msg.Hd.Cmd = m_ThrdRtl.CmdBase;\
	Tv_Msg.CmdId = cmdid;\
	Tv_Msg.PtrCaller = (void*)A_PtrMsg;\
	return MsgSend((Ptr_DrThrdMsgHd)&Tv_Msg, sizeof(Tv_Msg));}

#define	DR_THRD_IMP_POST_CMDID(classname,funcname,cmdid,stmsg)  Def_DrErrCode classname::funcname(stmsg* A_PtrMsg)\
	{((Ptr_DrThrdMsgCmdHd)A_PtrMsg)->Hd.Cmd = m_ThrdRtl.CmdBase;\
	((Ptr_DrThrdMsgCmdHd)A_PtrMsg)->CmdId = cmdid;\
	((Ptr_DrThrdMsgCmdHd)A_PtrMsg)->PtrCaller = NULL;\
	return MsgPost((Ptr_DrThrdMsgHd)A_PtrMsg, sizeof(stmsg));}

#define	DR_THRD_IMP_NP_SEND_CMDID(classname,funcname,cmdid)  Def_DrErrCode classname::funcname(void)\
	{St_DrThrdMsgCmdHd	Tv_Msg;\
	Tv_Msg.Hd.Cmd = m_ThrdRtl.CmdBase;\
	Tv_Msg.CmdId = cmdid;\
	Tv_Msg.PtrCaller = NULL;\
	return MsgSend((Ptr_DrThrdMsgHd)&Tv_Msg, sizeof(Tv_Msg));}

#define	DR_THRD_IMP_NP_POST_CMDID(classname,funcname,cmdid)  Def_DrErrCode classname::funcname(void)\
	{St_DrThrdMsgCmdHd	Tv_Msg;\
	Tv_Msg.Hd.Cmd = m_ThrdRtl.CmdBase;\
	Tv_Msg.CmdId = cmdid;\
	Tv_Msg.PtrCaller = NULL;\
	return MsgPost((Ptr_DrThrdMsgHd)&Tv_Msg, sizeof(Tv_Msg));}


//====================================================================
//global type

typedef	struct St_DrThrdTimerReg
{
	BOOL8		IsUse;
	__u32		Priod;
	__u32		TickLast;
}	*Ptr_DrThrdTimerReg;

typedef struct St_DrThrdRtl
{
	__u32		CmdBase;

	BOOL8		IsAble;
	BOOL8		IsRun;
	BOOL8		IsReqQuit;
	BOOL8		IsNewThrd;

	St_DrPrcThrd	ThrdMng;
	St_DrPrcCritSct	Crit;			//critical section

	Def_StrTag	StrName;
	__s32		SvrId;						// task server id

	// rest period
	__u32		RestPeriod;
	__u32		RestPrgs;

	// message 
	__u32		MsgPumpCnt;
	__u8		MsgBuf[E_DrThrdMsgBufSize];

	// timer 
	St_DrThrdTimerReg	TimerTbl[E_DrThrdMaxTimerCnt];

}*Ptr_DrThrdRtl;


//====================================================================
//class
class	Cls_DrThrdObj
{
	protected:
		St_DrThrdRtl	m_ThrdRtl;

		// function
		Def_DrErrCode	LcProcMsg(Ptr_DrThrdMsgHd A_PtrMsg);
		BOOL8	LcProcTimer(void);

		// api
		BOOL8	RegistTimer(__u32 A_Priod, __u32* A_PtrRtHndl);
		BOOL8	UnregistTimer(__u32 A_Hndl);

		void ThrdSafeInit(void);
		void ThrdSafeQuit(void);

		// event
		virtual	void	EvtThrdCreate(void);
		virtual	void	EvtThrdDestroy(void);

public:
		Cls_DrThrdObj(char* A_StrName = NULL, BOOL8 A_IsNewThrd = TRUE);
		virtual	~Cls_DrThrdObj();

		//runtime
		virtual	BOOL8	RtlMsgProc( Ptr_DrThrdMsgHd A_PtrMsg );		/* return = message processed */
		virtual	BOOL8	RtlBkgProc( void );											/* return = background processed */
		virtual	void	RtlTimer( __u32 A_Hndl );

		// information
		char*	ThrdNameGet(void);
		Def_DrPrcThrdId	ThreadId(void);
		BOOL8	IsThrdRun(void);		// now status run

		// maintenance
		BOOL8	ThrdDoStep(void);		//one setup process
		void	ThrdRun(void);			//task message loop with break time
		void	ThrdRestPeriodSet(__s32 A_Period);
		void	ThrdReqQuit(void);
		void	MsgPumpCntSet(__u32 A_Cnt);

		//message
		Def_DrErrCode	MsgSend(Ptr_DrThrdMsgHd A_PtrMsg, __u32 A_Size);
		Def_DrErrCode	MsgPost(Ptr_DrThrdMsgHd A_PtrMsg, __u32 A_Size, BOOL8 A_IsFast = FALSE, BOOL8 A_ShowMsg = TRUE);
		BOOL8	MsgExPost(__u32 A_Cmd, void* A_PtrData = NULL, __u32 A_SizeData = 0);

		//runtime - disable user
		void	RtlRun(void);
		BOOL8	RtlMsgPump(void);

};

//====================================================================
//global function
	
//====================================================================
//out Enviroment conditional
#endif
