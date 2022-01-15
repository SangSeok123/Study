/*
	file utility - on WIN32

*/

//--------------------------------------------------------------------
//Enviroment conditional
#ifndef	_EPC_DrFileTool
#define	_EPC_DrFileTool
//====================================================================
// uses

#ifdef	LINUX_APP
	#include	<sys/vfs.h>
	#include	<stdio.h>
	#include	<dirent.h>

#endif
	
//====================================================================
//constance

#define	E_DrFileAtbTypeMask			00170000	

#define	E_DrFileAtbTypeDir			0040000
#define	E_DrFileAtbTypeLink			0120000
#define	E_DrFileAtbTypeNormal		0100000
#define	E_DrFileAtbTypeBlkDev		0060000
#define	E_DrFileAtbTypeChrDev		0020000


//====================================================================
//global macro


//====================================================================
//global type

typedef	struct St_DrFileListA
{
#ifdef LINUX_APP

	DIR*	PtrDir;
#else
	WIN32_FIND_DATAA	FindInfo;
	HANDLE	Hndl;
#endif
	Def_StrName	StrExt;
}	*Ptr_DrFileListA;

//====================================================================
//class

	
//====================================================================
//global function

//file
BOOL8		DrFileIsExist( char* A_StrFn, BOOL8 A_IsNoEmu = FALSE );
BOOL8		DrFileIsExist( WCHAR* A_StrFn, BOOL8 A_IsNoEmu = FALSE );

BOOL8		DrFileDelete( char* A_StrFn, BOOL8 A_IsNoEmu = FALSE );
BOOL8		DrFileDelete( WCHAR* A_StrFn, BOOL8 A_IsNoEmu = FALSE );

BOOL8		DrFileRename(char* A_StrTg, char* A_StrSr, BOOL8 A_IsNoEmu = FALSE);
BOOL8		DrFileRename(WCHAR* A_StrTg, WCHAR* A_StrSr, BOOL8 A_IsNoEmu = FALSE);

__u64		DrFileGetSize( char* A_StrFn, BOOL8 A_IsNoEmu = FALSE );
__u64		DrFileGetSize( WCHAR* A_StrFn, BOOL8 A_IsNoEmu = FALSE );

BOOL8		DrFileCopy(char* A_StrSrFile, char* A_StrTgFile, BOOL8 A_IsNoEmu = FALSE);
BOOL8		DrFileCopy( WCHAR* A_StrSrFile, WCHAR* A_StrTgFile, BOOL8 A_IsNoEmu = FALSE );

BOOL8		DrFileGetAtb(char* A_StrFn, __u32* A_PtrRtMode, __u32* A_PtrRtPara, BOOL8 A_IsNoEmu = FALSE);
BOOL8		DrFileGetAtb(WCHAR* A_StrFn, __u32* A_PtrRtMode, __u32* A_PtrRtPara, BOOL8 A_IsNoEmu = FALSE);

//dir
BOOL8		DrFileIsExistDir( char* A_StrFn, BOOL8 A_IsNoEmu = FALSE );
BOOL8		DrFileIsExistDir( WCHAR* A_StrFn, BOOL8 A_IsNoEmu = FALSE );

BOOL8		DrFileDelDir( char* A_StrFn, BOOL8 A_IsNoEmu = FALSE );
BOOL8		DrFileDelDir( WCHAR* A_StrFn, BOOL8 A_IsNoEmu = FALSE );

BOOL8		DrFileCreateDir(char* A_StrPath, BOOL8 A_IsNoEmu = FALSE, __u32 A_Owner = 0777);
BOOL8		DrFileCreateDir(WCHAR* A_StrPath, BOOL8 A_IsNoEmu = FALSE, __u32 A_Owner = 0777);

//char only
BOOL8		DrFileListOpen(char* A_StrPath, Ptr_DrFileListA A_PtrFind, char* A_StrExt, BOOL8 A_IsNoEmu = FALSE);

BOOL8		DrFileListGet(Ptr_DrFileListA A_PtrFind, char* A_StrRt);

BOOL8		DrFileListClose( Ptr_DrFileListA A_PtrFind );

// partition
__u64		DrFilePartSize(char* A_PtrDev, __u64* A_PtrRtFree);		//partition size


//====================================================================
//out Enviroment conditional
#endif
