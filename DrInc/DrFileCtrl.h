/*
	emulation able file control

*/

//--------------------------------------------------------------------
//Enviroment conditional
#ifndef	_EPC_DrFileCtrl
#define	_EPC_DrFileCtrl

//====================================================================
// uses
#include <DrTypeBase.h>

#ifdef LINUX_APP

#include <unistd.h>

#endif

//====================================================================
//constance

#ifdef LINUX_APP

#define	E_DrFileCtrlPosBegin		SEEK_SET
#define	E_DrFileCtrlPosNow			SEEK_CUR
#define	E_DrFileCtrlPosEnd			SEEK_END

#else

#define	E_DrFileCtrlPosBegin		FILE_BEGIN
#define	E_DrFileCtrlPosNow			FILE_CURRENT
#define	E_DrFileCtrlPosEnd			FILE_END

#endif

//====================================================================
//global macro

//====================================================================
//global type

//====================================================================
//class

class	Cls_DrFileCtrl
{
	private:
		BOOL8			m_IsOpened;
		BOOL8			m_IsSync;

		#ifdef LINUX_APP
			__s32				m_HndlFile;
		#else
			HANDLE		m_HndlFile;
		#endif
	protected:

	public:
    Cls_DrFileCtrl( char* A_StrFileName, BOOL8 A_IsWrite, BOOL8 A_IsCreateAble, 
										BOOL8 A_IsExcute = FALSE, BOOL8 A_IsNonEmu = FALSE, BOOL8 A_IsSync = FALSE, 
										BOOL8 A_IsDirect = FALSE );
		Cls_DrFileCtrl( WCHAR* A_StrFileName, BOOL8 A_IsWrite, BOOL8 A_IsCreateAble, 
										BOOL8 A_IsExcute = FALSE, BOOL8 A_IsNonEmu = FALSE, BOOL8 A_IsSync = FALSE, 
										BOOL8 A_IsDirect = FALSE );
    virtual		~Cls_DrFileCtrl();

		__s64	Seek( __s64 A_Position, __u32 A_Mode = E_DrFileCtrlPosBegin);
		__u32		Read( void* A_Buf, __u32 A_Size );
		__u32		Write( void* A_Buf, __u32 A_Size );

		__s64	GetPos( void );
		__s64	GetSize( void );

		BOOL8		IsOpened( void );
		void	Flush(void);
};
	
//====================================================================
//global function

__u32	DrFileCtrlRead(char* A_StrFn, void* A_PtrBuf, __u32 A_Size, BOOL8 A_IsNonEmu = FALSE);
__u32	DrFileCtrlRead( WCHAR* A_StrFn, void* A_PtrBuf, __u32 A_Size, BOOL8 A_IsNonEmu = FALSE );
__u32	DrFileCtrlWrite(char* A_StrFn, BOOL8 A_IsCreate, void* A_PtrBuf, __u32 A_Size, BOOL8 A_IsNonEmu = FALSE, BOOL8 A_IsSync = FALSE, BOOL8 A_IsAdd = FALSE);
__u32	DrFileCtrlWrite( WCHAR* A_StrFn, BOOL8 A_IsCreate, void* A_PtrBuf, __u32 A_Size, BOOL8 A_IsNonEmu = FALSE, BOOL8 A_IsSync = FALSE, BOOL8 A_IsAdd = FALSE );

void	DrFileCtrlEmuPathSet(char* A_StrPath);
void	DrFileCtrlEmuPathSet( WCHAR* A_StrPath );
char*	DrFileCtrlEmuPathGetA( void );
WCHAR*	DrFileCtrlEmuPathGetW( void );

//====================================================================
//out Enviroment conditional
#endif

