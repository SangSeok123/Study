/*
 archive tool

 */

//====================================================================
// uses
#include <DrPartFile.h>

#include <DrDebug.h>

#include <DrStrTool.h>
#include <DrTimeTool.h>
#include <DrDumyTool.h>
#include <DrFileTool.h>


//====================================================================
//local type

//====================================================================
//local function

//====================================================================
//var


//====================================================================

//--------------------------------------------------------------------
Cls_DrPartFile::Cls_DrPartFile(BOOL8 A_IsNoEmu)
{
	m_IsNoEmu		=	A_IsNoEmu;

	DrDumyZeroMem(&m_FlHd, sizeof(m_FlHd));

	m_Fc				=	NULL;
}
//--------------------------------------------------------------------
Cls_DrPartFile::~Cls_DrPartFile()
{

	if(NULL != m_Fc)
	{
		delete	m_Fc;
		m_Fc	=	NULL;
	}

}
//--------------------------------------------------------------------
__s32	Cls_DrPartFile::LcFindPartByName(char* A_StrName)
{
	__u32	Tv_PrtIdx;

	for( Tv_PrtIdx = 0; Tv_PrtIdx < E_DrPartFilePartMaxCnt; Tv_PrtIdx++)
	{
		if(0 == m_FlHd.PartTbl[Tv_PrtIdx].Size)
		{
			continue;
		}

		if(0 == DrStrCmpAsc(m_FlHd.PartTbl[Tv_PrtIdx].StrName, A_StrName, TRUE))
		{
			// found
			return	(__s32)Tv_PrtIdx;
		}

	}

	return	-1;
}
//--------------------------------------------------------------------
Def_DrErrCode	Cls_DrPartFile::LcSave(Ptr_DrPartFilePartPara A_PtrParas, __u32 A_ParaCnt)
{
	__s32	Tv_PrtIdx;
	__u32	Tv_PrtCnt;
	__u32	Tv_InSize;
	__u32	Tv_WkIdx;

	if((0 == A_ParaCnt) || (E_DrPartFilePartMaxCnt < A_ParaCnt))
	{
		return	E_DrErrBadParam;
	}

	// build head
	DrDumyZeroMem(&m_FlHd, sizeof(m_FlHd));
	m_FlHd.Fcc		=	E_DrPartFileFcc;
	m_FlHd.Ver		=	E_DrPartFileVer;

	// build part table
	Tv_PrtCnt	=	0;
	Tv_InSize	=	sizeof(St_DrPartFileHead);
	for(Tv_WkIdx = 0; Tv_WkIdx < A_ParaCnt; Tv_WkIdx++)
	{
		// check item info
		if((0 == A_PtrParas[Tv_WkIdx].Size) || (!DrStrIsExist(A_PtrParas[Tv_WkIdx].StrName)))
		{
			continue;
		}
		
		// add
		m_FlHd.PartTbl[Tv_PrtCnt].Size	=	A_PtrParas[Tv_WkIdx].Size;
		DrStrCopyAsc(m_FlHd.PartTbl[Tv_PrtCnt].StrName, A_PtrParas[Tv_WkIdx].StrName,63);
		m_FlHd.PartTbl[Tv_PrtCnt].Offset	=	Tv_InSize;

		// next
		Tv_InSize	=	Tv_InSize + A_PtrParas[Tv_WkIdx].Size;
		Tv_PrtCnt ++;
	}

	// write head
	m_Fc->Write(&m_FlHd, sizeof(m_FlHd));

	// write part
	if(0 != Tv_PrtCnt)
	{
		for(Tv_WkIdx = 0; Tv_WkIdx < Tv_PrtCnt; Tv_WkIdx++)
		{
			// check item info
			if((0 == A_PtrParas[Tv_WkIdx].Size) || (!DrStrIsExist(A_PtrParas[Tv_WkIdx].StrName)))
			{
				continue;
			}

			Tv_PrtIdx	=	LcFindPartByName(A_PtrParas[Tv_WkIdx].StrName);
			if(0 > Tv_PrtIdx)
			{
				continue;
			}

			m_Fc->Write(A_PtrParas[Tv_WkIdx].Ptr, A_PtrParas[Tv_WkIdx].Size);
		}
	}

	// finish
	m_Fc->Flush();

	delete	m_Fc;

	m_Fc	=	NULL;

	return	E_DrErrNone;
}
//--------------------------------------------------------------------
Def_DrErrCode	Cls_DrPartFile::Save(char* A_StrFn, Ptr_DrPartFilePartPara A_PtrParas, __u32 A_ParaCnt)
{
	m_Fc	=	(Cls_DrFileCtrl*)new Cls_DrFileCtrl(A_StrFn, TRUE, TRUE, FALSE, m_IsNoEmu);
	if(NULL == m_Fc)
	{
		return	E_DrErrInitFail;
	}

	return	LcSave(A_PtrParas, A_ParaCnt);
}
//--------------------------------------------------------------------
Def_DrErrCode	Cls_DrPartFile::Save(WCHAR* A_StrFn, Ptr_DrPartFilePartPara A_PtrParas, __u32 A_ParaCnt)
{
	m_Fc	=	(Cls_DrFileCtrl*)new Cls_DrFileCtrl(A_StrFn, TRUE, TRUE, FALSE, m_IsNoEmu);
	if(NULL == m_Fc)
	{
		return	E_DrErrInitFail;
	}

	return	LcSave(A_PtrParas, A_ParaCnt);

}
//--------------------------------------------------------------------
Def_DrErrCode	Cls_DrPartFile::LcLoad(Ptr_DrPartFilePartPara A_PtrParas, __u32 A_ParaCnt)
{
	__u32	Tv_ParaIdx;
	__s32	Tv_PrtIdx;
	__u32	Tv_ReadSize;

	// load file header
	if(sizeof(St_DrPartFileHead) != m_Fc->Read(&m_FlHd,sizeof(St_DrPartFileHead)))
	{
		delete	m_Fc;
		m_Fc	=	NULL;
		return	E_DrErrBadIo;
	}
	
	for( Tv_ParaIdx = 0; Tv_ParaIdx < A_ParaCnt; Tv_ParaIdx++)
	{
		// check item info
		if((0 == A_PtrParas[Tv_ParaIdx].Size) || (!DrStrIsExist(A_PtrParas[Tv_ParaIdx].StrName)))
		{
			continue;
		}

		Tv_PrtIdx	=	LcFindPartByName(A_PtrParas[Tv_ParaIdx].StrName);
		if(0 > Tv_PrtIdx)
		{
			continue;
		}

		// load
		Tv_ReadSize	=	A_PtrParas[Tv_ParaIdx].Size;
		if(Tv_ReadSize >= m_FlHd.PartTbl[Tv_PrtIdx].Size )
		{
			Tv_ReadSize	=	m_FlHd.PartTbl[Tv_PrtIdx].Size;
		}

		m_Fc->Seek(m_FlHd.PartTbl[Tv_PrtIdx].Offset);
		m_Fc->Read(A_PtrParas[Tv_ParaIdx].Ptr, Tv_ReadSize);

	}

	return	E_DrErrNone;
}
//--------------------------------------------------------------------
Def_DrErrCode	Cls_DrPartFile::Load(char* A_StrFn, Ptr_DrPartFilePartPara A_PtrParas, __u32 A_ParaCnt)
{
	m_Fc	=	(Cls_DrFileCtrl*)new Cls_DrFileCtrl(A_StrFn, FALSE, FALSE, FALSE, m_IsNoEmu);
	if(NULL == m_Fc)
	{
		return	E_DrErrInitFail;
	}

	return	LcLoad(A_PtrParas, A_ParaCnt);

}
//--------------------------------------------------------------------
Def_DrErrCode	Cls_DrPartFile::Load(WCHAR* A_StrFn, Ptr_DrPartFilePartPara A_PtrParas, __u32 A_ParaCnt)
{
	m_Fc	=	(Cls_DrFileCtrl*)new Cls_DrFileCtrl(A_StrFn, FALSE, FALSE, FALSE, m_IsNoEmu);
	if(NULL == m_Fc)
	{
		return	E_DrErrInitFail;
	}

	return	LcLoad(A_PtrParas, A_ParaCnt);

}
//--------------------------------------------------------------------
