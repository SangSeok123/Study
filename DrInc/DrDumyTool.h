//--------------------------------------------------------------------
//Enviroment conditional
#ifndef	_EPC_DrDumyTool
#define	_EPC_DrDumyTool

//====================================================================
// uses
#include	<DrPlatform.h>
#include	<DrTypeBase.h>
#include	<string.h>

//====================================================================
//constance

#define 	E_DrDumyNotFoundIdx		0xFFFFFFFF

//====================================================================
//global macro
#define		DrDumyMemCmp	memcmp
#define		DrDumyMoveMem	memmove
#define		DrDumyCopyMem	memmove

#ifdef LINUX_APP
#define	DrDumyDwGetNoneZeroBitCount	(__u32)__builtin_popcount
#else
#define	DrDumyDwGetNoneZeroBitCount	(__u32)__popcnt
#endif 

#ifdef LINUX_APP
#define	DrDumyZeroMem(A_PtrTg, A_Size)	memset( A_PtrTg, 0, A_Size )
#else
#define	DrDumyZeroMem(A_PtrTg, A_Size)	ZeroMemory(A_PtrTg, A_Size)
#endif

//====================================================================
//global type

typedef struct St_DrDumyBase64Enc
{
	__u8	RmDat[3];
	__u8	RmSize;
}	*Ptr_DrDumyBase64Enc;

typedef struct St_DrDumyBase64Dec
{
	__u8	RmDat[4];
	__u32	RmSize;
}	*Ptr_DrDumyBase64Dec;

typedef struct St_DrDumyBitCtrl
{
	__u8*	PtrBuf;
	__u32	Byte;
	__u32	Bit;
}	*Ptr_DrDumyBitCtrl;


//====================================================================
//global function
	
	//void		DrDumyZeroMem( void* A_PtrTg, __u32 A_Size );		//zero memory	
	
	__u32		DrDumyDwGetFirstBitIdx( __u32 A_Mask );
	__u32		DrDumyDwClearBit( __u32 A_Mask,__u32 A_BitIdx );
	void		DrDumyInsSpace( void* A_PtrBuf, __u32 A_BufSize, __u32 A_InsPos, __u32 A_InsSize );
	void		DrDumyDelSpace( void* A_PtrBuf, __u32 A_BufSize, __u32 A_DelPos, __u32 A_DelSize );
	void		DrDumyCopyWithSpace( void* A_PtrTg, void* A_PtrSr, __u32 A_SizeSr, __u32 A_DivPos, __u32 A_SizeSpace );

					//input	-		A_PtrTbl	:	dword table pointer
					//					A_PtrRtCnt	:	table item counter variable pointer - store table item count
					//output	-	A_PtrRtCnt	:	store collected item count
	void		DrDumyCollectNullDword( void* A_PtrTbl, __s32* A_PtrRtCnt );		//collect null item dword unit

	__u32		DrDumyGetBits( void* A_PtrSr, __u32 A_BitPosSr, __u32 A_GetBitSize, __u32 A_GetBitPos, __u32* A_PtrRtMask );
	__u32		DrDumyGetNoneZeroBitMask( __u32 A_Value, __u32 A_BitUnit );

	BOOL8		DrDumyCmpDwUnit( void* A_PtrSr, void* A_PtrTg, __u32 A_DwCnt );

#ifdef DR_ACC_X86
	//----------------------------------------
	// x86 accelat - start
	//----------------------------------------

	inline	__u32	DrDumyDwGetZeroBitCntRight( __u32 A_Val )
	{
#ifdef LINUX_APP
		// local -------------------
		__u32		Tv_Result;
		// code --------------------
		//gcc
		__asm__ __volatile__(
			"movl	32 , %0	\n\t"
			"bsfl	%1 , %0										\n\t"
			:"=r" (Tv_Result) 
			:"rm" (A_Val)
			);
		return	Tv_Result;
#else
		//vcc
		__asm
		{
			mov		eax , 32
				bsf		eax , A_Val
		}
#endif
	}

	inline	__u32	DrDumyDwGetLastBitIdx( __u32 A_Mask )
	{
#ifdef LINUX_APP
		// local -------------------
		__u32		Tv_Result;
		// code --------------------
		//gcc
		__asm__ __volatile__(
			"movl	E_DrDumyNotFoundIdx , %0	\n\t"
			"bsrl	%1 , %0										\n\t"
			:"=r" (Tv_Result) 
			:"rm" (A_Mask)
			);
		return	Tv_Result;
#else
		//vcc
		__asm
		{
			mov		eax , E_DrDumyNotFoundIdx
				bsr		eax , A_Mask
		}
#endif
	}

	inline	void*	DrDumyFillD( void* A_PtrBuf, __u32 A_DwCnt, __u32 A_Val )
	{
#ifdef LINUX_APP
		//gcc
		// local -------------------
		void*		Tv_Result;
		// code --------------------
		__asm__ __volatile__(
			"movl	E_DrDumyNotFoundIdx , %0	\n\t"
			"rep				\n\t"
			"stosl			\n\t"
			:"=D"(Tv_Result)
			:"a"(A_Val),"c"(A_DwCnt), "D"(A_PtrBuf)
			);
		return	Tv_Result;
#else
		//vcc
		__asm
		{
			mov		eax , A_Val
				mov		edi , A_PtrBuf
				mov		ecx , A_DwCnt
				rep	stosd
				mov		eax , edi
		}
#endif
	}

	inline	void*	DrDumyFillW( void* A_PtrBuf, __u32 A_WdCnt, __u16 A_Val )
	{
#ifdef LINUX_APP
		//gcc
		// local -------------------
		void*		Tv_Result;
		// code --------------------
		__asm__ __volatile__(
			"movl	E_DrDumyNotFoundIdx , %0	\n\t"
			"rep				\n\t"
			"stosw			\n\t"
			:"=D"(Tv_Result)
			:"a"(A_Val),"c"(A_WdCnt), "D"(A_PtrBuf)
			);
		return	Tv_Result;
#else
		//vcc
		__asm
		{
			movzx	eax , A_Val
				mov		edi , A_PtrBuf
				mov		ecx , A_WdCnt
				rep	stosw
				mov		eax , edi
		}
#endif
	}

	inline	void*	DrDumyFillB( void* A_PtrBuf, __u32 A_BtCnt, __u8 A_Val )
	{
#ifdef LINUX_APP
		//gcc
		// local -------------------
		void*		Tv_Result;
		// code --------------------
		__asm__ __volatile__(
			"movl	E_DrDumyNotFoundIdx , %0	\n\t"
			"rep				\n\t"
			"stosw			\n\t"
			:"=D"(Tv_Result)
			:"a"(A_Val),"c"(A_BtCnt), "D"(A_PtrBuf)
			);
		return	Tv_Result;
#else
		//vcc
		__asm
		{
			movzx	eax , A_Val
				mov		edi , A_PtrBuf
				mov		ecx , A_BtCnt
				rep	stosb
				mov		eax , edi
		}
#endif
	}
	//----------------------------------------
	//defatul = x86 archtecture - finish
	//----------------------------------------
#else
//----------------------------------------
//not accelate - start
//----------------------------------------
#define DR_ACC_DUMYTOOL_NONE

__u32	DrDumyDwGetZeroBitCntRight( __u32 A_Val );
__u32	DrDumyDwGetLastBitIdx( __u32 A_Mask );
void*	DrDumyFillD( void* A_PtrBuf, __u32 A_DwCnt, __u32 A_Val );
void*	DrDumyFillW( void* A_PtrBuf, __u32 A_WdCnt, __u16 A_Val );
#define	DrDumyFillB( A_PtrBuf, A_BtCnt, A_Val )	memset(A_PtrBuf,A_Val,A_BtCnt)

__u8	DrDumyBitRvsU8(__u8 A_Src);
__u32	DrDumyDwBitReverse(__u32 A_Src, __u32 A_BitPos, __u32 A_BitCnt, __u32 A_Dst, __u32 A_BitDst);

//----------------------------------------
//not accelerate - finish
//----------------------------------------
#endif

__u32		DrDumyCalcChksumBt(void* A_PtrBuf, __u32 A_Size, __u32 A_PrevChksum = 0);		//calculate checksum byte unit
__u32		DrDumyCalcChksumDw(void* A_PtrBuf, __u32 A_Size, __u32 A_PrevChksum = 0);		//calculate checksum dword unit

	void	DrDumySwapByte1Bit( void* A_PtrBuf, __u32 A_ByteCnt );
	void	DrDumySwapByte4Bit( void* A_PtrBuf, __u32 A_ByteCnt );

	BOOL8	DrDumyAbandoneByDw(void* A_PtrBuf, __u32 A_BufSize, __u32 A_Basis, __u32* A_PtrRtSize);

	//endian
	void	DrDumyEndian4byteSet( void* A_PtrTg, __u32 A_Val );
	void	DrDumyEndian3byteSet( void* A_PtrTg, __u32 A_Val );
	void	DrDumyEndian2byteSet( void* A_PtrTg, __u16 A_Val );
	__u64	DrDumyEndian8byteGet(void* A_PtrSr);
	__u32	DrDumyEndian4byteGet( void* A_PtrSr );
	__u32	DrDumyEndian3byteGet( void* A_PtrSr );
	__u16	DrDumyEndian2byteGet( void* A_PtrSr );
	void	DrDumyWrite4Byte( void* A_PtrTg, __u32 A_Val );
	void	DrDumyWrite3Byte( void* A_PtrTg, __u32 A_Val );
	void	DrDumyWrite2Byte( void* A_PtrTg, __u16 A_Val );
	__u32	DrDumyRead4Byte( void* A_PtrSr );
	__u32	DrDumyRead3Byte( void* A_PtrSr );
	__u16	DrDumyRead2Byte( void* A_PtrSr );
	void	DrDumyEndianTrsBuf( void* A_PtrTg, void* A_PtrSr, __u32 A_Size );
	void	DrDumyEndianTrsDwBuf( void* A_PtrTg, void* A_PtrSr, __u32 A_Size );

	// basd 64
	void	DrDumyBase64EncInit(Ptr_DrDumyBase64Enc A_PtrEnv);
	__u32	DrDumyBase64EncAdd(Ptr_DrDumyBase64Enc A_PtrEnv, void* A_PtrSr, __u32 A_SrSize, void* A_PtrOut);
	__u32	DrDumyBase64EncFinish(Ptr_DrDumyBase64Enc A_PtrEnv, void* A_PtrOut);

	void	DrDumyBase64DecInit(Ptr_DrDumyBase64Dec A_PtrEnv);
	__u32	DrDumyBase64DecDo(Ptr_DrDumyBase64Dec A_PtrEnv, void* A_PtrSr, __u32 A_SrSize, void* A_PtrOut);
	void	DrDumyBase64DecFinish(Ptr_DrDumyBase64Dec A_PtrEnv);

	void	DrDumyBitCtrlInit(Ptr_DrDumyBitCtrl A_PtrCtl,void* A_PtrBuf);
	__u32	DrDumyBitCtrlGetInSize(Ptr_DrDumyBitCtrl A_PtrCtl);
	void	DrDumyBitCtrlAddBit(Ptr_DrDumyBitCtrl A_PtrCtl, __u32 A_Val, __u32 A_BitCnt);
	void	DrDumyBitCtrlAddExpGolombUs(Ptr_DrDumyBitCtrl A_PtrCtl, __u32 A_Val);
	void	DrDumyBitCtrlAddExpGolombSg(Ptr_DrDumyBitCtrl A_PtrCtl, __s32 A_Val);
	__u32	DrDumyBitCtrlGetBit(Ptr_DrDumyBitCtrl A_PtrCtl, __u32 A_BitCnt);
	__u32	DrDumyBitCtrlGetExpGolombUs(Ptr_DrDumyBitCtrl A_PtrCtl);
	__s32	DrDumyBitCtrlGetExpGolombSg(Ptr_DrDumyBitCtrl A_PtrCtl);

	__u32	DrDumyCalcAlignVal(__u32 A_Val, __u32 A_Align);

//====================================================================
//out Environment conditional
#endif

