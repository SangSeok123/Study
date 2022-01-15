//--------------------------------------------------------------------
//Environment conditional
#ifndef	_EPC_DrDtc1
#define	_EPC_DrDtc1

//====================================================================
// uses
#include	<DrPlatform.h>
#include	<DrTypeBase.h>

//====================================================================
//constance

#define E_DrDtc1ValLow		2631
#define E_DrDtc1ValHigh		5731

//====================================================================
//global macro

//====================================================================
//global type

typedef struct St_DrDtc1BitCtl
{
	void* PtrBuf;
	__u32 ByteCnt;
	__u32 BitCnt;
} *Ptr_DrDtc1BitCtl;


//====================================================================
//global function

void DrDtc1BitsWrite(Ptr_DrDtc1BitCtl A_PtrCtl, __u16 A_Val, __u32 A_BitCnt);
__u16 DrDtc1BitsRead(Ptr_DrDtc1BitCtl A_PtrCtl, __u32 A_BitCnt);

__u32 DrDtc1Encode(void* A_PtrSr, __u32 A_InCnt, void* A_PtrOut);
void DrDtc1Decode(void* A_PtrSr, __u32 A_OutCnt, void* A_PtrOut);


//====================================================================
//out Environment conditional
#endif

