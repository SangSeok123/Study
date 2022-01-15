/*
	dumy Tools	-	window version
		
*/

//====================================================================
// uses

#include	<DrDumyTool.h>

//====================================================================
//local type

//====================================================================
//local function

__u8	DrDumyBase64CharToCode(__u8 A_Char);

//====================================================================
//var

extern	char	V_DrStrToolBase64Tbl[65];

//====================================================================
__u32		DrDumyDwGetFirstBitIdx( __u32 A_Mask )
{
	// local -------------------
		__u32				Tv_Result;
		__u32				Tv_WkIdx;
		__u32				Tv_Mask;
	// code --------------------
		//init
		Tv_Result		=	E_DrDumyNotFoundIdx;
		Tv_Mask			=	A_Mask;
		//find
		for (Tv_WkIdx = 0;Tv_WkIdx < 32;Tv_WkIdx ++)
		{
			//check exist
			if ( 0 != (Tv_Mask & 1) ) 
			{
				//found
				Tv_Result		=	Tv_WkIdx;
				break;
			}
			//next
			Tv_Mask		=	Tv_Mask >> 1;
		}
	
		return	( Tv_Result );

}

//--------------------------------------------------------------------
__u32		DrDumyDwClearBit( __u32 A_Mask,__u32 A_BitIdx )
{
	// local -------------------
		__u32				Tv_Result;
		__u32				Tv_Mask;
	// code --------------------
		//init
		Tv_Result		=	A_Mask;
		//check current range
		if ( A_BitIdx < 32 ) 
		{
			//clear bit
			Tv_Mask			=	1 << A_BitIdx;
			Tv_Result		=	Tv_Result & (~Tv_Mask);
		}

		return	( Tv_Result );
}

//--------------------------------------------------------------------
void			DrDumyInsSpace( void* A_PtrBuf, __u32 A_BufSize, __u32 A_InsPos, __u32 A_InsSize )
{
	// local -------------------
		__u8*		Tv_PtrTail;
		__u32		Tv_SizeTail;
		__u8*		Tv_PtrTo;
  // code --------------------
		//init

		//calc tail address
		Tv_PtrTail		=	((__u8*)A_PtrBuf) + A_InsPos;
		Tv_SizeTail		=	A_BufSize - A_InsPos;
		Tv_PtrTo			=	Tv_PtrTail + A_InsSize;
		if ( A_InsPos < A_BufSize )
		{
			DrDumyMoveMem( (void*)Tv_PtrTo	, (void*)Tv_PtrTail, Tv_SizeTail );
		}

}

//--------------------------------------------------------------------
void		DrDumyDelSpace( void* A_PtrBuf, __u32 A_BufSize, __u32 A_DelPos, __u32 A_DelSize )
{
	// local -------------------
	__u8*		Tv_PtrTail;
		__u32		Tv_SizeTail;
		__u8*		Tv_PtrTo;
  // code --------------------
		//init

		//calc tail address
		Tv_PtrTail		=	((__u8*)A_PtrBuf) + A_DelPos + A_DelSize;
		Tv_SizeTail		=	A_BufSize - A_DelPos - A_DelSize;
		Tv_PtrTo			=	((__u8*)A_PtrBuf) + A_DelPos;
		if ( (A_DelPos + A_DelSize) < A_BufSize )
		{
			DrDumyMoveMem( (void*)Tv_PtrTo	, (void*)Tv_PtrTail, Tv_SizeTail );
		}

}

//--------------------------------------------------------------------
void		DrDumyCopyWithSpace( void* A_PtrTg, void* A_PtrSr, __u32 A_SizeSr, __u32 A_DivPos, __u32 A_SizeSpace )
{
	// local -------------------
		__u8*		Tv_PtrTail;
		__u32		Tv_SizeTail;
		__u8*		Tv_PtrTo;
  // code --------------------
		//init

		//copy head
		if ( 0 != A_DivPos )
		{
			DrDumyMoveMem( A_PtrTg, A_PtrSr, A_DivPos );
		}

		//copy tail
		Tv_PtrTail		=	((__u8*)A_PtrSr) + A_DivPos;
		Tv_SizeTail		=	A_SizeSr - A_DivPos;
		Tv_PtrTo			=	((__u8*)A_PtrTg) + A_DivPos + A_SizeSpace;
		if ( A_DivPos < A_SizeSr )
		{
			DrDumyMoveMem( (void*)Tv_PtrTo	, (void*)Tv_PtrTail, Tv_SizeTail );
		}

}

//--------------------------------------------------------------------
void		DrDumyCollectNullDword( void* A_PtrTbl, __s32* A_PtrRtCnt )
{
	// local -------------------
		__s32			Tv_WkIdx;
		__u32*		Tv_PtrChk;		//check pointer
		__u32*		Tv_PtrWt;			//write pointer
		__s32			Tv_WkCnt;
	// code --------------------
		//check workable
		if ( *A_PtrRtCnt != 0 )
		{
			Tv_WkCnt		=	*A_PtrRtCnt;
			Tv_PtrChk		=	(__u32*)A_PtrTbl;
			Tv_PtrWt		=	Tv_PtrChk;
			for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_WkCnt;Tv_WkIdx ++ )
			{
				if ( 0 == *Tv_PtrChk )
				{
					//patch
					(*A_PtrRtCnt)	--;
				}
				else 
				{
					if ( Tv_PtrWt != Tv_PtrChk )
					{
						//align
						*Tv_PtrWt	=	*Tv_PtrChk;
					}
					Tv_PtrWt ++;
				}
				//next
				Tv_PtrChk ++;
			}
		}

}
//--------------------------------------------------------------------
#if 0
void		DrDumyZeroMem( void* A_PtrTg, __u32 A_Size )
{
	#ifdef LINUX_APP
		memset( A_PtrTg, 0, A_Size );
	#else
		ZeroMemory( A_PtrTg, A_Size );
	#endif
}
#endif
//--------------------------------------------------------------------
/*!
 * \brief
 * sampling bits
 * 
 * \param A_PtrSr
 * pointer source
 * 
 * \param A_BitPos
 * bit position of source
 * 
 * \param A_GetBitSize
 * sampling bit size
 * 
 * \param A_GetBitPos
 * bit positon of result
 * 
 * \param A_PtrRtMask
 * pointer of result bit mask
 * 
 * \returns
 * sample data
 * 
 * \remarks
 * "DrDumyGetBits" can have parameter as sum of "A_GetBitSize" and "A_GetBitPos"
 * 
 */
__u32		DrDumyGetBits( void* A_PtrSr, __u32 A_BitPosSr, __u32 A_GetBitSize, __u32 A_GetBitPos, __u32* A_PtrRtMask )
{
	// local -------------------
		__u32		Tv_Result;
		__u32*	Tv_PtrSr;					//source pointer
		__u32		Tv_BpSr;					//bit position source
		__u32		Tv_BsCap;					//captured bit size
		__u32		Tv_BpCap;					//capture bit pos
		__u32		Tv_Mask;					//mask
		__u32		Tv_GapByte;
		__u32		Tv_Smp;						//sample
		__u32		Tv_SftSmp;				//sample shifter
		__u32		Tv_BsSmp;					//bit size sample

	// code --------------------
		//init
		Tv_Result			=	0;
		*A_PtrRtMask	=	0;
		Tv_BsCap			=	0;
		Tv_BpCap			=	A_GetBitPos;

		//check capture able
		if ( 32 >= (A_GetBitPos + A_GetBitSize) )
		{

			Tv_PtrSr			=	(__u32*)A_PtrSr;
			Tv_BpSr				=	A_BitPosSr;
			//patch source position
			Tv_GapByte	=	(Def_DrCalPtr)Tv_PtrSr & 3;
			if ( 0 != Tv_GapByte )
			{
				Tv_PtrSr	=	(__u32*)( (Def_DrCalPtr)Tv_PtrSr & ~3 );
				Tv_BpSr		=	Tv_BpSr + (Tv_GapByte	* 8);
			}

			Tv_GapByte	=	Tv_BpSr >> 5;
			if ( 0 != Tv_GapByte )
			{
				Tv_PtrSr	=	Tv_PtrSr + Tv_GapByte;
			}

			//start sampling
			while ( Tv_BsCap < A_GetBitSize )
			{
				//sampling
				Tv_Smp	=	*Tv_PtrSr;
				Tv_BsSmp		=	32 - Tv_BpSr;
				if ( Tv_BpCap > Tv_BpSr )
				{
					Tv_SftSmp		=	Tv_BpCap - Tv_BpSr;
					Tv_BsSmp		=	Tv_BsSmp - Tv_SftSmp;
					Tv_Smp		=	Tv_Smp << Tv_SftSmp;
				}
				else if ( Tv_BpCap < Tv_BpSr )
				{
					Tv_SftSmp		=	Tv_BpSr - Tv_BpCap;
					Tv_Smp			=	Tv_Smp >> Tv_SftSmp;
				}
				//make mask
				Tv_Mask		=	(1 << Tv_BsSmp) - 1;
				Tv_Mask		=	Tv_Mask << Tv_BpCap;

				//update result
				Tv_Result		=	Tv_Result | (Tv_Smp & Tv_Mask);
				*A_PtrRtMask	=	*A_PtrRtMask | Tv_Mask;

				//next
				Tv_BsCap		=	Tv_BsCap + Tv_BsSmp;
				Tv_BpCap		=	Tv_BpCap + Tv_BsSmp;
				Tv_BpSr			=	Tv_BpSr + Tv_BsSmp;

				//patch source pointer
				Tv_GapByte	=	Tv_BpSr >> 5;
				if ( 0 != Tv_GapByte  )
				{
					Tv_PtrSr	=	Tv_PtrSr + Tv_GapByte;
				}
			}
		}

		return		Tv_Result;

}
//--------------------------------------------------------------------
/*!
 * \brief
 * make bit mask for none zero area
 * 
 * \param A_Value
 * source value
 * 
 * \param A_BitUnit
 * check bit unit
 * 
 * \returns
 * none zero area bit mask
 * 
 * \remarks
 * Bit unit must be 1 , 2 , 4 , 8, 16 , 32
 * 
 */
__u32		DrDumyGetNoneZeroBitMask( __u32 A_Value, __u32 A_BitUnit )
{
	// local -------------------
		__u32		Tv_Result;
		__s32			Tv_LastBit;
		__u32		Tv_Mask;
		__u32		Tv_Smp;
	// code --------------------
		//init
		Tv_Result		=	0;
		Tv_LastBit	=	32;

		if (0 != A_BitUnit)
		{
			Tv_Smp		=	A_Value;
			Tv_Mask		=	(1 << A_BitUnit) - 1;
			do 
			{
				Tv_Result		=	Tv_Result << A_BitUnit;
				if ( 0 != ( Tv_Mask & Tv_Smp) )
				{
					Tv_Result		=	Tv_Result | Tv_Mask;
				}
				//next
				Tv_Smp			=	Tv_Smp >> A_BitUnit;
				Tv_LastBit	=	Tv_LastBit - A_BitUnit;
			} while( 0 < Tv_LastBit );
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
/*
inline	__u32		DrDumyDwGetZeroBitCntRight( __u32 A_Val )
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
*/

/*
orignal
// local -------------------
__u32		Tv_Result;
__u32		Tv_WkIdx;
__u32		Tv_Val;
// code --------------------
//init
Tv_Result		=	0;
Tv_Val			=	A_Val;

for ( Tv_WkIdx=0;Tv_WkIdx < 32;Tv_WkIdx ++ )
{
if ( 0 != (Tv_Val & 1) )
{
//found
break;
}
//next
Tv_Val		=	Tv_Val >> 1;
Tv_Result ++;
}

return	Tv_Result;
*/
//--------------------------------------------------------------------
__u32		DrDumyCalcChksumDw( void* A_PtrBuf, __u32 A_Size, __u32 A_PrevChksum )
{
	// local -------------------
		__u32		Tv_Result;
		__u32		Tv_LastSize;
		__u32*	Tv_PtrWk;
	// code --------------------
		//init
		Tv_Result		=	A_PrevChksum;
		Tv_LastSize	=	A_Size;
		Tv_PtrWk		=	(__u32*)A_PtrBuf;
		
		while ( 4 <= Tv_LastSize )
		{
			Tv_Result	=	Tv_Result + *Tv_PtrWk;
			//next
			Tv_PtrWk ++;
			Tv_LastSize		=	Tv_LastSize - 4;
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32		DrDumyCalcChksumBt( void* A_PtrBuf, __u32 A_Size, __u32 A_PrevChksum )
{
	// local -------------------
		__u32		Tv_Result;
		__u8*		Tv_PtrWk;
		__u32		Tv_Remain;
	// code --------------------
		Tv_Result	=	A_PrevChksum;
		Tv_PtrWk	=	(__u8*)A_PtrBuf;
		Tv_Remain	=	A_Size;
		while ( 0 != Tv_Remain )
		{
			Tv_Result	=	Tv_Result + (__u32)(*Tv_PtrWk);
			// next
			Tv_PtrWk ++;
			Tv_Remain --;
		}
		return	Tv_Result;
}
//--------------------------------------------------------------------
/*
inline	__u32		DrDumyDwGetLastBitIdx( __u32 A_Mask )
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
*/

/*
// local -------------------
__u32				Tv_Result;
__s32					Tv_WkIdx;
__u32				Tv_Mask;
// code --------------------
//init
Tv_Result		=	E_DrDumyNotFoundIdx;
Tv_Mask			=	1 << 31;
//find
for (Tv_WkIdx = 31;Tv_WkIdx >= 0;Tv_WkIdx --)
{
//check exist
if ( 0 != (Tv_Mask & A_Mask) ) 
{
//found
Tv_Result		=	(__u32)Tv_WkIdx;
break;
}
//next
Tv_Mask		=	Tv_Mask >> 1;
}

return	Tv_Result;
*/

//--------------------------------------------------------------------
/*
inline	void*		DrDumyFillD( void* A_PtrBuf, __u32 A_DwCnt, __u32 A_Val )
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
				stosd
				mov		eax , edi
			}
		#endif
}
*/
//--------------------------------------------------------------------
/*
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
				stosw
				mov		eax , edi
		}
	#endif
}
*/
//--------------------------------------------------------------------
/*
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
			stosb
			mov		eax , edi
		}
	#endif
}
*/
//--------------------------------------------------------------------
void	DrDumySwapByte1Bit( void* A_PtrBuf, __u32 A_ByteCnt )
{
	// local -------------------
		__u8*	Tv_PtrWk;
		__u32	Tv_WkIdx;
		__u8	Tv_Tmp;
		__u32	Tv_WkBit;
	// code --------------------
		Tv_PtrWk	=	(__u8*)A_PtrBuf;
		for ( Tv_WkIdx=0;Tv_WkIdx < A_ByteCnt;Tv_WkIdx++ )
		{
			Tv_Tmp			=	*Tv_PtrWk;
			*Tv_PtrWk		=	0;
			for (Tv_WkBit=0;Tv_WkBit < 8;Tv_WkBit++)
			{
				*Tv_PtrWk	=	*Tv_PtrWk | ((Tv_Tmp & 1) << (7 - Tv_WkBit));
				//next
				Tv_Tmp	=	Tv_Tmp >> 1;
			}
			//next
			Tv_PtrWk ++;
		}
}
//--------------------------------------------------------------------
void	DrDumySwapByte4Bit( void* A_PtrBuf, __u32 A_ByteCnt )
{
	// local -------------------
		__u8*	Tv_PtrWk;
		__u32	Tv_WkIdx;
		__u8	Tv_Tmp;
	// code --------------------
		Tv_PtrWk	=	(__u8*)A_PtrBuf;
		for ( Tv_WkIdx=0;Tv_WkIdx < A_ByteCnt;Tv_WkIdx++ )
		{
			Tv_Tmp			=	*Tv_PtrWk;
			*Tv_PtrWk		=	((Tv_Tmp & 0x0F) << 4) | ((Tv_Tmp & 0xF0) >> 4);
			//next
			Tv_PtrWk ++;
		}
}
//--------------------------------------------------------------------
#ifdef DR_ACC_DUMYTOOL_NONE
/*
	none accelate code start
*/ 

__u32	DrDumyDwGetZeroBitCntRight( __u32 A_Val )
{
	// local -------------------
		__u32		Tv_Result;
		__u32		Tv_WkIdx;
		__u32		Tv_Val;
	// code --------------------
		//init
		Tv_Result		=	0;
		Tv_Val			=	A_Val;

		for ( Tv_WkIdx=0;Tv_WkIdx < 32;Tv_WkIdx ++ )
		{
			if ( 0 != (Tv_Val & 1) )
			{
				//found
				break;
			}
			//next
			Tv_Val		=	Tv_Val >> 1;
			Tv_Result ++;
		}

		return	Tv_Result;
}
__u32	DrDumyDwGetLastBitIdx( __u32 A_Mask )
{
	__u32				Tv_Result;

#ifdef LINUX_APP
	if (0 == A_Mask)
	{
		Tv_Result		=	E_DrDumyNotFoundIdx;
	}
	else
	{
		Tv_Result	=	31 - __builtin_clzl(A_Mask);
		//Tv_Result	=	(unsigned long)__fls((unsigned long)A_Mask);
	}

#else
	DWORD	Tv_Idx;

	if (_BitScanReverse(&Tv_Idx,(DWORD)A_Mask))
	{
		Tv_Result	=	(__u32)Tv_Idx;
	}
	else
	{
		Tv_Result		=	E_DrDumyNotFoundIdx;
	}
#endif
	return	Tv_Result;
}
void*	DrDumyFillD( void* A_PtrBuf, __u32 A_DwCnt, __u32 A_Val )
{
	// local -------------------
		__u32		Tv_WkCnt;
		__u32*	Tv_PtrTg;
	// code --------------------
		Tv_PtrTg	=	(__u32*)A_PtrBuf;
		Tv_WkCnt	=	A_DwCnt;
		while ( 0 != Tv_WkCnt )
		{
			*Tv_PtrTg		=	A_Val;
			//next
			Tv_PtrTg ++;
			Tv_WkCnt --;
		}
		return	Tv_PtrTg;
}
void*	DrDumyFillW( void* A_PtrBuf, __u32 A_WdCnt, __u16 A_Val )
{
	// local -------------------
		__u32		Tv_WkCnt;
		__u16*		Tv_PtrTg;
	// code --------------------
		Tv_PtrTg	=	(__u16*)A_PtrBuf;
		Tv_WkCnt	=	A_WdCnt;
		while ( 0 != Tv_WkCnt )
		{
			*Tv_PtrTg		=	A_Val;
			//next
			Tv_PtrTg ++;
			Tv_WkCnt --;
		}
		return	Tv_PtrTg;
}

/*
none accelate code finish
*/ 

#endif
//--------------------------------------------------------------------
BOOL8		DrDumyCmpDwUnit( void* A_PtrSr, void* A_PtrTg, __u32 A_DwCnt )
{
	// local -------------------
		BOOL8	Tv_Result;
		__u32	Tv_WkCnt;
		__u32*	Tv_PtrSr;
		__u32*	Tv_PtrTg;
	// code --------------------
		Tv_Result	=	TRUE;

		Tv_PtrSr	=	(__u32*)A_PtrSr;
		Tv_PtrTg	=	(__u32*)A_PtrTg;
		for ( Tv_WkCnt=A_DwCnt;0 != Tv_WkCnt;Tv_WkCnt-- )
		{
			if ( (*Tv_PtrSr) != (*Tv_PtrTg) )
			{
				//differente
				Tv_Result	=	FALSE;
				break;
			}
			//next
			Tv_PtrSr ++;
			Tv_PtrTg ++;
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
void	DrDumyEndian4byteSet( void* A_PtrTg, __u32 A_Val )
{
	// local -------------------
		__u8*	volatile	Tv_PtrSr;
		__u8*	volatile	Tv_PtrTg;
	// code --------------------
		Tv_PtrSr	=	(__u8*)&A_Val;
		Tv_PtrTg	=	(__u8*)A_PtrTg;

		Tv_PtrTg[3]	=	Tv_PtrSr[0];
		Tv_PtrTg[2]	=	Tv_PtrSr[1];
		Tv_PtrTg[1]	=	Tv_PtrSr[2];
		Tv_PtrTg[0]	=	Tv_PtrSr[3];
}
//--------------------------------------------------------------------
void	DrDumyEndian3byteSet( void* A_PtrTg, __u32 A_Val )
{
	// local -------------------
		__u8*	volatile	Tv_PtrSr;
		__u8*	volatile	Tv_PtrTg;
	// code --------------------
		Tv_PtrSr	=	(__u8*)&A_Val;
		Tv_PtrTg	=	(__u8*)A_PtrTg;

		Tv_PtrTg[2]	=	Tv_PtrSr[0];
		Tv_PtrTg[1]	=	Tv_PtrSr[1];
		Tv_PtrTg[0]	=	Tv_PtrSr[2];
}
//--------------------------------------------------------------------
void	DrDumyEndian2byteSet( void* A_PtrTg, __u16 A_Val )
{
	// local -------------------
		__u8*	volatile	Tv_PtrSr;
		__u8*	volatile	Tv_PtrTg;
	// code --------------------
		Tv_PtrSr	=	(__u8*)&A_Val;
		Tv_PtrTg	=	(__u8*)A_PtrTg;

		Tv_PtrTg[1]	=	Tv_PtrSr[0];
		Tv_PtrTg[0]	=	Tv_PtrSr[1];
}
//--------------------------------------------------------------------
__u64	DrDumyEndian8byteGet(void* A_PtrSr)
{
	// local -------------------
		__u64	Tv_Result;
		__u8*	volatile	Tv_PtrSr;
		__u8*	volatile	Tv_PtrTg;
	// code --------------------
		Tv_PtrSr	=	(__u8*)A_PtrSr;
		Tv_PtrTg	=	(__u8*)&Tv_Result;

		Tv_PtrTg[7]	=	Tv_PtrSr[0];
		Tv_PtrTg[6]	=	Tv_PtrSr[1];
		Tv_PtrTg[5]	=	Tv_PtrSr[2];
		Tv_PtrTg[4]	=	Tv_PtrSr[3];
		Tv_PtrTg[3]	=	Tv_PtrSr[4];
		Tv_PtrTg[2]	=	Tv_PtrSr[5];
		Tv_PtrTg[1]	=	Tv_PtrSr[6];
		Tv_PtrTg[0]	=	Tv_PtrSr[7];

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrDumyEndian4byteGet( void* A_PtrSr )
{
	// local -------------------
		__u32	Tv_Result;
		__u8*	volatile	Tv_PtrSr;
		__u8*	volatile	Tv_PtrTg;
	// code --------------------
		Tv_PtrSr	=	(__u8*)A_PtrSr;
		Tv_PtrTg	=	(__u8*)&Tv_Result;

		Tv_PtrTg[3]	=	Tv_PtrSr[0];
		Tv_PtrTg[2]	=	Tv_PtrSr[1];
		Tv_PtrTg[1]	=	Tv_PtrSr[2];
		Tv_PtrTg[0]	=	Tv_PtrSr[3];

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrDumyEndian3byteGet( void* A_PtrSr )
{
	// local -------------------
		__u32	Tv_Result;
		__u8*	volatile	Tv_PtrSr;
		__u8*	volatile	Tv_PtrTg;
	// code --------------------
		Tv_PtrSr	=	(__u8*)A_PtrSr;
		Tv_PtrTg	=	(__u8*)&Tv_Result;

		Tv_PtrTg[3]	=	0;
		Tv_PtrTg[2]	=	Tv_PtrSr[0];
		Tv_PtrTg[1]	=	Tv_PtrSr[1];
		Tv_PtrTg[0]	=	Tv_PtrSr[2];

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u16	DrDumyEndian2byteGet( void* A_PtrSr )
{
	// local -------------------
		__u16	Tv_Result;
		__u8*	volatile	Tv_PtrSr;
		__u8*	volatile	Tv_PtrTg;
	// code --------------------
		Tv_PtrSr	=	(__u8*)A_PtrSr;
		Tv_PtrTg	=	(__u8*)&Tv_Result;

		Tv_PtrTg[1]	=	Tv_PtrSr[0];
		Tv_PtrTg[0]	=	Tv_PtrSr[1];

		return	Tv_Result;
}
//--------------------------------------------------------------------
void	DrDumyWrite4Byte( void* A_PtrTg, __u32 A_Val )
{
	// local -------------------
		__u8*	volatile	Tv_PtrSr;
		__u8*	volatile	Tv_PtrTg;
	// code --------------------
		Tv_PtrSr	=	(__u8*)&A_Val;
		Tv_PtrTg	=	(__u8*)A_PtrTg;

		Tv_PtrTg[0]	=	Tv_PtrSr[0];
		Tv_PtrTg[1]	=	Tv_PtrSr[1];
		Tv_PtrTg[2]	=	Tv_PtrSr[2];
		Tv_PtrTg[3]	=	Tv_PtrSr[3];
}
//--------------------------------------------------------------------
void	DrDumyWrite3Byte( void* A_PtrTg, __u32 A_Val )
{
	// local -------------------
		__u8*	volatile	Tv_PtrSr;
		__u8*	volatile	Tv_PtrTg;
	// code --------------------
		Tv_PtrSr	=	(__u8*)&A_Val;
		Tv_PtrTg	=	(__u8*)A_PtrTg;

		Tv_PtrTg[0]	=	Tv_PtrSr[0];
		Tv_PtrTg[1]	=	Tv_PtrSr[1];
		Tv_PtrTg[2]	=	Tv_PtrSr[2];
}
//--------------------------------------------------------------------
void	DrDumyWrite2Byte( void* A_PtrTg, __u16 A_Val )
{
	// local -------------------
		__u8*	volatile	Tv_PtrSr;
		__u8*	volatile	Tv_PtrTg;
	// code --------------------
		Tv_PtrSr	=	(__u8*)&A_Val;
		Tv_PtrTg	=	(__u8*)A_PtrTg;

		Tv_PtrTg[0]	=	Tv_PtrSr[0];
		Tv_PtrTg[1]	=	Tv_PtrSr[1];
}
//--------------------------------------------------------------------
__u32	DrDumyRead4Byte( void* A_PtrSr )
{
	// local -------------------
		__u32	Tv_Result;
		__u8*	volatile	Tv_PtrSr;
		__u8*	volatile	Tv_PtrTg;
	// code --------------------
		Tv_PtrSr	=	(__u8*)A_PtrSr;
		Tv_PtrTg	=	(__u8*)&Tv_Result;

		Tv_PtrTg[3]	=	Tv_PtrSr[3];
		Tv_PtrTg[2]	=	Tv_PtrSr[2];
		Tv_PtrTg[1]	=	Tv_PtrSr[1];
		Tv_PtrTg[0]	=	Tv_PtrSr[0];

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrDumyRead3Byte( void* A_PtrSr )
{
	// local -------------------
		__u32	Tv_Result;
		__u8*	volatile	Tv_PtrSr;
		__u8*	volatile	Tv_PtrTg;
	// code --------------------
		Tv_PtrSr	=	(__u8*)A_PtrSr;
		Tv_PtrTg	=	(__u8*)&Tv_Result;

		Tv_PtrTg[3]	=	0;
		Tv_PtrTg[2]	=	Tv_PtrSr[2];
		Tv_PtrTg[1]	=	Tv_PtrSr[1];
		Tv_PtrTg[0]	=	Tv_PtrSr[0];

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u16	DrDumyRead2Byte( void* A_PtrSr )
{
	// local -------------------
		__u16	Tv_Result;
		__u8*	volatile	Tv_PtrSr;
		__u8*	volatile	Tv_PtrTg;
	// code --------------------
		Tv_PtrSr	=	(__u8*)A_PtrSr;
		Tv_PtrTg	=	(__u8*)&Tv_Result;

		Tv_PtrTg[1]	=	Tv_PtrSr[1];
		Tv_PtrTg[0]	=	Tv_PtrSr[0];

		return	Tv_Result;
}
//--------------------------------------------------------------------
void	DrDumyEndianTrsBuf( void* A_PtrTg, void* A_PtrSr, __u32 A_Size )
{
	// local -------------------
		__u8*	Tv_PtrSr;
		__u8*	Tv_PtrTg;
		__u32	Tv_Remain;
	// code --------------------
		Tv_Remain	=	A_Size;
		Tv_PtrSr	=	(__u8*)A_PtrSr;
		Tv_PtrTg	=	(__u8*)A_PtrTg;
		while ( 2 <= Tv_Remain ) 
		{
			// swap
			Tv_PtrTg[0]	=	Tv_PtrSr[1];
			Tv_PtrTg[1]	=	Tv_PtrSr[0];
			// next
			Tv_PtrSr	=	Tv_PtrSr + 2;
			Tv_PtrTg	=	Tv_PtrTg + 2;
			Tv_Remain	=	Tv_Remain - 2;
		}
		
}
//--------------------------------------------------------------------
void	DrDumyEndianTrsDwBuf( void* A_PtrTg, void* A_PtrSr, __u32 A_Size )
{
	// local -------------------
		__u8*	Tv_PtrSr;
		__u8*	Tv_PtrTg;
		__u32	Tv_Remain;
		__u8	Tv_Tmp0;
		__u8	Tv_Tmp1;
	// code --------------------
		Tv_Remain	=	A_Size;
		Tv_PtrSr	=	(__u8*)A_PtrSr;
		Tv_PtrTg	=	(__u8*)A_PtrTg;
		while ( 4 <= Tv_Remain ) 
		{
			// swap
			Tv_Tmp0			=	Tv_PtrSr[0];
			Tv_Tmp1			=	Tv_PtrSr[1];
			Tv_PtrTg[1]	=	Tv_PtrSr[2];
			Tv_PtrTg[0]	=	Tv_PtrSr[3];
			Tv_PtrTg[2]	=	Tv_Tmp1;
			Tv_PtrTg[3]	=	Tv_Tmp0;

			// next
			Tv_PtrSr	=	Tv_PtrSr + 4;
			Tv_PtrTg	=	Tv_PtrTg + 4;
			Tv_Remain	=	Tv_Remain - 4;
		}
}
//--------------------------------------------------------------------
BOOL8	DrDumyAbandoneByDw(void* A_PtrBuf, __u32 A_BufSize, __u32 A_Basis, __u32* A_PtrRtSize)
{
	// local -------------------
		BOOL8	Tv_Result;
		__u8*	Tv_PtrBasis;
		__u8*	Tv_PtrBuf;
		__u32	Tv_BufIdx;
		__u32	Tv_RtSize;
	// code --------------------
		
		// check size
		if(A_BufSize < 4)
		{
			*A_PtrRtSize	=	A_BufSize;
			return	FALSE;
		}

		// init
		Tv_Result		=	FALSE;
		Tv_PtrBasis	=	(__u8*)&A_Basis;
		Tv_PtrBuf		=	(__u8*)A_PtrBuf;

		Tv_BufIdx		=	0;
		// find data
		while((Tv_BufIdx + 3) < A_BufSize)
		{
			if(Tv_PtrBasis[0] == Tv_PtrBuf[Tv_BufIdx])
			{
				// check remaining data
				if((Tv_PtrBasis[1] == Tv_PtrBuf[Tv_BufIdx + 1]) && (Tv_PtrBasis[2] == Tv_PtrBuf[Tv_BufIdx + 2]) && (Tv_PtrBasis[3] == Tv_PtrBuf[Tv_BufIdx + 3]))
				{
					// found
					Tv_Result	=	TRUE;
				}
			}

			// next
			Tv_BufIdx ++;
		}

		// abondone data
		Tv_RtSize	=	A_BufSize - Tv_BufIdx;
		if(0 != Tv_BufIdx)
		{
			DrDumyMoveMem(A_PtrBuf, &Tv_PtrBuf[Tv_BufIdx], Tv_RtSize);
		}
		*A_PtrRtSize	=	Tv_RtSize;

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u8	DrDumyBitRvsU8(__u8 A_Src)
{
	// local -------------------
		__u8	Tv_Result;
		__u8	Tv_Src;
		__u8	Tv_WkIdx;
	// code --------------------
		Tv_Result	=	0;
		Tv_Src		=	A_Src;
		for( Tv_WkIdx = 0; Tv_WkIdx < 8; Tv_WkIdx++)
		{
			Tv_Result	=	Tv_Result << 1;
			Tv_Result	=	Tv_Result | (Tv_Src & 1);
			Tv_Src		=	Tv_Src >> 1;
		}
		
		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrDumyDwBitReverse(__u32 A_Src, __u32 A_BitPos, __u32 A_BitCnt, __u32 A_Dst, __u32 A_BitDst)
{
	// local -------------------
		__u32	Tv_Result;
		__u32	Tv_WkIdx;
		__u32	Tv_SrMsk;
		__u32	Tv_TgMsk;
	// code --------------------
		Tv_Result	=	A_Dst;

		if(0 != A_BitCnt)
		{
			Tv_SrMsk		=	1 << A_BitPos;
			Tv_TgMsk		=	1 << (A_BitDst + A_BitCnt - 1);
			for( Tv_WkIdx = 0; Tv_WkIdx < A_BitCnt; Tv_WkIdx++)
			{
				if(0 != (A_Src & Tv_SrMsk))
				{
					Tv_Result	=	Tv_Result | Tv_TgMsk;
				}
				else
				{
					Tv_Result	=	Tv_Result & (~Tv_TgMsk);
				}
				// next
				Tv_SrMsk	=	Tv_SrMsk << 1;
				Tv_TgMsk	=	Tv_TgMsk >> 1;
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
void	DrDumyBase64EncInit(Ptr_DrDumyBase64Enc A_PtrEnv)
{
		A_PtrEnv->RmSize	=	0;
}
//--------------------------------------------------------------------
__u32	DrDumyBase64EncAdd(Ptr_DrDumyBase64Enc A_PtrEnv, void* A_PtrSr, __u32 A_SrSize, void* A_PtrOut)
{
	// local -------------------
		__u32	Tv_Result;
		__u32	Tv_Remain;
		__u8*	Tv_PtrSr;
		char*	Tv_PtrEc;
		__u8	Tv_EncIdx;
	// code --------------------
		Tv_Result	=	0;
		Tv_Remain	=	A_SrSize;

		Tv_PtrSr	=	(__u8*)A_PtrSr;
		Tv_PtrEc	=	(char*)A_PtrOut;

		// encode first
		if(0 != A_PtrEnv->RmSize)
		{
			while(0 != Tv_Remain)
			{
				// get one soruce
				A_PtrEnv->RmDat[A_PtrEnv->RmSize]	=	*Tv_PtrSr;
				// next
				Tv_PtrSr ++;
				Tv_Remain --;
				A_PtrEnv->RmSize ++;
				if(3 <= A_PtrEnv->RmSize)
				{
					break;
				}
			}

			// check filled
			if(3 <= A_PtrEnv->RmSize)
			{
				Tv_EncIdx		=	(A_PtrEnv->RmDat[0] >> 2) & 0x3F;
				Tv_PtrEc[0]	=	V_DrStrToolBase64Tbl[Tv_EncIdx];

				Tv_EncIdx		=	((A_PtrEnv->RmDat[0] & 0x03) << 4) | ((A_PtrEnv->RmDat[1] & 0xF0) >> 4);
				Tv_PtrEc[1]	=	V_DrStrToolBase64Tbl[Tv_EncIdx];

				Tv_EncIdx		=	((A_PtrEnv->RmDat[1] & 0x0F) << 2) | ((A_PtrEnv->RmDat[2] & 0xC0) >> 6);
				Tv_PtrEc[2]	=	V_DrStrToolBase64Tbl[Tv_EncIdx];

				Tv_EncIdx		=	(A_PtrEnv->RmDat[2] & 0x3F);
				Tv_PtrEc[3]	=	V_DrStrToolBase64Tbl[Tv_EncIdx];

				Tv_PtrEc		=	Tv_PtrEc + 4;
				Tv_Result		=	Tv_Result + 4;

				A_PtrEnv->RmSize	=	0;
			}
		}

		// encode main stream
		while(3 <= Tv_Remain)
		{
			// do encode
			Tv_EncIdx		=	(Tv_PtrSr[0] >> 2) & 0x3F;
			Tv_PtrEc[0]	=	V_DrStrToolBase64Tbl[Tv_EncIdx];

			Tv_EncIdx		=	((Tv_PtrSr[0] & 0x03) << 4) | ((Tv_PtrSr[1] & 0xF0) >> 4);
			Tv_PtrEc[1]	=	V_DrStrToolBase64Tbl[Tv_EncIdx];

			Tv_EncIdx		=	((Tv_PtrSr[1] & 0x0F) << 2) | ((Tv_PtrSr[2] & 0xC0) >> 6);
			Tv_PtrEc[2]	=	V_DrStrToolBase64Tbl[Tv_EncIdx];

			Tv_EncIdx		=	(Tv_PtrSr[2] & 0x3F);
			Tv_PtrEc[3]	=	V_DrStrToolBase64Tbl[Tv_EncIdx];

			Tv_PtrSr		=	Tv_PtrSr + 3;
			Tv_Remain		=	Tv_Remain - 3;
			Tv_PtrEc		=	Tv_PtrEc + 4;
			Tv_Result		=	Tv_Result + 4;

		}

		// remain soruce process
		while(0 != Tv_Remain)
		{
			A_PtrEnv->RmDat[A_PtrEnv->RmSize]	=	*Tv_PtrSr;
			Tv_PtrSr ++;
			Tv_Remain --;
			A_PtrEnv->RmSize ++;
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrDumyBase64EncFinish(Ptr_DrDumyBase64Enc A_PtrEnv, void* A_PtrOut)
{
	// local -------------------
		char*	Tv_PtrEc;
		__u8	Tv_EncIdx;
	// code --------------------
		// init
		Tv_PtrEc	=	(char*)A_PtrOut;
		// check exist remain
		if(0 == A_PtrEnv->RmSize)
		{
			Tv_PtrEc[0]	=	0;
			return	0;
		}

		Tv_EncIdx		=	(A_PtrEnv->RmDat[0] >> 2) & 0x3F;
		Tv_PtrEc[0]	=	V_DrStrToolBase64Tbl[Tv_EncIdx];

		if(1 == A_PtrEnv->RmSize)
		{

			Tv_EncIdx		=	((A_PtrEnv->RmDat[0] & 0x03) << 4);
			Tv_PtrEc[1]	=	V_DrStrToolBase64Tbl[Tv_EncIdx];

			Tv_PtrEc[2]	=	'=';
			Tv_PtrEc[3]	=	'=';

		}
		else
		{

			Tv_EncIdx		=	((A_PtrEnv->RmDat[0] & 0x03) << 4) | ((A_PtrEnv->RmDat[1] & 0xF0) >> 4);
			Tv_PtrEc[1]	=	V_DrStrToolBase64Tbl[Tv_EncIdx];

			Tv_EncIdx		=	((A_PtrEnv->RmDat[1] & 0x0F) << 2);
			Tv_PtrEc[2]	=	V_DrStrToolBase64Tbl[Tv_EncIdx];

			Tv_PtrEc[3]	=	'=';
		}

		Tv_PtrEc[4]	=	0;		// mark end of text
		A_PtrEnv->RmSize	=	0;
		return	4;

}
//--------------------------------------------------------------------
void	DrDumyBase64DecInit(Ptr_DrDumyBase64Dec A_PtrEnv)
{
		A_PtrEnv->RmSize	=	0;
}
//--------------------------------------------------------------------
__u8	DrDumyBase64CharToCode(char A_Char)
{
	// local -------------------
	// code --------------------
		if((A_Char >= 'A') && (A_Char <= 'Z'))
		{
			return	(__u8)A_Char - (__u8)'A';
		}
		else if((A_Char >= 'a') && (A_Char <= 'z'))
		{
			return	26 + (__u8)A_Char - (__u8)'a';
		}
		else if((A_Char >= '0') && (A_Char <= '9'))
		{
			return	52 + (__u8)A_Char - (__u8)'0';
		}
		else if(A_Char == '+')
		{
			return	62;
		}
		else if(A_Char == '/')
		{
			return	63;
		}

		return	0xFF;
}
//--------------------------------------------------------------------
__u32	DrDumyBase64DecDo(Ptr_DrDumyBase64Dec A_PtrEnv, void* A_PtrSr, __u32 A_SrSize, void* A_PtrOut)
{
	// local -------------------
		__u32	Tv_Result;
		__u32	Tv_Remain;
		char*	Tv_PtrSr;
		__u8*	Tv_PtrDc;
	// code --------------------
		Tv_Result	=	0;
		Tv_Remain	=	A_SrSize;

		Tv_PtrSr	=	(char*)A_PtrSr;
		Tv_PtrDc	=	(__u8*)A_PtrOut;

		// check remain exist
		if(0 != A_PtrEnv->RmSize)
		{
			while(0 != Tv_Remain)
			{
				// get one soruce
				A_PtrEnv->RmDat[A_PtrEnv->RmSize]	=	DrDumyBase64CharToCode(*Tv_PtrSr);
				// next
				Tv_PtrSr ++;
				Tv_Remain --;
				A_PtrEnv->RmSize ++;
				if(4 <= A_PtrEnv->RmSize)
				{
					break;
				}
			}

			// check filled
			if(4 <= A_PtrEnv->RmSize)
			{
				// check source size
				if(0xFF == A_PtrEnv->RmDat[2])
				{
					Tv_PtrDc[0]	=	A_PtrEnv->RmDat[0] | (A_PtrEnv->RmDat[1] << 6);

					Tv_PtrDc ++;
					Tv_Result	++;
				}
				else if(0xFF == A_PtrEnv->RmDat[3])
				{
					Tv_PtrDc[0]	=	A_PtrEnv->RmDat[0] | ((A_PtrEnv->RmDat[1] & 3) << 6);
					Tv_PtrDc[1]	=	(A_PtrEnv->RmDat[1] >> 2) | ((A_PtrEnv->RmDat[2] & 0x0F) << 4);

					Tv_PtrDc	=	Tv_PtrDc + 2;
					Tv_Result	=	Tv_Result + 2;
				}
				else
				{
					Tv_PtrDc[0]	=	A_PtrEnv->RmDat[0] | ((A_PtrEnv->RmDat[1] & 3) << 6);
					Tv_PtrDc[1]	=	(A_PtrEnv->RmDat[1] >> 2) | ((A_PtrEnv->RmDat[2] & 0x0F) << 4);
					Tv_PtrDc[2]	=	(A_PtrEnv->RmDat[2] >> 4) | (A_PtrEnv->RmDat[3] << 2);

					Tv_PtrDc	=	Tv_PtrDc + 3;
					Tv_Result	=	Tv_Result + 3;
				}

				A_PtrEnv->RmSize	=	0;
			}
		}

		// do main stream
		while(4 <= Tv_Remain)
		{
			// do decode
			A_PtrEnv->RmDat[0]	=	DrDumyBase64CharToCode(Tv_PtrSr[0]);
			A_PtrEnv->RmDat[1]	=	DrDumyBase64CharToCode(Tv_PtrSr[1]);
			A_PtrEnv->RmDat[2]	=	DrDumyBase64CharToCode(Tv_PtrSr[2]);
			A_PtrEnv->RmDat[3]	=	DrDumyBase64CharToCode(Tv_PtrSr[3]);

			// check source size
			if(0xFF == A_PtrEnv->RmDat[2])
			{
				Tv_PtrDc[0]	=	A_PtrEnv->RmDat[0] | (A_PtrEnv->RmDat[1] << 6);

				Tv_PtrDc ++;
				Tv_Result	++;
			}
			else if(0xFF == A_PtrEnv->RmDat[3])
			{
				Tv_PtrDc[0]	=	A_PtrEnv->RmDat[0] | ((A_PtrEnv->RmDat[1] & 3) << 6);
				Tv_PtrDc[1]	=	(A_PtrEnv->RmDat[1] >> 2) | ((A_PtrEnv->RmDat[2] & 0x0F) << 4);

				Tv_PtrDc	=	Tv_PtrDc + 2;
				Tv_Result	=	Tv_Result + 2;
			}
			else
			{
				Tv_PtrDc[0]	=	A_PtrEnv->RmDat[0] | ((A_PtrEnv->RmDat[1] & 3) << 6);
				Tv_PtrDc[1]	=	(A_PtrEnv->RmDat[1] >> 2) | ((A_PtrEnv->RmDat[2] & 0x0F) << 4);
				Tv_PtrDc[2]	=	(A_PtrEnv->RmDat[2] >> 4) | (A_PtrEnv->RmDat[3] << 2);

				Tv_PtrDc	=	Tv_PtrDc + 3;
				Tv_Result	=	Tv_Result + 3;
			}

			// next
			Tv_PtrSr		=	Tv_PtrSr + 4;
			Tv_Remain		=	Tv_Remain - 4;
		}

		// remain soruce process
		while(0 != Tv_Remain)
		{
			A_PtrEnv->RmDat[A_PtrEnv->RmSize]	=	DrDumyBase64CharToCode(*Tv_PtrSr);
			Tv_PtrSr ++;
			Tv_Remain --;
			A_PtrEnv->RmSize ++;
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
void	DrDumyBase64DecFinish(Ptr_DrDumyBase64Dec A_PtrEnv)
{
		A_PtrEnv->RmSize	=	0;
}
//--------------------------------------------------------------------
void	DrDumyBitCtrlInit(Ptr_DrDumyBitCtrl A_PtrCtl, void* A_PtrBuf)
{
	A_PtrCtl->PtrBuf		=	(__u8*)A_PtrBuf;
	A_PtrCtl->Byte			=	0;
	A_PtrCtl->Bit				=	8;

	A_PtrCtl->PtrBuf[0]	=	0;
}
//--------------------------------------------------------------------
__u32	DrDumyBitCtrlGetInSize(Ptr_DrDumyBitCtrl A_PtrCtl)
{
	__u32		Tv_Result;

	Tv_Result	=	A_PtrCtl->Byte;
	if (8 > A_PtrCtl->Bit)
	{
		Tv_Result ++;
	}

	return	Tv_Result;
}
//--------------------------------------------------------------------
void	DrDumyBitCtrlAddBit(Ptr_DrDumyBitCtrl A_PtrCtl, __u32 A_Val, __u32 A_BitCnt)
{
	__u32	Tv_Remain;
	__u32	Tv_AddCnt;
	__u32	Tv_WtVal;
	__u32	Tv_SrMask;
	Tv_Remain	=	(__s32)A_BitCnt;

	while (0 < Tv_Remain)
	{
		Tv_AddCnt	=	Tv_Remain;
		if (A_PtrCtl->Bit < Tv_AddCnt)
		{
			Tv_AddCnt	=	A_PtrCtl->Bit;
		}
		
		Tv_WtVal	=	A_Val >> (Tv_Remain - Tv_AddCnt);
		Tv_SrMask	=	(1 << Tv_AddCnt) - 1;
		Tv_WtVal	=	(Tv_WtVal & Tv_SrMask) << (A_PtrCtl->Bit - Tv_AddCnt);

		A_PtrCtl->PtrBuf[A_PtrCtl->Byte]	=	A_PtrCtl->PtrBuf[A_PtrCtl->Byte] | (__u8)Tv_WtVal;

		A_PtrCtl->Bit	=	A_PtrCtl->Bit - Tv_AddCnt;
		if (0 == A_PtrCtl->Bit)
		{
			A_PtrCtl->Byte ++;
			A_PtrCtl->Bit	=	8;
			A_PtrCtl->PtrBuf[A_PtrCtl->Byte]	=	0;
		}

		Tv_Remain	=	Tv_Remain - Tv_AddCnt;

	}
}
//--------------------------------------------------------------------
void	DrDumyBitCtrlAddExpGolombUs(Ptr_DrDumyBitCtrl A_PtrCtl, __u32 A_Val)
{
	__u32	Tv_Val;
	__u32	Tv_BitCnt;
	if (0 == A_Val)
	{
		Tv_Val		=	1;
		Tv_BitCnt	=	1;
	}
	else
	{
		Tv_Val	=	A_Val + 1;
		Tv_BitCnt	=	DrDumyDwGetLastBitIdx(Tv_Val);
		Tv_BitCnt	=	(Tv_BitCnt << 1) + 1;
	}

	DrDumyBitCtrlAddBit(A_PtrCtl, Tv_Val, Tv_BitCnt);
	
}
//--------------------------------------------------------------------
void	DrDumyBitCtrlAddExpGolombSg(Ptr_DrDumyBitCtrl A_PtrCtl, __s32 A_Val)
{
	__s32	Tv_Code;

	Tv_Code	=	0;
	if (0 >= A_Val)
	{
		Tv_Code	=	- A_Val;
		Tv_Code	=	Tv_Code << 1;
	}
	else 
	{
		Tv_Code	=	(A_Val << 1) - 1;
	}

	DrDumyBitCtrlAddExpGolombUs(A_PtrCtl, (__u32)Tv_Code);

}
//--------------------------------------------------------------------
__u32	DrDumyBitCtrlGetBit(Ptr_DrDumyBitCtrl A_PtrCtl, __u32 A_BitCnt)
{
	__u32	Tv_Result;
	__u32	Tv_Remain;
	__u32	Tv_CpyCnt;
	__u32	Tv_Val;
	__u32	Tv_SrMask;
	__u32	Tv_BitPrgs;

	Tv_Result		=	0;
	Tv_Remain		=	A_BitCnt;

	while (0 < Tv_Remain)
	{
		Tv_CpyCnt	=	A_PtrCtl->Bit;
		if (Tv_CpyCnt > Tv_Remain)
		{
			Tv_CpyCnt	=	Tv_Remain;
		}

		Tv_Val			=	(__u32)A_PtrCtl->PtrBuf[A_PtrCtl->Byte];
		Tv_Val			=	Tv_Val >> (A_PtrCtl->Bit - Tv_CpyCnt);
		Tv_SrMask		=	(1 << Tv_CpyCnt) - 1;
		Tv_Val			=	Tv_Val & Tv_SrMask;
		Tv_BitPrgs	=	A_BitCnt - Tv_Remain;
		Tv_Val			=	Tv_Val << Tv_BitPrgs;

		Tv_Result		=	Tv_Result | Tv_Val;

		// next
		Tv_Remain		=	Tv_Remain - Tv_CpyCnt;
	}

	return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrDumyBitCtrlGetExpGolombUs(Ptr_DrDumyBitCtrl A_PtrCtl)
{
	__u32	Tv_Result;
	__u32	Tv_Prefix;
	__u32	Tv_BitIdx;

	Tv_Result	=	0;
	Tv_Prefix	=	0;

	// get prefix
	while (0 == DrDumyBitCtrlGetBit(A_PtrCtl,1))
	{
		Tv_Prefix ++;
	}

	if (0 != Tv_Prefix)
	{
		Tv_Result	=	1;

		for ( Tv_BitIdx = 0; Tv_BitIdx < Tv_Prefix; Tv_BitIdx++)
		{
			Tv_Result	=	Tv_Result << 1;
			Tv_Result	=	Tv_Result | DrDumyBitCtrlGetBit(A_PtrCtl, 1);
		}

		Tv_Result	--;
	}

	return	Tv_Result;
}
//--------------------------------------------------------------------
__s32	DrDumyBitCtrlGetExpGolombSg(Ptr_DrDumyBitCtrl A_PtrCtl)
{
	__s32	Tv_Result;
	__u32	Tv_Code;

	Tv_Code	=	DrDumyBitCtrlGetExpGolombUs(A_PtrCtl);
	if (0 != (1 &Tv_Code))
	{
		Tv_Result	=	(__s32)((Tv_Code + 1) >> 1);
	}
	else
	{
		Tv_Result	=	-((__s32)(Tv_Code >> 1));
	}

	return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrDumyCalcAlignVal(__u32 A_Val, __u32 A_Align)
{
	__u32	Tv_Result;
	__u32	Tv_Remain;

	Tv_Result	=	A_Val;
	if (1 < A_Align)
	{
		Tv_Remain	=	Tv_Result % A_Align;
		if (0 != Tv_Remain)
		{
			Tv_Result	=	Tv_Result - Tv_Remain + A_Align;
		}
	}
	
	return	Tv_Result;
}
//--------------------------------------------------------------------
