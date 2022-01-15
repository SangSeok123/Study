/*
	socket library
		ip struct : 192.168.0.1   [0]192[1]168[2]0[3]1
*/

#include	<DrSocket.h>
#ifdef LINUX_APP

	#include	<asm/types.h>

#include	<sys/types.h> 
#include	<sys/socket.h>
#include	<net/if.h>

	#include	<errno.h>
	#include	<arpa/inet.h>
	#include	<fcntl.h>
	#include	<errno.h>
	#include	<netdb.h>

	#include	<netinet/in.h>
	#include	<netinet/ether.h> 
	#include	<net/ethernet.h> 

	#include	<sys/time.h> 
	#include	<unistd.h> 
	#include	<sys/ioctl.h> 
	#include	<sys/utsname.h>

	#include	<linux/sockios.h>
	#include	<net/route.h>
	#include	<linux/netlink.h>
	#include	<linux/rtnetlink.h>

	#include	<stdlib.h>

	#undef _XOPEN_SOURCE
	#define _XOPEN_SOURCE	500
	#include <signal.h>

	#include <resolv.h>

#else

#endif

#include <DrStrTool.h>
#include <DrFileCtrl.h>
#include <DrFileTool.h>
#include <DrTimeTool.h>
#include <DrDumyTool.h>
#include <DrDebug.h>
#include <DrThrdObj.h>

#include <DrError.h>

//====================================================================
//local const

//====================================================================
//local type

#ifdef LINUX_APP
struct in6_ifreq {
	struct in6_addr ifr6_addr;
	__u32 ifr6_prefixlen;
	unsigned int ifr6_ifindex;
};
#endif

//====================================================================
//global var

#ifdef LINUX_EMU

__u32 V_DrSockEmuMyIpV4 = 0;

#endif

//====================================================================
//functions

//--------------------------------------------------------------------
Def_DrErrCode		DrSockGetErrCode( void )
{
	// local -------------------
		__u32		Tv_Result;
		__s32		Tv_Err;
	// code --------------------
		//init
		Tv_Result		=	E_DrSockErrNone;

		#ifdef LINUX_APP
			Tv_Err	=	errno;
			switch( Tv_Err )
			{
				case EWOULDBLOCK:
					Tv_Result		=	E_DrSockErrWouldblock;
					break;
				case	EPIPE:
					Tv_Result		=	E_DrSockErrConnBroken;
					DbgMsgPrint("DrSockGetErrCode: epipe broken.\n" );
					break;
				case	ECONNRESET:
					Tv_Result		=	E_DrSockErrConnBroken;
					DbgMsgPrint("DrSockGetErrCode: reset peer conn.\n" );
					break;
				default:
					DbgMsgPrint("DrSockGetErrCode: unknown error code = %d\n", Tv_Err );
					Tv_Result		=	E_DrSockErrEtc;
			    break;
			}
		#else
			Tv_Err	=	WSAGetLastError();
			switch ( Tv_Err )
			{
				case WSAEWOULDBLOCK:
					Tv_Result		=	E_DrSockErrWouldblock;
					break;
				case WSAECONNRESET:
					Tv_Result		=	E_DrSockErrConnBroken;
					break;
				default:
					Tv_Result		=	E_DrSockErrEtc;
					DbgMsgPrint((char*)"Winsock error %d\n", Tv_Err);
			    break;
			}
		#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
Def_DrSock	DrSockOpen( BOOL8 A_IsUdp, BOOL8 A_IsNonblock, BOOL8 A_IsV6 )
{
	// local -------------------
		Def_DrSock	Tv_Result;
		__s32				Tv_Type;
		__s32				Tv_Af;
		#ifdef LINUX_APP
			__s32			Tv_Atb;
		#else
		u_long		Tv_Val;
		#endif
	// code --------------------
		Tv_Type			=	SOCK_STREAM;
		if ( A_IsUdp )
		{
			Tv_Type			=	SOCK_DGRAM;
		}
		Tv_Af			=	AF_INET;
		if ( A_IsV6 )
		{
			Tv_Af			=	AF_INET6;
		}
		Tv_Result	=	socket( Tv_Af, Tv_Type, 0 );
		if ( E_DrSockBadHandle != Tv_Result )
		{
			if ( A_IsNonblock )
			{
				//set non blocking socket
				#ifdef LINUX_APP
					Tv_Atb	=	fcntl(Tv_Result, F_GETFL, 0 );
					Tv_Atb	=	Tv_Atb | O_NONBLOCK;
					fcntl(Tv_Result, F_SETFL, Tv_Atb);
				#else
					Tv_Val		=	TRUE;
					ioctlsocket( Tv_Result, FIONBIO, &Tv_Val );
				#endif
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
void		DrSockSetBlockMode( Def_DrSock A_Hndl, BOOL8 A_IsNonBlock )
{
	// local -------------------
#ifdef LINUX_APP
		__s32			Tv_Atb;
#else
		__u32		Tv_Val;
#endif
	// code --------------------

		if ( A_IsNonBlock )
		{
			//set non blocking socket
#ifdef LINUX_APP
			Tv_Atb	=	fcntl(A_Hndl, F_GETFL, 0 );
			Tv_Atb	=	Tv_Atb | O_NONBLOCK;
			fcntl(A_Hndl, F_SETFL, Tv_Atb);
#else
			Tv_Val		=	TRUE;
			ioctlsocket( A_Hndl, FIONBIO, (u_long*)&Tv_Val );
#endif
		}
		else
		{
			//set blocking socket
#ifdef LINUX_APP
			Tv_Atb	=	fcntl(A_Hndl, F_GETFL, 0 );
			Tv_Atb	=	Tv_Atb & ~O_NONBLOCK;
			fcntl(A_Hndl, F_SETFL, Tv_Atb);
#else
			Tv_Val		=	FALSE;
			ioctlsocket( A_Hndl, FIONBIO, (u_long*)&Tv_Val );
#endif
		}
}
//--------------------------------------------------------------------
BOOL8		DrSockUdpPortAssign( Def_DrSock A_Hndl, __u16 A_Port, __u32 A_Ip)
{
	// local -------------------
		BOOL8		Tv_Result;
		sockaddr_in	Tv_NetAdr;
		int			Tv_IsReuse;
	// code --------------------
		//init
		Tv_Result		=	FALSE;

		//set mode
		Tv_IsReuse	=	TRUE;
		setsockopt( A_Hndl, SOL_SOCKET, SO_REUSEADDR, (char*)&Tv_IsReuse, sizeof(Tv_IsReuse) );

		DrDumyZeroMem(&Tv_NetAdr, sizeof(Tv_NetAdr));
		Tv_NetAdr.sin_family			=	AF_INET;
		Tv_NetAdr.sin_addr.s_addr	=	A_Ip;
		Tv_NetAdr.sin_port				=	htons(A_Port);
		if ( 0 <= bind( A_Hndl, (sockaddr*)&Tv_NetAdr, sizeof(Tv_NetAdr) ) )
		{
				Tv_Result		=	TRUE;
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrSockUdpIpAndPortAssign(Def_DrSock A_Hndl, void* A_PtrIp, __u16 A_Port, BOOL8 A_IsV6)
{
	// local -------------------
		BOOL8		Tv_Result;
		sockaddr_in	Tv_NetAdr;
		int			Tv_IsReuse;
#ifndef LINUX_APP
		int			Tv_Err;
#endif
	// code --------------------
		//init
		Tv_Result		=	FALSE;

		//set mode
		Tv_IsReuse	=	TRUE;
		setsockopt( A_Hndl, SOL_SOCKET, SO_REUSEADDR, (char*)&Tv_IsReuse, sizeof(Tv_IsReuse) );
		DrDumyZeroMem(&Tv_NetAdr, sizeof(Tv_NetAdr));
		Tv_NetAdr.sin_family			=	AF_INET;
		Tv_NetAdr.sin_addr.s_addr	=	*((__u32*)A_PtrIp);
		Tv_NetAdr.sin_port				=	A_Port;

		if ( 0 <= bind( A_Hndl, (sockaddr*)&Tv_NetAdr, sizeof(Tv_NetAdr) ) )
		{
			Tv_Result		=	TRUE;
		}
#ifndef LINUX_APP
		else
		{
			Tv_Err	=	WSAGetLastError();
			DbgMsgPrint((char*)"DrSockUdpIpAssign - bind error %d\n",Tv_Err);
		}
#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrSockSetListenMode(Def_DrSock A_Hndl, __u16 A_Port, BOOL8 A_IsV6, __u32 A_QueCnt)
{
	// local -------------------
		BOOL8		Tv_Result;
		int			Tv_IsReuse;
		int			Tv_RtCode;
	// code --------------------
		//init
		Tv_Result		=	FALSE;

		//set mode
		Tv_IsReuse	=	TRUE;
		setsockopt( A_Hndl, SOL_SOCKET, SO_REUSEADDR, (char*)&Tv_IsReuse, sizeof(Tv_IsReuse) );
		if ( A_IsV6 )
		{
			// IPV6
			sockaddr_in6	Tv_NetAdr;
			
			DrDumyZeroMem(&Tv_NetAdr, sizeof(Tv_NetAdr));

			Tv_NetAdr.sin6_family			=	AF_INET6;
			Tv_NetAdr.sin6_flowinfo		= 0;
			Tv_NetAdr.sin6_port				= htons(A_Port);
			Tv_NetAdr.sin6_addr				= in6addr_any;
			Tv_NetAdr.sin6_scope_id		= 0;		//if_nametoindex("eth0");

			Tv_RtCode	=	bind(A_Hndl, (sockaddr*)&Tv_NetAdr, sizeof(Tv_NetAdr));
		}
		else
		{
			// IPV4
			sockaddr_in	Tv_NetAdr;

			Tv_NetAdr.sin_family			=	AF_INET;
			Tv_NetAdr.sin_addr.s_addr	=	htonl(INADDR_ANY);
			Tv_NetAdr.sin_port				=	htons(A_Port);
			Tv_RtCode	=	bind(A_Hndl, (sockaddr*)&Tv_NetAdr, sizeof(Tv_NetAdr));
		}

		if ( 0 <=  Tv_RtCode)
		{
			if(0 <= listen(A_Hndl, A_QueCnt))
			{
				Tv_Result		=	TRUE;
			}
#ifdef LINUX_APP
			else
			{
				DbgMsgPrint("DrSockSetListenMode - listen error %d\n", errno);
			}
#endif
		}
#ifdef LINUX_APP
		else
		{
			DbgMsgPrint("DrSockSetListenMode - bind error %d\n", errno);
		}
#endif
		return	Tv_Result;
}
//--------------------------------------------------------------------
Def_DrSock	DrSockAccept( Def_DrSock A_HndlListen, BOOL8 A_IsNonblock, 
	void* A_PtrRtIp, __u16* A_PtrRtPort, BOOL8* A_PtrRtIsV6)
{
	// local -------------------
		Def_DrSock	Tv_Result;
		sockaddr_in6	Tv_AdrConn;
		socklen_t		Tv_RtSize;
		#ifdef LINUX_APP
			__s32			Tv_Atb;
			linger	Tv_Linger;
		#else
		u_long		Tv_Val;
		#endif
	// code --------------------
		*A_PtrRtIsV6	=	FALSE;
		Tv_RtSize			=	sizeof(Tv_AdrConn);
		DrDumyZeroMem(&Tv_AdrConn, sizeof(Tv_AdrConn));

		Tv_Result			=	accept( A_HndlListen, (sockaddr*)&Tv_AdrConn, &Tv_RtSize );
		if ( E_DrSockBadHandle != Tv_Result )
		{
			if ( A_IsNonblock )
			{
				//set non blocking socket
				#ifdef LINUX_APP
					Tv_Atb	=	fcntl(Tv_Result, F_GETFL, 0 );
					Tv_Atb	=	Tv_Atb | O_NONBLOCK;
					fcntl(Tv_Result, F_SETFL, Tv_Atb);
					/*
					Tv_Linger.l_onoff		=	TRUE;
					Tv_Linger.l_linger	=	0;
					setsockopt( Tv_Result, SOL_SOCKET, SO_LINGER, &Tv_Linger, sizeof(Tv_Linger) );	//fast close option
					*/
				#else
					Tv_Val		=	TRUE;
					ioctlsocket( Tv_Result, FIONBIO, (u_long*)&Tv_Val );
				#endif
			}

			//get ip address
			if(AF_INET == Tv_AdrConn.sin6_family)
			{
				sockaddr_in*	Tv_PtrAdrV4;

				Tv_PtrAdrV4		=	(sockaddr_in*)&Tv_AdrConn;

				*((__u32*)A_PtrRtIp)	=	Tv_PtrAdrV4->sin_addr.s_addr;
				*A_PtrRtPort	=	htons(Tv_PtrAdrV4->sin_port);
			}
			else if(AF_INET6 == Tv_AdrConn.sin6_family)
			{
				//DrDumyCopyMem(A_PtrRtIp, &Tv_AdrConn.sin6_addr, 16);
				*A_PtrRtPort	=	htons(Tv_AdrConn.sin6_port);
				*A_PtrRtIsV6	=	TRUE;
			}
			else
			{
				*((__u32*)A_PtrRtIp)	=	0;
				*A_PtrRtPort					=	0;
				DbgMsgPrint((char*)"DrSockAccept - unknown family type %d\n", Tv_AdrConn.sin6_family);
			}

		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
Def_DrErrCode		DrSockConnect( Def_DrSock A_Hndl, void* A_PtrIp, __u16 A_Port, BOOL8 A_IsV6 )
{
	Def_DrErrCode		Tv_Result;
	int			Tv_AdrSize;
	int			Tv_Err;

	//init
	Tv_Result		=	E_DrErrBusy;

	//try connect
	if(!A_IsV6)
	{
		// IPv4
		sockaddr_in	Tv_AdrConn;

		Tv_AdrConn.sin_family				=	AF_INET;
		Tv_AdrConn.sin_addr.s_addr	=	*((__u32*)A_PtrIp);
		Tv_AdrConn.sin_port					=	htons((__u16)A_Port);
		Tv_AdrSize									=	sizeof(Tv_AdrConn);
		if(0 <= connect(A_Hndl, (sockaddr*)&Tv_AdrConn, Tv_AdrSize))
		{
			return	E_DrErrNone;
		}
	}
	else
	{
		// IPv6
		sockaddr_in6	Tv_AdrConn;

		DrDumyZeroMem(&Tv_AdrConn, sizeof(Tv_AdrConn));

		Tv_AdrConn.sin6_family			=	AF_INET6;
		Tv_AdrConn.sin6_port				= htons(A_Port);
		DrDumyCopyMem(&Tv_AdrConn.sin6_addr, A_PtrIp,16);

		Tv_AdrSize									=	sizeof(Tv_AdrConn);

		if(0 <= connect(A_Hndl, (sockaddr*)&Tv_AdrConn, Tv_AdrSize))
		{
			return	E_DrErrNone;
		}
	}

#ifdef LINUX_APP
	Tv_Err	=	errno;
	if ( (EINPROGRESS == Tv_Err) || (EALREADY == Tv_Err) )
	{
		Tv_Result	=	E_DrErrWaiting;
	}
	else if ( (EISCONN == Tv_Err) )
	{
		Tv_Result		=	E_DrErrNone;
	}
	else
	{
		DbgMsgPrint((char*)"DrSockConnect - connect error - ip=%8X , err = %d \n", *((__u32*)A_PtrIp), Tv_Err );
	}
#else
	Tv_Err	=	WSAGetLastError();
	if ( (WSAEWOULDBLOCK == Tv_Err)  )
	{
		Tv_Result	=	E_DrErrWaiting;
	}
	else if ( WSAEISCONN == Tv_Err )
	{
		Tv_Result		=	E_DrErrNone;
	}
	else
	{
		DbgMsgPrint((char*)"DrSockConnect - connect error - ip=%8X , err = %d \n", *((__u32*)A_PtrIp), Tv_Err );
	}
#endif

	return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8	DrSockGetMacAddr( __u8 A_DevIdx, __u8* A_PtrRt )
{
	// local -------------------
		BOOL8			Tv_Result;
		#ifdef LINUX_APP
			__s32			Tv_Hndl;
			struct ifreq	Tv_IfReq; 
			__u8*		Tv_PtrData;
			__u32		Tv_WkIdx;
			__u8*		Tv_PtrTg;
		#endif
	// code --------------------
		//init
		Tv_Result		=	FALSE;
		
#ifdef LINUX_APP
			Tv_Hndl		=	socket( AF_INET, SOCK_STREAM, 0 );
			if ( 0 <= Tv_Hndl )
			{
				DrStrCopyAsc( Tv_IfReq.ifr_name, "eth0" );
				Tv_IfReq.ifr_name[3]	=	Tv_IfReq.ifr_name[3] + A_DevIdx;
				if ( 0 <= ioctl( Tv_Hndl, SIOCGIFHWADDR, &Tv_IfReq ) )
				{
					Tv_PtrData		=	(__u8*)( &Tv_IfReq.ifr_hwaddr.sa_data );
					Tv_PtrTg			=	A_PtrRt;
					for ( Tv_WkIdx=0;Tv_WkIdx < 6;Tv_WkIdx++ )
					{
						*Tv_PtrTg	=	*Tv_PtrData;
						Tv_PtrData ++;
						Tv_PtrTg ++;
					}
					Tv_Result	=	TRUE;
				}
				close( Tv_Hndl );
			}
#else
		A_PtrRt[0]	=	0x00;
		A_PtrRt[1]	=	0x18;
		A_PtrRt[2]	=	0xCF;
		A_PtrRt[3]	=	0xE8;
		A_PtrRt[4]	=	0x00;
		A_PtrRt[5]	=	0x10;

		Tv_Result		=	TRUE;
#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrSockSetMacAddr(__u8 A_DevIdx, __u8* A_PtrMac)
{
#ifdef LINUX_APP
	// local -------------------
		BOOL8	Tv_Result;
		__s32			Tv_Hndl;
		struct ifreq	Tv_IfReq; 
		__u8*		Tv_PtrData;
		__u32		Tv_WkIdx;
		__u8*		Tv_PtrTg;
	// code --------------------
		Tv_Result	=	FALSE;

		Tv_Hndl		=	socket(AF_INET, SOCK_STREAM, 0);
		if(0 <= Tv_Hndl)
		{
			DrStrCopyAsc(Tv_IfReq.ifr_name, "eth0");
			Tv_IfReq.ifr_name[3]	=	Tv_IfReq.ifr_name[3] + A_DevIdx;
			DrDumyCopyMem(Tv_IfReq.ifr_hwaddr.sa_data, A_PtrMac, 6);
			Tv_IfReq.ifr_hwaddr.sa_family = ARPHRD_ETHER;

			if(0 > ioctl(Tv_Hndl, SIOCSIFHWADDR, &Tv_IfReq))
			{
				DbgMsgPrint("DrSockSetMacAddr - change mac addres failure(%d)\n", errno);
			}
			else
			{
				Tv_Result	=	TRUE;
			}
			close(Tv_Hndl);
		}

		return	Tv_Result;
#else

		return	TRUE;
#endif
}
//--------------------------------------------------------------------
__u32		DrSockUdpSend( Def_DrSock A_Hndl, void* A_PtrIp, __u16 A_Port, void* A_PtrData, __u32 A_SizeData, BOOL8 A_IsV6 )
{
	// local -------------------
		__s32			Tv_Result;
		sockaddr_in	Tv_AdrConn;
		__u32		Tv_AdrSize;
	// code --------------------
		//set address
		DrDumyZeroMem(&Tv_AdrConn, sizeof(Tv_AdrConn));
		Tv_AdrConn.sin_family				=	AF_INET;
		Tv_AdrConn.sin_addr.s_addr	=	*((__u32*)A_PtrIp);
		Tv_AdrConn.sin_port					=	htons((__u16)A_Port);
		Tv_AdrSize		=	sizeof(Tv_AdrConn);
		
		#ifdef LINUX_APP
			Tv_Result		=	sendto( A_Hndl, A_PtrData, (size_t)A_SizeData, 0, 
										(sockaddr*)&Tv_AdrConn, (socklen_t)Tv_AdrSize );
		#else
			Tv_Result		=	sendto( A_Hndl, (char*)A_PtrData, (__s32)A_SizeData, 0, 
										(sockaddr*)&Tv_AdrConn, (__s32)Tv_AdrSize );
		#endif

		//patch result
		if ( 0 > Tv_Result )
		{
			Tv_Result		=	0;
		}
		
		return	(__u32)Tv_Result;
}
//--------------------------------------------------------------------
__u32		DrSockUdpGatherSend(Def_DrSock A_Hndl, void* A_PtrIp, __u16 A_Port, St_DrSockIov* A_PtrIovs, __u32 A_IovCnt, BOOL8 A_IsV6)
{
	__u32		Tv_Result;
	sockaddr_in	Tv_AdrConn;
	__u32		Tv_AdrSize;

	//set address
	DrDumyZeroMem(&Tv_AdrConn, sizeof(Tv_AdrConn));
	Tv_AdrConn.sin_family				=	AF_INET;
	Tv_AdrConn.sin_addr.s_addr	=	*((__u32*)A_PtrIp);
	Tv_AdrConn.sin_port					=	htons((__u16)A_Port);
	Tv_AdrSize		=	sizeof(Tv_AdrConn);


#ifdef LINUX_APP
	{
		ssize_t	Tv_RtCode;

		msghdr	Tv_Hdr;
		Tv_Hdr.msg_name				=	&Tv_AdrConn;
		Tv_Hdr.msg_namelen		=	sizeof(Tv_AdrConn);
		Tv_Hdr.msg_iov				=	A_PtrIovs;
		Tv_Hdr.msg_iovlen			=	A_IovCnt;
		Tv_Hdr.msg_control		=	NULL;
		Tv_Hdr.msg_controllen	=	0;
		Tv_Hdr.msg_flags			=	0;

		Tv_RtCode	=	sendmsg(A_Hndl, &Tv_Hdr, 0);
		Tv_Result	=	Tv_RtCode;
		if (0 > Tv_RtCode)
		{
			Tv_Result	=	0;
		}
	}
#else
	{
		DWORD	Tv_Sent;
		int		Tv_RtCode;

		Tv_RtCode	=	WSASendTo(A_Hndl, (LPWSABUF)A_PtrIovs, (DWORD)A_IovCnt, &Tv_Sent, 0, 
			(sockaddr*)&Tv_AdrConn,sizeof(Tv_AdrConn), NULL, NULL);
		if (0 != Tv_RtCode )
		{
			int		Tv_ErrCode;
			Tv_ErrCode		=	WSAGetLastError();
			DbgMsgPrint((char*)"error - %d\n", Tv_ErrCode);
			Tv_Sent		=	0;
		}
		Tv_Result	=	(__u32)Tv_Sent;
	}
#endif

	return	Tv_Result;
}
//--------------------------------------------------------------------
__u32		DrSockUdpRead(Def_DrSock A_Hndl, void* A_PtrRtIp, __u16* A_PtrRtPort, void* A_PtrData, __u32 A_SizeData,
	BOOL8 A_IsV6, Def_DrErrCode* A_PtrRtErrCode)
{
	// local -------------------
		__s32			Tv_Result;
		sockaddr_in	Tv_AdrConn;
		socklen_t		Tv_AdrSize;
	// code --------------------
		// init
		*A_PtrRtErrCode	=	E_DrErrNone;
		//set address
		Tv_AdrConn.sin_family				=	AF_INET;
		Tv_AdrConn.sin_addr.s_addr	=	INADDR_ANY;
		Tv_AdrConn.sin_port					=	0;
		Tv_AdrSize		=	sizeof(Tv_AdrConn);

		#ifdef LINUX_APP
			Tv_Result		=	recvfrom( A_Hndl, A_PtrData, (size_t)A_SizeData, 0, 
										(sockaddr*)&Tv_AdrConn, &Tv_AdrSize );
			if(0 > Tv_Result)
			{
				*A_PtrRtErrCode	=	E_DrErrBadParam;
			}
		#else
			Tv_Result		=	recvfrom( A_Hndl, (char*)A_PtrData, (int)A_SizeData, 0, 
										(sockaddr*)&Tv_AdrConn, (int*)&Tv_AdrSize );
			if(0 > Tv_Result)
			{
				int	Tv_RtCode;

				Tv_RtCode	=	WSAGetLastError();
				switch( Tv_RtCode )
				{
					case WSANOTINITIALISED:
						*A_PtrRtErrCode	=	E_DrErrInitFail;
						break;
					case WSAEMSGSIZE:
						*A_PtrRtErrCode	=	E_DrErrBufFull;
						break;
					default:
						*A_PtrRtErrCode	=	E_DrErrBadParam;
						break;
				}
			}
		#endif

		//patch result
		if ( 0 > Tv_Result )
		{
			Tv_Result		=	0;
		}
		else
		{
			//update ip
			*((__u32*)A_PtrRtIp)		=	(__u32)Tv_AdrConn.sin_addr.s_addr;
			// update port
			*A_PtrRtPort	=	(__u16)htons(Tv_AdrConn.sin_port);
		}

		return	(__u32)Tv_Result;
}
//--------------------------------------------------------------------
BOOL8	DrSockGetMyIp( __u8 A_DevIdx, void* A_PtrRt, BOOL8 A_IsV6 )
{
	// local -------------------
		BOOL8		Tv_Result;
	// code --------------------
		//init
		Tv_Result		=	FALSE;

#ifdef LINUX_APP
		{
			__u32		Tv_WkIdx;
			__s32		Tv_Hndl;
			char		Tv_StrIfn[8];

			if(A_IsV6)
			{
				// IPV6
				struct in6_ifreq	Tv_IfReq;
				struct sockaddr_in6*	Tv_PtrIpAdr;

				Tv_Hndl		=	socket(AF_INET6, SOCK_STREAM, 0);

				if(0 <= Tv_Hndl)
				{

					DrDumyZeroMem(&Tv_IfReq, sizeof(Tv_IfReq));
					Tv_PtrIpAdr	=	(struct sockaddr_in6 *) &Tv_IfReq.ifr6_addr;
					Tv_PtrIpAdr->sin6_family		=	AF_INET6;

					DrStrCopyAsc(Tv_StrIfn, "eth0");
					Tv_StrIfn[3]	=	Tv_StrIfn[3] + A_DevIdx;

					Tv_PtrIpAdr->sin6_scope_id	=	if_nametoindex(Tv_StrIfn);

					if(0 <= ioctl(Tv_Hndl, SIOCGIFADDR, &Tv_IfReq))
					{
						DrDumyCopyMem(A_PtrRt, &Tv_PtrIpAdr->sin6_addr, 16);
						Tv_Result	=	TRUE;
					}
					close(Tv_Hndl);
				}

			}
			else
			{
				// IPV4
				struct ifreq	Tv_IfReq;
				struct sockaddr_in*	Tv_PtrIpAdr;

				Tv_Hndl		=	socket(AF_INET, SOCK_STREAM, 0);

				if(0 <= Tv_Hndl)
				{
					DrStrCopyAsc(Tv_IfReq.ifr_name, "eth0");
					Tv_IfReq.ifr_name[3]	=	Tv_IfReq.ifr_name[3] + A_DevIdx;
					if(0 <= ioctl(Tv_Hndl, SIOCGIFADDR, &Tv_IfReq))
					{
						Tv_PtrIpAdr		=	(struct sockaddr_in*)&Tv_IfReq.ifr_ifru.ifru_addr;
						*((__u32*)A_PtrRt)	=	Tv_PtrIpAdr->sin_addr.s_addr;
						Tv_Result	=	TRUE;
					}
					close(Tv_Hndl);
				}

			}

		}

#else
		{

#if 1
			struct addrinfo	Tv_Hints;
			struct addrinfo*	Tv_PtrRes;
			struct addrinfo*	Tv_PtrWk;

			Def_StrTag	Tv_StrName;
			int		Tv_Err;

#ifdef LINUX_EMU

			if((!A_IsV6) && (0 != V_DrSockEmuMyIpV4))
			{
				*((__u32*)A_PtrRt)	= V_DrSockEmuMyIpV4;
				return TRUE;
			}

#endif

			gethostname(Tv_StrName, sizeof(Tv_StrName)- 1);

			DrDumyZeroMem(&Tv_Hints, sizeof(Tv_Hints));
			Tv_Hints.ai_socktype	=	SOCK_STREAM;
			Tv_Hints.ai_family		=	AF_INET;
			if( A_IsV6)
			{
				Tv_Hints.ai_family		=	AF_INET6;
			}

			Tv_Err	=	getaddrinfo(Tv_StrName, NULL, &Tv_Hints, &Tv_PtrRes);
			if(0 != Tv_Err)
			{
				return	FALSE;
			}

			Tv_PtrWk	=	Tv_PtrRes;

			while(NULL != Tv_PtrWk)
			{
				if(AF_INET == Tv_PtrWk->ai_family)
				{
					if(!A_IsV6)
					{
						sockaddr_in*	Tv_PtrAdr;
						Tv_PtrAdr		=	(sockaddr_in*)Tv_PtrWk->ai_addr;
						*((__u32*)A_PtrRt)	=	Tv_PtrAdr->sin_addr.S_un.S_addr;
						Tv_Result	=	TRUE;
						break;
					}
				}
				else if(AF_INET6 == Tv_PtrWk->ai_family)
				{
					if(A_IsV6)
					{
						sockaddr_in6*	Tv_PtrAdr;
						Tv_PtrAdr		=	(sockaddr_in6*)Tv_PtrWk->ai_addr;
						DrDumyCopyMem(A_PtrRt, Tv_PtrAdr->sin6_addr.u.Byte, 16);
						Tv_Result	=	TRUE;
						break;
					}
				}

				// next
				Tv_PtrWk	=	Tv_PtrWk->ai_next;
			}


#else
			Def_StrTag	Tv_StrName;
			hostent*	Tv_PtrEnt;
			char**		Tv_PtrAdr;
			__u32*		Tv_PtrIp;

			gethostname( Tv_StrName, sizeof(Tv_StrName) - 1 );
			Tv_PtrEnt	=	gethostbyname( Tv_StrName );
			if ( NULL != Tv_PtrEnt )
			{
				Tv_PtrAdr	=	Tv_PtrEnt->h_addr_list;
				if ( NULL != *Tv_PtrAdr )
				{
					Tv_PtrIp		=	(__u32*)*Tv_PtrAdr;
					*((__u32*)A_PtrRt)	=	*Tv_PtrIp;
					Tv_Result		=	TRUE;
				}
			}
#endif
		}
#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrSockGetMyNetmask( __u8 A_DevIdx, void* A_PtrRt, BOOL8 A_IsV6 )
{
	// local -------------------
		BOOL8		Tv_Result;
#ifdef LINUX_APP
		__s32			Tv_Hndl;
		struct ifreq	Tv_IfReq; 
		__u32		Tv_WkIdx;
		struct sockaddr_in*	Tv_PtrIpAdr;
#endif
	// code --------------------
		//init
		Tv_Result		=	FALSE;

	#ifdef LINUX_APP
		Tv_Hndl		=	socket( AF_INET, SOCK_STREAM, 0 );

		if ( 0 <= Tv_Hndl )
		{
			DrStrCopyAsc( Tv_IfReq.ifr_name, "eth0" );
			Tv_IfReq.ifr_name[3]	=	Tv_IfReq.ifr_name[3] + A_DevIdx;
			if ( 0 <= ioctl( Tv_Hndl, SIOCGIFNETMASK, &Tv_IfReq ) )
			{
				Tv_PtrIpAdr		=	(struct sockaddr_in*)&Tv_IfReq.ifr_ifru.ifru_addr;
				*((__u32*)A_PtrRt)	=	Tv_PtrIpAdr->sin_addr.s_addr;
				Tv_Result		=	TRUE;
			}
			close( Tv_Hndl );
		}
	#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrSockIsLink( __s8 A_DevIdx )
{
#ifdef LINUX_APP
	// local -------------------
		BOOL8	Tv_Result;
		__s32		Tv_Sock;
		struct  ifreq	Tv_IfReq;
	// code --------------------
		Tv_Result	=	FALSE;
		Tv_Sock	=	socket(AF_INET, SOCK_DGRAM,0);
		if ( 0 <= Tv_Sock )
		{
			//make name
			DrStrCopyAsc( Tv_IfReq.ifr_name, "eth0" );
			Tv_IfReq.ifr_name[3]	=	Tv_IfReq.ifr_name[3] + A_DevIdx;
			if ( ioctl(Tv_Sock, SIOCGIFFLAGS, &Tv_IfReq) >= 0 )
			{
				//check on
				if ( 0 != ( Tv_IfReq.ifr_flags & IFF_RUNNING ) )
				{
					Tv_Result	=	TRUE;
				}
			}

			//close socket
			close( Tv_Sock );
		}

		return	Tv_Result;
#else
		return	TRUE;
#endif
}
//--------------------------------------------------------------------
BOOL8		DrSockNetIfActive( __s8 A_DevIdx, BOOL8 A_IsOn )
{
#ifdef LINUX_APP
	// local -------------------
		BOOL8	Tv_Result;
		__s32		Tv_Sock;
		struct  ifreq	Tv_IfReq;
	// code --------------------
		Tv_Result	=	FALSE;
		Tv_Sock	=	socket(AF_INET, SOCK_DGRAM,0);
		if ( 0 <= Tv_Sock )
		{
			//make name
			DrStrCopyAsc( Tv_IfReq.ifr_name, "eth0" );
			Tv_IfReq.ifr_name[3]	=	Tv_IfReq.ifr_name[3] + A_DevIdx;
			if ( ioctl(Tv_Sock, SIOCGIFFLAGS, &Tv_IfReq) >= 0 )
			{
				if ( A_IsOn )
				{
					Tv_IfReq.ifr_flags |= (IFF_UP | IFF_RUNNING);
				}
				else
				{
					Tv_IfReq.ifr_flags &= ~IFF_UP;
				}
				DrStrCopyAsc( Tv_IfReq.ifr_name, "eth0" );
				Tv_IfReq.ifr_name[3]	=	Tv_IfReq.ifr_name[3] + A_DevIdx;
				ioctl(Tv_Sock, SIOCSIFFLAGS, &Tv_IfReq);
				Tv_Result	=	TRUE;
			}

			//close socket
			close( Tv_Sock );
		}

		return	Tv_Result;
#else
	return	TRUE;
#endif
}
//--------------------------------------------------------------------
BOOL8		DrSockNetIfMtuSet( __s8 A_DevIdx, __s32 A_Mtu )
{
	#ifdef LINUX_APP
	// local -------------------
		BOOL8	Tv_Result;
		__s32		Tv_Sock;
		struct  ifreq	Tv_IfReq;
	// code --------------------
		Tv_Result	=	FALSE;
		Tv_Sock	=	socket(AF_INET, SOCK_DGRAM,0);
		if ( 0 <= Tv_Sock )
		{
			//make name
			DrStrCopyAsc( Tv_IfReq.ifr_name, "eth0" );
			Tv_IfReq.ifr_name[3]	=	Tv_IfReq.ifr_name[3] + A_DevIdx;
			Tv_IfReq.ifr_mtu	=	A_Mtu;
			if ( 0 <= ioctl(Tv_Sock, SIOCSIFMTU, &Tv_IfReq) )
			{
				Tv_Result	=	TRUE;
			}
			//close socket
			close( Tv_Sock );
		}

		return	Tv_Result;
	#else
		return	TRUE;
	#endif
}
//--------------------------------------------------------------------
BOOL8		DrSockSetNetAdr( __u8 A_DevIdx, void* A_PtrIp, void* A_PtrNetMask, void* A_PtrGateWay, BOOL8 A_IsV6 )
{
	BOOL8	Tv_Result;

	Tv_Result	=	FALSE;

#if 0

	// command type
	{
		char	Tv_StrCmd[100];
		char* Tv_StrWk;

		if(!A_IsV6)
		{
			// ip v4

			// ip and net mask
			Tv_StrWk	=	DrStrCopyAsc(Tv_StrCmd, "ifconfig eth");
			DrStrIntToStrAsc(Tv_StrWk, (__s32)A_DevIdx);
			Tv_StrWk	=	DrStrCatAsc(Tv_StrWk, " ");
			DrStrIpToStrAsc(Tv_StrWk, *((__u32*)A_PtrIp));
			Tv_StrWk	=	DrStrCatAsc(Tv_StrWk, " netmask ");
			DrStrIpToStrAsc(Tv_StrWk, *((__u32*)A_PtrNetMask));

			DbgMsgPrint("[IPSET] %s \n", Tv_StrCmd);
#ifdef LINUX_APP
			system(Tv_StrCmd);
#endif

			Tv_StrWk	=	DrStrCopyAsc(Tv_StrCmd, "route add default gw ");
			DrStrIpToStrAsc(Tv_StrWk, *((__u32*)A_PtrGateWay));

			DbgMsgPrint("[GATEWAYSET] %s \n", Tv_StrCmd);
#ifdef LINUX_APP
			system(Tv_StrCmd);
#endif
			Tv_Result	=	TRUE;
		}
	}


#else
	// local -------------------
#ifdef LINUX_APP
		__s32		Tv_Rslt;
		__s32		Tv_Sock;
		struct  ifreq	Tv_IfReq;
		struct sockaddr_in*	Tv_PtrIpAdr;
		struct rtentry Tv_Route;
		struct sockaddr_in*	Tv_PtrRtDst;
		struct sockaddr_in*	Tv_PtrRtGw;
		struct sockaddr_in*	Tv_PtrRtMask;
#endif
	// code --------------------
#ifdef LINUX_APP
		Tv_Sock	=	socket(AF_INET, SOCK_DGRAM,0);
		if ( 0 <= Tv_Sock )
		{
			Tv_Result	=	TRUE;
			//ip set
			DrDumyZeroMem( &Tv_IfReq, sizeof(Tv_IfReq) );
			DrStrCopyAsc( Tv_IfReq.ifr_name, "eth0" );
			Tv_IfReq.ifr_name[3]	=	Tv_IfReq.ifr_name[3] + A_DevIdx;
			Tv_PtrIpAdr	=	(struct sockaddr_in*)&Tv_IfReq.ifr_ifru.ifru_addr;
			Tv_PtrIpAdr->sin_family = AF_INET;
			Tv_PtrIpAdr->sin_addr.s_addr = *((__u32*)A_PtrIp);
			Tv_PtrIpAdr->sin_port = 0;
			if ( ioctl(Tv_Sock, SIOCSIFADDR, &Tv_IfReq) < 0 )
			{
				DbgMsgPrint("DrSockSetNetAdr: IP ( %d.%d.%d.%d ) setting error\n", 
					*((__u8*)A_PtrIp), *((__u8*)((Def_DrCalPtr)A_PtrIp + 1)), *((__u8*)((Def_DrCalPtr)A_PtrIp + 2)), *((__u8*)((Def_DrCalPtr)A_PtrIp + 3)) );
				Tv_Result	=	FALSE;
			}
			else
			{
				DbgMsgPrint("DrSockSetNetAdr: IP ( %d.%d.%d.%d ) set.\n", 
					*((__u8*)A_PtrIp), *((__u8*)((Def_DrCalPtr)A_PtrIp + 1)), *((__u8*)((Def_DrCalPtr)A_PtrIp + 2)), *((__u8*)((Def_DrCalPtr)A_PtrIp + 3)) );
			}

			//NETMASK set
			DrDumyZeroMem( &Tv_IfReq, sizeof(Tv_IfReq) );
			DrStrCopyAsc( Tv_IfReq.ifr_name, "eth0" );
			Tv_IfReq.ifr_name[3]	=	Tv_IfReq.ifr_name[3] + A_DevIdx;
			Tv_PtrIpAdr	=	(struct sockaddr_in*)&Tv_IfReq.ifr_ifru.ifru_addr;
			Tv_PtrIpAdr->sin_family = AF_INET;
			Tv_PtrIpAdr->sin_addr.s_addr = *((__u32*)A_PtrNetMask);
			Tv_PtrIpAdr->sin_port = 0;
			if ( ioctl(Tv_Sock, SIOCSIFNETMASK, &Tv_IfReq) < 0 )
			{
				DbgMsgPrint("DrSockSetNetAdr: NETMASK ( %d.%d.%d.%d ) setting error\n" , 
					*((__u8*)A_PtrNetMask), *((__u8*)((Def_DrCalPtr)A_PtrNetMask + 1)), *((__u8*)((Def_DrCalPtr)A_PtrNetMask + 2)), *((__u8*)((Def_DrCalPtr)A_PtrNetMask + 3)) );
				Tv_Result	=	FALSE;
			}
			else
			{
				DbgMsgPrint("DrSockSetNetAdr: NETMASK ( %d.%d.%d.%d ) set\n" , 
					*((__u8*)A_PtrNetMask), *((__u8*)((Def_DrCalPtr)A_PtrNetMask + 1)), *((__u8*)((Def_DrCalPtr)A_PtrNetMask + 2)), *((__u8*)((Def_DrCalPtr)A_PtrNetMask + 3)) );
			}

			//gateway set
			DrDumyZeroMem( &Tv_Route, sizeof(Tv_Route) );
			Tv_PtrRtDst		=	(struct sockaddr_in*)(&(Tv_Route.rt_dst));
			Tv_PtrRtGw		= (struct sockaddr_in *)(&(Tv_Route.rt_gateway));
			Tv_PtrRtMask	= (struct sockaddr_in *)(&(Tv_Route.rt_genmask));
			Tv_PtrRtDst->sin_family		=	AF_INET;
			Tv_PtrRtGw->sin_family		=	AF_INET;
			Tv_PtrRtMask->sin_family	=	AF_INET;
			Tv_PtrRtDst->sin_addr.s_addr	=	0;
			Tv_PtrRtGw->sin_addr.s_addr		=	0;
			Tv_PtrRtMask->sin_addr.s_addr	=	0;
			Tv_Route.rt_flags = RTF_UP | RTF_GATEWAY;
			ioctl(Tv_Sock,SIOCDELRT,&Tv_Route);			//remove default gateway

			Tv_PtrRtDst->sin_addr.s_addr	= 0;
			Tv_PtrRtGw->sin_addr.s_addr		= *((__u32*)A_PtrGateWay);
			Tv_PtrRtMask->sin_addr.s_addr = 0;
			Tv_Route.rt_metric				=	1;
			Tv_Route.rt_flags = RTF_UP | RTF_GATEWAY;
			ioctl(Tv_Sock,SIOCDELRT,&Tv_Route);			//remove already exist gateway


			DrDumyZeroMem(&Tv_Route, sizeof(Tv_Route));
			Tv_PtrRtDst		=	(struct sockaddr_in*)(&(Tv_Route.rt_dst));
			Tv_PtrRtGw		= (struct sockaddr_in *)(&(Tv_Route.rt_gateway));
			Tv_PtrRtMask	= (struct sockaddr_in *)(&(Tv_Route.rt_genmask));
			Tv_PtrRtDst->sin_family		=	AF_INET;
			Tv_PtrRtGw->sin_family		=	AF_INET;
			Tv_PtrRtMask->sin_family	=	AF_INET;
			Tv_PtrRtDst->sin_addr.s_addr	=	INADDR_ANY;
			Tv_PtrRtGw->sin_addr.s_addr		= *((__u32*)A_PtrGateWay);
			Tv_PtrRtMask->sin_addr.s_addr	=	INADDR_ANY;
			Tv_Route.rt_flags = RTF_UP | RTF_GATEWAY;
			Tv_Route.rt_metric				=	0;

			Tv_Rslt	=	ioctl(Tv_Sock,SIOCADDRT,&Tv_Route);
			if( 0 > Tv_Rslt ) 
			{
				DbgMsgPrint("DrSockSetNetAdr: gateway ( %d.%d.%d.%d ) setting error %d \n", 
					*((__u8*)A_PtrGateWay), *((__u8*)((Def_DrCalPtr)A_PtrGateWay + 1)), *((__u8*)((Def_DrCalPtr)A_PtrGateWay + 2)), *((__u8*)((Def_DrCalPtr)A_PtrGateWay + 3)),
						Tv_Rslt );
				Tv_Result	=	FALSE;
			}
			else
			{
				DbgMsgPrint("DrSockSetNetAdr: gateway ( %d.%d.%d.%d ) set\n", 
					*((__u8*)A_PtrGateWay), *((__u8*)((Def_DrCalPtr)A_PtrGateWay + 1)), *((__u8*)((Def_DrCalPtr)A_PtrGateWay + 2)), *((__u8*)((Def_DrCalPtr)A_PtrGateWay + 3)) );
			}

			if ( Tv_Result )
			{
				DbgMsgPrint("DrSockSetNetAdr: network address change success.\n");
			}

			//close socket
			close( Tv_Sock );
		}
		else
		{
			DbgMsgPrint("DrSockSetNetAdr: socket create error.\n");
		}
#endif

#endif

	{
		St_DrThrdMsgIpChg	Tv_Msg;

		// notify network change
		Tv_Msg.MsgHd.Cmd	=	E_DrThrdMsgIpChg;
		Tv_Msg.IfIdx			=	A_DevIdx;
		DrThrdMsgBroadCast((Ptr_DrThrdMsgHd)&Tv_Msg, sizeof(Tv_Msg));

		return	Tv_Result;

	}
}
//--------------------------------------------------------------------
BOOL8		DrSockGetMyGateway( __u8 A_DevIdx, void* A_PtrRt, BOOL8 A_IsV6 )
{
	// local -------------------
		BOOL8	Tv_Result;
#ifdef LINUX_APP
		__s32		Tv_Sock;
		__s8	Tv_MsgBuf[8192];	
		struct nlmsghdr*	Tv_PtrNlMsg;		//NET LINK message
		struct rtmsg*			Tv_PtrRtMsg;		//route message
		__s32		Tv_MsgSeq;
		__s32		Tv_Pid;
		struct nlmsghdr*	Tv_PtrNlHdr;
		__s32		Tv_ReadLen;
		__s32		Tv_MsgLen;
		__s8*	Tv_PtrBuf;
#endif
	// code --------------------
		Tv_Result	=	FALSE;

#ifdef LINUX_APP
		Tv_Sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
		if ( 0 <= Tv_Sock )
		{
			DrDumyZeroMem( Tv_MsgBuf, sizeof(Tv_MsgBuf) );
			Tv_PtrNlMsg	=	(struct nlmsghdr*)Tv_MsgBuf;
			Tv_PtrRtMsg	=	(struct rtmsg*)NLMSG_DATA(Tv_PtrNlMsg);

			/* Fill in the Tv_PtrNlMsg header*/
			Tv_MsgSeq		=	0;
			Tv_Pid			=	getpid();
			Tv_PtrNlMsg->nlmsg_len		= NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
			Tv_PtrNlMsg->nlmsg_type		= RTM_GETROUTE;		// Get the routes from kernel routing table .
			Tv_PtrNlMsg->nlmsg_flags	= NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
			Tv_PtrNlMsg->nlmsg_seq		= Tv_MsgSeq++; // Sequence of the message packet.
			Tv_PtrNlMsg->nlmsg_pid		= Tv_Pid; // PID of process sending the request.

			/* Send the request */
			if( 0 <= send(Tv_Sock, Tv_PtrNlMsg, Tv_PtrNlMsg->nlmsg_len, 0))
			{
				/* Read the response */
				Tv_ReadLen	=	0;
				Tv_MsgLen		=	0;
				Tv_PtrBuf		=	Tv_MsgBuf;

				do
				{
					/* receive response from the kernel */
					Tv_ReadLen	=	recv(Tv_Sock, Tv_PtrBuf, sizeof(Tv_MsgBuf) - Tv_MsgLen, 0);
					if( 0 <= Tv_ReadLen )
					{
						Tv_PtrNlHdr = (struct nlmsghdr *)Tv_PtrBuf;
						/* Check if the header is valid */
						if( (NLMSG_OK(Tv_PtrNlHdr, Tv_ReadLen) == 0) || (Tv_PtrNlHdr->nlmsg_type == NLMSG_ERROR))
						{
							DbgMsgPrint("DrSockGetMyGateway:Error in packet!\n");
							Tv_MsgLen	=	-1;
							break;
						}
						/* Check if the its the last message */
						if(Tv_PtrNlHdr->nlmsg_type == NLMSG_DONE) 
						{
							break;
						}
						else
						{
							/* Else move the pointer to buffer appropriately */
							Tv_PtrBuf += Tv_ReadLen;
							Tv_MsgLen += Tv_ReadLen;
						}

						/* Check if its a multi part message */
						if((Tv_PtrNlHdr->nlmsg_flags & NLM_F_MULTI) == 0) 
						{
							/* return if its not */
							break;
						}
					}
					else
					{
						DbgMsgPrint("DrSockGetMyGateway:SockNlReadError\n");
						Tv_MsgLen	=	-1;
						break;
					}
				} while((Tv_PtrNlHdr->nlmsg_seq != Tv_MsgSeq) || (Tv_PtrNlHdr->nlmsg_pid != Tv_Pid));

				//check message read success
				if ( 0 <= Tv_MsgLen )
				{
					/* Parse the response */
					for(;NLMSG_OK(Tv_PtrNlMsg,Tv_MsgLen);Tv_PtrNlMsg = NLMSG_NEXT(Tv_PtrNlMsg,Tv_MsgLen))
					{
						struct rtmsg*		Tv_PtrWkRtMsg;
						struct rtattr*	Tv_PtrRtAttr;
						__s32			Tv_RtLen;
						__s8		Tv_TmpBuf[100];
						__u32		Tv_DstAdr;
						//__u32		Tv_GateWay;

						Tv_DstAdr		=	0xFFFFFFFF;
						//Tv_GateWay	=	0;

						Tv_PtrWkRtMsg = (struct rtmsg*)NLMSG_DATA(Tv_PtrNlMsg);

						/* If the route is not for AF_INET or does not belong to main routing table
						then return. */
						if((Tv_PtrWkRtMsg->rtm_family != AF_INET) || (Tv_PtrWkRtMsg->rtm_table != RT_TABLE_MAIN))
						{
							break;
						}
						/* get the RTATTR field */
						Tv_PtrRtAttr = (struct rtattr*)RTM_RTA(Tv_PtrWkRtMsg);
						Tv_RtLen = RTM_PAYLOAD(Tv_PtrNlMsg);
						for(;RTA_OK(Tv_PtrRtAttr,Tv_RtLen);Tv_PtrRtAttr = RTA_NEXT(Tv_PtrRtAttr,Tv_RtLen))
						{
							switch(Tv_PtrRtAttr->rta_type) 
							{
								/*
								case RTA_OIF:
									if_indextoname(*(__s32 *)RTA_DATA(rtAttr), rtInfo->ifName);
									break;
									*/
								case RTA_GATEWAY:
									*((__u32*)A_PtrRt)	=	*(__u32*)RTA_DATA(Tv_PtrRtAttr);
									break;
									/*
								case RTA_PREFSRC:
									rtInfo->srcAddr = *(u_int *)RTA_DATA(Tv_PtrRtAttr);
									break;
									*/
								case RTA_DST:
									Tv_DstAdr		=	*(__u32*)RTA_DATA(Tv_PtrRtAttr);
									break;
							}
						}
					}
				}
				else
				{
					DbgMsgPrint("DrSockGetMyGateway:Message read failure!\n");
				}
			}
			else
			{
				DbgMsgPrint("DrSockGetMyGateway: write socket fail!\n");
			}
			//close socket
			close(Tv_Sock);
		}
		else
		{
			DbgMsgPrint("DrSockGetMyGateway:socket open error!\n");
		}
#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrSockChkCloseByRecv( __s32 A_RecvVal )
{
	// local -------------------
		BOOL8	Tv_Result;
	// code --------------------
		Tv_Result	=	FALSE;
		if ( 0 == A_RecvVal )
		{
			Tv_Result	=	TRUE;
		}
		return	Tv_Result;
}
//--------------------------------------------------------------------
void		DrSockSetCloseTimeOut( Def_DrSock A_Hndl, __u16 A_TimeOutSec )
{
	// local -------------------
		linger	Tv_Linger;
	// code --------------------
		Tv_Linger.l_onoff		=	TRUE;
		Tv_Linger.l_linger	=	A_TimeOutSec;
		if ( 0xFFFF == A_TimeOutSec )
		{
			Tv_Linger.l_onoff		=	FALSE;
			Tv_Linger.l_linger	=	0;
		}
#ifdef LINUX_APP
		setsockopt( A_Hndl, SOL_SOCKET, SO_LINGER, &Tv_Linger, sizeof(Tv_Linger) );	//fast close option
#else
		setsockopt( A_Hndl, SOL_SOCKET, SO_LINGER, (char*)&Tv_Linger, sizeof(Tv_Linger) );	//fast close option
#endif
}
//--------------------------------------------------------------------
void		DrSockTcpNodelySet( Def_DrSock A_Hndl, BOOL8 A_IsOn )
{
	// local -------------------
		__s32		Tv_IsOn;
	// code --------------------
		Tv_IsOn	=	(__s32)A_IsOn;
	#ifdef LINUX_APP
		setsockopt( A_Hndl, SOL_SOCKET, SO_LINGER, &Tv_IsOn, sizeof(Tv_IsOn) );	//no delay
	#else
		setsockopt( A_Hndl, SOL_SOCKET, SO_LINGER, (char*)&Tv_IsOn, sizeof(Tv_IsOn) );	//no delay
	#endif
}
//--------------------------------------------------------------------
__u16		DrSockCalcIpV4Chksum( void* A_PtrBuf, __u32 A_Size, void* A_PtrExt, __u32 A_ExtSize )
{
	// local -------------------
		__u32	Tv_Result;
#if 0
		__u16*	Tv_PtrWk;
		__s32		Tv_Last;
		__u8*	Tv_PtrBt;
#endif
		__u32		Tv_ValAdd;
		__u8*		Tv_PtrSr;
		__u8*		Tv_PtrAdd;
		__u32		Tv_Remain;
	// code --------------------
		Tv_Result	=	0;

		Tv_ValAdd	=	0;
		Tv_PtrAdd	=	(__u8*)&Tv_ValAdd;

		// check extention exist
		if((NULL != A_PtrExt) && (0 != A_ExtSize))
		{
			Tv_Remain	=	A_ExtSize;
			Tv_PtrSr	=	(__u8*)A_PtrExt;
			while(1 < Tv_Remain)
			{
				Tv_PtrAdd[1]	=	*Tv_PtrSr;
				Tv_PtrSr ++;
				Tv_PtrAdd[0]	=	*Tv_PtrSr;
				Tv_PtrSr ++;

				Tv_Result		=	Tv_Result + Tv_ValAdd;

				// next
				Tv_Remain	=	Tv_Remain - 2;
			}
		}

		// contents
		Tv_Remain	=	A_Size;
		Tv_PtrSr	=	(__u8*)A_PtrBuf;
		while(1 < Tv_Remain)
		{
			Tv_PtrAdd[1]	=	*Tv_PtrSr;
			Tv_PtrSr ++;
			Tv_PtrAdd[0]	=	*Tv_PtrSr;
			Tv_PtrSr ++;

			Tv_Result		=	Tv_Result + Tv_ValAdd;

			// next
			Tv_Remain	=	Tv_Remain - 2;
		}

		// sum upper 16bit
		while (0 != (Tv_Result & 0xFFFF0000))
		{
			Tv_Result	=	((Tv_Result >> 16) & 0xFFFF) + (Tv_Result & 0xFFFF);
		}
		Tv_Result	=	~Tv_Result;

		Tv_Result	=	DrDumyEndian2byteGet(&Tv_Result);

#if 0
		if ( (NULL != A_PtrExt) && (0 != A_ExtSize) )
		{
			Tv_Last		=	(__s32)A_ExtSize;
			Tv_PtrWk	=	(__u16*)A_PtrExt;
			while ( 1 < Tv_Last )
			{
				Tv_Result	=	Tv_Result + (__u32)(*Tv_PtrWk);
				//next
				Tv_PtrWk ++;
				Tv_Last	=	Tv_Last - 2;
			}
		}

		Tv_Last		=	(__s32)A_Size;
		Tv_PtrWk	=	(__u16*)A_PtrBuf;
		while (1 < Tv_Last)
		{
			Tv_Result	=	Tv_Result + (__u32)(*Tv_PtrWk);
			//next
			Tv_PtrWk ++;
			Tv_Last	=	Tv_Last - 2;
		}
		
		//check last byte
		if ( 0 != Tv_Last )
		{
			Tv_PtrBt	=	(__u8*)Tv_PtrWk;
			Tv_Result	=	Tv_Result + (__u32)(*Tv_PtrBt);
		}
		//fold 32bit sum to 16bit
		while ( 0 != (0xFFFF0000 & Tv_Result) )
		{
			Tv_Result	=	(Tv_Result & 0xFFFF) + (Tv_Result >> 16);
		}

		Tv_Result	=	(~Tv_Result) & 0xFFFF;
#endif
		return	(__u16)Tv_Result;
}
//--------------------------------------------------------------------
__u8		DrSockNetIfAdaptarIdGet( __u8 A_DevIdx )
{
	// var -------------------
		__u8		Tv_Result;
#ifdef LINUX_APP
		Def_DrSock	Tv_Hndl;
		struct ifreq		Tv_IfReq;
		struct sockaddr_in*		Tv_PtrNetAddr;
#endif
	// code ------------------
		//init
		Tv_Result		=	0xFF;
#ifdef LINUX_APP
		DrDumyZeroMem( &Tv_IfReq, sizeof(Tv_IfReq) );

		Tv_Hndl	=	socket( AF_INET, SOCK_RAW, IPPROTO_RAW );
		if ( E_DrSockBadHandle != Tv_Hndl )
		{
			Tv_IfReq.ifr_addr.sa_family = AF_INET;
			DrStrCopyAsc( Tv_IfReq.ifr_name, "eth0" );
			Tv_IfReq.ifr_name[3]	=	Tv_IfReq.ifr_name[3] + A_DevIdx;
			//get network interface index
			if ( 0 == ioctl(Tv_Hndl, SIOCGIFINDEX, &Tv_IfReq) )
			{
				Tv_Result	= Tv_IfReq.ifr_ifindex;
			}
		}

		DrSockClose( Tv_Hndl );
#endif
		return	Tv_Result;
}
//--------------------------------------------------------------------
Def_DrErrCode		DrSockWaitConnect( Def_DrSock A_Hndl, __u32 A_TimeOut )
{
	Def_DrErrCode	Tv_Result;
	struct timeval	Tv_TmVal;
	fd_set	Tv_FdSet;
	__s32		Tv_Rval;
	__s32		Tv_Err;

	Tv_Result	=	E_DrErrWaiting;
		
	//set timeout
	Tv_TmVal.tv_sec		=	0;
	Tv_TmVal.tv_usec	=	0;	
	if ( 0 != A_TimeOut )
	{
		Tv_TmVal.tv_sec		=	A_TimeOut / 1000;
		Tv_TmVal.tv_usec	=	(A_TimeOut % 1000) * 1000;
	}
	FD_ZERO( &Tv_FdSet );
	FD_SET( A_Hndl, &Tv_FdSet );
#if defined(_WIN32) || defined(_WIN64)
	Tv_Rval	=	select( 0, NULL, &Tv_FdSet, NULL, &Tv_TmVal );	// Window is not used any more nfds.
#else
	Tv_Rval	=	select( A_Hndl + 1, NULL, &Tv_FdSet, NULL, &Tv_TmVal );
#endif
	if ( 0 > Tv_Rval )
	{
		//check error
#ifdef LINUX_APP
		Tv_Err	=	errno;
		if ( (EINTR != Tv_Err) && (EINPROGRESS != Tv_Err) )
		{
			/* if (EINPROGRESS != Tv_Err) */
			{
				DbgMsgPrint((char*)"DrSockWaitConnect : error - err = %d \n", Tv_Err );
			}
			Tv_Result	=	E_DrErrWaiting;
		}
#else
		Tv_Err	=	WSAGetLastError();
		if ( WSAEWOULDBLOCK != Tv_Err )
		{
			DbgMsgPrint((char*)"DrSockWaitConnect : error - err = %d \n", Tv_Err );
			Tv_Result	=	E_DrErrWaiting;
		}
#endif
	}
	else if ( 0 != Tv_Rval )
	{
		Tv_Result	=	E_DrErrNone;
	}

	return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrSockUdpBroad(Def_DrSock A_Hndl, __u8 A_DevIdx)
{
	//set mode
	int			Tv_IsBroad;
	Tv_IsBroad	=	1;
	if (0 > setsockopt(A_Hndl, SOL_SOCKET, SO_BROADCAST, (char*)&Tv_IsBroad, sizeof(Tv_IsBroad)))
	{
		return	FALSE;
	}

#ifdef LINUX_APP
	{
		struct ifreq	Tv_IfBind;
		DrStrCopyAsc(Tv_IfBind.ifr_name, "eth0");
		Tv_IfBind.ifr_name[3]	=	Tv_IfBind.ifr_name[3] + A_DevIdx;
		if (0 > setsockopt(A_Hndl, SOL_SOCKET, SO_BINDTODEVICE, (char *)&Tv_IfBind, sizeof(Tv_IfBind)))
		{
			DbgMsgPrint("DrSockUdpBroad : SO_BINDTODEVICE error\n");
			return	FALSE;
		}

	}
	
#endif

	return	TRUE;
}
//--------------------------------------------------------------------
BOOL8	DrSockUdpMcast(Def_DrSock A_Hndl, __u32 A_Ip)
{
	struct ip_mreq	Tv_JoinAdr;

#ifdef LINUX_APP
	Tv_JoinAdr.imr_multiaddr.s_addr	=	A_Ip;
	Tv_JoinAdr.imr_interface.s_addr	=	htonl(INADDR_ANY);
#else
	Tv_JoinAdr.imr_multiaddr.S_un.S_addr	=	A_Ip;
	Tv_JoinAdr.imr_interface.S_un.S_addr	=	htonl(INADDR_ANY);
#endif

	if (0 == setsockopt(A_Hndl, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&Tv_JoinAdr, sizeof(Tv_JoinAdr)))
	{
		return	TRUE;
	}
	
	return	FALSE;
}
//--------------------------------------------------------------------
BOOL8		DrSockUdpTtlSet(Def_DrSock A_Hndl, __u32 A_Ttl)
{
	int		Tv_Ttl;

	Tv_Ttl	=	(int)A_Ttl;

	if (0 == setsockopt(A_Hndl, IPPROTO_IP, IP_TTL, (char*)&Tv_Ttl, sizeof(Tv_Ttl)))
	{
		return	TRUE;
	}

	return	FALSE;
}
//--------------------------------------------------------------------
void		DrSockInit( void )
{
	// local -------------------
#ifndef LINUX_APP
		WSADATA		Tv_WsaData;
#endif
	// code --------------------
#ifdef LINUX_APP
		sigset(SIGPIPE, SIG_IGN);
#else
		//WSAStartup(MAKEWORD(1,0), &Tv_WsaData);
		WSAStartup(MAKEWORD(2, 2), &Tv_WsaData);
#endif

}
//--------------------------------------------------------------------
void		DrSockFinish( void )
{
#ifndef	LINUX_APP
		WSACleanup();
#endif
}
//--------------------------------------------------------------------
__u32	DrSockDnsRrNameInfo(Ptr_DrSockDnsRrParsEnv A_PtrEnv, void** A_PtrRtPtr)
{
	// local -------------------
		__u32	Tv_Result;
		__u8*	Tv_PtrWp;		// work position
		__u32	Tv_Remain;
		__u32	Tv_LenThis;
	// code --------------------
		// init
		Tv_Result		=	0;
		*A_PtrRtPtr	=	NULL;

		Tv_PtrWp	=	(__u8*)((Def_DrCalPtr)A_PtrEnv->PtrRr + (Def_DrCalPtr)A_PtrEnv->Prgs);
		Tv_Remain	=	A_PtrEnv->RrSize - A_PtrEnv->Prgs;

		// check pointer
		if(0xC0 == (*Tv_PtrWp))
		{
			// reference addrss
			*A_PtrRtPtr	=	(void*)((Def_DrCalPtr)A_PtrEnv->PtrRr + (Def_DrCalPtr)Tv_PtrWp[1]);
			Tv_Result	=	Tv_Result + 2;
			Tv_PtrWp	=	(__u8*)((Def_DrCalPtr)Tv_PtrWp + 2);
		}
		else
		{
			// direct address
			*A_PtrRtPtr	=	(void*)Tv_PtrWp;
			while(0 != Tv_PtrWp[0])
			{
				// check size
				Tv_LenThis	=	(__u32)Tv_PtrWp[0] + 1;
				if(Tv_Remain <= Tv_LenThis)
				{
					// break data
					Tv_Result	=	Tv_Result + Tv_Remain;
					break;
				}
				// next
				Tv_PtrWp	=	(__u8*)((Def_DrCalPtr)Tv_PtrWp + (Def_DrCalPtr)Tv_LenThis);
				Tv_Result	=	Tv_Result + Tv_LenThis;
				Tv_Remain	=	Tv_Remain - Tv_LenThis;
			}
			// mark finish
			Tv_Result ++;
			Tv_PtrWp	++;
			Tv_Remain --;
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8	DrSockDnsRrParsQuery(Ptr_DrSockDnsRrParsEnv A_PtrEnv, Ptr_DrSockDnsRrParsQueryInfo A_PtrRt)
{
	// local -------------------
		BOOL8	Tv_Result;
		__u32	Tv_FitLen;
		__u16*	Tv_PtrU16;
	// code --------------------
		Tv_Result	=	FALSE;

		A_PtrRt->Class			=	0;
		A_PtrRt->Type			=	0;
		A_PtrRt->PtrName		=	NULL;

		// get name position
		Tv_FitLen	=	DrSockDnsRrNameInfo(A_PtrEnv, &A_PtrRt->PtrName);
		if((Tv_FitLen + 4 + A_PtrEnv->Prgs) <= A_PtrEnv->RrSize)
		{
			// go next pointer
			A_PtrEnv->Prgs		=	A_PtrEnv->Prgs + Tv_FitLen;
			
			// get type and class
			Tv_PtrU16	=	(__u16*)((Def_DrCalPtr)A_PtrEnv->PtrRr + (Def_DrCalPtr)A_PtrEnv->Prgs);
			A_PtrRt->Type		=	Tv_PtrU16[0];	// type
			A_PtrRt->Class		=	Tv_PtrU16[1];	// class

			// go next pointer
			A_PtrEnv->Prgs		=	A_PtrEnv->Prgs + 4;

			// success
			Tv_Result	=	TRUE;
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
Def_DrErrCode	DrSockUrlAscToIp(char* A_StrUrl, __u32* A_PtrRtIpV4, __u8* A_PtrRtIpV6)
{

#ifdef LINUX_APP
	struct addrinfo Tv_Hint;
	struct addrinfo *	Tv_RtAdr;
	struct addrinfo *	Tv_ChkAdr;
#else
	ADDRINFO	Tv_Hint;
	ADDRINFO*	Tv_RtAdr;
	ADDRINFO*	Tv_ChkAdr;
#endif
	__u32		Tv_IpV4;

	// init
	if (NULL != A_PtrRtIpV4)
	{
		*A_PtrRtIpV4	=	0;
	}
	if (NULL != A_PtrRtIpV6)
	{
		DrDumyZeroMem(A_PtrRtIpV6, 16);
	}

	// check ipv4
	if (DrStrStripToIpV4Asc(A_StrUrl,&Tv_IpV4))
	{
		*A_PtrRtIpV4	=	Tv_IpV4;
		return	E_DrErrNone;
	}

	// check ipv6


	Tv_RtAdr	=	NULL;
	// get ip
	DrDumyZeroMem(&Tv_Hint, sizeof(Tv_Hint));
	Tv_Hint.ai_family		=	AF_UNSPEC;
	Tv_Hint.ai_socktype	= SOCK_STREAM;
	//Tv_Hint.ai_protocol	=	IPPROTO_TCP;

#ifdef LINUX_APP
	if (0 != getaddrinfo(A_StrUrl, NULL, &Tv_Hint, &Tv_RtAdr))
	{
		return	E_DrErrFail;
	}
#else
	{
		INT	Tv_RtCode;

		Tv_RtCode	=	GetAddrInfoA(A_StrUrl, NULL, &Tv_Hint, &Tv_RtAdr);
		if (0 != Tv_RtCode)
		{
			return	E_DrErrFail;
		}
	}
#endif
	Tv_ChkAdr	=	Tv_RtAdr;
	while (NULL != Tv_ChkAdr)
	{
		// check family
		if (AF_INET == Tv_ChkAdr->ai_family)
		{
			// ip v4
			if (NULL != A_PtrRtIpV4)
			{
				*A_PtrRtIpV4	=	(__u32)((sockaddr_in*)Tv_ChkAdr->ai_addr)->sin_addr.s_addr;
			}
		}
		else if (AF_INET6 == Tv_ChkAdr->ai_family)
		{
			// ip v6
			if (NULL != A_PtrRtIpV6)
			{
				DrDumyCopyMem(A_PtrRtIpV6, &((sockaddr_in6*)Tv_ChkAdr->ai_addr)->sin6_addr, 16);
			}
		}
		// next
		Tv_ChkAdr	=	Tv_ChkAdr->ai_next;
	}

#ifdef LINUX_APP
	freeaddrinfo(Tv_RtAdr);
#else
	FreeAddrInfoA(Tv_RtAdr);
#endif

	return	E_DrErrNone;
}
//--------------------------------------------------------------------
Def_DrErrCode	DrSockHttpGetData(void* A_PtrIp, BOOL8 A_IsIpV6, __u16 A_Port, char* A_StrParam, void* A_PtrRtBuf, __u32 A_BufLen, __u32* A_PtrRtRecvLen)
{
	Def_DrErrCode	Tv_Result;
	Def_DrSock	Tv_Sock;

	Def_StrShort	Tv_StrBuf;
	char*	Tv_StrWk;

	__s32	Tv_RecvLen;
	__u32	Tv_TryCnt;

	Tv_RecvLen	=	0;

	DrSockInit();

	//connect
	Tv_Sock	=	DrSockOpen(FALSE, FALSE, FALSE);
	if (E_DrSockBadHandle == Tv_Sock)
	{
		Tv_Result	=	E_DrErrInitFail;
		goto	L_Err0;
	}

	if (E_DrErrNone != DrSockConnect(Tv_Sock, A_PtrIp, A_Port, FALSE))
	{
		Tv_Result	=	E_DrErrBadParam;
		goto L_Err1;
	}
	

	// make message
	Tv_StrWk	=	DrStrCopyAsc(Tv_StrBuf, "GET ");
	Tv_StrWk	=	DrStrCopyAsc(Tv_StrWk, A_StrParam);
	Tv_StrWk	=	DrStrCopyAsc(Tv_StrWk, "\r\nHTTP/1.1\r\n");

	// send
	if (0 >= DrSockSend(Tv_Sock, Tv_StrBuf, DrStrLenAsc(Tv_StrBuf)))
	{
		Tv_Result	=	E_DrErrBadIo;
		goto	L_Err1;
	}

	// receive
	for ( Tv_TryCnt = 0; Tv_TryCnt < 10; Tv_TryCnt++)
	{
		Tv_RecvLen	=	DrSockRecv(Tv_Sock, A_PtrRtBuf, A_BufLen);
		if (DrSockChkCloseByRecv(Tv_RecvLen))
		{
			break;
		}
		
		if (0 < Tv_RecvLen )
		{
			break;
		}
		DrPrcSleep(30);
	}
	if (Tv_RecvLen <= 0 )
	{
		Tv_Result	=	E_DrErrBadIo;
		Tv_RecvLen	=	0;
	}
	else
	{
		Tv_Result	=	E_DrErrNone;
	}
L_Err1:
	DrSockClose(Tv_Sock);
L_Err0:
	DrSockFinish();

	*A_PtrRtRecvLen	=	Tv_RecvLen;
	return	Tv_Result;
}
//--------------------------------------------------------------------
#ifdef LINUX_EMU

void DrSockEmuMyIpV4Set(__u32 A_IpV4)
{
	V_DrSockEmuMyIpV4 = A_IpV4;
}

#endif
//--------------------------------------------------------------------
