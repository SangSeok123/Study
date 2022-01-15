/*
	part file

*/

//--------------------------------------------------------------------
//Enviroment conditional
#ifndef	_EPC_DrPartFile
#define	_EPC_DrPartFile

//====================================================================
// uses
#include <DrPlatform.h>
#include <DrTypeBase.h>

#include <DrError.h>

#include <DrFileCtrl.h>

//====================================================================
//constance

#define	E_DrPartFilePartMaxCnt		32

#define	E_DrPartFileFcc						Mac_DrMakeFcc('D','R','P','F')
#define	E_DrPartFileVer						1

//====================================================================
//global macro

//====================================================================
//global type

typedef struct St_DrPartFilePartPara
{
	void* Ptr;
	__u32	Size;
	Def_StrTag	StrName;
}	*Ptr_DrPartFilePartPara;

typedef struct St_DrPartFilePartItem
{
	Def_StrTag	StrName;
	__u32	Size;
	__u32	Offset;
}	*Ptr_DrPartFilePartItem;

typedef struct St_DrPartFileHead
{
	__u32	Fcc;
	__u32	Ver;
	__u32	_rsvd0;
	__u32	_rsvd1;
	St_DrPartFilePartItem	PartTbl[E_DrPartFilePartMaxCnt];

}*Ptr_DrPartFileHead;

//====================================================================
//class

class	Cls_DrPartFile
{
	protected:
		BOOL8	m_IsNoEmu;

		St_DrPartFileHead	m_FlHd;
		Cls_DrFileCtrl*	m_Fc;		// file control

		__s32	LcFindPartByName(char* A_StrName);

		Def_DrErrCode	LcSave(Ptr_DrPartFilePartPara A_PtrParas, __u32 A_ParaCnt);
		Def_DrErrCode	LcLoad(Ptr_DrPartFilePartPara A_PtrParas, __u32 A_ParaCnt);

	public:
		Cls_DrPartFile(BOOL8 A_IsNoEmu = FALSE);
		virtual		~Cls_DrPartFile();

		Def_DrErrCode	Save(char* A_StrFn, Ptr_DrPartFilePartPara A_PtrParas, __u32 A_ParaCnt);
		Def_DrErrCode	Save(WCHAR* A_StrFn, Ptr_DrPartFilePartPara A_PtrParas, __u32 A_ParaCnt);
		Def_DrErrCode	Load(char* A_StrFn, Ptr_DrPartFilePartPara A_PtrParas, __u32 A_ParaCnt);
		Def_DrErrCode	Load(WCHAR* A_StrFn, Ptr_DrPartFilePartPara A_PtrParas, __u32 A_ParaCnt);

};

	
//====================================================================
//global function

//====================================================================
//out Enviroment conditional
#endif

