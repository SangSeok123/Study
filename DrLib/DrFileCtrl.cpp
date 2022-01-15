/*
 standard file control - emulation able

 */

//====================================================================
// uses
#include	<DrPlatform.h>
#include	<DrTypeBase.h>
#include	<DrStrTool.h>
#include	<DrFileCtrl.h>
#include	<DrTimeTool.h>

#ifdef	LINUX_APP
#define	__USE_FILE_OFFSET64
#define LINUX_FILE_CTRL
#define _LARGEF64_SOURCE
#define	__USE_LARGEFILE64
#define __FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE

	#include	<sys/types.h>
	#include	<sys/stat.h>
	#include	<sys/ioctl.h>
	#include	<sys/time.h>
	#include	<time.h>
	#include	<pthread.h>
	#include	<fcntl.h>
	#include	<unistd.h>
	#include	<sys/mman.h>

#endif

//====================================================================
//local type

//====================================================================
//local function

//====================================================================
//var

#ifdef LINUX_EMU
	Def_StrShort	V_DrFileCtrlStrEmuPath	=	"D:/LnxEmu";	
	Def_WstrShort	V_DrFileCtrlWstrEmuPath	=	L"D:/LnxEmu";	
#endif

//====================================================================

//--------------------------------------------------------------------
	Cls_DrFileCtrl::Cls_DrFileCtrl(char* A_StrFileName, BOOL8 A_IsWrite, BOOL8 A_IsCreateAble,
															 BOOL8 A_IsExcute, BOOL8 A_IsNonEmu, BOOL8 A_IsSync, BOOL8 A_IsDirect )
{
		#ifdef LINUX_APP
		// local -------------------
			__s32		Tv_Mode;
			__s32		Tv_Flags;
		// code --------------------
			//init
			m_IsOpened			=	FALSE;
			m_IsSync				=	A_IsSync;
			Tv_Flags		=	O_RDONLY;
			if ( A_IsWrite )
			{
				Tv_Flags		=	O_RDWR;
				if ( A_IsCreateAble )
				{
					Tv_Flags		=	O_CREAT | O_RDWR;
				}
				if ( A_IsDirect )
				{
					Tv_Flags		=	Tv_Flags | O_SYNC;
				}
			}
			Tv_Mode		=	(6 << 6) | (6 << 3) | 6;
			if ( A_IsExcute )
			{
				Tv_Mode		=	Tv_Mode | ((1 <<6) | (1 << 3) | 1);
			}

			m_HndlFile	=	open( A_StrFileName, Tv_Flags, Tv_Mode );
			if ( -1 != m_HndlFile )
			{
				m_IsOpened		=	TRUE;
			}
		#else
		// local -------------------
			__u32		Tv_CtrlMode;
			__u32		Tv_ShareMode;
			__u32		Tv_CreateType;
			#ifdef LINUX_EMU
				Def_StrShort	Tv_StrFn;
			#endif
		// code --------------------
			//init
			m_IsOpened			=	FALSE;
			m_IsSync				=	A_IsSync;
			Tv_CtrlMode			=	GENERIC_READ;
			Tv_ShareMode		=	FILE_SHARE_READ | FILE_SHARE_WRITE;
			Tv_CreateType		=	OPEN_EXISTING;

			//set type
			if ( A_IsWrite )
			{
				Tv_CtrlMode		=	Tv_CtrlMode | GENERIC_WRITE;
				if ( A_IsCreateAble )
				{
					Tv_CreateType		=	CREATE_ALWAYS;
				}
			}
			//open file
			#ifdef LINUX_EMU
				if ( A_IsNonEmu )
				{
					m_HndlFile		=	CreateFileA( A_StrFileName, Tv_CtrlMode, Tv_ShareMode, NULL,
						Tv_CreateType, FILE_ATTRIBUTE_NORMAL, 0 );
				}
				else
				{
					DrStrCopyAsc( Tv_StrFn, DrFileCtrlEmuPathGetA() );
					if ( '/' != A_StrFileName[0] )
					{
						DrStrCatAsc( Tv_StrFn, "/" );
					}
					DrStrCatAsc( Tv_StrFn, A_StrFileName );
					m_HndlFile		=	CreateFileA( Tv_StrFn, Tv_CtrlMode, Tv_ShareMode, NULL,
						Tv_CreateType, FILE_ATTRIBUTE_NORMAL, 0 );
				}
			#else
				m_HndlFile		=	CreateFileA( A_StrFileName, Tv_CtrlMode, Tv_ShareMode, NULL,
																		Tv_CreateType, FILE_ATTRIBUTE_NORMAL, 0 );
			#endif
			if ( INVALID_HANDLE_VALUE != m_HndlFile )
			{ 
				m_IsOpened		=	TRUE;
			}
		#endif
}
//--------------------------------------------------------------------
Cls_DrFileCtrl::Cls_DrFileCtrl( WCHAR* A_StrFileName, BOOL8 A_IsWrite, BOOL8 A_IsCreateAble, 
															 BOOL8 A_IsExcute, BOOL8 A_IsNonEmu, BOOL8 A_IsSync, BOOL8 A_IsDirect )
{
	#ifdef LINUX_APP
	// local -------------------
		__s32		Tv_Mode;
		__s32		Tv_Flags;
		Def_StrShort	Tv_StrFn;
	// code --------------------
		//init
		m_IsOpened			=	FALSE;
		m_IsSync				=	A_IsSync;
		DrStrU16leToAsc( Tv_StrFn, A_StrFileName );
		Tv_Flags		=	O_RDONLY;
		if ( A_IsWrite )
		{
			Tv_Flags		=	O_RDWR;
			if ( A_IsCreateAble )
			{
				Tv_Flags		=	O_CREAT | O_RDWR;
			}
			if ( A_IsDirect )
			{
				Tv_Flags		=	Tv_Flags | O_SYNC;
			}
		}
		Tv_Mode		=	(6 << 6) | (6 << 3) | 6;
		if ( A_IsExcute )
		{
			Tv_Mode		=	Tv_Mode | ((1 <<6) | (1 << 3) | 1);
		}

		m_HndlFile	=	open( Tv_StrFn, Tv_Flags, Tv_Mode );
		if ( -1 != m_HndlFile )
		{
			m_IsOpened		=	TRUE;
		}
	#else
	// local -------------------
		__u32		Tv_CtrlMode;
		__u32		Tv_ShareMode;
		__u32		Tv_CreateType;
	#ifdef LINUX_EMU
		Def_WstrShort	Tv_StrFn;
	#endif
	// code --------------------
		//init
		m_IsOpened			=	FALSE;
		m_IsSync				=	A_IsSync;
		Tv_CtrlMode			=	GENERIC_READ;
		Tv_ShareMode		=	FILE_SHARE_READ | FILE_SHARE_WRITE;
		Tv_CreateType		=	OPEN_EXISTING;

		//set type
		if ( A_IsWrite )
		{
			Tv_CtrlMode		=	Tv_CtrlMode | GENERIC_WRITE;
			if ( A_IsCreateAble )
			{
				Tv_CreateType		=	CREATE_ALWAYS;
			}
		}
		//open file
	#ifdef LINUX_EMU
		if ( A_IsNonEmu )
		{
			m_HndlFile		=	CreateFileW( A_StrFileName, Tv_CtrlMode, Tv_ShareMode, NULL,
				Tv_CreateType, FILE_ATTRIBUTE_NORMAL, 0 );
		}
		else
		{
			DrStrCopyU16( Tv_StrFn, V_DrFileCtrlWstrEmuPath );
			if ( (WCHAR)'/' != A_StrFileName[0] )
			{
				DrStrCatU16( Tv_StrFn, L"/" );
			}
			DrStrCatU16( Tv_StrFn, A_StrFileName );
			m_HndlFile		=	CreateFileW( Tv_StrFn, Tv_CtrlMode, Tv_ShareMode, NULL,
				Tv_CreateType, FILE_ATTRIBUTE_NORMAL, 0 );
		}
	#else
		m_HndlFile		=	CreateFileW( A_StrFileName, Tv_CtrlMode, Tv_ShareMode, NULL,
			Tv_CreateType, FILE_ATTRIBUTE_NORMAL, 0 );
	#endif
		if ( INVALID_HANDLE_VALUE != m_HndlFile )
		{ 
			m_IsOpened		=	TRUE;
		}
	#endif
}
//--------------------------------------------------------------------
Cls_DrFileCtrl::~Cls_DrFileCtrl()
{
	// local -------------------

  // code --------------------
		#ifdef LINUX_APP
			//close file
			if ( m_IsOpened )
			{
				if ( m_IsSync )
				{
					fsync( m_HndlFile );
				}
				close( m_HndlFile );
			}
		#else
			//close file
			if ( m_IsOpened )
			{
				CloseHandle( m_HndlFile );
			}
		#endif
}
//--------------------------------------------------------------------
__s64		Cls_DrFileCtrl::Seek( __s64 A_Position, __u32 A_Mode )
{
		#ifdef LINUX_APP
		// local -------------------
			__s64		Tv_Result;
			__s32				Tv_Mode;
		// code --------------------
			//init
			Tv_Result		=	-1;
			//check open
			if ( m_IsOpened )
			{
				Tv_Mode		=	SEEK_SET;
				if ( E_DrFileCtrlPosNow == A_Mode )
				{
					Tv_Mode		=	SEEK_CUR;
				}
				else if ( E_DrFileCtrlPosEnd == A_Mode )
				{
					Tv_Mode		=	SEEK_END;
				}

				Tv_Result	=	lseek64( m_HndlFile, A_Position, Tv_Mode ); 
			}
		#else
		// local -------------------
			__s64		Tv_Result;
			LARGE_INTEGER		Tv_RtVal;
			LARGE_INTEGER		Tv_Pos;
		// code --------------------
			//init
			Tv_Result		=	-1;

			//check open
			if ( m_IsOpened )
			{
				//move position
				Tv_Pos.QuadPart		=	A_Position;
				if ( SetFilePointerEx( m_HndlFile, Tv_Pos, &Tv_RtVal, A_Mode ) )
				{
					Tv_Result	=	Tv_RtVal.QuadPart;
				}
				else
				{
					Tv_Result		=	-1;
				}
			}
		#endif

		return		Tv_Result;
}
//--------------------------------------------------------------------
__u32			Cls_DrFileCtrl::Read( void* A_Buf, __u32 A_Size )
{
		#ifdef LINUX_APP
		// local -------------------
			__u32		Tv_Result;
		// code --------------------
			//init
			Tv_Result		=	0;

			//check opend
			if ( m_IsOpened )
			{
				Tv_Result	=	read( m_HndlFile, A_Buf, A_Size );
			}
		#else
		// local -------------------
			__u32		Tv_Result;
			__u32		Tv_Readed;
		// code --------------------
			//init
			Tv_Result		=	0;

			//check opend
			if ( m_IsOpened )
			{
				if ( ReadFile(m_HndlFile, A_Buf, A_Size, (LPDWORD)&Tv_Readed, NULL ) )
				{
					Tv_Result		=	Tv_Readed;
				}
			}
		#endif

		return		Tv_Result;
}
//--------------------------------------------------------------------
__u32			Cls_DrFileCtrl::Write( void* A_Buf, __u32 A_Size )
{
		#ifdef LINUX_APP
		// local -------------------
			__u32		Tv_Result;
		// code --------------------
			//init
			Tv_Result		=	0;
			//check opened
			if ( m_IsOpened )
			{
				Tv_Result	=	write( m_HndlFile, A_Buf, A_Size );
			}
		#else
		// local -------------------
			__u32		Tv_Result;
			__u32		Tv_Writed;
		// code --------------------
			//init
			Tv_Result		=	0;
			//check opened
			if ( m_IsOpened )
			{
				if ( WriteFile(m_HndlFile, A_Buf, A_Size, (LPDWORD)&Tv_Writed, NULL) )
				{
					Tv_Result		=	Tv_Writed;
				}
			}
		#endif

		return		Tv_Result;
}
//--------------------------------------------------------------------
__s64	Cls_DrFileCtrl::GetPos( void )
{
		#ifdef LINUX_APP
		// local -------------------
			__s64	Tv_Result;
		// code --------------------
			//init
			Tv_Result		=	(__s32)this->Seek( 0,E_DrFileCtrlPosNow );
			return		Tv_Result;
		#else
			return		this->Seek( 0,FILE_CURRENT );
		#endif
}
//--------------------------------------------------------------------
__s64	Cls_DrFileCtrl::GetSize( void )
{
		#ifdef LINUX_APP
		// local -------------------
			__s64		Tv_Result;
		// code --------------------
			//init
			Tv_Result		=	0;

			if ( m_IsOpened )
			{
				Tv_Result	=	lseek64( m_HndlFile, 0, SEEK_END );
			}
		#else
		// local -------------------
			__s64		Tv_Result;
		// code --------------------
			//init
			Tv_Result		=	0;

			if ( m_IsOpened )
			{
				if ( !GetFileSizeEx( m_HndlFile, (PLARGE_INTEGER)(&Tv_Result) ) )
				{
					Tv_Result		=	0;
				}
			}
		#endif

		return		Tv_Result;
}
//--------------------------------------------------------------------
BOOL8	Cls_DrFileCtrl::IsOpened( void )
{
		return	m_IsOpened;
}
//--------------------------------------------------------------------
void	Cls_DrFileCtrl::Flush(void)
{
#ifdef LINUX_APP
		fsync(m_HndlFile);
#endif
}
//--------------------------------------------------------------------
__u32	DrFileCtrlRead(char* A_StrFn, void* A_PtrBuf, __u32 A_Size, BOOL8 A_IsNonEmu)
{
	// local -------------------
		__u32		Tv_Result;
	// code --------------------
		Tv_Result		=	0;
		
		{
			Cls_DrFileCtrl	Tv_ObjFl( A_StrFn, FALSE, FALSE,FALSE,A_IsNonEmu );
			if ( Tv_ObjFl.IsOpened() )
			{
				Tv_Result	=	Tv_ObjFl.Read( A_PtrBuf, A_Size );
			}
		}
		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrFileCtrlRead( WCHAR* A_StrFn, void* A_PtrBuf, __u32 A_Size, BOOL8 A_IsNonEmu )
{
	// local -------------------
		__u32		Tv_Result;
	// code --------------------
		Tv_Result		=	0;

		{
			Cls_DrFileCtrl	Tv_ObjFl( A_StrFn, FALSE, FALSE, FALSE, A_IsNonEmu );
			if ( Tv_ObjFl.IsOpened() )
			{
				Tv_Result	=	Tv_ObjFl.Read( A_PtrBuf, A_Size );
			}
		}
		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrFileCtrlWrite(char* A_StrFn, BOOL8 A_IsCreate, void* A_PtrBuf, __u32 A_Size, BOOL8 A_IsNonEmu, BOOL8 A_IsSync, BOOL8 A_IsAdd)
{
	// local -------------------
		__u32		Tv_Result;
	// code --------------------
		//init
		Tv_Result		=	0;

		{
			Cls_DrFileCtrl	Tv_ObjFl( A_StrFn, TRUE, A_IsCreate, FALSE, A_IsNonEmu, A_IsSync );
			if ( Tv_ObjFl.IsOpened() )
			{
				if ( A_IsAdd )
				{
					Tv_ObjFl.Seek( 0, E_DrFileCtrlPosEnd );
				}
				Tv_Result	=	Tv_ObjFl.Write( A_PtrBuf, A_Size );
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrFileCtrlWrite( WCHAR* A_StrFn, BOOL8 A_IsCreate, void* A_PtrBuf, __u32 A_Size, BOOL8 A_IsNonEmu, BOOL8 A_IsSync, BOOL8 A_IsAdd )
{
	// local -------------------
		__u32		Tv_Result;
	// code --------------------
		//init
		Tv_Result		=	0;

		{
			Cls_DrFileCtrl	Tv_ObjFl( A_StrFn, TRUE, A_IsCreate, FALSE, A_IsNonEmu, A_IsSync );
			if ( Tv_ObjFl.IsOpened() )
			{
				if ( A_IsAdd )
				{
					Tv_ObjFl.Seek( 0, E_DrFileCtrlPosEnd );
				}
				Tv_Result	=	Tv_ObjFl.Write( A_PtrBuf, A_Size );
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
void	DrFileCtrlEmuPathSet(char* A_StrPath)
{
		#ifdef LINUX_EMU
			DrStrCopyAsc( V_DrFileCtrlStrEmuPath, A_StrPath );
			DrStrAscToU16le( V_DrFileCtrlWstrEmuPath, V_DrFileCtrlStrEmuPath );
		#endif
}
//--------------------------------------------------------------------
void	DrFileCtrlEmuPathSet( WCHAR* A_StrPath )
{
	#ifdef LINUX_EMU
		DrStrCopyU16( V_DrFileCtrlWstrEmuPath, A_StrPath );
		DrStrU16leToAsc( V_DrFileCtrlStrEmuPath, V_DrFileCtrlWstrEmuPath );
	#endif
}
//--------------------------------------------------------------------
char*	DrFileCtrlEmuPathGetA( void )
{
	#ifdef LINUX_EMU
		return	V_DrFileCtrlStrEmuPath;
	#else
		return	NULL;
	#endif
}
//--------------------------------------------------------------------
WCHAR*	DrFileCtrlEmuPathGetW( void )
{
	#ifdef LINUX_EMU
		return	V_DrFileCtrlWstrEmuPath;
	#else
		return	NULL;
	#endif
}
//--------------------------------------------------------------------

