/*
 String Tools

 */

//====================================================================
// uses
#include	<DrPlatform.h>

#include <DrDebug.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#ifdef	LINUX_APP
	#include	<sys/stat.h>
	#include	<sys/types.h>
	#include	<unistd.h>
	#include	<linux/hdreg.h>
	#include	<fcntl.h>
	#include	<sys/ioctl.h>
	#include	<DrTypeBase.h>
#else
	#include	<DrTypeBase.h>
	#include	<shellapi.h>
#endif
#include	<DrStrTool.h>
#include	<DrFileCtrl.h>
#include	<DrFileTool.h>

//====================================================================
//local define
#define E_DrFileToolCopyBufSize			0x10000

//====================================================================
//local type

//====================================================================
//local function
__s32		DrFileGetDrvIdxByPaht(char* A_StrPath);			//windows only
void	DrFileMakeDevNameByDrvIdx(__s32 A_DrvIdx, char* A_StrRtDevName);

//====================================================================
//var

#ifdef	LINUX_APP
	char	V_DrFileToolDelCmd[]		=	"rm -f -r ";		//delete system command
	char	V_DrFileDrvLstSrc[]			=	"/proc/partitions";
	char	V_DrFileDrvLstTstMnt[]	=	"/mnt/DrFileMntTest";
#endif

	char V_DrFileToolStrWild[] = "*";
	char V_DrFileToolStrAllFile[] = "*.*";

//====================================================================
BOOL8		DrFileIsExist( char* A_StrFn, BOOL8 A_IsNoEmu )
{
	// local -------------------
		BOOL8		Tv_Result;
		#ifndef	LINUX_APP
			WIN32_FIND_DATAA	Tv_FindInfo;
			HANDLE	Tv_HndlFind;
			#ifdef LINUX_EMU
				Def_StrShort	Tv_StrFn;
			#endif
		#else
			struct	stat	Tv_Fstat;
		#endif

  // code --------------------
		//init
		Tv_Result		=	FALSE;

		//find
		#ifndef	LINUX_APP
			#ifdef LINUX_EMU
				if ( A_IsNoEmu )
				{
					Tv_HndlFind	=	FindFirstFileA( A_StrFn, &Tv_FindInfo );
				}
				else
				{
					DrStrCopyAsc( Tv_StrFn, DrFileCtrlEmuPathGetA() );
					if ( '/' != A_StrFn[0] )
					{
						DrStrCatAsc( Tv_StrFn, "/" );
					}
					DrStrCatAsc( Tv_StrFn, A_StrFn );
					Tv_HndlFind	=	FindFirstFileA( Tv_StrFn, &Tv_FindInfo );
				}
			#else
				Tv_HndlFind	=	FindFirstFileA( A_StrFn, &Tv_FindInfo );
			#endif			
			if ( INVALID_HANDLE_VALUE != Tv_HndlFind )
			{
				//file exist
				//check file type
				if ( 0 == (FILE_ATTRIBUTE_DIRECTORY &	Tv_FindInfo.dwFileAttributes) )
				{
					Tv_Result		=	TRUE;
				}
				FindClose( Tv_HndlFind );
				Tv_Result		=	TRUE;
			}
		#else
			//check exist file
			if ( 0 <= access( A_StrFn, F_OK ) )
			{
				//exist
				Tv_Result		=	TRUE;
			}
		#endif

		return		Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileIsExist( WCHAR* A_StrFn, BOOL8 A_IsNoEmu )
{
	// local -------------------
		BOOL8		Tv_Result;
	#ifndef	LINUX_APP
		WIN32_FIND_DATAW	Tv_FindInfo;
		HANDLE	Tv_HndlFind;
		#ifdef LINUX_EMU
			Def_WstrShort	Tv_StrFn;
		#endif
	#else
		struct	stat	Tv_Fstat;
		Def_StrShort	Tv_StrFn;
	#endif
	// code --------------------
		//init
		Tv_Result		=	FALSE;

		//find
	#ifndef	LINUX_APP
		#ifdef LINUX_EMU
			if ( A_IsNoEmu )
			{
				Tv_HndlFind	=	FindFirstFileW( A_StrFn, &Tv_FindInfo );
			}
			else
			{
				DrStrCopyU16( Tv_StrFn, DrFileCtrlEmuPathGetW() );
				if ( L'/' != A_StrFn[0] )
				{
					DrStrCatU16( Tv_StrFn, L"/" );
				}
				DrStrCatU16( Tv_StrFn, A_StrFn );
				Tv_HndlFind	=	FindFirstFileW( Tv_StrFn, &Tv_FindInfo );
			}
		#else
			Tv_HndlFind	=	FindFirstFileW( A_StrFn, &Tv_FindInfo );
		#endif			

		if ( INVALID_HANDLE_VALUE != Tv_HndlFind )
		{
			//file exist
			//check file type
			if ( 0 == (FILE_ATTRIBUTE_DIRECTORY &	Tv_FindInfo.dwFileAttributes) )
			{
				Tv_Result		=	TRUE;
			}
			FindClose( Tv_HndlFind );
			Tv_Result		=	TRUE;
		}
	#else
		//check exist file
		DrStrU16leToAsc( Tv_StrFn, A_StrFn );
		if ( 0 <= access( Tv_StrFn, F_OK ) )
		{
			//exist
			Tv_Result		=	TRUE;
		}
	#endif

		return		Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileDelete( char* A_StrFn, BOOL8 A_IsNoEmu )
{
	// local -------------------
		BOOL8		Tv_Result;
		#ifndef	LINUX_APP
			SHFILEOPSTRUCTA	Tv_SoInfo;
			Def_StrDrFnPath	Tv_StrFn;
			__u32						Tv_LenFn;
			char*						Tv_PtrFn;
		#endif

  // code --------------------
		//init
		Tv_Result		=	FALSE;

		#ifndef	LINUX_APP
			#ifdef LINUX_EMU
				if ( A_IsNoEmu )
				{
					DrStrCopyAsc( Tv_StrFn,A_StrFn,E_DrFnLongWithPathLen - 2 );
				}
				else
				{
					//set parameter
					DrStrCopyAsc( Tv_StrFn, DrFileCtrlEmuPathGetA() );
					if ( ('/' != A_StrFn[0]) && '\\' != A_StrFn[0] )
					{
						DrStrCatAsc( Tv_StrFn, "/" );
					}
					DrStrCatAsc( Tv_StrFn,A_StrFn,E_DrFnLongWithPathLen - 2 );
				}
			#else
				//set parameter
				DrStrCopyAsc( Tv_StrFn,A_StrFn,E_DrFnLongWithPathLen - 2 );
			#endif
			Tv_LenFn    =	DrStrLenAsc( Tv_StrFn );
			Tv_PtrFn    =	((char*)(&Tv_StrFn)) + Tv_LenFn + 1;
			*Tv_PtrFn		=	0;					//add double null terminate

			//set file delete
			ZeroMemory( &Tv_SoInfo,sizeof(SHFILEOPSTRUCTA) );
			Tv_SoInfo.wFunc		= FO_DELETE;
			Tv_SoInfo.pFrom   = Tv_StrFn;
			Tv_SoInfo.fFlags  =	FOF_FILESONLY | FOF_NOERRORUI | FOF_SILENT | FOF_NOCONFIRMATION;
			if ( 0 == SHFileOperationA( &Tv_SoInfo ) )
			{
				Tv_Result		=	TRUE;
			}
		#else

			if ( 0 == remove( A_StrFn ) )
			{
				Tv_Result		=	TRUE;
			}
			
		#endif

		return		Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileDelete( WCHAR* A_StrFn, BOOL8 A_IsNoEmu )
{
	// local -------------------
		BOOL8		Tv_Result;
	#ifdef	LINUX_APP
		Def_StrDrFnPath	Tv_StrFn;
	#else
		SHFILEOPSTRUCTW	Tv_SoInfo;
		Def_WstrDrFnPath	Tv_StrFn;
		__u32						Tv_LenFn;
	#endif

	// code --------------------
		//init
		Tv_Result		=	FALSE;

		#ifndef	LINUX_APP
			#ifdef LINUX_EMU
				if ( A_IsNoEmu )
				{
					DrStrCopyU16( Tv_StrFn,A_StrFn );
				}
				else
				{
					//set parameter
					DrStrCopyU16( Tv_StrFn, DrFileCtrlEmuPathGetW() );
					if ( (L'/' != A_StrFn[0]) && L'\\' != A_StrFn[0] )
					{
						DrStrCatU16( Tv_StrFn, L"/" );
					}
					DrStrCatU16( Tv_StrFn,A_StrFn );
				}
			#else
				//set parameter
				DrStrCopyU16( Tv_StrFn,A_StrFn );
			#endif
			Tv_LenFn    =	DrStrLenU16( Tv_StrFn );
			Tv_StrFn[Tv_LenFn + 1]	=	0;	//add double null terminate

			//set file delete
			ZeroMemory( &Tv_SoInfo,sizeof(SHFILEOPSTRUCTW) );
			Tv_SoInfo.wFunc		= FO_DELETE;
			Tv_SoInfo.pFrom   = Tv_StrFn;
			Tv_SoInfo.fFlags  =	FOF_FILESONLY | FOF_NOERRORUI | FOF_SILENT | FOF_NOCONFIRMATION;
			if ( 0 == SHFileOperationW( &Tv_SoInfo ) )
			{
				Tv_Result		=	TRUE;
			}
		#else
			DrStrU16leToAsc( Tv_StrFn, A_StrFn );
			if ( 0 == remove( Tv_StrFn ) )
			{
				Tv_Result		=	TRUE;
			}
		#endif

		return		Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileRename(char* A_StrTg, char* A_StrSr, BOOL8 A_IsNoEmu)
{
	// local -------------------
		BOOL8	Tv_Result;
#if defined(_WIN32) || defined(_WIN64)
#ifdef LINUX_EMU
		Def_StrDrFnPathLong	Tv_StrSr;
		Def_StrDrFnPathLong	Tv_StrTg;
#endif
#endif
	// code --------------------
		Tv_Result	=	FALSE;

#if defined(_WIN32) || defined(_WIN64)

#ifdef LINUX_EMU
		DrStrCopyAsc(Tv_StrSr, DrFileCtrlEmuPathGetA());
		DrStrFnAddBkSlshAsc(Tv_StrSr);
		DrStrCatAsc(Tv_StrSr, A_StrSr);

		DrStrCopyAsc(Tv_StrTg, DrFileCtrlEmuPathGetA());
		DrStrFnAddBkSlshAsc(Tv_StrTg);
		DrStrCatAsc(Tv_StrTg, A_StrTg);

		if(MoveFileA(Tv_StrSr, Tv_StrTg))
		{
			Tv_Result	=	TRUE;
		}

#else

		if(MoveFileA(A_StrSr, A_StrTg))
		{
			Tv_Result	=	TRUE;
		}

#endif

#else

		if(0 == rename(A_StrSr, A_StrTg))
		{
			Tv_Result	=	TRUE;
		}

#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileRename(WCHAR* A_StrTg, WCHAR* A_StrSr, BOOL8 A_IsNoEmu)
{
	// local -------------------
		BOOL8	Tv_Result;
		
#if defined(_WIN32) || defined(_WIN64)
#ifdef LINUX_EMU
		Def_WstrDrFnPathLong	Tv_StrSr;
		Def_WstrDrFnPathLong	Tv_StrTg;
#endif
#else
		// linux
		Def_StrDrFnPathLong	Tv_StrSr;
		Def_StrDrFnPathLong	Tv_StrTg;
#endif
	// code --------------------
		Tv_Result	=	FALSE;

#if defined(_WIN32) || defined(_WIN64)

#ifdef LINUX_EMU
		DrStrCopyU16(Tv_StrSr, DrFileCtrlEmuPathGetW());
		DrStrFnAddBkSlshU16le(Tv_StrSr);
		DrStrCatU16(Tv_StrSr, A_StrSr);

		DrStrCopyU16(Tv_StrTg, DrFileCtrlEmuPathGetW());
		DrStrFnAddBkSlshU16le(Tv_StrTg);
		DrStrCatU16(Tv_StrTg, A_StrTg);

		if(MoveFileW(Tv_StrSr, Tv_StrTg))
		{
			Tv_Result	=	TRUE;
		}

#else

		if(MoveFileW(A_StrSr, A_StrTg))
		{
			Tv_Result	=	TRUE;
		}

#endif

#else

		DrStrU16leToAsc( Tv_StrSr, A_StrSr );
		DrStrU16leToAsc( Tv_StrTg, A_StrTg );
		if(0 == rename(Tv_StrSr, Tv_StrTg))
		{
			Tv_Result	=	TRUE;
		}

#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileDelDir( char* A_StrFn, BOOL8 A_IsNoEmu )
{
	// local -------------------
		BOOL8		Tv_Result;
		#ifndef	LINUX_APP
			SHFILEOPSTRUCTA	Tv_SoInfo;
			Def_StrDrFnPath	Tv_StrFn;
			__u32						Tv_LenFn;
			char*						Tv_PtrFn;
		#else
			Def_StrShort		Tv_StrDelCmd;
		#endif
  // code --------------------
		//init
		Tv_Result		=	FALSE;

		#ifndef	LINUX_APP
			//set parameter
			#ifdef LINUX_EMU
				if ( A_IsNoEmu )
				{
					DrStrCopyAsc( Tv_StrFn,A_StrFn,E_DrFnLongWithPathLen - 2 );
				}
				else
				{
					//set parameter
					DrStrCopyAsc( Tv_StrFn, DrFileCtrlEmuPathGetA() );
					if ( ('/' != A_StrFn[0]) && '\\' != A_StrFn[0] )
					{
						DrStrCatAsc( Tv_StrFn, "/" );
					}
					DrStrCatAsc( Tv_StrFn,A_StrFn );
				}
			#else
				DrStrCopyAsc( Tv_StrFn,A_StrFn,E_DrFnLongWithPathLen - 2 );
			#endif			
			DrStrFnReverseSlashAsc( Tv_StrFn );
			Tv_LenFn    =	DrStrLenAsc( Tv_StrFn );
			Tv_PtrFn    =	((char*)(&Tv_StrFn)) + Tv_LenFn + 1;
			*Tv_PtrFn		=	0;					//add double null terminate

			//set file delete
			ZeroMemory( &Tv_SoInfo,sizeof(SHFILEOPSTRUCTA) );
			Tv_SoInfo.wFunc		= FO_DELETE;
			Tv_SoInfo.pFrom   = Tv_StrFn;
			Tv_SoInfo.fFlags  =	FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
			if ( 0 == SHFileOperationA( &Tv_SoInfo ) )
			{
				Tv_Result		=	TRUE;
			}
		#else
			DrStrCopyAsc( Tv_StrDelCmd, V_DrFileToolDelCmd );
			DrStrCatAsc( Tv_StrDelCmd, A_StrFn );
			system( Tv_StrDelCmd );
			Tv_Result		=	TRUE;
		#endif

		return		Tv_Result;

}
//--------------------------------------------------------------------
BOOL8		DrFileDelDir( WCHAR* A_StrFn, BOOL8 A_IsNoEmu )
{
	// local -------------------
		BOOL8		Tv_Result;
	#ifndef	LINUX_APP
		SHFILEOPSTRUCTW	Tv_SoInfo;
		Def_WstrDrFnPath	Tv_StrFn;
		__u32						Tv_LenFn;
	#else
		Def_StrShort		Tv_StrDelCmd;
		Def_StrDrFnPath	Tv_StrFn;
	#endif
	// code --------------------
		//init
		Tv_Result		=	FALSE;

		#ifndef	LINUX_APP
			//set parameter
			#ifdef LINUX_EMU
				if ( A_IsNoEmu )
				{
					DrStrCopyU16( Tv_StrFn,A_StrFn );
				}
				else
				{
					//set parameter
					DrStrCopyU16( Tv_StrFn, DrFileCtrlEmuPathGetW() );
					if ( (L'/' != A_StrFn[0]) && L'\\' != A_StrFn[0] )
					{
						DrStrCatU16( Tv_StrFn, L"/" );
					}
					DrStrCatU16( Tv_StrFn,A_StrFn );
				}
			#else
				DrStrCopyU16( Tv_StrFn,A_StrFn );
			#endif			

			DrStrFnReverseSlashU16le( Tv_StrFn );
			Tv_LenFn    =	DrStrLenU16( Tv_StrFn );
			Tv_StrFn[Tv_LenFn+1]	=	0;		//add double null terminate

			//set file delete
			ZeroMemory( &Tv_SoInfo,sizeof(SHFILEOPSTRUCTW) );
			Tv_SoInfo.wFunc		= FO_DELETE;
			Tv_SoInfo.pFrom   = Tv_StrFn;
			Tv_SoInfo.fFlags  =	FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
			if ( 0 == SHFileOperationW( &Tv_SoInfo ) )
			{
				Tv_Result		=	TRUE;
			}
		#else
			DrStrCopyAsc( Tv_StrDelCmd, V_DrFileToolDelCmd );
			DrStrU16leToAsc( Tv_StrFn, A_StrFn );
			DrStrCatAsc( Tv_StrDelCmd, Tv_StrFn );
			system( Tv_StrDelCmd );
			Tv_Result		=	TRUE;
		#endif

		return		Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileIsExistDir( char* A_StrFn, BOOL8 A_IsNoEmu )
{
	// local -------------------
		BOOL8		Tv_Result;
		#ifndef	LINUX_APP
			WIN32_FIND_DATAA	Tv_FindInfo;
			HANDLE	Tv_HndlFind;
			#ifdef LINUX_EMU
				Def_StrShort	Tv_StrFn;
			#endif
		#else
			struct	stat	Tv_Fstat;
		#endif

  // code --------------------
		//init
		Tv_Result		=	FALSE;

		//find
		#ifndef	LINUX_APP
			#ifdef LINUX_EMU
				if ( A_IsNoEmu )
				{
					Tv_HndlFind	=	FindFirstFileA( A_StrFn, &Tv_FindInfo );
				}
				else
				{
					//set parameter
					DrStrCopyAsc( Tv_StrFn, DrFileCtrlEmuPathGetA() );
					if ( ('/' != A_StrFn[0]) && '\\' != A_StrFn[0] )
					{
						DrStrCatAsc( Tv_StrFn, "/" );
					}
					DrStrCatAsc( Tv_StrFn,A_StrFn );
					Tv_HndlFind	=	FindFirstFileA( Tv_StrFn, &Tv_FindInfo );
				}
			#else
				Tv_HndlFind	=	FindFirstFileA( A_StrFn, &Tv_FindInfo );
			#endif
			if ( INVALID_HANDLE_VALUE != Tv_HndlFind )
			{
				//file exist
				//check file type
				if ( 0 != (FILE_ATTRIBUTE_DIRECTORY &	Tv_FindInfo.dwFileAttributes) )
				{
					Tv_Result		=	TRUE;
				}
				FindClose( Tv_HndlFind );
			}
		#else
			//check exist file
			if ( 0 == stat( A_StrFn, &Tv_Fstat ) )
			{
				//check file
				if ( S_ISDIR( Tv_Fstat.st_mode ) )
				{
					//normal regular file
					Tv_Result		=	TRUE;
				}
			}
		#endif

		return	Tv_Result;

}
//--------------------------------------------------------------------
BOOL8		DrFileIsExistDir( WCHAR* A_StrFn, BOOL8 A_IsNoEmu )
{
	// local -------------------
		BOOL8		Tv_Result;
	#ifndef	LINUX_APP
		WIN32_FIND_DATAW	Tv_FindInfo;
		HANDLE	Tv_HndlFind;
		#ifdef LINUX_EMU
			Def_WstrShort	Tv_StrFn;
		#endif
	#else
		struct	stat	Tv_Fstat;
		Def_StrDrFnPath	Tv_StrFn;
	#endif

	// code --------------------
		//init
		Tv_Result		=	FALSE;

		//find
	#ifndef	LINUX_APP
		#ifdef LINUX_EMU
			if ( A_IsNoEmu )
			{
				Tv_HndlFind	=	FindFirstFileW( A_StrFn, &Tv_FindInfo );
			}
			else
			{
				//set parameter
				DrStrCopyU16( Tv_StrFn, DrFileCtrlEmuPathGetW() );
				if ( (L'/' != A_StrFn[0]) && L'\\' != A_StrFn[0] )
				{
					DrStrCatU16( Tv_StrFn, L"/" );
				}
				DrStrCatU16( Tv_StrFn,A_StrFn );
				Tv_HndlFind	=	FindFirstFileW( Tv_StrFn, &Tv_FindInfo );
			}
		#else
			Tv_HndlFind	=	FindFirstFileW( A_StrFn, &Tv_FindInfo );
		#endif

		if ( INVALID_HANDLE_VALUE != Tv_HndlFind )
		{
			//file exist
			//check file type
			if ( 0 != (FILE_ATTRIBUTE_DIRECTORY &	Tv_FindInfo.dwFileAttributes) )
			{
				Tv_Result		=	TRUE;
			}

			FindClose( Tv_HndlFind );

		}
	#else
		//check exist file
		DrStrU16leToAsc( Tv_StrFn, A_StrFn );
		if ( 0 == stat( Tv_StrFn, &Tv_Fstat ) )
		{
			//check file
			if ( S_ISDIR( Tv_Fstat.st_mode ) )
			{
				//normal regular file
				Tv_Result		=	TRUE;
			}
		}
	#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileCreateDir(char* A_StrPath, BOOL8 A_IsNoEmu, __u32 A_Owner)
{
	// local -------------------
		BOOL8		Tv_Result;
		#ifdef LINUX_EMU
			Def_StrShort	Tv_StrFn;
		#endif

	// code --------------------
		//init
		Tv_Result		=	FALSE;

#ifdef	LINUX_APP
		
		if ( 0 == mkdir( A_StrPath, (mode_t)(A_Owner & 0777) ) )
		{
				//success
				Tv_Result		=	TRUE;
		}
#else
			#ifdef LINUX_EMU
				if ( A_IsNoEmu )
				{
					Tv_Result	=	CreateDirectoryA( A_StrPath, NULL );
				}
				else
				{
					//set parameter
					DrStrCopyAsc( Tv_StrFn, DrFileCtrlEmuPathGetA() );
					if ( ('/' != A_StrPath[0]) && '\\' != A_StrPath[0] )
					{
						DrStrCatAsc( Tv_StrFn, "/" );
					}
					DrStrCatAsc( Tv_StrFn,A_StrPath );
					Tv_Result	=	CreateDirectoryA( Tv_StrFn, NULL );
				}
			#else
				Tv_Result	=	CreateDirectoryA( A_StrPath, NULL );
			#endif
#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileCreateDir(WCHAR* A_StrPath, BOOL8 A_IsNoEmu, __u32 A_Owner)
{
	// local -------------------
		BOOL8		Tv_Result;
		#ifdef	LINUX_APP
			Def_StrDrFnPath	Tv_StrFn;
		#endif
		#ifdef LINUX_EMU
			Def_WstrShort	Tv_StrFn;
		#endif
	// code --------------------
		//init
		Tv_Result		=	FALSE;

#ifdef	LINUX_APP
			DrStrU16leToAsc( Tv_StrFn, A_StrPath );
			if ( 0 == mkdir( Tv_StrFn, (mode_t)A_Owner ) )
			{
				//success
				Tv_Result		=	TRUE;
			}
#else
			#ifdef LINUX_EMU
				if ( A_IsNoEmu )
				{
					Tv_Result	=	CreateDirectoryW( A_StrPath, NULL );
				}
				else
				{
					//set parameter
					DrStrCopyU16( Tv_StrFn, DrFileCtrlEmuPathGetW() );
					if ( (L'/' != A_StrPath[0]) && L'\\' != A_StrPath[0] )
					{
						DrStrCatU16( Tv_StrFn, L"/" );
					}
					DrStrCatU16( Tv_StrFn,A_StrPath );
					Tv_Result	=	CreateDirectoryW( Tv_StrFn, NULL );
				}
			#else
				Tv_Result	=	CreateDirectoryW( A_StrPath, NULL );
			#endif

#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u64		DrFileGetSize( char* A_StrFn, BOOL8 A_IsNoEmu )
{
	// local -------------------
		__u64		Tv_Result;
		#ifdef	LINUX_APP
			struct	stat	Tv_FileStat;
		#else
			#ifdef LINUX_EMU
				char	Tv_StrFn[512];
			#endif
			HANDLE	Tv_HndlFile;
		#endif
	// code --------------------
		//init
		Tv_Result		=	0;

		#ifdef	LINUX_APP
			//get file size
			if ( 0 == stat( A_StrFn, &Tv_FileStat ) )
			{
				//check file type
				if ( S_ISREG(Tv_FileStat.st_mode) )
				{
					//update file size
					Tv_Result		=	Tv_FileStat.st_size;
				}
			}
		#else
			#ifdef LINUX_EMU
				if ( A_IsNoEmu )
				{
					Tv_HndlFile	=	CreateFileA( A_StrFn, GENERIC_READ, FILE_SHARE_WRITE, NULL, 
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
				}
				else
				{
					//set parameter
					DrStrCopyAsc( Tv_StrFn, DrFileCtrlEmuPathGetA() );
					if ( ('/' != A_StrFn[0]) && '\\' != A_StrFn[0] )
					{
						DrStrCatAsc( Tv_StrFn, "/" );
					}
					DrStrCatAsc( Tv_StrFn,A_StrFn );
					Tv_HndlFile	=	CreateFileA( Tv_StrFn, GENERIC_READ, FILE_SHARE_WRITE, NULL, 
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
				}
			#else
				Tv_HndlFile	=	CreateFileA( A_StrFn, GENERIC_READ, FILE_SHARE_WRITE, NULL, 
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
			#endif

			if ( INVALID_HANDLE_VALUE != Tv_HndlFile )
			{
				GetFileSizeEx( Tv_HndlFile, (PLARGE_INTEGER)&Tv_Result );
				CloseHandle( Tv_HndlFile );
			}

		#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u64		DrFileGetSize( WCHAR* A_StrFn, BOOL8 A_IsNoEmu )
{
	// local -------------------
		__u64		Tv_Result;
	#ifdef	LINUX_APP
		struct	stat	Tv_FileStat;
		Def_StrDrFnPath	Tv_StrFn;
	#else
		HANDLE	Tv_HndlFile;
		#ifdef LINUX_EMU
			WCHAR	Tv_StrFn[512];
		#endif
	#endif
	// code --------------------
		//init
		Tv_Result		=	0;

	#ifdef	LINUX_APP
		//get file size
		DrStrU16leToAsc( Tv_StrFn, A_StrFn );
		if ( 0 == stat( Tv_StrFn, &Tv_FileStat ) )
		{
			//check file type
			if ( S_ISREG(Tv_FileStat.st_mode) )
			{
				//update file size
				Tv_Result		=	Tv_FileStat.st_size;
			}
		}
	#else
		#ifdef LINUX_EMU
			if ( A_IsNoEmu )
			{
				Tv_HndlFile	=	CreateFileW( A_StrFn, GENERIC_READ, FILE_SHARE_WRITE, NULL, 
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
			}
			else
			{
				//set parameter
				DrStrCopyU16( Tv_StrFn, DrFileCtrlEmuPathGetW() );
				if ( (L'/' != A_StrFn[0]) && L'\\' != A_StrFn[0] )
				{
					DrStrCatU16( Tv_StrFn, L"/" );
				}
				DrStrCatU16( Tv_StrFn,A_StrFn );
				Tv_HndlFile	=	CreateFileW( Tv_StrFn, GENERIC_READ, FILE_SHARE_WRITE, NULL, 
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
			}
		#else
			Tv_HndlFile	=	CreateFileW( A_StrFn, GENERIC_READ, FILE_SHARE_WRITE, NULL, 
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		#endif

		if ( INVALID_HANDLE_VALUE != Tv_HndlFile )
		{
			GetFileSizeEx( Tv_HndlFile, (PLARGE_INTEGER)&Tv_Result );
			CloseHandle( Tv_HndlFile );
		}

	#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
/*!
 * \brief
 * get drive index number
 * 
 * \param A_StrPath
 * path 
 * 
 * \returns
 * drive index , 0 = A: 1 = B: ... , -1 = failure
 * 
 * \remarks
 * this function can use WIN32 mode only
 * 
 */
__s32		DrFileGetDrvIdxByPaht(char* A_StrPath)
{
	// local -------------------
		__s32			Tv_Result;
	// code --------------------
		//init
		Tv_Result		=	-1;

		#ifndef	LINUX_APP
			//check range
			if ( ( 'a' <= *A_StrPath ) && ( 'z' >= *A_StrPath) )
			{
				Tv_Result	=	(__s32)( *A_StrPath - 'a' );
			}
			else if ( ( 'A' <= *A_StrPath ) && ( 'Z' >= *A_StrPath) )
			{
				Tv_Result	=	(__s32)( *A_StrPath - 'A' );
			}
		#endif

		return	Tv_Result;
}

//--------------------------------------------------------------------
void	DrFileMakeDevNameByDrvIdx(__s32 A_DrvIdx, char* A_StrRtDevName)
{
	#ifndef	LINUX_APP
	// local -------------------
		char*		Tv_PtrDrvLtr;
	// code --------------------
		DrStrCopyAsc( A_StrRtDevName, "\\\\.\\A:" );
		Tv_PtrDrvLtr	=	A_StrRtDevName + 4;
		*Tv_PtrDrvLtr	=	'A' + (__s8)A_DrvIdx;
	#endif
}
//--------------------------------------------------------------------
BOOL8		DrFileCopy(char* A_StrSrFile, char* A_StrTgFile, BOOL8 A_IsNoEmu)
{
	// var --------------------------
		BOOL8		Tv_Result;
		Cls_DrFileCtrl*			Tv_ObjFlSr;
		Cls_DrFileCtrl*			Tv_ObjFlTg;
		void*			Tv_PtrBuf;
		__u32			Tv_FileSize;
		__u32			Tv_LastSize;
		__u32			Tv_WkSize;
	// code -------------------------
		//init
		Tv_Result		=	FALSE;

		Tv_ObjFlSr	=	(Cls_DrFileCtrl*)	new Cls_DrFileCtrl( A_StrSrFile, FALSE, FALSE,FALSE,A_IsNoEmu );
		Tv_FileSize	=		(__u32)Tv_ObjFlSr->GetSize();
		if ( Tv_FileSize > 0 )
		{
			//buffer allocate
			Tv_PtrBuf	=	(void*)malloc( E_DrFileToolCopyBufSize );
			if ( NULL != Tv_PtrBuf )
			{
				//check target file exist
				if ( DrFileIsExist( A_StrTgFile ) )
				{
					//delete file
					DrFileDelete( A_StrTgFile );
				}
				//create target file
				Tv_ObjFlTg	=	(Cls_DrFileCtrl*)	new Cls_DrFileCtrl( A_StrTgFile, TRUE, TRUE,FALSE,A_IsNoEmu );
				//check success
				if ( Tv_ObjFlTg->IsOpened() )
				{
					//init copy
					Tv_ObjFlSr->Seek(0);
					Tv_LastSize		=	Tv_FileSize;
					//run
					Tv_Result		=	TRUE;
					while ( 0 != Tv_LastSize )
					{
						Tv_WkSize		=	E_DrFileToolCopyBufSize;
						if ( E_DrFileToolCopyBufSize > Tv_LastSize )
						{
							Tv_WkSize		=	Tv_LastSize;
						}
						if ( Tv_WkSize == Tv_ObjFlSr->Read( Tv_PtrBuf, Tv_WkSize ) )
						{
							if ( Tv_WkSize != Tv_ObjFlTg->Write( Tv_PtrBuf, Tv_WkSize ) )
							{
								DbgMsgPrint((char*)"DrFileCopy - target file %s writing fail!.\n", A_StrTgFile);
								Tv_Result		=	FALSE;
								break;
							}
						}
						//next
						Tv_LastSize		=	Tv_LastSize - Tv_WkSize;
					}
				}
				else
				{
					DbgMsgPrint((char*)"DrFileCopy - target file %s create fail.\n", A_StrTgFile);
				}
				delete	Tv_ObjFlTg;
				free( Tv_PtrBuf );
			}
			else
			{
				DbgMsgPrint((char*)"DrFileCopy - not enough memory.\n");
			}
		}
		else
		{
			DbgMsgPrint((char*)"DrFileCopy - Source file %s not exist.\n", A_StrSrFile);
		}
		
		delete	Tv_ObjFlSr;

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileCopy( WCHAR* A_StrSrFile, WCHAR* A_StrTgFile, BOOL8 A_IsNoEmu )
{
	// var --------------------------
		BOOL8		Tv_Result;
		Cls_DrFileCtrl*			Tv_ObjFlSr;
		Cls_DrFileCtrl*			Tv_ObjFlTg;
		void*			Tv_PtrBuf;
		__u32			Tv_FileSize;
		__u32			Tv_LastSize;
		__u32			Tv_WkSize;
	// code -------------------------
		//init
		Tv_Result		=	FALSE;

		Tv_ObjFlSr	=	(Cls_DrFileCtrl*)	new Cls_DrFileCtrl( A_StrSrFile, FALSE, FALSE, FALSE, A_IsNoEmu );
		Tv_FileSize	=		(__u32)Tv_ObjFlSr->GetSize();
		if ( Tv_FileSize > 0 )
		{
			//buffer allocate
			Tv_PtrBuf	=	(void*)malloc( E_DrFileToolCopyBufSize );
			if ( NULL != Tv_PtrBuf )
			{
				//check target file exist
				if ( DrFileIsExist( A_StrTgFile ) )
				{
					//delete file
					DrFileDelete( A_StrTgFile );
				}
				//create target file
				Tv_ObjFlTg	=	(Cls_DrFileCtrl*)	new Cls_DrFileCtrl( A_StrTgFile, TRUE, TRUE,FALSE,A_IsNoEmu );
				//check success
				if ( Tv_ObjFlTg->IsOpened() )
				{
					//init copy
					Tv_ObjFlSr->Seek(0);
					Tv_LastSize		=	Tv_FileSize;
					//run
					Tv_Result		=	TRUE;
					while ( 0 != Tv_LastSize )
					{
						Tv_WkSize		=	E_DrFileToolCopyBufSize;
						if ( E_DrFileToolCopyBufSize > Tv_LastSize )
						{
							Tv_WkSize		=	Tv_LastSize;
						}
						if ( Tv_WkSize == Tv_ObjFlSr->Read( Tv_PtrBuf, Tv_WkSize ) )
						{
							if ( Tv_WkSize != Tv_ObjFlTg->Write( Tv_PtrBuf, Tv_WkSize ) )
							{
								DbgMsgPrint((char*)"DrFileCopy - target file writing fail!.\n");
								Tv_Result		=	FALSE;
								break;
							}
						}
						//next
						Tv_LastSize		=	Tv_LastSize - Tv_WkSize;
					}
				}
				else
				{
					DbgMsgPrint((char*)"DrFileCopy - target file create fail.\n");
				}
				delete	Tv_ObjFlTg;
				free( Tv_PtrBuf );
			}
			else
			{
				DbgMsgPrint((char*)"DrFileCopy - not enough memory.\n");
			}
		}
		else
		{
			DbgMsgPrint((char*)"DrFileCopy - Source file not exist.\n");
		}

		delete	Tv_ObjFlSr;

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileListOpen(char* A_StrPath, Ptr_DrFileListA A_PtrFind, char* A_StrExt, BOOL8 A_IsNoEmu)
{
	// local -------------------
		BOOL8	Tv_Result;
#ifndef LINUX_APP
		Def_StrDrFnPathShort	Tv_StrPath;
#endif

	// code --------------------
		Tv_Result	=	FALSE;

		DrStrClear( A_PtrFind->StrExt );
		if ( NULL != A_StrExt )
		{
			DrStrCopyAsc( A_PtrFind->StrExt, A_StrExt );
		}

#ifdef LINUX_APP

#ifdef DR_64BIT
		A_PtrFind->PtrDir	=	opendir(A_StrPath);
#else
		A_PtrFind->PtrDir	=	opendir( A_StrPath );
#endif
		if ( NULL != A_PtrFind->PtrDir )
		{
			Tv_Result	=	TRUE;
		}
#else

#ifdef LINUX_EMU
		if ( A_IsNoEmu )
		{
			DrStrCopyAsc( Tv_StrPath, A_StrPath );
		}
		else
		{
			DrStrCopyAsc( Tv_StrPath, DrFileCtrlEmuPathGetA() );
			DrStrFnAddBkSlshAsc( Tv_StrPath );
			DrStrCatAsc( Tv_StrPath, A_StrPath );
		}
#else
		DrStrCopyAsc( Tv_StrPath, A_StrPath );
#endif
		DrStrFnAddBkSlshAsc( Tv_StrPath );
		if ( DrStrIsExist( A_PtrFind->StrExt ) )
		{
			DrStrCatAsc(Tv_StrPath, V_DrFileToolStrWild);
			DrStrCatAsc( Tv_StrPath, A_StrExt );
		}
		else
		{
			DrStrCatAsc( Tv_StrPath, V_DrFileToolStrAllFile);
		}
		A_PtrFind->Hndl	=	FindFirstFileA( Tv_StrPath, &A_PtrFind->FindInfo );
		if ( INVALID_HANDLE_VALUE != A_PtrFind->Hndl )
		{
			Tv_Result	=	TRUE;
		}
#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileListGet(Ptr_DrFileListA A_PtrFind, char* A_StrRt)
{
	// local -------------------
		BOOL8	Tv_Result;
#ifdef	LINUX_APP
#ifdef DR_64BIT
		dirent64* Tv_PtrDirent;
#else
		dirent*	Tv_PtrDirent;
#endif
		Def_StrName	Tv_StrExt;
#else
		BOOL8	Tv_IsNext;
#endif
	// code --------------------
		Tv_Result	=	FALSE;

#ifdef LINUX_APP
		while ( !Tv_Result )
		{
#ifdef DR_64BIT
			Tv_PtrDirent	=	readdir64( A_PtrFind->PtrDir );
#else
			Tv_PtrDirent	=	readdir(A_PtrFind->PtrDir);
#endif
			if ( NULL == Tv_PtrDirent )
			{
				break;
			}
			//check usable
			if (	( 0 != DrStrCmpAsc( Tv_PtrDirent->d_name , ".", TRUE ) ) &&
						( 0 != DrStrCmpAsc( Tv_PtrDirent->d_name , "..", TRUE ) ) )
			{
				//check match extention
				Tv_Result		=	TRUE;
				if ( DrStrIsExist( A_PtrFind->StrExt ) )
				{
					//compare extention
					DrStrFnGetExtAsc( Tv_PtrDirent->d_name, Tv_StrExt );
					if ( 0 != (DrStrCmpAsc( Tv_StrExt , A_PtrFind->StrExt, FALSE ) ) )
					{
						Tv_Result		=	FALSE;
					}
				}
				//update result
				if ( Tv_Result )
				{
					DrStrCopyAsc( A_StrRt, Tv_PtrDirent->d_name );
				}
			}
		}
#else

		do
		{
			Tv_IsNext	=	FALSE;
			if (DrStrIsExist(A_PtrFind->FindInfo.cFileName))
			{
				Tv_IsNext	=	TRUE;
				//check usable
				if ((0 != DrStrCmpAsc(A_PtrFind->FindInfo.cFileName, ".", TRUE)) &&
					(0 != DrStrCmpAsc(A_PtrFind->FindInfo.cFileName, "..", TRUE)))
				{
					//copy result
					DrStrCopyAsc(A_StrRt, A_PtrFind->FindInfo.cFileName);
					Tv_IsNext	=	FALSE;
					Tv_Result	=	TRUE;
				}
				//go next
				if (!FindNextFileA(A_PtrFind->Hndl, &A_PtrFind->FindInfo))
				{
					//next data none
					DrStrClear(A_PtrFind->FindInfo.cFileName);
				}
			}


		} while (Tv_IsNext);


#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileListClose( Ptr_DrFileListA A_PtrFind )
{
#ifdef LINUX_APP
	// local -------------------
		BOOL8	Tv_Result;
	// code --------------------
		Tv_Result	=	FALSE;
#ifdef DR_64BIT
		if(0 == closedir(A_PtrFind->PtrDir))
#else
		if ( 0 == closedir( A_PtrFind->PtrDir ) )
#endif
		{
			Tv_Result	=	TRUE;
		}
		return	Tv_Result;
#else
		return	FindClose( A_PtrFind->Hndl );
#endif
}
//--------------------------------------------------------------------
__u64		DrFilePartSize(char* A_PtrDev, __u64* A_PtrRtFree)
{
#ifdef LINUX_APP
	// local -------------------
		__u64	Tv_Result;
		struct statfs Tv_StatFs;
	// code --------------------
		Tv_Result			=	0;
		*A_PtrRtFree	=	0;
		if ( 0 == statfs( A_PtrDev, &Tv_StatFs ) )
		{
			Tv_Result			=	(__u64)Tv_StatFs.f_bsize * (__u64)Tv_StatFs.f_blocks;
			*A_PtrRtFree	=	(__u64)Tv_StatFs.f_bsize * (__u64)Tv_StatFs.f_bavail;
		}
		return	Tv_Result;
#else
		*A_PtrRtFree	=	0;
		return	0;
#endif
}
//--------------------------------------------------------------------
BOOL8		DrFileGetAtb(char* A_StrFn, __u32* A_PtrRtMode, __u32* A_PtrRtPara, BOOL8 A_IsNoEmu)
{
	// local -------------------
		BOOL8	Tv_Result;
#ifdef LINUX_APP
		struct stat Tv_Stat;
#else
		DWORD	Tv_Atb;
#endif
	// code --------------------
		Tv_Result	=	FALSE;

		*A_PtrRtMode	=	0;
		*A_PtrRtPara	=	0;

#ifdef LINUX_APP
		if (0 == lstat(A_StrFn,&Tv_Stat))
		{
			*A_PtrRtMode	=	(__u32)Tv_Stat.st_mode;
			*A_PtrRtPara	=	(__u32)Tv_Stat.st_dev;
			Tv_Result	=	TRUE;
		}
#else
		Tv_Atb	=	GetFileAttributesA(A_StrFn);
		if (INVALID_FILE_ATTRIBUTES != Tv_Atb)
		{
			*A_PtrRtMode	=	E_DrFileAtbTypeNormal;
			if (0 != (FILE_ATTRIBUTE_DIRECTORY & Tv_Atb))
			{
				*A_PtrRtMode	=	E_DrFileAtbTypeDir;
			}
			Tv_Result	=	TRUE;
		}
#endif

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrFileGetAtb(WCHAR* A_StrFn, __u32* A_PtrRtMode, __u32* A_PtrRtPara, BOOL8 A_IsNoEmu)
{
	// local -------------------
		BOOL8	Tv_Result;
#ifdef LINUX_APP
		Def_StrDrFnPath	Tv_StrFn;
		struct stat Tv_Stat;
#else
		DWORD	Tv_Atb;
#endif
	// code --------------------
		Tv_Result	=	FALSE;

		*A_PtrRtMode	=	0;
		*A_PtrRtPara	=	0;

#ifdef LINUX_APP
		DrStrU16leToAsc( Tv_StrFn, A_StrFn );
		if (0 == stat(Tv_StrFn, &Tv_Stat))
		{
			*A_PtrRtMode	=	(__u32)Tv_Stat.st_mode;
			*A_PtrRtPara	=	(__u32)Tv_Stat.st_dev;
			Tv_Result	=	TRUE;
		}
#else
		Tv_Atb	=	GetFileAttributesW(A_StrFn);
		if (INVALID_FILE_ATTRIBUTES != Tv_Atb)
		{
			*A_PtrRtMode	=	E_DrFileAtbTypeNormal;
			if (0 != (FILE_ATTRIBUTE_DIRECTORY & Tv_Atb))
			{
				*A_PtrRtMode	=	E_DrFileAtbTypeDir;
			}
		}
#endif

		return	Tv_Result;

}
//--------------------------------------------------------------------

