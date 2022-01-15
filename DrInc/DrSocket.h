/*
	GAUSE socket 
*/

//--------------------------------------------------------------------
//Environment conditional
#ifndef	_EPC_DrSocket
#define	_EPC_DrSocket

//====================================================================
// uses

#include <DrPlatform.h>
#include <DrTypeBase.h>

#ifndef LINUX_APP

#include <WinSock2.h>
#include <WS2tcpip.h>

#endif


#ifdef LINUX_APP
	#include <unistd.h>
#include <sys/uio.h>

#endif


#include <DrSockHd.h>
#include <DrError.h>

#if defined(_WIN32) || defined(_WIN64)

#pragma	comment(lib, "ws2_32.lib")

#endif

//====================================================================
//constance

#if 0
#define E_DrSockConnOk					E_DrErrNone				/* connect ok */
#define E_DrSockConnWaiting			E_DrErrWaiting		/* waiting non block socket */
#define E_DrSockConnTimeOut			E_DrErrBusy				/* time out */
#endif

//====================================================================
//global macro


#if defined(_WIN32) || defined(_WIN64)
	#define DrSockClose(A_Hndl)	closesocket((A_Hndl))
	#define DrSockSend(A_Hndl,A_PtrBuf,A_Len)	send( (SOCKET)(A_Hndl), (char*)(A_PtrBuf), (int)(A_Len) , 0 )
	#define DrSockRecv(A_Hndl,A_PtrBuf,A_Len)	recv( (SOCKET)(A_Hndl), (char*)(A_PtrBuf), (int)(A_Len) , 0 )
#else
	#define DrSockClose(A_Hndl)	close((A_Hndl))
	#define DrSockSend(A_Hndl,A_PtrBuf,A_Len)	write( (A_Hndl), (A_PtrBuf), (A_Len) )
	#define DrSockRecv(A_Hndl,A_PtrBuf,A_Len)	read( (A_Hndl), (A_PtrBuf), (A_Len) )
#endif

#if defined(_WIN32) || defined(_WIN64)
#define	Def_DrSockIovPtr		CHAR*

typedef struct St_DrSockIov
{
	ULONG iov_len;     /* the length of the buffer */
	_Field_size_bytes_(len) CHAR FAR *iov_base; /* the pointer to the buffer */
}	*Ptr_DrSockIov;
#else
#define	St_DrSockIov	iovec

#define	Def_DrSockIovPtr		void*
#endif

//====================================================================
//global type

typedef struct St_DrSockDnsRrParsEnv
{
	void*	PtrRr;
	__u32	RrSize;
	__u32	Prgs;
}	*Ptr_DrSockDnsRrParsEnv;

typedef struct St_DrSockDnsRrParsQueryInfo
{
	void*	PtrName;
	__u16	Type;
	__u16	Class;
}	*Ptr_DrSockDnsRrParsQueryInfo;

//====================================================================
//class

//====================================================================
//global function
void		DrSockInit( void );
void		DrSockFinish( void );

Def_DrSock	DrSockOpen( BOOL8 A_IsUdp, BOOL8 A_IsNonblock, BOOL8 A_IsV6 );
BOOL8		DrSockSetListenMode( Def_DrSock A_Hndl, __u16 A_Port, BOOL8 A_IsV6, __u32 A_QueCnt = 5 );
void		DrSockSetCloseTimeOut( Def_DrSock A_Hndl, __u16 A_TimeOutSec = 0xFFFF );
void		DrSockSetBlockMode( Def_DrSock A_Hndl, BOOL8 A_IsNonBlock );
Def_DrSock	DrSockAccept( Def_DrSock A_HndlListen, BOOL8 A_IsNonblock, 
													void* A_PtrRtIp, __u16* A_PtrRtPort, BOOL8* A_PtrRtIsV6 );
Def_DrErrCode		DrSockConnect( Def_DrSock A_Hndl, void* A_PtrIp, __u16 A_Port, BOOL8 A_IsV6 );
Def_DrErrCode		DrSockGetErrCode( void );
BOOL8		DrSockGetMacAddr( __u8 A_DevIdx, __u8* A_PtrRt );
BOOL8		DrSockSetMacAddr(__u8 A_DevIdx, __u8* A_PtrMac);
BOOL8		DrSockGetMyIp( __u8 A_DevIdx, void* A_PtrRt, BOOL8 A_IsV6 );
BOOL8		DrSockGetMyNetmask( __u8 A_DevIdx, void* A_PtrRt, BOOL8 A_IsV6 );
BOOL8		DrSockGetMyGateway( __u8 A_DevIdx, void* A_PtrRt, BOOL8 A_IsV6 );
BOOL8		DrSockChkCloseByRecv( __s32 A_RecvVal );
void		DrSockTcpNodelySet( Def_DrSock A_Hndl, BOOL8 A_IsOn );
Def_DrErrCode		DrSockWaitConnect( Def_DrSock A_Hndl, __u32 A_TimeOut );

BOOL8		DrSockUdpPortAssign( Def_DrSock A_Hndl, __u16 A_Port,__u32 A_Ip = 0);
BOOL8		DrSockUdpIpAndPortAssign( Def_DrSock A_Hndl, void* A_PtrIp, __u16 A_Port, BOOL8 A_IsV6 );
__u32		DrSockUdpSend( Def_DrSock A_Hndl, void* A_PtrIp, __u16 A_Port, void* A_PtrData, __u32 A_SizeData, BOOL8 A_IsV6 );
__u32		DrSockUdpGatherSend(Def_DrSock A_Hndl, void* A_PtrIp, __u16 A_Port, St_DrSockIov* A_PtrIovs, __u32 A_IovCnt, BOOL8 A_IsV6);
__u32		DrSockUdpRead(Def_DrSock A_Hndl, void* A_PtrRtIp, __u16* A_PtrRtPort, void* A_PtrData, __u32 A_SizeData,
	 BOOL8 A_IsV6, Def_DrErrCode* A_PtrRtErrCode);
BOOL8		DrSockUdpBroad(Def_DrSock A_Hndl, __u8 A_DevIdx = 0);
BOOL8		DrSockUdpMcast(Def_DrSock A_Hndl, __u32 A_Ip);
BOOL8		DrSockUdpTtlSet(Def_DrSock A_Hndl, __u32 A_Ttl);

BOOL8		DrSockSetNetAdr( __u8 A_DevIdx, void* A_PtrIp, void* A_PtrNetMask, void* A_PtrGateWay, BOOL8 A_IsV6 );

BOOL8		DrSockIsLink( __s8 A_DevIdx );
BOOL8		DrSockNetIfActive( __s8 A_DevIdx, BOOL8 A_IsOn );
BOOL8		DrSockNetIfMtuSet( __s8 A_DevIdx, __s32 A_Mtu );
__u8		DrSockNetIfAdaptarIdGet( __u8 A_DevIdx );

__u16		DrSockCalcIpV4Chksum( void* A_PtrBuf, __u32 A_Size, void* A_PtrExt = NULL, __u32 A_ExtSize = 0 );

// new dns
Def_DrErrCode	DrSockUrlAscToIp(char* A_StrUrl,__u32* A_PtrRtIpV4, __u8* A_PtrRtIpV6);

// DNS parse
__u32	DrSockDnsRrNameInfo(Ptr_DrSockDnsRrParsEnv A_PtrEnv, void** A_PtrRtPtr);
BOOL8	DrSockDnsRrParsQuery(Ptr_DrSockDnsRrParsEnv A_PtrEnv, Ptr_DrSockDnsRrParsQueryInfo A_PtrRt);

// HTTP
Def_DrErrCode	DrSockHttpGetData(void* A_PtrIp, BOOL8 A_IsIpV6, __u16 A_Port, char* A_StrParam, void* A_PtrRtBuf, __u32 A_BufLen,__u32* A_PtrRtRecvLen);

// emulate
#ifdef LINUX_EMU

void DrSockEmuMyIpV4Set(__u32 A_IpV4);

#endif

//====================================================================
//global variable

//====================================================================

//out Environment conditional
#endif


