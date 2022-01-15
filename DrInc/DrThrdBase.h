/*
	thread base
*/

//--------------------------------------------------------------------
//Environment conditional
#ifndef	_EPC_DrThrdBase
#define	_EPC_DrThrdBase

//====================================================================
// uses
#include <DrPlatform.h>
#include <DrTypeBase.h>
#include <DrProcess.h>
#include <DrError.h>

//====================================================================
//constance

// base
#define E_DrThrdMsgCmdTskBase				2					/* task server static message start */

#define	E_DrThrdMsgCmdSysBase				100

#define E_DrThrdMsgCmdUiBase				200				/* UI message start */

#define	E_DrThrdMsgCmdDevBase				300				/* device message */

#define E_DrThrdMsgCmdAlloc					0x8000		/* client allocate message start */

// control
#define E_DrThrdObjMaxCnt						512		/* max thread object count */

#define E_DrThrdTmHndlPdPos					16		/* priod position */

#define E_DrThrdMsgWaitStep					10		/* message process wait */

#define E_DrThrdMsgFcodeNone				0			/* wait */
#define E_DrThrdMsgFcodeOk					1			/* process success */
#define E_DrThrdMsgFcodeNoCli				2			/* not exist client */

#define E_DrThrdMaxMsgCmdCnt				1024	/* client message count */
#define E_DrThrdMsgCmdTblSize				(E_DrThrdMaxMsgCmdCnt/8)	/* allocate command table size */	


// system message
#define E_DrThrdMsgSysErr						(0	+ E_DrThrdMsgCmdSysBase)		/* system error : St_DrThrdMsgSysErr */
#define E_DrThrdMsgTimeUpdt					(1	+ E_DrThrdMsgCmdSysBase)		/* time update : St_DrThrdMsgTimeUpdt */
#define E_DrThrdMsgIpChg						(2	+ E_DrThrdMsgCmdSysBase)		/* network ip change : St_DrThrdMsgIpChg */
#define E_DrThrdMsgDiskFormatCpl		(3	+ E_DrThrdMsgCmdSysBase)		/* disk format completed : St_DrThrdMsgDiskFmtCpl */
#define E_DrThrdMsgDiskFormatPrgs		(4 + E_DrThrdMsgCmdSysBase)			/* disk format proDress : St_DrThrdMsgDiskFmtPrgs */
#define	E_DrThrdMsgSysInitCpl				(5	+ E_DrThrdMsgCmdSysBase)		/* system initialized : St_DrThrdMsgHd */
#define	E_DrThrdMsgGroupFsFix				(6	+ E_DrThrdMsgCmdSysBase)		/* Group disk file system fix : St_DrThrdMsgDrpDskFsFix */
#define	E_DrThrdMsgHddIoError				(7	+ E_DrThrdMsgCmdSysBase)		/* disk read write error : St_DrThrdMsgDskIoErr */
#define	E_DrThrdMsgNtpRslt					(8	+ E_DrThrdMsgCmdSysBase)		/* NTP result : St_DrThrdMsgNtpRslt */
#define	E_DrThrdMsgDiskFsUpgd				(9	+ E_DrThrdMsgCmdSysBase)		/* disk file system upDrade : St_DrThrdMsgDrpDskFsFix */

// UI message
#define E_DrThrdMsgUiMsg						(0	+ E_DrThrdMsgCmdUiBase)
#define E_DrThrdMsgUiEvt						(1	+ E_DrThrdMsgCmdUiBase)		/* UI event message - not use? */

// device message
#define E_DrThrdMsgDevCfgChg				(0	+ E_DrThrdMsgCmdDevBase)		/* device config changed : St_DrThrdMsgHd */
#define E_DrThrdMsgFsMinChg					(1	+ E_DrThrdMsgCmdDevBase)		/* filesystem min unit change : St_DrThrdMsgFsChg */
#define E_DrThrdMsgFsDayChg					(2	+ E_DrThrdMsgCmdDevBase)		/* filesystem day unit change : St_DrThrdMsgFsChg */

#define E_DrThrdMsgFwUpgdStart			(16	+ E_DrThrdMsgCmdDevBase)		/* firmware upDrade start : St_DrThrdMsgHd */
#define E_DrThrdMsgFwUpgdFinish			(17	+ E_DrThrdMsgCmdDevBase)		/* firmware upDrade finish : St_DrThrdMsgFwUpgdEnd */
#define E_DrThrdMsgFsStatChg				(19	+ E_DrThrdMsgCmdDevBase)		/* file system Stat changed : St_DrThrdMsgHd */
#define	E_DrThrdMsgAutoLogout				(20	+ E_DrThrdMsgCmdDevBase)		/* auto logout : St_DrThrdMsgHd */
#define	E_DrThrdMsgAppNotify				(21	+ E_DrThrdMsgCmdDevBase)		/* application notify : St_DrThrdMsgAppNotify */
#define	E_DrThrdMsgEmgcRec					(23	+ E_DrThrdMsgCmdDevBase)		/* emergency record : St_DrThrdMsgHd */
#define	E_DrThrdMsgRecGrpFull				(26	+ E_DrThrdMsgCmdDevBase)		/* record Group is full : St_DrThrdMsgRecGrpFull */
#define E_DrThrdMsgFwUpgdFail				(27	+ E_DrThrdMsgCmdDevBase)		/* firmware upDrade error : St_DrThrdMsgHd */


//#define E_DrThrdMsgQuadPosCfg				(0x001B	+ E_DrThrdMsgCmdDevBase)		/* quad position change : St_DrThrdMsgQuadPosChg */

// system error
#define	E_DrThrdSysErrRaidDiskMis		1		/* RAID disk id missmatch , para0 = Group id */
#define	E_DrThrdSysErrRaidBroken		2		/* RAID information broken , para0 = Group id */

//====================================================================
//global macro


//====================================================================
//global type

typedef struct St_DrThrdCtrl
{
	void*	Obj;
	Def_DrPrcThrdId	ThrdId;
	__u32	PrgsCnt;
	__u32	PrgsChk;
	Def_StrTag	StrName;		// thread name
}	*Ptr_DrThrdCtrl;


typedef struct St_DrThrdEnv
{
	__u32	ObjCnt;

	St_DrThrdCtrl		ObjTbl[E_DrThrdObjMaxCnt];

	St_DrPrcCritSct	BaseCrit;

	__u8	CmdAlocTbl[E_DrThrdMsgCmdTblSize];

}	*Ptr_DrThrdEnv;

typedef	struct St_DrThrdMsgRtl			/* message result */
{
	__u32		Stat;					//finish status
}	*Ptr_DrThrdMsgRtl;


typedef	struct St_DrThrdMsgHd 
{
	__u32		Cmd;													// message command
	__u32		Size;													// message size
	Ptr_DrThrdMsgRtl	PtrRtl;							// runtime pointer - do not touch user
}	*Ptr_DrThrdMsgHd;

typedef struct St_DrThrdMsgCmdHd
{
	St_DrThrdMsgHd	Hd;
	void*		PtrCaller;		// caller message pointer
	__u32		CmdId;
}	*Ptr_DrThrdMsgCmdHd;

typedef struct St_DrThrdMsgFsChg
{
	St_DrThrdMsgHd	MsgHd;
	__u32	FsId;		// file system id
	__u32	_rsvd0;
}	*Ptr_DrThrdMsgFsChg;

typedef struct St_DrThrdMsgIpChg
{
	St_DrThrdMsgHd	MsgHd;
	__u8	IfIdx;		// interface index
	__u8	_rsvd0;
	__u8	_rsvd1;
	__u8	_rsvd2;
}	*Ptr_DrThrdMsgIpChg;

typedef	struct St_DrThrdMsgSysErr
{
	St_DrThrdMsgHd	MsgHd;
	__u32		ErrCode;
	__u32		ParaD1;
	__u32		ParaD2;
}	*Ptr_DrThrdMsgSysErr;

typedef	struct St_DrThrdMsgDiskFmtCpl
{
	St_DrThrdMsgHd	MsgHd;
	__u8		GroupMap;
	__u8		DiskMap;
	BOOL8		IsSuccess;
	__u8		_rsvd0;
	Def_DrErrCode	ErrCode;
}	*Ptr_DrThrdMsgDiskFmtCpl;

typedef	struct St_DrThrdMsgDiskFmtPrgs
{
	St_DrThrdMsgHd	MsgHd;
	__u8		GroupMap;
	__u8		DiskMap;
	__u16		_rsvd0;
	__u32		Prgs;		// percent
}	*Ptr_DrThrdMsgDiskFmtPrgs;

typedef struct St_DrThrdMsgTimeUpdt
{
	St_DrThrdMsgHd	MsgHd;
	Def_DrTime	Time;
	__u32	Fcc;				// owner device FCC
}	*Ptr_DrThrdMsgTimeUpdt;

typedef struct St_DrThrdMsgFwUpgdEnd
{
	St_DrThrdMsgHd	MsgHd;
	BOOL8	IsReboot;
	__u8	ErrCode;
}	*Ptr_DrThrdMsgFwUpgdEnd;

typedef struct St_DrThrdMsgBkupSizeEstm
{
	St_DrThrdMsgHd	MsgHd;
	__u64	RsltSize;
}	*Ptr_DrThrdMsgBkupSizeEstm;

typedef struct St_DrThrdMsgBkupEnd
{
	St_DrThrdMsgHd	MsgHd;
	__u8	ErrCode;
	__u8	_rsvd0;
	__u8	_rsvd1;
	__u8	_rsvd2;
}	*Ptr_DrThrdMsgBkupEnd;

typedef struct St_DrThrdMsgBkupPrgs
{
	St_DrThrdMsgHd	MsgHd;
	__u8	Prgs;
	__u8	_rsvd0;
	__u8	_rsvd1;
	__u8	_rsvd2;
}	*Ptr_DrThrdMsgBkupPrgs;

typedef struct St_DrThrdMsgDrpDskFsFix
{
	St_DrThrdMsgHd	MsgHd;
	__u8		Type;		// 0 = disk , 1 = Group
	__u8		Index;	// index
	__u16		_rsvd1;
}	*Ptr_DrThrdMsgDrpDskFsFix;

typedef struct St_DrThrdMsgDskFsUpgd
{
	St_DrThrdMsgHd	MsgHd;
	__u8		Type;		// 0 = disk , 1 = Group
	__u8		Index;	// index
	__u16		_rsvd1;
	__u32		NewVer;
	__u32		OldVer;
}	*Ptr_DrThrdMsgDskFsUpgd;

typedef struct St_DrThrdMsgDskIoErr
{
	St_DrThrdMsgHd	MsgHd;
	__u8		DiskIdx;
	__u8		_rsvd0;
	__u16		_rsvd1;
}	*Ptr_DrThrdMsgDskIoErr;

typedef struct St_DrThrdMsgAppNotify
{
	St_DrThrdMsgHd	MsgHd;
	__u32	Para0;
	__u32	Para1;
}	*Ptr_DrThrdMsgAppNotify;

typedef struct St_DrThrdMsgRecTimeMapDone
{
	St_DrThrdMsgHd	MsgHd;
	__u32	UserData;
}	*Ptr_DrThrdMsgRecTimeMapDone;

typedef struct St_DrThrdMsgDdnsRslt
{
	St_DrThrdMsgHd	MsgHd;
	BOOL8		IsOk;
	__u8		_rsvd0;
	__u8		_rsvd1;
	__u8		_rsvd2;
}	*Ptr_DrThrdMsgDdnsRslt;

typedef struct St_DrThrdMsgFsLogGetDone
{
	St_DrThrdMsgHd	MsgHd;
	__u32	Hndl;
}	*Ptr_DrThrdMsgFsLogGetDone;

typedef struct St_DrThrdMsgRecGrpFull
{
	St_DrThrdMsgHd	MsgHd;
	__u8	DrpType;
	__u8		_rsvd0;
	__u8		_rsvd1;
	__u8		_rsvd2;
}	*Ptr_DrThrdMsgRecGrpFull;

/*
typedef struct St_DrThrdMsgQuadPosChg
{
	St_DrThrdMsgHd	MsgHd;
	St_DrRect	RcQuad;		// quad screen rect
}	*Ptr_DrThrdMsgQuadPosChg;
*/

typedef struct St_DrThrdMsgNtpRslt
{
	St_DrThrdMsgHd	MsgHd;
	BOOL8		IsOk;
	__u8		_rsvd0;
	__u8		_rsvd1;
	__u8		_rsvd2;
}	*Ptr_DrThrdMsgNtpRslt;

//====================================================================
//class

//====================================================================
//global function
	
__s32	DrThrdObjReg(void* A_Obj);
BOOL8	DrThrdObjUnreg(void* A_Obj);

void	DrThrdInit(void);
void	DrThrdFinish(void);

void*	DrThrdObjGetByName(char* A_StrName);
void*	DrThrdObjFindByThrdId(Def_DrPrcThrdId A_ThrdId, BOOL8 A_IsThrdSafe = TRUE);
BOOL8	DrThrdObjStepDoThis(void);
__u32	DrThrdMsgCmdAlloc(void);			//return = 0: failure
void	DrThrdMsgCmdFree(__u32 A_Cmd);
void	DrThrdMsgBroadCast(Ptr_DrThrdMsgHd A_PtrMsg, __u32 A_Size, BOOL8 A_IsFast = FALSE);
void	DrThrdMsgSysErrNotify(__u32 A_ErrCode, __u32 A_ParaD1 = 0, __u32 A_ParaD2 = 0);

__s32	DrThrdObjGetUnstable(char* A_StrRtName);

//====================================================================
//out Environment conditional
#endif
