/*
	socket header
*/

//--------------------------------------------------------------------
//Environment conditional
#ifndef	_DrSockHd
#define	_DrSockHd

//====================================================================
// uses
#include <DrPlatform.h>
#include <DrTypeBase.h>

#ifndef LINUX_APP
#include <WinSock2.h>
#endif

//====================================================================
//constance

#define E_DrSockErrNone						0
#define E_DrSockErrWouldblock			1
#define E_DrSockErrConnBroken			2
#define E_DrSockErrEtc						0x70000000

#define E_DrSockPrtcTypeIp				8		/* protocol type ip */

#if defined(_WIN64)
#define E_DrSockBadHandle				0xFFFFFFFFFFFFFFFF
#elif defined(_WIN32)
#define E_DrSockBadHandle				0xFFFFFFFF
#else
#define E_DrSockBadHandle				-1
#endif

#define E_DrSockIpv4AddrAny				INADDR_ANY
#define E_DrSockIpv4AddrSelf			0x0100007F
#define E_DrSockIpv4AddrBroad			0xFFFFFFFF

//====================================================================
//global macro

#define Mac_DrSockMakeIp(ch0, ch1, ch2, ch3)	\
	((__u32)(__u8)(ch0) | ((__u32)(__u8)(ch1) << 8) | \
	((__u32)(__u8)(ch2) << 16) | ((__u32)(__u8)(ch3) << 24 ))


//====================================================================
//global type
#if defined(_WIN32) || defined(_WIN64)

typedef	int	socklen_t;
typedef	SOCKET		Def_DrSock;			/* socket */

#else

typedef	__s32			Def_DrSock;			/* socket */

#endif

typedef	struct St_DrSockAddrPkt		/* address packet type  */
{
	__u16	Family;			/* Always AF_PACKET */
	__u16	Protocol;		/* Physical layer protocol */
	__s32   IfIdx;			/* Interface number */
	__u16	HdType;			/* Header type */
	__u8	PktType;		/* Packet type */
	__u8	AdrLen;     /* Length of address */
	__u8  AdrBuf[8];  /* Physical layer address */
}	*Ptr_DrSockAddrPkt;

typedef	struct St_DrSockIpHead
{
	__u8	HlenVer;		//high 4bit = ipv4(4) , low 4bit = dword count (5 = 20byte)
	__u8  Service;		//service type
	__u16  TtLen;			//total length
	__u16  Id;
	__u16  FragOfs;		//fragment offset
	__u8  Ttl;			
	__u8  Protocol;
	__u16  ChkSum;
	__u32 SrcIp;
	__u32 DstIp;
}	*Ptr_DrSockIpHead;

typedef	struct St_DrSockUdpHead 
{
	__u16	SrcPort;
	__u16	DstPort;
	__u16	Len;
	__u16	ChkSum;
}	*Ptr_DrSockUdpHead;

typedef	struct St_DrSockCsVtHead		//checksum virtual header 
{
	__u32	SrcIp;
	__u32	DstIp;
	__u8	Zero;
	__u8	Protocol;
	__u16	Len;				//length
}	*Ptr_DrSockCsVtHead;


//====================================================================
//class

//====================================================================
//global function


//====================================================================
//global variable

//====================================================================

//out Environment conditional
#endif


