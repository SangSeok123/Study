/*
 String Tools

 */

//debug mode
//#pragma optimize("", off)


//====================================================================
// uses

#include <DrPlatform.h>
#include	<string.h>
#include	<DrTypeBase.h>
#include	<DrStrTool.h>
#include	<DrTimeTool.h>

#ifndef LINUX_APP
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

#ifdef LINUX_APP
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

//====================================================================
//local const

#define	E_DrStrToolPassSpcCnt			20

//====================================================================
//local type

//====================================================================
//local function
void		LcDrStrStripQuotHeadAsc( char* A_StrSrc, char* A_StrRt,__u32 A_LenLim );
void		LcDrStrStripQuotTailAsc( char* A_StrDst, __u32 A_LenLim );
void		LcDrStrStripQuotHeadAscThisAsc( char* A_StrDst, __u32 A_LenLim );

BOOL8		LcDrStrBase64FindIdx(char A_Code, __u32* A_PtrRtIdx);

//====================================================================
//var

char	V_DrStrToolBase64Tbl[65]	=	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
__u16	V_DrStrToolUniKorFirTbl[E_DrStrKorFirCntU16le]	=	
{ 0x3131, 0x3132, 0x3134, 0x3137, 0x3138, 0x3139, 0x3141, 0x3142, 0x3143, 0x3145, 
	0x3146,	0x3147, 0x3148, 0x3149, 0x314A, 0x314B, 0x314C, 0x314D, 0x314E };
__u8	V_DrStrToolDateDispType		=	E_DrStrDateDispYyMmDd;

__u16	V_DrStrPassSpcU16leTbl[E_DrStrToolPassSpcCnt]	=	
{ 0x7E, 0x60, 0x21, 0x40, 0x23,     0x24, 0x5E, 0x28, 0x29, 0x2D,    0x5F, 0x5B, 0x5D, 0x7B, 0x7D,    0x3B, 0x3A, 0x2C, 0x2E,0x20 };

__u32	V_DrStrTempDivisorTbl[5]	=
{100000,10000,1000,100,10};


//====================================================================
Def_DrCalPtr		DrStrLenU16( WCHAR* A_StrSrc, Def_DrCalPtr A_LenLim )
{
	Def_DrCalPtr				Tv_Result;
	__u16*			Tv_PtrWk;
	Def_DrCalPtr				Tv_Lim;

	//init
	Tv_Result		=	0;

	if (NULL != A_StrSrc)
	{
		Tv_PtrWk		=	(__u16*)(A_StrSrc);

		//get limite
		Tv_Lim			=	(A_LenLim - 1) & E_DrStrLimitMask;

		while (Tv_Result <= Tv_Lim)
		{
			//check
			if (0 == *Tv_PtrWk)
			{
				//found
				break;
			}

			//next
			Tv_Result ++;
			Tv_PtrWk ++;
		}
	}
	
	return	( Tv_Result );
}
//--------------------------------------------------------------------
WCHAR*	DrStrCopyU16( WCHAR* A_StrDst,const WCHAR* A_StrSrc, Def_DrCalPtr A_LenLim )
{
	// local -------------------
		const WCHAR*		Tv_PtrWkSr;
		WCHAR*		Tv_PtrWkTg;
		Def_DrCalPtr			Tv_Lim;
		Def_DrCalPtr			Tv_WkIdx;
	// code --------------------
		//get string length
		Tv_Lim				=	(A_LenLim - 1) & E_DrStrLimitMask;

		//init
		Tv_PtrWkSr			=	A_StrSrc;
		Tv_PtrWkTg			=	A_StrDst;

		// check null string
		if ( NULL != A_StrSrc )
		{
			for	( Tv_WkIdx = 0;Tv_WkIdx <= Tv_Lim;Tv_WkIdx++ )
			{
				//copy word
				*Tv_PtrWkTg		=	*Tv_PtrWkSr;
				//check finish
				if ( 0 == *Tv_PtrWkTg ) 
				{
					break;
				}
				//next
				Tv_PtrWkSr	++;
				Tv_PtrWkTg	++;
			}
		}

		//mark finish
		*Tv_PtrWkTg		=	0;

		return	Tv_PtrWkTg;
}

//--------------------------------------------------------------------
WCHAR*	DrStrCatU16( WCHAR* A_StrDst, WCHAR* A_StrSrc, __u32 A_LenLim )
{
	// local -------------------
		__u32		Tv_Len;
		WCHAR*	Tv_PtrTg;
		WCHAR*	Tv_PtrSr;
		__u32		Tv_Lim;
	// code --------------------
		//get len
		Tv_Len		=	(__u32)DrStrLenU16( A_StrDst,A_LenLim );
		Tv_Lim    = (A_LenLim - 1) & 0x7FFFFFFF;
		Tv_PtrSr	=	A_StrSrc;
		Tv_PtrTg	=	A_StrDst + Tv_Len;
		// check exist source
		if ( NULL != A_StrSrc )
		{
			//check copy able
			while ( Tv_Len < Tv_Lim )
			{
				//update
				*Tv_PtrTg		=	*Tv_PtrSr;
				if ( 0 == *Tv_PtrSr )
				{
					break;
				}
				//next
				Tv_PtrTg ++;
				Tv_PtrSr ++;
				Tv_Len ++;
			}
		}
		//check limite finish
		*Tv_PtrTg	=	0;

		return	Tv_PtrTg;
}
//--------------------------------------------------------------------
char*		DrStrAddChar(char* A_StrDst, char A_Char, __u32 A_LenLim)
{
	__u32	Tv_Len;
	char*	Tv_PtrTg;
	__u32	Tv_Lim;

	//get len
	Tv_Len = (__u32)DrStrLenAsc(A_StrDst, A_LenLim);
	Tv_Lim = (A_LenLim - 1) & 0x7FFFFFFF;
	Tv_PtrTg = A_StrDst + Tv_Len;
	//check copy able
	if ((Tv_Len < Tv_Lim) && (0 != A_Char))
	{
		*Tv_PtrTg = A_Char;
		Tv_PtrTg++;
		*Tv_PtrTg = 0;
	}

	return	Tv_PtrTg;
	
}
//--------------------------------------------------------------------
WCHAR*	DrStrAddWchar( WCHAR* A_StrDst, WCHAR A_Wchar, __u32 A_LenLim )
{
	// local -------------------
		__u32		Tv_Len;
		WCHAR*	Tv_PtrTg;
		__u32		Tv_Lim;
	// code --------------------
		//get len
		Tv_Len		=	(__u32)DrStrLenU16( A_StrDst,A_LenLim );
		Tv_Lim    = (A_LenLim - 1) & 0x7FFFFFFF;
		Tv_PtrTg	=	A_StrDst + Tv_Len;
		//check copy able
		if ( (Tv_Len < Tv_Lim) && (0 != A_Wchar) )
		{
			*Tv_PtrTg	=	A_Wchar;
			Tv_PtrTg ++;
			*Tv_PtrTg	=	0;
		}

		return	Tv_PtrTg;
}
//--------------------------------------------------------------------
BOOL8		DrStrIntToStrU16le( WCHAR* A_StrDst, __s32 A_Value, __u32 A_LenLim,
				BOOL8 A_IsFix, BOOL8 A_IsSign )
{
	// local -------------------
		BOOL8				Tv_Result;

		__u32				Tv_LenDst;
		__u32				Tv_Lim;
		WCHAR*			Tv_WkPtr;
		__u32				Tv_WkDiv;
		__u32				Tv_Val;
		BOOL8				Tv_IsStc;
		__u32				Tv_WtVal;
		__u32				Tv_NumPos;
							
	// code --------------------

		//init
		Tv_Result			=	FALSE;
		Tv_Lim				=	A_LenLim;
		if ( 0 == Tv_Lim ) 
		{
			Tv_Lim			=	0x7FFFFFFF;
		}

		//check limite
		if ( 0 != Tv_Lim )
		{
			//init var
			Tv_IsStc		=	FALSE;
			Tv_LenDst   = 0;
			Tv_WkPtr    = A_StrDst;

			//set direction
			if ( (0 > A_Value) && A_IsSign )
			{
				*Tv_WkPtr		=	(WCHAR)'-';				//mark minus
				Tv_Val			=	(~((__u32)A_Value)) + 1;
				Tv_LenDst ++;
				Tv_WkPtr ++;
			}
			else
			{
				Tv_Val			=	(__u32)(A_Value);
			}

			//analysis number
			Tv_WkDiv		=  1000000000;
			Tv_NumPos		=  9;
			while ( Tv_LenDst < Tv_Lim )
			{
				//check start position
				if ( A_IsFix && ( (Tv_Lim - Tv_LenDst) > Tv_NumPos ) )
				{
					Tv_IsStc		=  TRUE;
				}
						 
				if ( Tv_WkDiv <= Tv_Val ) 
				{
					//get val
					Tv_WtVal		=	Tv_Val / Tv_WkDiv;
					Tv_Val			= Tv_Val % Tv_WkDiv;
					//set char
					if ( (!Tv_IsStc) || (0 != Tv_WtVal) || (1 == Tv_WkDiv) ) 
					{
						*Tv_WkPtr		=	(WCHAR)Tv_WtVal + 0x30;
						Tv_IsStc		= TRUE;
						Tv_LenDst ++;
						Tv_WkPtr ++;
					}
				}
				else
				{
					if ( ( Tv_IsStc ) || ( 1== Tv_WkDiv ) ) 
					{
						*Tv_WkPtr		=	(WCHAR)'0';
						Tv_LenDst ++;
						Tv_WkPtr ++;
					}
				}

				//check end positon
				if ( 1 == Tv_WkDiv )
				{
					Tv_Result		=	TRUE;
					break;
				}
				//next
				Tv_WkDiv		=	Tv_WkDiv / 10;
				Tv_NumPos --;
			}
			//mark finish
			*Tv_WkPtr			=  0;
		}

		return	( Tv_Result );

}

//--------------------------------------------------------------------
BOOL8    DrStrStrToIntU16le( WCHAR* A_StrNum, __s32* A_PtrRtVal, __u32 A_LenLim )
{
	// local -------------------
		BOOL8			Tv_Result;

		__u32			Tv_LenSrc;
		WCHAR*		Tv_WkPtr;
		__s32				Tv_Num;
		__u32			Tv_WkIdx;
		__s32				Tv_WkMul;
		BOOL8			Tv_IsStx;

	// code --------------------

		//init
		Tv_Result			=	FALSE;
		*A_PtrRtVal		=	0;

		//get string
		Tv_LenSrc			=  (__u32)DrStrLenU16( A_StrNum, A_LenLim );
		if ( 0 != Tv_LenSrc )
		{
			//check last charactor
			Tv_IsStx		=	FALSE;
			Tv_WkMul		=	1;
			Tv_WkPtr		=	A_StrNum + Tv_LenSrc - 1;
			for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_LenSrc;Tv_WkIdx ++ )
			{
				//check value
				if ( ( 0x30 <= *Tv_WkPtr ) && ( 0x39 >= *Tv_WkPtr ) )
				{
					//get value
					Tv_Num			=	*Tv_WkPtr - 0x30;
					*A_PtrRtVal	=	*A_PtrRtVal + ( Tv_Num * Tv_WkMul );
					//next value
					Tv_WkMul		= Tv_WkMul * 10;
					//success
					Tv_Result		= TRUE;
				}
				else if ( !Tv_IsStx)
				{
					//check minus
					if ( 0x2D == *Tv_WkPtr ) 
					{
						//value minus
						*A_PtrRtVal		=	-(*A_PtrRtVal);
					}
					//quit
					break;
				}
				//next
				Tv_WkPtr --;
						 
			}
		}
						 
		return	( Tv_Result );

}
//--------------------------------------------------------------------
Def_DrCalPtr	DrStrLenAsc(const char* A_StrSrc, Def_DrCalPtr A_LenLim )
{
	Def_DrCalPtr		Tv_Result;
	Def_DrCalPtr		Tv_Lim;
	const char*		Tv_WkPtr;

	Tv_Result	=	0;
	Tv_WkPtr	=	A_StrSrc;
	Tv_Lim		=	( A_LenLim - 1 ) & E_DrStrLimitMask;

	while	(	Tv_Result <= Tv_Lim )
	{
		if ( 0 == *Tv_WkPtr )	{	break;	}
		//next
		Tv_WkPtr ++;
		Tv_Result ++;
	}

	return	Tv_Result;

}
//--------------------------------------------------------------------
char*		DrStrCopyAsc( char* A_StrDst,const char* A_StrSrc, Def_DrCalPtr A_LenLim )
{
	Def_DrCalPtr			Tv_Lim;
	Def_DrCalPtr			Tv_Copied;
	const char*				Tv_PtrSr;
	char*			Tv_PtrTg;

	//init
	Tv_PtrSr	=	A_StrSrc;
	Tv_PtrTg	=	A_StrDst;
	Tv_Copied	=	0;
	Tv_Lim		=	A_LenLim;
	if ( 0 == Tv_Lim )	{	Tv_Lim	= E_DrStrLimitMask;	}

	//check string not exist
	if ( NULL != A_StrSrc )
	{
		//get source size
		while	(0 != *Tv_PtrSr)
		{
			if (Tv_Copied >= Tv_Lim)
			{
				break;
			}	
			//copy
			*Tv_PtrTg		=	*Tv_PtrSr;
			//next
			Tv_PtrSr++;
			Tv_PtrTg++;
			Tv_Copied++;
		}
	}
	//mark finish target
	*Tv_PtrTg		=	0;

	//finish
	return		Tv_PtrTg;

}
//--------------------------------------------------------------------
__u32		DrStrParseGetWordLenAsc( char* A_Str, __u32 A_LenLim )
{
	// local -------------------
		__u32		Tv_Result;
		__u32		Tv_Lim;
		__u32		Tv_LenStr;
		char*		Tv_PtrChk;
		__u32		Tv_WkIdx;
  // code --------------------
		//init
		Tv_Result		=	0;
		Tv_Lim			=	A_LenLim;
		
		//get limite
		if ( 0 == Tv_Lim )
		{
			Tv_Lim			=	0x7FFFFFFF;	
		}
		Tv_LenStr		=	(__u32)DrStrLenAsc( A_Str );
		if ( Tv_Lim > Tv_LenStr )
		{
			Tv_Lim		=	Tv_LenStr;
		}

		//check start
		Tv_WkIdx		=	0;
		Tv_PtrChk		=	A_Str;
		while ( Tv_WkIdx < Tv_Lim )
		{
			//normal mode
			//check charactor range
			if ( (0 <= *Tv_PtrChk)&&(0x20 >= *Tv_PtrChk) )
			{
				//none word
				break;
			}
			//next
			Tv_Result ++;
			Tv_PtrChk ++;
			Tv_WkIdx ++;
			
		}		//Tv_WkIdx endwhile

		return		Tv_Result;
}

//--------------------------------------------------------------------
/*
parsing find word start position
	input	
		A_Mode		:	parsing mode
		A_Str			:	parsing string
		A_LenLim	:	string size limite , 0 = not limite
	output
		result		:	success = word position
								failure	=	E_DrStrOutOfString
*/
__u32		DrStrParseFindWordStartAsc( __u32 A_Mode, char* A_Str, __u32 A_LenLim )
{
	// local -------------------
		__u32		Tv_Result;
		__u32		Tv_Lim;
		__u32		Tv_LenStr;
		char*		Tv_PtrChk;
		__u32		Tv_WkIdx;
  // code --------------------
		//init
		Tv_Result		=	0;
		Tv_Lim			=	A_LenLim;
		
		//get limite
		if ( 0 == Tv_Lim )
		{
			Tv_Lim			=	0x7FFFFFFF;	
		}
		Tv_LenStr		=	(__u32)DrStrLenAsc( A_Str );
		if ( Tv_Lim > Tv_LenStr )
		{
			Tv_Lim		=	Tv_LenStr;
		}

		//find word start
		Tv_WkIdx		=	0;
		Tv_PtrChk		=	A_Str;
		while ( Tv_WkIdx < Tv_Lim )
		{
			//check charactor range
			if ( (0 > *Tv_PtrChk)||(0x20 < *Tv_PtrChk) )
			{
				break;
			}
			//next
			Tv_PtrChk ++;
			Tv_WkIdx ++;
			Tv_Result ++;
		}			//find word while

		//check out of position
		if (Tv_Result >= Tv_Lim)
		{
			Tv_Result		=	E_DrStrOutOfString;	
		}

		return		Tv_Result;

}

//--------------------------------------------------------------------
/*

result = exclude cr cn code

*/
__u32		DrStrGetOneLineSizeAsc(char* A_Str, __u32* A_PtrRtCrSize, __u32 A_LenLim)
{
	__u32		Tv_Result;
	__u32		Tv_Lim;
	__u32		Tv_LenStr;
	__u32		Tv_WkIdx;

	//init
	Tv_Result		=	0;
	Tv_Lim			=	A_LenLim;
	*A_PtrRtCrSize	=	0;
		
	//get limite
	if ( 0 == Tv_Lim )
	{
		Tv_Lim			=	0x7FFFFFFF;	
	}
	Tv_LenStr		=	(__u32)DrStrLenAsc( A_Str );
	if ( Tv_Lim > Tv_LenStr )
	{
		Tv_Lim		=	Tv_LenStr;
	}

	//find word start
	Tv_WkIdx		=	0;
	while (Tv_WkIdx < Tv_Lim)
	{
		// check enter code
		if ((Tv_WkIdx + 1) < Tv_Lim)
		{
			if ((0x0d == A_Str[Tv_WkIdx]) && (0x0a == A_Str[Tv_WkIdx + 1]))
			{
				// found line
				Tv_Result	=	Tv_WkIdx;
				*A_PtrRtCrSize	=	2;
				break;
			}
			if ((0x0a == A_Str[Tv_WkIdx]) && (0x0d == A_Str[Tv_WkIdx + 1]))
			{
				// found line
				Tv_Result	=	Tv_WkIdx;
				*A_PtrRtCrSize	=	2;
				break;
			}
		}

		if (0x0a == A_Str[Tv_WkIdx])
		{
			// found line
			Tv_Result	=	Tv_WkIdx;
			*A_PtrRtCrSize	=	1;
			break;
		}

		// next
		Tv_WkIdx ++;
	}

	if (0 == *A_PtrRtCrSize)
	{
		Tv_Result	=	Tv_WkIdx;
	}

	return		Tv_Result;

}

//--------------------------------------------------------------------
/*
string compare
	input	
		A_StrSrc	:	source string
		A_StrDst	:	destination string
		A_IsCaseSense	:	use case sesitive
		A_LenLim	:	string size limite , 0 = not limite
	output
		result		:	match		= 0
								source > destination = 1
								source < destination = -1
*/
__s32			DrStrCmpAsc(const char* A_StrSrc, const char* A_StrDst, BOOL8 A_IsCaseSense, __u32 A_LenLim )
{
	// local -------------------
		__s32			Tv_Result;

    __s32			Tv_LenSrc;
    __s32			Tv_LenDst;
    __s32			Tv_LenComon;

    __u8		Tv_CharSr;
    __u8		Tv_CharTg;
		__u8*		Tv_WkPtrSrc;
		__u8*		Tv_WkPtrDst;
    __s32			Tv_WkIdx;

  // code --------------------
		//init
		Tv_Result		=	0;

    //get info
    Tv_LenSrc		=	(__u32)DrStrLenAsc( A_StrSrc,A_LenLim );
    Tv_LenDst   = (__u32)DrStrLenAsc( A_StrDst,A_LenLim );
    //compare smaller size
    Tv_LenComon	=	Tv_LenDst;
		if ( Tv_LenSrc < Tv_LenDst )	{	Tv_LenComon		=	Tv_LenSrc;	}
    //check zero size
    if ( Tv_LenComon != 0 ) 
		{
      //compare common area
      Tv_WkPtrSrc		=	(__u8*)(A_StrSrc);
			Tv_WkPtrDst   =	(__u8*)(A_StrDst);
      for	( Tv_WkIdx = 0; Tv_WkIdx < Tv_LenComon;Tv_WkIdx ++ )
			{
        //sample data
        Tv_CharSr		=	*Tv_WkPtrSrc;
        Tv_CharTg   = *Tv_WkPtrDst;
        //patch case
        if ( !A_IsCaseSense )
				{
					if ( (Tv_CharSr >= 0x41) && (Tv_CharSr <= 0x5A) )		{	Tv_CharSr	=	Tv_CharSr + 0x20;	}
					if ( (Tv_CharTg >= 0x41) && (Tv_CharTg <= 0x5A) )		{	Tv_CharTg = Tv_CharTg + 0x20;	}
				}
        //compare diffrent
        if ( Tv_CharSr > Tv_CharTg ) 
				{
          Tv_Result			=	1;
          break;
				}
        else if ( Tv_CharSr < Tv_CharTg )
				{
          Tv_Result			= -1;
          break;
				}
        //next
        Tv_WkPtrSrc ++;
        Tv_WkPtrDst ++;
			}
      //check common area is equal
      if ( 0 == Tv_Result ) 
			{
        //check string size
				if			( Tv_LenSrc > Tv_LenDst )	{	Tv_Result		=	1;	}
				else if ( Tv_LenSrc < Tv_LenDst ) {	Tv_Result		= -1;	}
			}
		}
    else
		{
      //set zero string compare
			if			( 0 != Tv_LenSrc ) {	Tv_Result	=	1;	}
			else if ( 0 != Tv_LenDst ) {	Tv_Result = -1;	}
		}

		return		Tv_Result;

}
//--------------------------------------------------------------------
__s32			DrStrCmpU16le( WCHAR* A_StrSrc, WCHAR* A_StrDst, BOOL8 A_IsCaseSense, __u32 A_LenLim )
{
	// local -------------------
		__s32			Tv_Result;

		__s32			Tv_LenSrc;
		__s32			Tv_LenDst;
		__s32			Tv_LenComon;

		__u16		Tv_CharSr;
		__u16		Tv_CharTg;
		WCHAR*	Tv_WkPtrSrc;
		WCHAR*	Tv_WkPtrDst;
		__s32			Tv_WkIdx;
	// code --------------------
		//init
		Tv_Result		=	0;

		//get info
		Tv_LenSrc		=	(__u32)DrStrLenU16( A_StrSrc,A_LenLim );
		Tv_LenDst   = (__u32)DrStrLenU16( A_StrDst,A_LenLim );
		//compare smaller size
		Tv_LenComon	=	Tv_LenDst;
		if ( Tv_LenSrc < Tv_LenDst )	{	Tv_LenComon		=	Tv_LenSrc;	}
		//check zero size
		if ( Tv_LenComon != 0 ) 
		{
			//compare common area
			Tv_WkPtrSrc		=	A_StrSrc;
			Tv_WkPtrDst   =	A_StrDst;
			for	( Tv_WkIdx = 0; Tv_WkIdx < Tv_LenComon;Tv_WkIdx ++ )
			{
				//sample data
				Tv_CharSr		=	(__u16)*Tv_WkPtrSrc;
				Tv_CharTg   = (__u16)*Tv_WkPtrDst;
				//patch case
				if ( !A_IsCaseSense )
				{
					if ( (Tv_CharSr >= (WCHAR)'A') && (Tv_CharSr <= (WCHAR)'Z') )		{	Tv_CharSr	=	Tv_CharSr + 0x20;	}
					if ( (Tv_CharTg >= (WCHAR)'A') && (Tv_CharTg <= (WCHAR)'Z') )		{	Tv_CharTg = Tv_CharTg + 0x20;	}
				}
				//compare diffrent
				if ( Tv_CharSr > Tv_CharTg ) 
				{
					Tv_Result			=	1;
					break;
				}
				else if ( Tv_CharSr < Tv_CharTg )
				{
					Tv_Result			= -1;
					break;
				}
				//next
				Tv_WkPtrSrc ++;
				Tv_WkPtrDst ++;
			}
			//check common area is equal
			if ( 0 == Tv_Result ) 
			{
				//check string size
				if			( Tv_LenSrc > Tv_LenDst )	{	Tv_Result		=	1;	}
				else if ( Tv_LenSrc < Tv_LenDst ) {	Tv_Result		= -1;	}
			}
		}
		else
		{
			//set zero string compare
			if			( 0 != Tv_LenSrc ) {	Tv_Result	=	1;	}
			else if ( 0 != Tv_LenDst ) {	Tv_Result = -1;	}
		}

		return		Tv_Result;
}
//--------------------------------------------------------------------
/*
get drive from fullpath file name
	input	
		A_StrFn		:	source file name
		A_StrRtDrv	:	pointer of recived drive name
		A_LenLim	:	string size limite , 0 = not limite
	output
		result		:	TRUE / FALSE
*/
BOOL8		DrStrFnGetDrvAsc( char* A_StrFn, char* A_StrRtDrv, __u32 A_LenLim )
{
	// local -------------------
		BOOL8			Tv_Result;
		__u32			Tv_DeshPos;
  // code --------------------
		//init
    Tv_Result		=	FALSE;
    *A_StrRtDrv	=	0;  
    //find string
    Tv_DeshPos  =	DrStrFindChar( A_StrFn,':',0,A_LenLim );
    if ( E_DrStrOutOfString != Tv_DeshPos ) 
		{
      //get drive name
      DrStrCopyAsc( A_StrRtDrv,A_StrFn,Tv_DeshPos + 1 );
      Tv_Result		=	TRUE;
		}

		return		Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrStrFnGetDrvU16le(WCHAR* A_StrFn, WCHAR* A_StrRtDrv, __u32 A_LenLim)
{
	BOOL8			Tv_Result;
	__u32			Tv_DeshPos;

	Tv_Result		=	FALSE;
	*A_StrRtDrv	=	0;

	//find string
	Tv_DeshPos  =	DrStrFindWchar(A_StrFn, L':', A_LenLim);
	if(E_DrStrOutOfString != Tv_DeshPos)
	{
		//get drive name
		DrStrCopyU16(A_StrRtDrv, A_StrFn, Tv_DeshPos + 1);
		Tv_Result		=	TRUE;
	}

	return		Tv_Result;
}
//--------------------------------------------------------------------
/*
get drive from fullpath file name
	input	
		A_StrDst		:	destination string
		A_FindChar	:	find charactor
		A_IsMultibyte	:	find mode
		A_LenLim	:	string size limite , 0 = not limite
	output
		result		:	success = found position
								failure = E_DrStrOutOfString
*/
__u32		DrStrFindChar( char* A_StrDst, char A_FindChar, BOOL8 A_IsMultibyte, __u32 A_LenLim )
{
	// local -------------------
		__u32		Tv_Result;
		__u32		Tv_LenSrc;
    char*		Tv_WkPtr;
    __u32		Tv_WkIdx;
		BOOL8		Tv_IsMtByteMode;		//in multi byte mode

  // code --------------------
		//init
    Tv_Result		=	E_DrStrOutOfString;
		Tv_IsMtByteMode	=	FALSE;

    //finding
    Tv_LenSrc		=	(__u32)DrStrLenAsc( A_StrDst,A_LenLim );
    Tv_WkPtr    = A_StrDst;

    //finding char
    if ( 0 != Tv_LenSrc )
		{
			for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_LenSrc;Tv_WkIdx ++ )
			{
				//check multi byte mode
				if ( Tv_IsMtByteMode )
				{
					//multibyte mode
					Tv_IsMtByteMode	=	FALSE;
				}
				else
				{
					//check match charactor
					if ( *Tv_WkPtr == A_FindChar )
					{
						//found
						Tv_Result	=	Tv_WkIdx;
						break;
					}
					else
					{
						//check multi byte
						if ( (0 > *Tv_WkPtr)||(127 < *Tv_WkPtr) )
						{
							if ( A_IsMultibyte )
							{
								//multibyte
								Tv_IsMtByteMode		=	TRUE;
							}
						}
					}
				}

        //next
				Tv_WkPtr	=	Tv_WkPtr + 1;
			}
		}

		return		Tv_Result;

}
//--------------------------------------------------------------------
__u32		DrStrFindWchar( WCHAR* A_StrDst, WCHAR A_FindChar, __u32 A_LenLim )
{
	// local -------------------
		__u32	Tv_Result;
		__u32	Tv_Limite;
		__u32	Tv_Idx;
		WCHAR*	Tv_PtrStr;
	// code --------------------
		Tv_Result	=	E_DrStrOutOfString;
		Tv_Limite	=	A_LenLim;
		if ( 0 == A_LenLim )
		{
			Tv_Limite	=	0xFFFFFFFF;
		}
		Tv_PtrStr	=	A_StrDst;
		Tv_Idx		=	0;
		while ( 0 != (*Tv_PtrStr) )
		{
			// check find
			if ( A_FindChar == (*Tv_PtrStr) )
			{
				// found
				Tv_Result	=	Tv_Idx;
				break;
			}
			// next
			Tv_PtrStr ++;
			Tv_Idx ++;
			if ( Tv_Idx >= Tv_Limite )
			{
				break;
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
/*
get file extention from fullpath file name
	input
		A_StrFn			:	fullpath file name
		A_RtStrExt	:	pointer of recive extention
		A_LenLim	:	string size limite , 0 = not limite
	output
		A_RtStrExt	:	extention (*include DOT "." )
		retrun		:	TRUE / FALSE

*/
BOOL8		DrStrFnGetExtAsc( char* A_StrFn, char* A_RtStrExt, __u32 A_LenLim )
{
	// local -------------------
		BOOL8		Tv_Result;
    __u32		Tv_LenSr;
    char*		Tv_WkPtrFn;
    __u32		Tv_WkIdx;
    __u32		Tv_LenExt;
    __u32		Tv_WkCnt;

  // code --------------------
    //init
    Tv_Result			=	FALSE;
    *A_RtStrExt   =	0;

    Tv_LenSr	=	(__u32)DrStrLenAsc( A_StrFn,A_LenLim );
    if ( 0 != Tv_LenSr )
		{
      //find coma from last
      Tv_WkPtrFn	=	A_StrFn + Tv_LenSr - 1;
      Tv_WkCnt    = Tv_LenSr;
			if ( 4 < Tv_WkCnt )	{	Tv_WkCnt	=	4;	}
      for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_WkCnt;Tv_WkIdx ++ )
			{
        //check charactor
        if ( '.' == *Tv_WkPtrFn )
				{
          //find
          Tv_Result		=	TRUE;
          Tv_LenExt		=	Tv_WkIdx + 1;
          break;
				}
				else if	( ('\\' == *Tv_WkPtrFn) || ('/' == *Tv_WkPtrFn) || (':' == *Tv_WkPtrFn) ||
									(' ' == *Tv_WkPtrFn) || (';' == *Tv_WkPtrFn) )
				{
          //bad file name
          break;
				}
        //next
        Tv_WkPtrFn --;
			}
      //check success then copy to result
      if ( Tv_Result )
			{
        DrStrCopyAsc( A_RtStrExt,Tv_WkPtrFn,Tv_LenExt );
			}
		}

		return		Tv_Result;

}
//--------------------------------------------------------------------
BOOL8		DrStrFnGetExtU16le(WCHAR* A_StrFn, WCHAR* A_RtStrExt, __u32 A_LenLim)
{
	BOOL8		Tv_Result;
	__u32		Tv_LenSr;
	WCHAR* Tv_WkPtrFn;
	__u32		Tv_WkIdx;
	__u32		Tv_LenExt;
	__u32		Tv_WkCnt;

	//init
	Tv_Result			=	FALSE;
	*A_RtStrExt   =	0;

	Tv_LenSr	=	(__u32)DrStrLenU16(A_StrFn, A_LenLim);
	if(0 != Tv_LenSr)
	{
		//find coma from last
		Tv_WkPtrFn	=	A_StrFn + Tv_LenSr - 1;
		Tv_WkCnt    = Tv_LenSr;
		for(Tv_WkIdx = 0; Tv_WkIdx < Tv_WkCnt; Tv_WkIdx ++)
		{
			//check charactor
			if('.' == *Tv_WkPtrFn)
			{
				//find
				Tv_Result		=	TRUE;
				Tv_LenExt		=	Tv_WkIdx + 1;
				break;
			}
			else if(('\\' == *Tv_WkPtrFn) || ('/' == *Tv_WkPtrFn) || (':' == *Tv_WkPtrFn) ||
				(' ' == *Tv_WkPtrFn) || (';' == *Tv_WkPtrFn))
			{
				//bad file name
				break;
			}
			//next
			Tv_WkPtrFn --;
		}
		//check success then copy to result
		if(Tv_Result)
		{
			DrStrCopyU16(A_RtStrExt, Tv_WkPtrFn, Tv_LenExt);
		}
	}

	return		Tv_Result;

}
//--------------------------------------------------------------------
char* DrStrFnAddBkSlshAsc( char* A_StrFn, __u32 A_LenLim )
{
	char* Tv_Result;
  __u32		Tv_Len;

	//init
	Tv_Result	=	A_StrFn;

  //check end position
  Tv_Len		=	(__u32)DrStrLenAsc( A_StrFn,A_LenLim );
#ifdef LINUX_APP
	if((0 ==  Tv_Len) || ('/' != A_StrFn[Tv_Len - 1]))
#else
	if ( (0 ==  Tv_Len) || (('\\' != A_StrFn[Tv_Len - 1]) && ('/' != A_StrFn[Tv_Len - 1])) )
#endif
	{
    //add back slash
    if ( (Tv_Len < A_LenLim) || (0 == A_LenLim) )
		{
			Tv_Result = &A_StrFn[Tv_Len];
#ifdef LINUX_APP
			* Tv_Result =	'/';
#else
			* Tv_Result =	'\\';
#endif
			Tv_Result++;
      *Tv_Result =	0;
		}
	}

	return		Tv_Result;
}
//--------------------------------------------------------------------
WCHAR* DrStrFnAddBkSlshU16le( WCHAR* A_StrFn, __u32 A_LenLim )
{
	WCHAR* Tv_Result;
	__u32		Tv_Len;

	//init
	Tv_Result	=	A_StrFn;

	//check end position
	Tv_Len		=	(__u32)DrStrLenU16( A_StrFn,A_LenLim );
	//check last character is backslash
#ifdef LINUX_APP
	if ((0 == Tv_Len) || (0x2F != A_StrFn[Tv_Len - 1]))
#else
	if ((0 == Tv_Len) || ((L'\\' != A_StrFn[Tv_Len - 1]) && (L'/' != A_StrFn[Tv_Len - 1])))
#endif
	{
		//add back slash
		if ( (Tv_Len < A_LenLim) || (0 == A_LenLim) )
		{
			Tv_Result = &A_StrFn[Tv_Len];
#ifdef LINUX_APP
			*Tv_Result =	0x2F;
#else
			*Tv_Result =	L'\\';
#endif
			Tv_Result++;
			*Tv_Result =	0;
		}
	}

	return		Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrStrFnGetPathAsc( char* A_StrFn, char* A_StrRtPath, __u32 A_LenLim )
{
	// local -------------------
		BOOL8		Tv_Result;

    __u32		Tv_LenSr;
    char*		Tv_WkPtrFn;
    __u32		Tv_WkIdx;
    __u32		Tv_LenPath;
  // code --------------------

    //init
    Tv_Result			=	FALSE;
    *A_StrRtPath	= 0;

    Tv_LenSr	=	(__u32)DrStrLenAsc( A_StrFn,A_LenLim );

    if ( 0 != Tv_LenSr )
		{

      Tv_LenPath		=	Tv_LenSr;

      //find back slash from last
      Tv_WkPtrFn		=	A_StrFn + Tv_LenSr - 1;
      for (Tv_WkIdx = 0;Tv_WkIdx < Tv_LenSr;Tv_WkIdx ++ )
			{
        //check charactor
        if ( '\\' == *Tv_WkPtrFn )
				{
          //find
          Tv_Result		=	TRUE;
          Tv_LenPath --;

          break;
				}
				else if (':' == *Tv_WkPtrFn)
				{
          Tv_Result		=	TRUE;
          break;
				}
        else if (';' == *Tv_WkPtrFn)
				{
          //bad file name
          break;
				}
        //next
        Tv_WkPtrFn --;
        Tv_LenPath --;
			}
      //check success then copy to result
      if ( Tv_Result )
			{
				//strip qutation
        DrStrCopyAsc( A_StrRtPath,A_StrFn,Tv_LenPath );
			}
		}

		return		Tv_Result;

}
//--------------------------------------------------------------------
BOOL8		DrStrFnGetPathU16le( WCHAR* A_StrFn, WCHAR* A_StrRtPath, __u32 A_LenLim )
{
	// local -------------------
		BOOL8		Tv_Result;
		__u32		Tv_LenSr;
		WCHAR*	Tv_WkPtrFn;
		__u32		Tv_WkIdx;
		__u32		Tv_LenPath;
	// code --------------------
		//init
		Tv_Result			=	FALSE;
		*A_StrRtPath	= 0;

		Tv_LenSr	=	(__u32)DrStrLenU16( A_StrFn, A_LenLim );
		if ( 0 != Tv_LenSr )
		{
			Tv_LenPath		=	Tv_LenSr;
			//find back slash from last
			Tv_WkPtrFn		=	A_StrFn + Tv_LenSr - 1;
			for (Tv_WkIdx = 0;Tv_WkIdx < Tv_LenSr;Tv_WkIdx ++ )
			{
				//check charactor
				if ( (WCHAR)'\\' == *Tv_WkPtrFn )
				{
					//find
					Tv_Result		=	TRUE;
					Tv_LenPath --;

					break;
				}
				else if ( (WCHAR)':' == *Tv_WkPtrFn)
				{
					Tv_Result		=	TRUE;
					break;
				}
				else if ( (WCHAR)';' == *Tv_WkPtrFn)
				{
					//bad file name
					break;
				}
				//next
				Tv_WkPtrFn --;
				Tv_LenPath --;
			}
			//check success then copy to result
			if ( Tv_Result )
			{
				//strip qutation
				DrStrCopyU16( A_StrRtPath,A_StrFn,Tv_LenPath );
			}
		}

		return		Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrStrFnGetNameOnlyAsc( char* A_StrFn, char* A_StrRtName, __u32 A_LenLim )
{
	// local -------------------
		BOOL8		Tv_Result;

		__u32		Tv_LenSr;

		char*		Tv_WkPtrFn;
		__u32		Tv_WkIdx;
		__u32		Tv_WkCnt;

	  __u32		Tv_LenExt;
		__u32		Tv_LenCont;
		char*		Tv_PtrCont;

  // code --------------------

		//init
		Tv_Result			=	FALSE;
		DrStrClear( A_StrRtName );

		Tv_LenSr	=	(__u32)DrStrLenAsc( A_StrFn,A_LenLim );

		if ( 0 != Tv_LenSr )
		{
			//init
			Tv_LenExt		=	0;
			Tv_LenCont	=	0;

			//get extention size
			Tv_WkPtrFn  =	A_StrFn + Tv_LenSr - 1;
			Tv_WkCnt    = Tv_LenSr;
			if ( 4 < Tv_WkCnt )	{	Tv_WkCnt	=	4;	}
			for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_WkCnt;Tv_WkIdx++ )
			{
				//check charactor
				if ( '.' == *Tv_WkPtrFn )
				{
					//find
					Tv_LenExt		=	Tv_WkIdx + 1;
					break;
				}
				else if (	('\\' == *Tv_WkPtrFn) || ('/' == *Tv_WkPtrFn) || (':' == *Tv_WkPtrFn) ||
									(' ' == *Tv_WkPtrFn) || (';' == *Tv_WkPtrFn) )
				{
					//bad file name
					break;
				}
				//next
				Tv_WkPtrFn --;
			}

			//get name size
			Tv_WkCnt		=	Tv_LenSr - Tv_LenExt;
			Tv_WkPtrFn  = A_StrFn + Tv_WkCnt - 1;
			if ( 0 < Tv_WkCnt )
			{
				for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_WkCnt;Tv_WkIdx ++ )
				{
					//check charactor
					if	(	('\\' == *Tv_WkPtrFn) || ('/' == *Tv_WkPtrFn) || (':' == *Tv_WkPtrFn) ||
								(';' == *Tv_WkPtrFn) )
					{
						//bad file name
						break;
					}
					//next
					Tv_LenCont ++;
					Tv_WkPtrFn --;
				}
			}

			//check success
			if ( 0 != Tv_LenCont ) 
			{
				//copy result
				Tv_PtrCont		=	A_StrFn + Tv_LenSr - Tv_LenCont - Tv_LenExt;
				DrStrCopyAsc( A_StrRtName,Tv_PtrCont,Tv_LenCont );
				Tv_Result		=	TRUE;
			}

		}

		return		Tv_Result;

}
//--------------------------------------------------------------------
__u32		DrStrFnGetNameOnlyU16le(WCHAR* A_StrFn, WCHAR* A_StrRtName, __u32 A_LenLim)
{
	// local -------------------
		__u32	Tv_Result;		// get name length
		__u32	Tv_LenTt;
		__u32	Tv_WkPos;
		__u32	Tv_PosEnd;
		__u32	Tv_PosSt;
		BOOL8	Tv_IsSkipExt;
	// code --------------------
		Tv_Result	=	0;

		Tv_LenTt	=	(__u32)DrStrLenU16(A_StrFn, A_LenLim);
		if (0 != Tv_LenTt)
		{
			Tv_WkPos	=	Tv_LenTt;
			Tv_PosEnd	=	Tv_LenTt - 1;
			Tv_PosSt	=	0;
			Tv_IsSkipExt	=	TRUE;
			do
			{
				Tv_WkPos --;
				// find ext
				if (Tv_IsSkipExt)
				{
					if ((WCHAR)'.' == A_StrFn[Tv_WkPos])
					{
						if (0 == Tv_WkPos)
						{
							break;
						}
						// found ext
						Tv_PosEnd	=	Tv_WkPos - 1;
						Tv_IsSkipExt	=	FALSE;
					}
				}
				// check end of file name
				if ((A_StrFn[Tv_WkPos] == (WCHAR)'\\') || (A_StrFn[Tv_WkPos] == (WCHAR)'/') || 
					(A_StrFn[Tv_WkPos] == (WCHAR)':') || (A_StrFn[Tv_WkPos] == (WCHAR)';'))
				{
					Tv_PosSt	=	Tv_WkPos + 1;
					break;
				}
			} while (0 != Tv_WkPos);

			// check exist file name
			if (Tv_PosSt <= Tv_PosEnd)
			{
				Tv_Result	=	Tv_PosEnd - Tv_PosSt + 1;
				DrStrCopyU16(A_StrRtName, &A_StrFn[Tv_PosSt], Tv_Result);
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrStrFnGetFileNameAsc( char* A_StrFn, char* A_StrRtName, __u32 A_LenLim )
{
	// local -------------------
		BOOL8		Tv_Result;

		__u32		Tv_LenSr;

		char*		Tv_WkPtrFn;
		__u32		Tv_WkIdx;
		__u32		Tv_WkCnt;

		__u32		Tv_LenCont;
		char*		Tv_PtrCont;

  // code --------------------

		//init
		Tv_Result			= FALSE;
		DrStrClear( A_StrRtName );

		Tv_LenSr		=	(__u32)DrStrLenAsc( A_StrFn,A_LenLim );

		if ( 0 != Tv_LenSr )
		{
			//init
			Tv_LenCont		=	0;

			//get name size
			Tv_WkPtrFn  =	A_StrFn + Tv_LenSr - 1;
			Tv_WkCnt    = Tv_LenSr;
			if ( 0 != Tv_WkCnt ) 
			{
				for (Tv_WkIdx = 0;Tv_WkIdx < Tv_WkCnt;Tv_WkIdx++ )
				{
					//check charactor
					if	(	('\\' == *Tv_WkPtrFn) || ('/' == *Tv_WkPtrFn) || (':' == *Tv_WkPtrFn) ||
								(';' == *Tv_WkPtrFn) )
					{
						//bad file name
						break;
					}
					//next
					Tv_LenCont ++;
					Tv_WkPtrFn --;
				}
			}

			//check success
			if ( 0 != Tv_LenCont)
			{
				//copy result
				Tv_PtrCont	=	A_StrFn + Tv_LenSr - Tv_LenCont;
				DrStrCopyAsc( A_StrRtName,Tv_PtrCont,Tv_LenCont );
				Tv_Result		= TRUE;
			}

		}


		return		Tv_Result;

}

//--------------------------------------------------------------------
void		DrStrTrimLeftAsc( char* A_StrSrc, char* A_StrRt, __u32 A_LenLim )
{
	// local -------------------
		char*		Tv_StrWk;
		__u32		Tv_LenSr;
		__u32		Tv_WkIdx;
  // code --------------------
		//init
		DrStrClear( A_StrRt );
		Tv_LenSr		=	(__u32)DrStrLenAsc( A_StrSrc );

		//find space
		Tv_StrWk		=	A_StrSrc;
		Tv_WkIdx		=	0;
		while ( Tv_WkIdx < Tv_LenSr )
		{
			//check not use
			if ( (0 > *Tv_StrWk) && (0x20 < *Tv_StrWk) )
			{
				//found
				DrStrCopyAsc( A_StrRt, Tv_StrWk, Tv_LenSr - Tv_WkIdx );

				break;
			}
			//next
			Tv_StrWk ++;
			Tv_WkIdx ++;
		}

}

//--------------------------------------------------------------------
void		DrStrTrimRightAsc( char* A_StrDst, __u32 A_LenLim )
{
	// local -------------------
		char*		Tv_StrWk;
		__u32		Tv_LenSr;
		__u32		Tv_WkIdx;
  // code --------------------
		//init
		Tv_LenSr		=	(__u32)DrStrLenAsc( A_StrDst );

		//find space
		if ( 0 != Tv_LenSr )
		{
			Tv_StrWk		=	A_StrDst + Tv_LenSr - 1;
			Tv_WkIdx		=	0;
			while ( Tv_WkIdx < Tv_LenSr )
			{
				//check not use
				if ( (0 > *Tv_StrWk) && (0x20 < *Tv_StrWk) )
				{
					//found
					Tv_StrWk ++;
					*Tv_StrWk		=	0;

					break;
				}
				//next
				Tv_StrWk --;
				Tv_WkIdx ++;
			}
		}

}

//--------------------------------------------------------------------
void		DrStrTrimBothAsc( char* A_StrSrc, char* A_StrRt, __u32 A_LenLim )
{
	DrStrTrimLeftAsc( A_StrSrc, A_StrRt, A_LenLim );
	DrStrTrimRightAsc( A_StrRt, A_LenLim );
}

//--------------------------------------------------------------------
void		LcDrStrStripQuotHeadAsc( char* A_StrSrc, char* A_StrRt, __u32 A_LenLim )
{
	// local -------------------
		char*		Tv_StrWk;
		__u32		Tv_LenSr;
		__u32		Tv_WkIdx;
  // code --------------------
		//init
		DrStrClear( A_StrRt );
		Tv_LenSr		=	(__u32)DrStrLenAsc( A_StrSrc );

		//trim space
		Tv_StrWk		=	A_StrSrc;
		Tv_WkIdx		=	0;
		while ( Tv_WkIdx < Tv_LenSr )
		{
			//check not use
			if ( (0 > *Tv_StrWk) || (0x20 < *Tv_StrWk) )
			{
				//check quatation
				if (0x22 == *Tv_StrWk)
				{
					Tv_StrWk ++;
					Tv_WkIdx ++;
				}
				//set result
				if (0 != (Tv_LenSr - Tv_WkIdx) )
				{
					DrStrCopyAsc( A_StrRt, Tv_StrWk, Tv_LenSr - Tv_WkIdx );
				}

				break;
			}
			//next
			Tv_StrWk ++;
			Tv_WkIdx ++;
		}

}

//--------------------------------------------------------------------
void		LcDrStrStripQuotTailAsc( char* A_StrDst, __u32 A_LenLim )
{
	// local -------------------
		char*		Tv_StrWk;
		__u32		Tv_LenSr;
		__u32		Tv_WkIdx;
  // code --------------------
		//init
		Tv_LenSr		=	(__u32)DrStrLenAsc( A_StrDst );

		//find space
		if ( 0 != Tv_LenSr )
		{
			Tv_StrWk		=	A_StrDst + Tv_LenSr - 1;
			Tv_WkIdx		=	0;
			while ( Tv_WkIdx < Tv_LenSr )
			{
				//check not use
				if ( (0 > *Tv_StrWk) || (0x20 < *Tv_StrWk) )
				{
					if (0x22 != *Tv_StrWk)
					{
						//found
						Tv_StrWk ++;
					}
					*Tv_StrWk		=	0;

					break;
				}
				//next
				Tv_StrWk --;
				Tv_WkIdx ++;
			}
		}

}
//--------------------------------------------------------------------
void		DrStrStripQuotationAsc( char* A_StrSrc, char* A_StrRt, __u32 A_LenLim )
{

		//strip head
		LcDrStrStripQuotHeadAsc( A_StrSrc,A_StrRt,A_LenLim );

		//strip tail
		LcDrStrStripQuotTailAsc( A_StrRt, A_LenLim );

}
//--------------------------------------------------------------------
char*	DrStrCatAsc( char* A_StrDst, char* A_StrSrc, __u32 A_LenLim )
{
	// local -------------------
    __u32		Tv_Len;
    char*		Tv_PtrTg;
		char*		Tv_PtrSr;
    __u32		Tv_Lim;
  // code --------------------
    //get len
    Tv_Len		=	(__u32)DrStrLenAsc( A_StrDst,A_LenLim );
    Tv_Lim    = (A_LenLim - 1) & 0x7FFFFFFF;
		Tv_PtrSr	=	A_StrSrc;
		Tv_PtrTg	=	A_StrDst + Tv_Len;
    //check copy able
		if ( NULL != A_StrSrc )
		{
			while ( Tv_Len < Tv_Lim )
			{
				//update
				*Tv_PtrTg		=	*Tv_PtrSr;
				if ( 0 == *Tv_PtrSr )
				{
					break;
				}
				//next
				Tv_Len ++;
				Tv_PtrTg ++;
				Tv_PtrSr ++;
			}
		}
		//check limite finish
		*Tv_PtrTg	=	0;

		return	Tv_PtrTg;
}

//--------------------------------------------------------------------
void		DrStrDressQuotationAsc( char* A_StrSrc, char* A_StrRt, __u32 A_LenLim )
{
	// local -------------------
		char*		Tv_StrWk;
		__u32		Tv_LenRt;
  // code --------------------
		//init
		Tv_StrWk		=	A_StrRt;
		DrStrClear( A_StrRt );
		//check head dressed
		if ( 0x22 != *A_StrSrc )
		{
			*Tv_StrWk		=	0x22;
			Tv_StrWk++;
			*Tv_StrWk		=	0;
		}
		//cat string
		DrStrCatAsc( A_StrRt, A_StrSrc, A_LenLim );
		Tv_LenRt	=	(__u32)DrStrLenAsc( A_StrRt );
		//check already exist quotation
		Tv_StrWk		=	A_StrRt + Tv_LenRt - 1;
		if ( 0x22 != *Tv_StrWk )
		{
			//check limite
			if ( (0 == A_LenLim) || (Tv_LenRt != A_LenLim) )
			{
				//add quotation tail
				Tv_StrWk ++;
				*Tv_StrWk		=	0x22;
				Tv_StrWk ++;
				*Tv_StrWk		=	0;
			}
		}

}

//--------------------------------------------------------------------
BOOL8		DrStrFnDivNameAndParaAsc( char* A_StrSr, char* A_StrRtFn, char* A_StrRtPara )
{
	// local -------------------
		BOOL8		Tv_Result;
		__u32		Tv_Pos;
		__u32		Tv_Len;
		char*		Tv_StrPrs;
  // code --------------------
		//init
		Tv_Result		=	FALSE;
		DrStrClear( A_StrRtFn );
		DrStrClear( A_StrRtPara );
		Tv_StrPrs		=	A_StrSr;

		//find first section
		Tv_Pos		=	DrStrParseFindWordStartAsc( 0, Tv_StrPrs );
		if ( E_DrStrOutOfString != Tv_Pos )
		{
			Tv_StrPrs		=	Tv_StrPrs + Tv_Pos;
			//get file name
			Tv_Len		=	DrStrParseGetWordLenAsc( Tv_StrPrs );
			if ( 0 != Tv_Len )
			{
				//get this program file name
				DrStrCopyAsc( A_StrRtFn, Tv_StrPrs, Tv_Len );
				DrStrStripQuotationAscThisAsc( A_StrRtFn );
				//get para
				Tv_StrPrs		=	Tv_StrPrs + Tv_Len;
				Tv_Pos		=	DrStrParseFindWordStartAsc( 0, Tv_StrPrs );
				if ( E_DrStrOutOfString != Tv_Pos )
				{
					//set parameter
					Tv_StrPrs		=	Tv_StrPrs + Tv_Pos;
					DrStrCopyAsc( A_StrRtPara, Tv_StrPrs );	
					//success
					Tv_Result		=	TRUE;
				}
			}
		}

		return		Tv_Result;
}

//--------------------------------------------------------------------
void		DrStrStripQuotationAscThisAsc( char* A_Str, __u32 A_LenLim )
{
		//strip head
		LcDrStrStripQuotHeadAscThisAsc( A_Str, A_LenLim );

		//strip tail
		LcDrStrStripQuotTailAsc( A_Str, A_LenLim );
}

//--------------------------------------------------------------------
void		LcDrStrStripQuotHeadAscThisAsc( char* A_StrDst, __u32 A_LenLim )
{
	// local -------------------
		char*		Tv_StrWk;
		__u32		Tv_LenSr;
		__u32		Tv_WkIdx;
  // code --------------------
		//init
		Tv_LenSr		=	(__u32)DrStrLenAsc( A_StrDst );

		//trim space
		Tv_StrWk		=	A_StrDst;
		Tv_WkIdx		=	0;
		while ( Tv_WkIdx < Tv_LenSr )
		{
			//check not use
			if ( (0 > *Tv_StrWk) || (0x20 < *Tv_StrWk) )
			{
				//check quatation
				if (0x22 == *Tv_StrWk)
				{
					Tv_StrWk ++;
					Tv_WkIdx ++;
				}
				//set result
				if (0 != (Tv_LenSr - Tv_WkIdx) )
				{
					memmove( (void*)A_StrDst, (void*)Tv_StrWk, Tv_LenSr - Tv_WkIdx + 1 );
				}

				break;
			}
			//next
			Tv_StrWk ++;
			Tv_WkIdx ++;
		}
}

//--------------------------------------------------------------------
void		DrStrDressQuotationAscThisAsc( char* A_StrDst, __u32 A_LenLim )
{
	// local -------------------
		char*		Tv_StrWk;
		__u32		Tv_LenStr;
  // code --------------------
		//dress tail

		DrStrAddChar(A_StrDst, 0x22, A_LenLim);
		//dress head
		Tv_LenStr		=	(__u32)DrStrLenAsc( A_StrDst, A_LenLim );
		Tv_StrWk		=	A_StrDst + 1;
		memmove( (void*)Tv_StrWk, (void*)A_StrDst, (Def_DrCalPtr)Tv_LenStr + (Def_DrCalPtr)1 );
		*A_StrDst		=	0x22;

}
//--------------------------------------------------------------------
BOOL8		DrStrIsRelativeAsc( char* A_StrFn )
{
	// local -------------------
		BOOL8		Tv_Result;
  // code --------------------
		//init
		Tv_Result		=	FALSE;

		//check full path
		if ( E_DrStrOutOfString == DrStrFindChar( A_StrFn, ':', 0 ) )
		{
			Tv_Result	=	TRUE;
		}
		
		return		Tv_Result;
}
//--------------------------------------------------------------------
char* DrStrTempToStrAsc(char* A_StrTg, __s16 A_Temp)
{
	char* Tv_Result;
	BOOL8	Tv_IsMinus;
	BOOL8	Tv_IsStart;
	__u32	Tv_TempRemain;
	__u32	Tv_DivIdx;
	__u32	Tv_Dividen;

	Tv_IsMinus	=	FALSE;
	Tv_Result	=	A_StrTg;

	Tv_TempRemain	=	(__u32)A_Temp;
	if(0 > A_Temp)
	{
		Tv_IsMinus	=	TRUE;
		Tv_TempRemain	=	(__u32)(-A_Temp);
		*Tv_Result	=	'-';
		Tv_Result ++;
	}

	Tv_IsStart	=	FALSE;
	Tv_DivIdx	=	0;
	while(5 > Tv_DivIdx)
	{
		Tv_Dividen	=	Tv_TempRemain / V_DrStrTempDivisorTbl[Tv_DivIdx];
		Tv_TempRemain	=	Tv_TempRemain % V_DrStrTempDivisorTbl[Tv_DivIdx];
		if((0 != Tv_Dividen) || Tv_IsStart)
		{
			*Tv_Result	=	0x30 + (char)Tv_Dividen;
			Tv_IsStart	=	TRUE;
			Tv_Result ++;
		}
		// next
		Tv_DivIdx ++;
	}

	// all zero
	if(!Tv_IsStart)
	{
		*Tv_Result	=	'0';
		Tv_Result ++;
	}

	// add comma
	*Tv_Result	=	'.';
	Tv_Result ++;
	*Tv_Result	=	0x30 + (char)Tv_TempRemain;
	Tv_Result ++;

	// terminator
	*Tv_Result	=	0;

	return	Tv_Result;
}
//--------------------------------------------------------------------
WCHAR* DrStrTempToStrU16le(WCHAR* A_StrTg, __s16 A_Temp)
{
	WCHAR* Tv_Result;
	BOOL8	Tv_IsMinus;
	BOOL8	Tv_IsStart;
	__u32	Tv_TempRemain;
	__u32	Tv_DivIdx;
	__u32	Tv_Dividen;

	Tv_IsMinus	=	FALSE;
	Tv_Result	=	A_StrTg;

	Tv_TempRemain	=	(__u32)A_Temp;
	if(0 > A_Temp)
	{
		Tv_IsMinus	=	TRUE;
		Tv_TempRemain	=	(__u32)(-A_Temp);
		*Tv_Result	=	L'-';
		Tv_Result ++;
	}

	Tv_IsStart	=	FALSE;
	Tv_DivIdx	=	0;
	while(5 > Tv_DivIdx)
	{
		Tv_Dividen	=	Tv_TempRemain / V_DrStrTempDivisorTbl[Tv_DivIdx];
		Tv_TempRemain	=	Tv_TempRemain % V_DrStrTempDivisorTbl[Tv_DivIdx];
		if((0 != Tv_Dividen) || Tv_IsStart)
		{
			*Tv_Result	=	0x30 + (WCHAR)Tv_Dividen;
			Tv_IsStart	=	TRUE;
			Tv_Result ++;
		}
		// next
		Tv_DivIdx ++;
	}

	// all zero
	if(!Tv_IsStart)
	{
		*Tv_Result	=	L'0';
		Tv_Result ++;
	}

	// add comma
	*Tv_Result	=	L'.';
	Tv_Result ++;
	*Tv_Result	=	0x30 + (WCHAR)Tv_TempRemain;
	Tv_Result ++;

	// terminator
	*Tv_Result	=	0;

	return	Tv_Result;

}
//--------------------------------------------------------------------
BOOL8	DrStrTempToSwordAsc(char* A_StrTemp, __s16* A_PtrRtVal, __u32 A_LenLim)
{
	BOOL8	Tv_IsMinus;
	__s32	Tv_Remain;
	char* Tv_StrWk;
	__s32	Tv_ValCnt;
	__s32	Tv_ValTbl[4];
	__s32	Tv_Under;
	__s32	Tv_OutVal;
	__u32	Tv_WkIdx;

	Tv_Remain	=	A_LenLim;
	if(0 == Tv_Remain)
	{
		Tv_Remain = 0x7FFFFFFF;
	}

	Tv_Under		=	0;
	Tv_IsMinus	=	FALSE;
	Tv_StrWk	=	A_StrTemp;

	if('-' == *Tv_StrWk)
	{
		Tv_IsMinus	=	TRUE;
		Tv_StrWk ++;
		Tv_Remain --;
	}

	// head
	Tv_ValCnt	=	0;
	while(0 < Tv_Remain)
	{
		if(('0' > *Tv_StrWk) || ('9' < *Tv_StrWk) )
		{
			break;
		}

		if(4 <=Tv_ValCnt)
		{
			return	FALSE;
		}

		// insert
		Tv_ValTbl[Tv_ValCnt]	=	(__s32)(*Tv_StrWk) - 0x30;
		Tv_ValCnt ++;
		// next
		Tv_StrWk ++;
		Tv_Remain --;
	}
	
	if(0 == Tv_ValCnt)
	{
		return	FALSE;
	}

	// check exist comma
	if(0 < Tv_Remain)
	{
		if(0x20 < *Tv_StrWk)
		{
			if('.' != *Tv_StrWk)
			{
				return	FALSE;
			}

			Tv_StrWk ++;
			Tv_Remain --;

			if(0 < Tv_Remain)
			{
				if(0x20 < *Tv_StrWk)
				{
					if(('0' > *Tv_StrWk) || ('9' < *Tv_StrWk))
					{
						return	FALSE;
					}

					Tv_Under	=	(__s32)*Tv_StrWk - 0x30;
				}
			}
		}
	}

	// calculate result
	Tv_OutVal	=	0;
	Tv_WkIdx	=	4;
	while(0 < Tv_ValCnt)
	{
		Tv_OutVal	=	Tv_OutVal + Tv_ValTbl[Tv_ValCnt - 1] * (__s32)V_DrStrTempDivisorTbl[Tv_WkIdx];

		Tv_WkIdx --;
		Tv_ValCnt --;
	}
	
	Tv_OutVal	=	Tv_OutVal + Tv_Under;

	if(0x7FFF < Tv_OutVal)
	{
		return	FALSE;
	}

	if(Tv_IsMinus)
	{
		Tv_OutVal	=	-Tv_OutVal;
	}

	*A_PtrRtVal	=	(__s32)Tv_OutVal;

	return	TRUE;
}
//--------------------------------------------------------------------
BOOL8	DrStrTempToSwordU16le(WCHAR* A_StrTemp, __s16* A_PtrRtVal, __u32 A_LenLim)
{
	BOOL8	Tv_IsMinus;
	__s32	Tv_Remain;
	WCHAR* Tv_StrWk;
	__s32	Tv_ValCnt;
	__s32	Tv_ValTbl[4];
	__s32	Tv_Under;
	__s32	Tv_OutVal;
	__u32	Tv_WkIdx;

	Tv_Remain	=	A_LenLim;
	if(0 == Tv_Remain)
	{
		Tv_Remain = 0x7FFFFFFF;
	}

	Tv_Under		=	0;
	Tv_IsMinus	=	FALSE;
	Tv_StrWk	=	A_StrTemp;

	if(L'-' == *Tv_StrWk)
	{
		Tv_IsMinus	=	TRUE;
		Tv_StrWk ++;
		Tv_Remain --;
	}

	// head
	Tv_ValCnt	=	0;
	while(0 < Tv_Remain)
	{
		if((L'0' > *Tv_StrWk) || (L'9' < *Tv_StrWk))
		{
			break;
		}

		if(4 <=Tv_ValCnt)
		{
			return	FALSE;
		}

		// insert
		Tv_ValTbl[Tv_ValCnt]	=	(__s32)(*Tv_StrWk) - 0x30;
		Tv_ValCnt ++;
		// next
		Tv_StrWk ++;
		Tv_Remain --;
	}

	if(0 == Tv_ValCnt)
	{
		return	FALSE;
	}

	// check exist comma
	if(0 < Tv_Remain)
	{
		if(0x20 < *Tv_StrWk)
		{
			if(L'.' != *Tv_StrWk)
			{
				return	FALSE;
			}

			Tv_StrWk ++;
			Tv_Remain --;

			if(0 < Tv_Remain)
			{
				if(0x20 < *Tv_StrWk)
				{
					if((L'0' > *Tv_StrWk) || (L'9' < *Tv_StrWk))
					{
						return	FALSE;
					}

					Tv_Under	=	(__s32)*Tv_StrWk - 0x30;
				}
			}
		}
	}

	// calculate result
	Tv_OutVal	=	0;
	Tv_WkIdx	=	4;
	while(0 < Tv_ValCnt)
	{
		Tv_OutVal	=	Tv_OutVal + Tv_ValTbl[Tv_ValCnt - 1] * (__s32)V_DrStrTempDivisorTbl[Tv_WkIdx];

		Tv_WkIdx --;
		Tv_ValCnt --;
	}

	Tv_OutVal	=	Tv_OutVal + Tv_Under;

	if(0x7FFF < Tv_OutVal)
	{
		return	FALSE;
	}

	if(Tv_IsMinus)
	{
		Tv_OutVal	=	-Tv_OutVal;
	}

	*A_PtrRtVal	=	(__s32)Tv_OutVal;

	return	TRUE;
}
//--------------------------------------------------------------------
/*
convert integer to string
	input	
		A_StrRt		:	pointer recive string
		A_Value		:	convert value
		A_LenLim	:	string length limite (0 = no limite)
		A_IsFix		:	fix digit
	output
		result		:	TRUE / FALSE
*/
BOOL8		DrStrIntToStrAsc( char* A_StrRt, __s32 A_Value, __u32 A_LenLim, BOOL8 A_IsFix, BOOL8 A_IsSign )
{
	// local -------------------
		BOOL8		Tv_Result;
		__u32		Tv_LenDst;
    __u32		Tv_Lim;
    char*		Tv_WkPtr;
    __u32		Tv_WkDiv;
    __u32		Tv_Val;
    BOOL8		Tv_IsStc;
    __u32		Tv_WtVal;
    __u32		Tv_NumPos;
  
  // code --------------------
		//init
    Tv_Result		= FALSE;
    Tv_Lim			= (A_LenLim - 1) & 0x7FFFFFFF;
    if ( A_IsFix && ( 0 == A_LenLim) ) 
		{
			Tv_Lim  =  0x7FFFFFFF;
		}

    //check limite
    if ( 0 != Tv_Lim )
		{
      //init var
      Tv_IsStc		=	FALSE;
      Tv_LenDst   = 0;
      Tv_WkPtr    = A_StrRt;
      //set direction
      if ( (0 > A_Value) && A_IsSign )
			{
        *Tv_WkPtr		=	'-';
        Tv_Val			=	(__u32)( -A_Value );
        Tv_LenDst ++;
        Tv_WkPtr ++;
			}
      else
			{
        Tv_Val		=	(__u32)A_Value;
			}
      //analysis number
      Tv_WkDiv		=	1000000000;
      Tv_NumPos		=	9;
      while ( Tv_LenDst <= Tv_Lim )
			{
        //check start position
        if ( A_IsFix && ( (Tv_Lim - Tv_LenDst) >= Tv_NumPos ) )
				{
          Tv_IsStc		= TRUE;
				}
        if ( Tv_WkDiv <= Tv_Val )
				{
          //get val
          Tv_WtVal	=	Tv_Val / Tv_WkDiv;
          Tv_Val    = Tv_Val % Tv_WkDiv;
          //set char
          if ( (!Tv_IsStc) || (0 != Tv_WtVal) || (1 == Tv_WkDiv) ) 
					{
            *Tv_WkPtr	=	(char)((__u8)(Tv_WtVal + 0x30));
            Tv_IsStc  =	TRUE;
            Tv_LenDst ++;
            Tv_WkPtr ++;
					}
				}
        else
				{
          if ( (Tv_IsStc) || ( 1 == Tv_WkDiv) )
					{
            *Tv_WkPtr	=	'0';
            Tv_LenDst ++;
            Tv_WkPtr ++;
					}
				}
        //check end positon
        if ( 1 == Tv_WkDiv )
				{
          Tv_Result	=	TRUE;
          break;
				}
        //next
        Tv_WkDiv	=	Tv_WkDiv / 10;
        Tv_NumPos --;
			}
      //mark finish
      *Tv_WkPtr		=	0;
		}

		return		Tv_Result;
}

//--------------------------------------------------------------------
BOOL8		DrStrDwordToHexAsc( char* A_StrRt, __u32 A_Value, __u32 A_LenLim, BOOL8 A_IsFix )
{
	// local -------------------
		BOOL8		Tv_Result;
		__u32		Tv_LenDst;
		__u32		Tv_Lim;
		char*		Tv_WkPtr;
		__u32		Tv_WkShft;
		__u32		Tv_Val;
		BOOL8		Tv_IsStc;
		__u32		Tv_WtVal;
		__u32		Tv_NumPos;

  // code --------------------
		//init
		Tv_Result		=	FALSE;
		Tv_Lim      =	(A_LenLim - 1) & 0x7FFFFFFF;

		if ( A_IsFix && (0 == A_LenLim) )
		{
			Tv_Lim		=		0;
		}
		//check limite
		if ( 0 != Tv_Lim )
		{
			//init var
			Tv_IsStc		=	FALSE;
			Tv_LenDst   = 0;
			Tv_WkPtr    = A_StrRt;
			Tv_Val      = A_Value;
			//analysis number
			Tv_WkShft   = 28;
			Tv_NumPos   = 8;
			while ( Tv_LenDst <= Tv_Lim )
			{
				//check start position
				if ( A_IsFix && ( (Tv_Lim - Tv_LenDst) >= (Tv_NumPos - 1) ) )
				{
					Tv_IsStc		=	TRUE;
				}

				if ( 1 == Tv_NumPos )
				{
					Tv_IsStc		=	TRUE;
				}

				//get value
				Tv_WtVal		=	(Tv_Val >> Tv_WkShft) & 0xF;
				if ( 0 != Tv_WtVal )
				{
					Tv_IsStc		=	TRUE;
				}
				if ( Tv_IsStc )
				{
					//write value
					if ( 9 < Tv_WtVal )
					{
						//hex asc
						*Tv_WkPtr		=	(char)((__u8)(Tv_WtVal + 65 - 10));
					}
					else
					{
						//hex numeral
						*Tv_WkPtr		=  (char)((__u8)(Tv_WtVal + 0x30));
					}
					//inc
					Tv_LenDst ++;
					Tv_WkPtr ++;
				}

				//check finish
				if ( 0 == Tv_WkShft)
				{
					break;
				}

				//next
				Tv_WkShft		=	Tv_WkShft - 4;
				Tv_NumPos --;
			}
			//mark finish
			*Tv_WkPtr		=		0;

			//success
			Tv_Result		=	TRUE;
		}

		return		Tv_Result;

}
//--------------------------------------------------------------------
BOOL8		DrStrFnReverseSlashAsc( char* A_StrFn, __u32 A_LenLim )
{
	// local -------------------
		BOOL8		Tv_Result;
		char*		Tv_PtrChr;
		__u32		Tv_Len;
		__u32		Tv_WkIdx;
	// code --------------------
		//init
		Tv_Result		=	FALSE;
		Tv_PtrChr		=	A_StrFn;		
		Tv_Len			=	(__u32)DrStrLenAsc( A_StrFn, A_LenLim );
		if ( 0 != Tv_Len )
		{
			//replase
			for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_Len;Tv_WkIdx++ )
			{
				//find
				if ( '/' == *Tv_PtrChr )
				{
					*Tv_PtrChr	=	'\\';
					Tv_Result		=	TRUE;
				}
				//next
				Tv_PtrChr ++;
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrStrFnReverseSlashU16le( WCHAR* A_StrFn, __u32 A_LenLim )
{
	// local -------------------
		BOOL8		Tv_Result;
		WCHAR*	Tv_PtrChr;
		__u32		Tv_Len;
		__u32		Tv_WkIdx;
	// code --------------------
		//init
		Tv_Result		=	FALSE;
		Tv_PtrChr		=	A_StrFn;		
		Tv_Len			=	(__u32)DrStrLenU16( A_StrFn, A_LenLim );
		if ( 0 != Tv_Len )
		{
			//replase
			for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_Len;Tv_WkIdx++ )
			{
				//find
				if ( (WCHAR)'/' == *Tv_PtrChr )
				{
					*Tv_PtrChr	=	(WCHAR)'\\';
					Tv_Result		=	TRUE;
				}
				//next
				Tv_PtrChr ++;
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8    DrStrStrToIntAsc( char* A_StrNum, __s32* A_PtrRtVal, __u32 A_LenLim )
{
	// local -------------------
		BOOL8			Tv_Result;

		__u32			Tv_LenSrc;
		char*			Tv_WkPtr;
		__s32				Tv_Num;
		__u32			Tv_WkIdx;
		__s32				Tv_WkMul;
		BOOL8			Tv_IsStx;

	// code --------------------

		//init
		Tv_Result			=	FALSE;
		(*A_PtrRtVal)	=	0;

		//get string
		Tv_LenSrc			=  (__u32)DrStrLenAsc( A_StrNum, A_LenLim );
		if ( 0 != Tv_LenSrc )
		{
			//check last charactor
			Tv_IsStx		=	FALSE;
			Tv_WkMul		=	1;
			Tv_WkPtr		=	A_StrNum + Tv_LenSrc - 1;
			for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_LenSrc;Tv_WkIdx ++ )
			{
				//check value
				if ( ( 0x30 <= *Tv_WkPtr ) && ( 0x39 >= *Tv_WkPtr ) )
				{
					//get value
					Tv_Num			=	*Tv_WkPtr - 0x30;
					(*A_PtrRtVal)	=	(*A_PtrRtVal) + ( Tv_Num * Tv_WkMul );
					//next value
					Tv_WkMul		= Tv_WkMul * 10;
					//success
					Tv_Result		= TRUE;
				}
				else if ( !Tv_IsStx)
				{
					//check minus
					if ( 0x2D == *Tv_WkPtr ) 
					{
						//value minus
						(*A_PtrRtVal)	=	-(*A_PtrRtVal);
					}
					//quit
					break;
				}
				//next
				Tv_WkPtr --;
						 
			}
		}
		return	( Tv_Result );
}
//--------------------------------------------------------------------
void		DrStrInsAsc( char* A_StrDst, char* A_StrIns, __u32 A_InsPos )
{
	// local -------------------
		__u32		Tv_LenDst;
		__u32		Tv_LenIns;
		__u32		Tv_WkIdx;
		__u32		Tv_WkCnt;
		char*		Tv_PtrSr;
		char*		Tv_PtrTg;
	// code --------------------
		//init
		Tv_LenDst		=	(__u32)DrStrLenAsc( A_StrDst );
		Tv_LenIns		=	(__u32)DrStrLenAsc( A_StrIns );

		//check need make space
		if ( A_InsPos < Tv_LenDst )
		{
			//make insert space
			Tv_PtrSr		=	A_StrDst + Tv_LenDst;	
			Tv_PtrTg		=	Tv_PtrSr + Tv_LenIns;
			Tv_WkCnt		=	Tv_LenDst - A_InsPos;
			for ( Tv_WkIdx = 0;Tv_WkIdx <= Tv_WkCnt;Tv_WkIdx++ )
			{
				*Tv_PtrTg		=	*Tv_PtrSr;
				//next
				Tv_PtrSr --;
				Tv_PtrTg --;
			}
			//insert string
			Tv_PtrSr	=	A_StrIns;
			Tv_PtrTg	=	A_StrDst + A_InsPos;
			for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_LenIns;Tv_WkIdx++ )
			{
				*Tv_PtrTg		=	*Tv_PtrSr;
				//next
				Tv_PtrSr ++;
				Tv_PtrTg ++;
			}
		}
		else
		{
			DrStrCatAsc( A_StrDst, A_StrIns );
		}
}
//--------------------------------------------------------------------
void		DrStrInsU16( WCHAR* A_StrDst, WCHAR* A_StrIns, __u32 A_InsPos )
{
	// local -------------------
		__u32		Tv_LenDst;
		__u32		Tv_LenIns;
		__u32		Tv_WkIdx;
		__u32		Tv_WkCnt;
		WCHAR*	Tv_PtrSr;
		WCHAR*	Tv_PtrTg;
	// code --------------------
		//init
		Tv_LenDst		=	(__u32)DrStrLenU16( A_StrDst );
		Tv_LenIns		=	(__u32)DrStrLenU16( A_StrIns );

		//check need make space
		if ( A_InsPos < Tv_LenDst )
		{
			//make insert space
			Tv_PtrSr		=	A_StrDst + Tv_LenDst;	
			Tv_PtrTg		=	Tv_PtrSr + Tv_LenIns;
			Tv_WkCnt		=	Tv_LenDst - A_InsPos;
			for ( Tv_WkIdx = 0;Tv_WkIdx <= Tv_WkCnt;Tv_WkIdx++ )
			{
				*Tv_PtrTg		=	*Tv_PtrSr;
				//next
				Tv_PtrSr --;
				Tv_PtrTg --;
			}
			//insert string
			Tv_PtrSr	=	A_StrIns;
			Tv_PtrTg	=	A_StrDst + A_InsPos;
			for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_LenIns;Tv_WkIdx++ )
			{
				*Tv_PtrTg		=	*Tv_PtrSr;
				//next
				Tv_PtrSr ++;
				Tv_PtrTg ++;
			}
		}
		else
		{
			DrStrCatU16( A_StrDst, A_StrIns );
		}
}
//--------------------------------------------------------------------
void		DrStrDelAsc( char* A_StrDst, __u32 A_DelPos, __u32 A_DelSize )
{
	// local -------------------
		__u32		Tv_LenDst;
		__u32		Tv_WkIdx;
		__u32		Tv_DelSize;
		__u32		Tv_WkCnt;
		char*		Tv_PtrSr;
		char*		Tv_PtrTg;
	// code --------------------
		//init
		Tv_LenDst		=	(__u32)DrStrLenAsc( A_StrDst );
		Tv_DelSize	=	A_DelSize;

		//check delete able
		if ( A_DelPos < Tv_LenDst )
		{
			//check delete size
			if ( (A_DelPos + Tv_DelSize) > Tv_LenDst )
			{
				Tv_DelSize	=	Tv_LenDst - A_DelPos;
			}
			Tv_PtrSr	=	A_StrDst + A_DelPos + Tv_DelSize;
			Tv_PtrTg	=	A_StrDst + A_DelPos;
			Tv_WkCnt	=	Tv_LenDst - A_DelPos - Tv_DelSize;
			for ( Tv_WkIdx = 0;Tv_WkIdx <= Tv_WkCnt;Tv_WkIdx++ )
			{
				*Tv_PtrTg		=	*Tv_PtrSr;
				//next
				Tv_PtrSr ++;
				Tv_PtrTg ++;
			}
		}
}
//--------------------------------------------------------------------
void		DrStrDelU16( WCHAR* A_StrDst, __u32 A_DelPos, __u32 A_DelSize )
{
	// local -------------------
		__u32		Tv_LenDst;
		__u32		Tv_WkIdx;
		__u32		Tv_DelSize;
		__u32		Tv_WkCnt;
		WCHAR*	Tv_PtrSr;
		WCHAR*	Tv_PtrTg;
	// code --------------------
		//init
		Tv_LenDst		=	(__u32)DrStrLenU16( A_StrDst );
		Tv_DelSize	=	A_DelSize;

		//check delete able
		if ( A_DelPos < Tv_LenDst )
		{
			//check delete size
			if ( (A_DelPos + Tv_DelSize) > Tv_LenDst )
			{
				Tv_DelSize	=	Tv_LenDst - A_DelPos;
			}
			Tv_PtrSr	=	A_StrDst + A_DelPos + Tv_DelSize;
			Tv_PtrTg	=	A_StrDst + A_DelPos;
			Tv_WkCnt	=	Tv_LenDst - A_DelPos - Tv_DelSize;
			for ( Tv_WkIdx = 0;Tv_WkIdx <= Tv_WkCnt;Tv_WkIdx++ )
			{
				*Tv_PtrTg		=	*Tv_PtrSr;
				//next
				Tv_PtrSr ++;
				Tv_PtrTg ++;
			}
		}
}
//--------------------------------------------------------------------
BOOL8		DrStrFnIsSameExtAsc( char* A_StrFn, char* A_StrExt )
{
	// var --------------------
	BOOL8			Tv_Result;
	char			Tv_FileExt[4];
	// code -------------------
	Tv_Result = FALSE;

	if(DrStrFnGetExtAsc(A_StrFn, Tv_FileExt))
	{
		if(DrStrCmpAsc(Tv_FileExt, A_StrExt, FALSE) == 0)
		{
			Tv_Result = TRUE;
		}
	}

	return Tv_Result;
}
//--------------------------------------------------------------------
BOOL8	DrStrHexToDwordAsc( char* A_StrNum, __u32* A_PtrRtVal, __u32 A_LenLim )
{
	// local -------------------
		BOOL8			Tv_Result;
		__u32			Tv_LenSrc;
		char*			Tv_WkPtr;
		__s32				Tv_Num;
		__u32			Tv_WkIdx;
		__s32				Tv_WkMul;
		BOOL8			Tv_IsStx;
	// code --------------------

		//init
		Tv_Result			=	FALSE;
		*A_PtrRtVal		=	0;

		//get string
		Tv_LenSrc			= (__u32)DrStrLenAsc( A_StrNum, A_LenLim );
		if ( 0 != Tv_LenSrc )
		{
			//check last charactor
			Tv_IsStx		=	FALSE;
			Tv_WkMul		=	1;
			Tv_WkPtr		=	A_StrNum + Tv_LenSrc - 1;
			for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_LenSrc;Tv_WkIdx ++ )
			{
				//check value
				if ( ( 0x30 <= *Tv_WkPtr ) && ( 0x39 >= *Tv_WkPtr ) )
				{
					//get value
					Tv_Num			=	*Tv_WkPtr - 0x30;
					*A_PtrRtVal	=	*A_PtrRtVal + ( Tv_Num * Tv_WkMul );
					//next value
					Tv_WkMul		= Tv_WkMul << 4;
					//success
					Tv_Result		= TRUE;
				}
				else if ( ( 'a' <= *Tv_WkPtr ) && ( 'f' >= *Tv_WkPtr ) )
				{
					//get value
					Tv_Num			=	*Tv_WkPtr - 'a' + 10;
					*A_PtrRtVal	=	*A_PtrRtVal + ( Tv_Num * Tv_WkMul );
					//next value
					Tv_WkMul		= Tv_WkMul << 4;
					//success
					Tv_Result		= TRUE;
				}
				else if ( ( 'A' <= *Tv_WkPtr ) && ( 'F' >= *Tv_WkPtr ) )
				{
					//get value
					Tv_Num			=	*Tv_WkPtr - 'A' + 10;
					*A_PtrRtVal	=	*A_PtrRtVal + ( Tv_Num * Tv_WkMul );
					//next value
					Tv_WkMul		= Tv_WkMul << 4;
					//success
					Tv_Result		= TRUE;
				}
				else
				{
					Tv_Result		=	FALSE;
					break;
				}
				//next
				Tv_WkPtr --;
			}
		}
		return	Tv_Result;
}
//--------------------------------------------------------------------
void	DrStrPrsInitAsc(Ptr_DrStrParsCtrlAsc A_PtrPrs, char* A_StrSrc, __u32 A_SrcMax)
{

	// init
	A_PtrPrs->StrSrc		=	A_StrSrc;
	A_PtrPrs->SrcLen		=	(__u32)DrStrLenAsc(A_StrSrc, A_SrcMax);

	A_PtrPrs->LineSt		=	0;
	A_PtrPrs->LineLenCont	=	DrStrGetOneLineSizeAsc(A_StrSrc, &A_PtrPrs->LineCrSize, A_PtrPrs->SrcLen);

	A_PtrPrs->WordPrgs	=	0;
	A_PtrPrs->WordLen		=	0;

}
//--------------------------------------------------------------------
BOOL8		DrStrPrsLineNext(Ptr_DrStrParsCtrlAsc A_PtrPrs)
{

	// move line progress
	A_PtrPrs->LineSt	=	A_PtrPrs->LineSt + A_PtrPrs->LineLenCont + A_PtrPrs->LineCrSize;
	if (A_PtrPrs->LineSt >= A_PtrPrs->SrcLen)
	{
		A_PtrPrs->LineLenCont	=	0;
		A_PtrPrs->LineCrSize	=	0;
		A_PtrPrs->WordPrgs		=	A_PtrPrs->LineSt;
		A_PtrPrs->WordLen			=	0;
		return	FALSE;
	}

	// get line size
	A_PtrPrs->LineLenCont	=	DrStrGetOneLineSizeAsc(&A_PtrPrs->StrSrc[A_PtrPrs->LineSt], &A_PtrPrs->LineCrSize, A_PtrPrs->SrcLen - A_PtrPrs->LineSt);

	// update info
	A_PtrPrs->WordPrgs		=	A_PtrPrs->LineSt;
	A_PtrPrs->WordLen			=	0;

	return	TRUE;

}
//--------------------------------------------------------------------
char*		DrStrPrsWordSkipSpace(Ptr_DrStrParsCtrlAsc A_PtrPrs)
{
	A_PtrPrs->WordLen	=	0;	// reset word length

	while (A_PtrPrs->WordPrgs < (A_PtrPrs->LineSt + A_PtrPrs->LineLenCont))
	{
		if (0x20 < A_PtrPrs->StrSrc[A_PtrPrs->WordPrgs])
		{
			break;
		}
		A_PtrPrs->WordPrgs ++;
	}

	if (A_PtrPrs->WordPrgs >= (A_PtrPrs->LineSt + A_PtrPrs->LineLenCont))
	{
		return	NULL;
	}

	return	&A_PtrPrs->StrSrc[A_PtrPrs->WordPrgs];

}
//--------------------------------------------------------------------
BOOL8		DrStrPrsWordGoNext(Ptr_DrStrParsCtrlAsc A_PtrPrs)
{
	A_PtrPrs->WordPrgs	=	A_PtrPrs->WordPrgs + A_PtrPrs->WordLen;
	A_PtrPrs->WordLen		=	0;

	if (A_PtrPrs->WordPrgs >= (A_PtrPrs->LineSt + A_PtrPrs->LineLenCont))
	{
		return	FALSE;
	}

	return	TRUE;
}
//--------------------------------------------------------------------
BOOL8		DrStrPrsWordGoMarkNext(Ptr_DrStrParsCtrlAsc A_PtrPrs, char A_Mark)
{
	BOOL8	Tv_Result;

	Tv_Result	=	FALSE;
	A_PtrPrs->WordLen	=	0;	// reset word length

	while (A_PtrPrs->WordPrgs < (A_PtrPrs->LineSt + A_PtrPrs->LineLenCont))
	{
		if (A_Mark == A_PtrPrs->StrSrc[A_PtrPrs->WordPrgs])
		{
			// found
			Tv_Result	=	TRUE;
			A_PtrPrs->WordPrgs ++;
			break;
		}
		A_PtrPrs->WordPrgs ++;
	}

	// patch result
	if (A_PtrPrs->WordPrgs >= (A_PtrPrs->LineSt + A_PtrPrs->LineLenCont))
	{
		Tv_Result	=	FALSE;
	}

	return	Tv_Result;

}
//--------------------------------------------------------------------
char*		DrStrPrsWordSkipByCount(Ptr_DrStrParsCtrlAsc A_PtrPrs, __u32 A_SkinCnt)
{
	A_PtrPrs->WordLen	=	0;	// reset word length

	if ((A_PtrPrs->WordPrgs + A_SkinCnt) >= (A_PtrPrs->LineSt + A_PtrPrs->LineLenCont))
	{
		// go end line
		A_PtrPrs->WordPrgs	=	A_PtrPrs->LineSt + A_PtrPrs->LineLenCont;
		return	NULL;
	}

	A_PtrPrs->WordPrgs	=	A_PtrPrs->WordPrgs + A_SkinCnt;

	return	&A_PtrPrs->StrSrc[A_PtrPrs->WordPrgs];

}
//--------------------------------------------------------------------
char*		DrStrPrsWordFindChars(Ptr_DrStrParsCtrlAsc A_PtrPrs, char* A_StrChars)
{
	__u32	Tv_ChrLen;
	__u32	Tv_ChrIdx;
	BOOL8	Tv_IsFound;
	A_PtrPrs->WordLen	=	0;	// reset word length

	Tv_ChrLen	=	(__u32)DrStrLenAsc(A_StrChars);
	if (0 == Tv_ChrLen)
	{
		return	NULL;
	}

	while (A_PtrPrs->WordPrgs < (A_PtrPrs->LineSt + A_PtrPrs->LineLenCont))
	{
		Tv_IsFound	=	FALSE;
		for ( Tv_ChrIdx = 0; Tv_ChrIdx < Tv_ChrLen; Tv_ChrIdx++)
		{
			if (A_StrChars[Tv_ChrIdx] == A_PtrPrs->StrSrc[A_PtrPrs->WordPrgs])
			{
				Tv_IsFound	=	TRUE;
				break;
			}
		}

		if (Tv_IsFound)
		{
			break;
		}

		A_PtrPrs->WordPrgs ++;
	}

	if (A_PtrPrs->WordPrgs >= (A_PtrPrs->LineSt + A_PtrPrs->LineLenCont))
	{
		return	NULL;
	}

	return	&A_PtrPrs->StrSrc[A_PtrPrs->WordPrgs];

}
//--------------------------------------------------------------------
char*		DrStrPrsWordGet(Ptr_DrStrParsCtrlAsc A_PtrPrs, Ptr_DrStrParsWordRngAsc A_PtrRngTbl, __u32 A_RngTblCnt)
{
	BOOL8	Tv_IsGood;
	char	Tv_Val;
	__u32	Tv_TblIdx;
	__u32	Tv_Len;

	A_PtrPrs->WordLen	=	0;	// reset word length

	// check already end
	if (A_PtrPrs->WordPrgs >= (A_PtrPrs->LineSt + A_PtrPrs->LineLenCont))
	{
		return	NULL;
	}

	Tv_Len	=	0;

	while ((A_PtrPrs->WordPrgs + Tv_Len) < (A_PtrPrs->LineSt + A_PtrPrs->LineLenCont))
	{
		// check range
		Tv_IsGood	=	FALSE;

		Tv_Val	=	A_PtrPrs->StrSrc[A_PtrPrs->WordPrgs + Tv_Len];
		for ( Tv_TblIdx = 0; Tv_TblIdx < A_RngTblCnt; Tv_TblIdx++)
		{
			if ((Tv_Val >= A_PtrRngTbl[Tv_TblIdx].St) && (Tv_Val <= A_PtrRngTbl[Tv_TblIdx].Ed))
			{
				Tv_IsGood	=	TRUE;
				break;
			}
		}

		if (!Tv_IsGood)
		{
			break;
		}

		Tv_Len ++;
	}

	A_PtrPrs->WordLen	=	Tv_Len;
	return	&A_PtrPrs->StrSrc[A_PtrPrs->WordPrgs];
}
//--------------------------------------------------------------------
void		DrStrIpToStrAsc( char* A_StrRt, __u32 A_Ip )
{
	// local -------------------
		__u8*		Tv_PtrWk;
		char*		Tv_StrWk;
	// code --------------------
		//init
		Tv_PtrWk		=	(__u8*)&A_Ip;
		
		DrStrIntToStrAsc( A_StrRt, Tv_PtrWk[0] ); 
		Tv_StrWk	= DrStrAddChar( A_StrRt, '.' );
		DrStrIntToStrAsc( Tv_StrWk, Tv_PtrWk[1] ); 
		Tv_StrWk	= DrStrAddChar( Tv_StrWk, '.' );
		DrStrIntToStrAsc( Tv_StrWk, Tv_PtrWk[2] ); 
		Tv_StrWk	= DrStrAddChar( Tv_StrWk, '.' );
		DrStrIntToStrAsc( Tv_StrWk, Tv_PtrWk[3] ); 
}
//--------------------------------------------------------------------
void	DrStrIpToStrU16le( WCHAR* A_StrRt, __u32 A_Ip )
{
	// local -------------------
		__u8*		Tv_PtrWk;
		WCHAR*	Tv_StrWk;
	// code --------------------
		//init
		Tv_PtrWk		=	(__u8*)&A_Ip;
		Tv_StrWk		=	A_StrRt;
		DrStrIntToStrU16le( Tv_StrWk, Tv_PtrWk[0] ); 
		Tv_StrWk	=	DrStrAddWchar( Tv_StrWk, (WCHAR)'.' );
		DrStrIntToStrU16le( Tv_StrWk, Tv_PtrWk[1] ); 
		Tv_StrWk	=	DrStrAddWchar( Tv_StrWk, (WCHAR)'.' );
		DrStrIntToStrU16le( Tv_StrWk, Tv_PtrWk[2] ); 
		Tv_StrWk	=	DrStrAddWchar( Tv_StrWk, (WCHAR)'.' );
		DrStrIntToStrU16le( Tv_StrWk, Tv_PtrWk[3] ); 
}
//--------------------------------------------------------------------
BOOL8	DrStrHexToQwordAsc( char* A_StrNum, __u64* A_PtrRtVal, __u32 A_LenLim )
{
	// local -------------------
		BOOL8			Tv_Result;
		__u32			Tv_LenSrc;
		char*			Tv_WkPtr;
		__u64			Tv_Num;
		__u32			Tv_WkIdx;
		__u64			Tv_WkMul;
		BOOL8			Tv_IsStx;
	// code --------------------

		//init
		Tv_Result			=	FALSE;
		*A_PtrRtVal		=	0;

		//get string
		Tv_LenSrc			=  (__u32)DrStrLenAsc( A_StrNum, A_LenLim );
		if ( 0 != Tv_LenSrc )
		{
			//check last charactor
			Tv_IsStx		=	FALSE;
			Tv_WkMul		=	1;
			Tv_WkPtr		=	A_StrNum + Tv_LenSrc - 1;
			for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_LenSrc;Tv_WkIdx ++ )
			{
				//check value
				if ( ( 0x30 <= *Tv_WkPtr ) && ( 0x39 >= *Tv_WkPtr ) )
				{
					//get value
					Tv_Num			=	*Tv_WkPtr - 0x30LL;
					*A_PtrRtVal	=	*A_PtrRtVal + ( Tv_Num * Tv_WkMul );
					//next value
					Tv_WkMul		= Tv_WkMul << 4;
					//success
					Tv_Result		= TRUE;
				}
				else if ( ( 'a' <= *Tv_WkPtr ) && ( 'f' >= *Tv_WkPtr ) )
				{
					//get value
					Tv_Num			=	(__u64)*Tv_WkPtr - (__u64)'a' + 10LL;
					*A_PtrRtVal	=	*A_PtrRtVal + ( Tv_Num * Tv_WkMul );
					//next value
					Tv_WkMul		= Tv_WkMul << 4;
					//success
					Tv_Result		= TRUE;
				}
				else if ( ( 'A' <= *Tv_WkPtr ) && ( 'F' >= *Tv_WkPtr ) )
				{
					//get value
					Tv_Num			=	*Tv_WkPtr - (__u64)'A' + 10LL;
					*A_PtrRtVal	=	*A_PtrRtVal + ( Tv_Num * Tv_WkMul );
					//next value
					Tv_WkMul		= Tv_WkMul << 4;
					//success
					Tv_Result		= TRUE;
				}
				else
				{
					Tv_Result		=	FALSE;
					break;
				}
				//next
				Tv_WkPtr --;
			}
		}
		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrStrHexToQwordU16le(WCHAR* A_StrNum, __u64* A_PtrRtVal, __u32 A_LenLim)
{
	// local -------------------
		BOOL8			Tv_Result;
		__u32			Tv_LenSrc;
		WCHAR*			Tv_WkPtr;
		__u64			Tv_Num;
		__u32			Tv_WkIdx;
		__u64			Tv_WkMul;
		BOOL8			Tv_IsStx;
	// code --------------------
		//init
		Tv_Result			=	FALSE;
		*A_PtrRtVal		=	0;

		//get string
		Tv_LenSrc			=  (__u32)DrStrLenU16(A_StrNum, A_LenLim);
		if(0 != Tv_LenSrc)
		{
			//check last charactor
			Tv_IsStx		=	FALSE;
			Tv_WkMul		=	1;
			Tv_WkPtr		=	A_StrNum + Tv_LenSrc - 1;
			for(Tv_WkIdx = 0; Tv_WkIdx < Tv_LenSrc; Tv_WkIdx ++)
			{
				//check value
				if((0x30 <= *Tv_WkPtr) && (0x39 >= *Tv_WkPtr))
				{
					//get value
					Tv_Num			=	*Tv_WkPtr - 0x30;
					*A_PtrRtVal	=	*A_PtrRtVal + (Tv_Num * Tv_WkMul);
					//next value
					Tv_WkMul		= Tv_WkMul << 4;
					//success
					Tv_Result		= TRUE;
				}
				else if((0x61 <= *Tv_WkPtr) && (0x66 >= *Tv_WkPtr))
				{
					//get value
					Tv_Num			=	(__u64)*Tv_WkPtr - 0x61LL + 10LL;
					*A_PtrRtVal	=	*A_PtrRtVal + (Tv_Num * Tv_WkMul);
					//next value
					Tv_WkMul		= Tv_WkMul << 4;
					//success
					Tv_Result		= TRUE;
				}
				else if((0x41 <= *Tv_WkPtr) && (0x46 >= *Tv_WkPtr))
				{
					//get value
					Tv_Num			=	(__u64)*Tv_WkPtr - 0x41 + 10;
					*A_PtrRtVal	=	*A_PtrRtVal + (Tv_Num * Tv_WkMul);
					//next value
					Tv_WkMul		= Tv_WkMul << 4;
					//success
					Tv_Result		= TRUE;
				}
				else
				{
					Tv_Result		=	FALSE;
					break;
				}
				//next
				Tv_WkPtr --;
			}
		}
		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrStrQwordToHexAsc( char* A_StrRt, __u64 A_Value, __u32 A_LenLim, BOOL8 A_IsFix )
{
	// local -------------------
		BOOL8		Tv_Result;
		__u32		Tv_LenDst;
		__u32		Tv_Lim;
		char*		Tv_WkPtr;
		__u32		Tv_WkShft;
		__u64		Tv_Val;
		BOOL8		Tv_IsStc;
		__u64		Tv_WtVal;
		__u32		Tv_NumPos;

	// code --------------------
		//init
		Tv_Result		=	FALSE;
		Tv_Lim      =	(A_LenLim - 1) & 0x7FFFFFFF;

		if ( A_IsFix && (0 == A_LenLim) )
		{
			Tv_Lim		=		0;
		}
		//check limite
		if ( 0 != Tv_Lim )
		{
			//init var
			Tv_IsStc		=	FALSE;
			Tv_LenDst   = 0;
			Tv_WkPtr    = A_StrRt;
			Tv_Val      = A_Value;
			//analysis number
			Tv_WkShft   = 60;
			Tv_NumPos   = 16;
			while ( Tv_LenDst <= Tv_Lim )
			{
				//check start position
				if ( A_IsFix && ( (Tv_Lim - Tv_LenDst) >= (Tv_NumPos - 1) ) )
				{
					Tv_IsStc		=	TRUE;
				}

				if ( 1 == Tv_NumPos )
				{
					Tv_IsStc		=	TRUE;
				}

				//get value
				Tv_WtVal		=	(Tv_Val >> Tv_WkShft) & 0xF;
				if ( 0 != Tv_WtVal )
				{
					Tv_IsStc		=	TRUE;
				}
				if ( Tv_IsStc )
				{
					//write value
					if ( 9 < Tv_WtVal )
					{
						//hex asc
						*Tv_WkPtr		=	(char)((__u8)(Tv_WtVal + 65 - 10));
					}
					else
					{
						//hex numeral
						*Tv_WkPtr		=  (char)((__u8)(Tv_WtVal + 0x30));
					}
					//inc
					Tv_LenDst ++;
					Tv_WkPtr ++;
				}

				//check finish
				if ( 0 == Tv_WkShft)
				{
					break;
				}

				//next
				Tv_WkShft		=	Tv_WkShft - 4;
				Tv_NumPos --;
			}
			//mark finish
			*Tv_WkPtr		=		0;

			//success
			Tv_Result		=	TRUE;
		}

		return		Tv_Result;

}
//--------------------------------------------------------------------
BOOL8		DrStrQwordToHexU16le(WCHAR* A_StrRt, __u64 A_Value, __u32 A_LenLim , BOOL8 A_IsFix)
{
	// local -------------------
		BOOL8		Tv_Result;
		__u32		Tv_LenDst;
		__u32		Tv_Lim;
		WCHAR*	Tv_WkPtr;
		__u32		Tv_WkShft;
		__u64		Tv_Val;
		BOOL8		Tv_IsStc;
		__u64		Tv_WtVal;
		__u32		Tv_NumPos;
	// code --------------------
		//init
		Tv_Result		=	FALSE;
		Tv_Lim      =	(A_LenLim - 1) & 0x7FFFFFFF;

		if(A_IsFix && (0 == A_LenLim))
		{
			Tv_Lim		=		0;
		}
		//check limite
		if(0 != Tv_Lim)
		{
			//init var
			Tv_IsStc		=	FALSE;
			Tv_LenDst   = 0;
			Tv_WkPtr    = A_StrRt;
			Tv_Val      = A_Value;
			//analysis number
			Tv_WkShft   = 60;
			Tv_NumPos   = 16;
			while(Tv_LenDst <= Tv_Lim)
			{
				//check start position
				if(A_IsFix && ((Tv_Lim - Tv_LenDst) >= (Tv_NumPos - 1)))
				{
					Tv_IsStc		=	TRUE;
				}

				if(1 == Tv_NumPos)
				{
					Tv_IsStc		=	TRUE;
				}

				//get value
				Tv_WtVal		=	(Tv_Val >> Tv_WkShft) & 0xF;
				if(0 != Tv_WtVal)
				{
					Tv_IsStc		=	TRUE;
				}
				if(Tv_IsStc)
				{
					//write value
					if(9 < Tv_WtVal)
					{
						//hex asc
						*Tv_WkPtr		=	(WCHAR)((__u16)(Tv_WtVal + 65 - 10));
					}
					else
					{
						//hex numeral
						*Tv_WkPtr		=  (WCHAR)((__u16)(Tv_WtVal + 0x30));
					}
					//inc
					Tv_LenDst ++;
					Tv_WkPtr ++;
				}

				//check finish
				if(0 == Tv_WkShft)
				{
					break;
				}

				//next
				Tv_WkShft		=	Tv_WkShft - 4;
				Tv_NumPos --;
			}
			//mark finish
			*Tv_WkPtr		=		0;

			//success
			Tv_Result		=	TRUE;
		}

		return		Tv_Result;

}
//--------------------------------------------------------------------
__u32	DrStrBase64Enc(char* A_StrSrc, char* A_StrRtEnc)
{
	// local -------------------
		__u32		Tv_Result;
		__u8		Tv_WkBuf[4];
		__u8		Tv_BufInCnt;
		char*		Tv_PtrTg;
		__u8*		Tv_PtrSr;
		__u32		Tv_LastCnt;
		__u8		Tv_WkIdx;
	// code --------------------
		//init
		Tv_Result		=	0;
		Tv_LastCnt	=	(__u32)DrStrLenAsc( A_StrSrc );
		Tv_PtrSr		=	(__u8*)A_StrSrc;
		Tv_PtrTg		=	A_StrRtEnc;

		while ( 0 != Tv_LastCnt )
		{
			//sampling
			switch( Tv_LastCnt )
			{
				case	1:
					Tv_WkBuf[0]	=	(Tv_PtrSr[0] >> 2) & 0x3F;
					Tv_WkBuf[1]	=	((Tv_PtrSr[0] & 0x03) << 4);
					Tv_BufInCnt	=	2;
					Tv_LastCnt	=	0;
					break;
				case	2:
					Tv_WkBuf[0]	=	(Tv_PtrSr[0] >> 2) & 0x3F;
					Tv_WkBuf[1]	=	((Tv_PtrSr[0] & 0x03) << 4) | ((Tv_PtrSr[1] & 0xF0) >> 4);
					Tv_WkBuf[2]	=	((Tv_PtrSr[1] & 0x0F) << 2);
					Tv_BufInCnt	=	3;
					Tv_LastCnt	=	0;
					break;
				default:
					Tv_WkBuf[0]	=	(Tv_PtrSr[0] >> 2) & 0x3F;
					Tv_WkBuf[1]	=	((Tv_PtrSr[0] & 0x03) << 4) | ((Tv_PtrSr[1] & 0xF0) >> 4);
					Tv_WkBuf[2]	=	((Tv_PtrSr[1] & 0x0F) << 2) | ((Tv_PtrSr[2] & 0xC0) >> 6);
					Tv_WkBuf[3]	=	(Tv_PtrSr[2] & 0x3F);
					Tv_BufInCnt	=	4;
					Tv_LastCnt	=	Tv_LastCnt - 3;
					break;
			}
			//write buffer
			for ( Tv_WkIdx=0;Tv_WkIdx < 4;Tv_WkIdx++ )
			{
				Tv_PtrTg[Tv_WkIdx]	=	'=';
				if ( Tv_WkIdx < Tv_BufInCnt )
				{
					Tv_PtrTg[Tv_WkIdx]	=	V_DrStrToolBase64Tbl[ Tv_WkBuf[Tv_WkIdx] ];
				}
			}
			//next pointer
			Tv_PtrSr	=	Tv_PtrSr + 3;
			Tv_PtrTg	=	Tv_PtrTg + 4;
			Tv_Result	=	Tv_Result + 4;
		}

		//finish mark
		*Tv_PtrTg	=	0;

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8	LcDrStrBase64FindIdx( char A_Code, __u32* A_PtrRtIdx )
{
	// local -------------------
		BOOL8		Tv_Result;
	// code --------------------
		//init
		Tv_Result		=	TRUE;

		//check range
		if ( (A_Code >= 'A' ) && (A_Code <= 'Z' ) )
		{
			*A_PtrRtIdx	=	(__u32)A_Code - (__u32)'A';
		}
		else if ( (A_Code >= 'a' ) && (A_Code <= 'z' ) )
		{
			*A_PtrRtIdx	=	26 + (__u32)A_Code - (__u32)'a';
		}
		else if ( (A_Code >= '0' ) && (A_Code <= '9' ) )
		{
			*A_PtrRtIdx	=	52 + (__u32)A_Code - (__u32)'0';
		}
		else if (A_Code == '+' )
		{
			*A_PtrRtIdx		=	62;
		}
		else if (A_Code == '/' )
		{
			*A_PtrRtIdx		=	63;
		}
		else if (A_Code == '=' )
		{
			*A_PtrRtIdx		=	0;
		}
		else
		{
			Tv_Result		=	FALSE;
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrStrBase64Dec( char* A_StrSrc, char* A_StrRtDec )
{
	__u32		Tv_Result;
	char*		Tv_StrRt;
	__u32		Tv_SrIdx;

	__u32		Tv_SmpBuf;
	__u32		Tv_Code;

	//init
	Tv_Result		=	0;
	Tv_StrRt		=	A_StrRtDec;
	Tv_SrIdx		=	0;

	Tv_SmpBuf		=	0;

	while (TRUE)
	{
		if ((0 == (A_StrSrc[Tv_SrIdx])) || ('=' == A_StrSrc[Tv_SrIdx]) || (!LcDrStrBase64FindIdx(A_StrSrc[Tv_SrIdx], &Tv_Code)))
		{
			if (0 != Tv_SmpBuf)
			{
				*Tv_StrRt	=	(char)(Tv_SmpBuf & 0xFF);
				Tv_StrRt ++;
			}
			// mark end
			*Tv_StrRt	=	0;
			break;
		}

		switch ((Tv_SrIdx & 3))
		{
		case 0:
			Tv_SmpBuf	=	Tv_Code << 2;
			break;
		case 1:
			Tv_SmpBuf	=	Tv_SmpBuf | (Tv_Code >> 4) & 3;
			*Tv_StrRt	=	(char)Tv_SmpBuf;
			Tv_StrRt ++;
			Tv_SmpBuf	=	(Tv_Code & 0x0F) << 4;
			break;
		case 2:
			Tv_SmpBuf	=	Tv_SmpBuf | (Tv_Code >> 2) & 0xF;
			*Tv_StrRt	=	(char)Tv_SmpBuf;
			Tv_StrRt ++;
			Tv_SmpBuf	=	(Tv_Code & 0x03) << 6;
			break;
		default:
			Tv_SmpBuf	=	Tv_SmpBuf | Tv_Code & 0x3F;
			*Tv_StrRt	=	(char)Tv_SmpBuf;
			Tv_StrRt ++;
			Tv_SmpBuf	=	0;
			break;
		}
		// next
		Tv_SrIdx ++;
	}


	return	(__u32)((Def_DrCalPtr)Tv_StrRt - (Def_DrCalPtr)A_StrRtDec);
}
//--------------------------------------------------------------------
BOOL8		DrStrHexToDwordU16le( WCHAR* A_StrNum, __u32* A_PtrRtVal, __u32 A_LenLim )
{
	// local -------------------
		BOOL8			Tv_Result;
		__u32			Tv_LenSrc;
		WCHAR*		Tv_WkPtr;
		__u32			Tv_Num;
		__u32			Tv_WkIdx;
		__u32			Tv_WkMul;
		BOOL8			Tv_IsStx;
	// code --------------------

		//init
		Tv_Result			=	FALSE;
		*A_PtrRtVal		=	0;

		//get string
		Tv_LenSrc			=  (__u32)DrStrLenU16( A_StrNum, A_LenLim );
		if ( 0 != Tv_LenSrc )
		{
			//check last charactor
			Tv_IsStx		=	FALSE;
			Tv_WkMul		=	1;
			Tv_WkPtr		=	A_StrNum + Tv_LenSrc - 1;
			for ( Tv_WkIdx = 0;Tv_WkIdx < Tv_LenSrc;Tv_WkIdx ++ )
			{
				//check value
				if ( ( (WCHAR)'0' <= *Tv_WkPtr ) && ( (WCHAR)'9' >= *Tv_WkPtr ) )
				{
					//get value
					Tv_Num			=	*Tv_WkPtr - (WCHAR)'0';
					*A_PtrRtVal	=	*A_PtrRtVal + ( Tv_Num * Tv_WkMul );
					//next value
					Tv_WkMul		= Tv_WkMul << 4;
					//success
					Tv_Result		= TRUE;
				}
				else if ( ( (WCHAR)'a' <= *Tv_WkPtr ) && ( (WCHAR)'f' >= *Tv_WkPtr ) )
				{
					//get value
					Tv_Num			=	*Tv_WkPtr - (WCHAR)'a' + 10;
					*A_PtrRtVal	=	*A_PtrRtVal + ( Tv_Num * Tv_WkMul );
					//next value
					Tv_WkMul		= Tv_WkMul << 4;
					//success
					Tv_Result		= TRUE;
				}
				else if ( ( (WCHAR)'A' <= *Tv_WkPtr ) && ( (WCHAR)'F' >= *Tv_WkPtr ) )
				{
					//get value
					Tv_Num			=	*Tv_WkPtr - (WCHAR)'A' + 10;
					*A_PtrRtVal	=	*A_PtrRtVal + ( Tv_Num * Tv_WkMul );
					//next value
					Tv_WkMul		= Tv_WkMul << 4;
					//success
					Tv_Result		= TRUE;
				}
				else
				{
					Tv_Result		=	FALSE;
					break;
				}
				//next
				Tv_WkPtr --;
			}
		}
		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8	DrStrDwordToHexU16le( WCHAR* A_StrRt, __u32 A_Value, __u32 A_LenLim, BOOL8 A_IsFix )
{
	// local -------------------
		BOOL8		Tv_Result;
		__u32		Tv_LenDst;
		__u32		Tv_Lim;
		WCHAR*	Tv_WkPtr;
		__u32		Tv_WkShft;
		__u32		Tv_Val;
		BOOL8		Tv_IsStc;
		__u32		Tv_WtVal;
		__u32		Tv_NumPos;
	// code --------------------
		//init
		Tv_Result		=	FALSE;
		Tv_Lim      =	(A_LenLim - 1) & 0x7FFFFFFF;

		if ( A_IsFix && (0 == A_LenLim) )
		{
			Tv_Lim		=		0;
		}
		//check limite
		if ( 0 != Tv_Lim )
		{
			//init var
			Tv_IsStc		=	FALSE;
			Tv_LenDst   = 0;
			Tv_WkPtr    = A_StrRt;
			Tv_Val      = A_Value;
			//analysis number
			Tv_WkShft   = 28;
			Tv_NumPos   = 8;
			while ( Tv_LenDst <= Tv_Lim )
			{
				//check start position
				if ( A_IsFix && ( (Tv_Lim - Tv_LenDst) >= (Tv_NumPos - 1) ) )
				{
					Tv_IsStc		=	TRUE;
				}

				if ( 1 == Tv_NumPos )
				{
					Tv_IsStc		=	TRUE;
				}

				//get value
				Tv_WtVal		=	(Tv_Val >> Tv_WkShft) & 0xF;
				if ( 0 != Tv_WtVal )
				{
					Tv_IsStc		=	TRUE;
				}
				if ( Tv_IsStc )
				{
					//write value
					if ( 9 < Tv_WtVal )
					{
						//hex asc
						*Tv_WkPtr		=	(WCHAR)(Tv_WtVal + (WCHAR)'A' - 10);
					}
					else
					{
						//hex numeral
						*Tv_WkPtr		= (WCHAR)(Tv_WtVal + (WCHAR)'0');
					}
					//inc
					Tv_LenDst ++;
					Tv_WkPtr ++;
				}

				//check finish
				if ( 0 == Tv_WkShft)
				{
					break;
				}

				//next
				Tv_WkShft		=	Tv_WkShft - 4;
				Tv_NumPos --;
			}
			//mark finish
			*Tv_WkPtr		=		0;

			//success
			Tv_Result		=	TRUE;
		}

		return		Tv_Result;
}
//--------------------------------------------------------------------
void	DrStrU16leToAsc( char* A_StrRt, WCHAR* A_StrSrc, __u32 A_Language )
{
	// local -------------------
		char*		Tv_PtrTg;
		WCHAR*	Tv_PtrSr;
		__u32		Tv_LenSr;
	// code --------------------
		//init
		Tv_LenSr	=	(__u32)DrStrLenU16( A_StrSrc );
		Tv_PtrTg	=	A_StrRt;
		//check size
		if ( 0 != Tv_LenSr )
		{
			Tv_PtrSr	=	A_StrSrc;
			while ( 0 != *Tv_PtrSr )
			{
				//write
				*Tv_PtrTg	=	(char)*Tv_PtrSr;
				Tv_PtrSr ++;
				Tv_PtrTg ++;
			}
		}
		//mark zero
		*Tv_PtrTg	=	0;
}
//--------------------------------------------------------------------
void		DrStrAscToU16le( WCHAR* A_StrRt, char* A_StrSrc, __u32 A_Language )
{
	// local -------------------
		WCHAR*	Tv_PtrTg;
		char*		Tv_PtrSr;
		__u32		Tv_LenSr;
	// code --------------------
		//init
		Tv_LenSr	=	(__u32)DrStrLenAsc( A_StrSrc );
		Tv_PtrTg	=	A_StrRt;

		//check size
		if ( 0 != Tv_LenSr )
		{
			Tv_PtrSr	=	A_StrSrc;
			while ( 0 != *Tv_PtrSr )
			{
				//write
				*Tv_PtrTg	=	(WCHAR)*Tv_PtrSr;
				Tv_PtrSr ++;
				Tv_PtrTg ++;
			}
		}
		//mark zero
		*Tv_PtrTg	=	0;
}
//--------------------------------------------------------------------
void		DrStrKorDecU16le( __u16 A_Code, __u8* A_PtrRtFirst, __u8* A_PtrRtMid, __u8* A_PtrRtLast )
{
	// local -------------------
		__u16		Tv_Code;
		__u16		Tv_Tmp;
	// code --------------------
		//init
		*A_PtrRtFirst		=	0;
		*A_PtrRtMid			=	E_DrStrKorNoMidLastU16le;
		*A_PtrRtLast		=	E_DrStrKorNoMidLastU16le;
		//check code
		if ( (E_DrStrKorCmbJamoStartU16le <= A_Code) && (E_DrStrKorCmbJamoEndU16le >= A_Code) )
		{
			//jamo
			*A_PtrRtFirst	=	DrStrKorJamoToFirstU16le( A_Code );
		}
		else if ( (E_DrStrKorCodeStartU16le <= A_Code) && (E_DrStrKorCodeEndU16le >= A_Code) )
		{
			//code
			Tv_Code		=	A_Code - E_DrStrKorCodeStartU16le;
			*A_PtrRtLast	=	(__u8)(Tv_Code % E_DrStrKorLastStepU16le);
			*A_PtrRtFirst	=	(__u8)( Tv_Code / E_DrStrKorFirStepU16le );
			Tv_Tmp		=	Tv_Code % E_DrStrKorFirStepU16le;
			*A_PtrRtMid		=	(__u8)( Tv_Tmp / E_DrStrKorLastStepU16le );
		}
}
//--------------------------------------------------------------------
__u16		DrStrKorEncU16le( __u8 A_First, __u8 A_Mid, __u8 A_Last )
{
	return	( (__u16)A_First * E_DrStrKorFirStepU16le ) + ( (__u16)A_Mid * E_DrStrKorLastStepU16le ) + (__u16)A_Last + E_DrStrKorCodeStartU16le;
}
//--------------------------------------------------------------------
__u8		DrStrKorJamoToFirstU16le( __u16 A_Code )
{
	// local -------------------
		__u8		Tv_Result;
		__u8		Tv_WkIdx;
	// code --------------------
		Tv_Result		=	0;

		for ( Tv_WkIdx = 0;Tv_WkIdx < E_DrStrKorFirCntU16le;Tv_WkIdx++ )
		{
			if ( A_Code == V_DrStrToolUniKorFirTbl[Tv_WkIdx] )
			{
				//found
				Tv_Result		=	Tv_WkIdx;
				break;
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u8		DrStrKorJamoToMiddleU16le( __u16 A_Code )
{
	// local -------------------
		__u8		Tv_Result;
	// code --------------------
		//init
		Tv_Result	=	E_DrStrKorNoMidLastU16le;

		if ( (0x314F <= A_Code) && (0x3163 >= A_Code) )
		{
			Tv_Result	=	(__u8)( A_Code - 0x314F );
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u8		DrStrKorJamoToLastU16le( __u16 A_Code )
{
	// local -------------------
		__u8		Tv_Result;
	// code --------------------
		//init
		Tv_Result	=	E_DrStrKorNoMidLastU16le;

		if ( (0x3131 <= A_Code) && (0x314E >= A_Code) )
		{
			Tv_Result	=	(__u8)( A_Code - 0x3131 + 1 );
			if ( 0x3138 <= A_Code )
			{
				Tv_Result --;
			}
			if ( 0x3143 <= A_Code )
			{
				Tv_Result --;
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
WCHAR*	DrStrFillSpaceU16le( WCHAR* A_Str, __u8 A_Cnt )
{
	// local -------------------
		WCHAR*	Tv_Result;
		__u8		Tv_Cnt;
	// code --------------------
		Tv_Result	=	A_Str;
		Tv_Cnt		=	A_Cnt;
		while ( 0 != Tv_Cnt )
		{
			*Tv_Result	=	0x20;		//fill space
			//next
			Tv_Result ++;
			Tv_Cnt --;
		}
		//set finish
		*Tv_Result	=	0;

		return	Tv_Result;
}
//--------------------------------------------------------------------
char*		DrStrFillSpaceAsc( char* A_Str, __u8 A_Cnt )
{
	// local -------------------
		char*	Tv_Result;
		__u8		Tv_Cnt;
	// code --------------------
		Tv_Result	=	A_Str;
		Tv_Cnt		=	A_Cnt;
		while ( 0 != Tv_Cnt )
		{
			*Tv_Result	=	0x20;		//fill space
			//next
			Tv_Result ++;
			Tv_Cnt --;
		}
		//set finish
		*Tv_Result	=	0;

		return	Tv_Result;
}
//--------------------------------------------------------------------
void	DrStrDateDispTypeSet( __u8 A_Mode )
{
		V_DrStrToolDateDispType	=	A_Mode;
}
//--------------------------------------------------------------------
__u8		DrStrDateDispTypeGet( void )
{
		return	V_DrStrToolDateDispType;
}
//--------------------------------------------------------------------
void	DrStrDateToStrU16le( WCHAR* A_StrRt, __u16 A_Year, __u8 A_Month, __u8 A_Day ,__u8 A_GapDate )
{
	// local -------------------
		WCHAR*	Tv_StrRt;
	// code --------------------

		DrStrClear( A_StrRt );
		Tv_StrRt	=	A_StrRt;
		switch( V_DrStrToolDateDispType )
		{
			case	E_DrStrDateDispMmDdYy:
				DrStrIntToStrU16le( Tv_StrRt, (__s32)A_Month );
				Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
				*Tv_StrRt	=	(WCHAR)'/';		//insert divider
				Tv_StrRt ++;
				*Tv_StrRt	=	0;
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
				DrStrIntToStrU16le( Tv_StrRt, (__s32)A_Day );
				Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
				*Tv_StrRt	=	(WCHAR)'/';		//insert divider
				Tv_StrRt ++;
				*Tv_StrRt	=	0;
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
				DrStrIntToStrU16le( Tv_StrRt, (__s32)A_Year );
				Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
				break;
			case	E_DrStrDateDispDdMmYy:
				DrStrIntToStrU16le( Tv_StrRt, (__s32)A_Day );
				Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
				*Tv_StrRt	=	(WCHAR)'/';		//insert divider
				Tv_StrRt ++;
				*Tv_StrRt	=	0;
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
				DrStrIntToStrU16le( Tv_StrRt, (__s32)A_Month );
				Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
				*Tv_StrRt	=	(WCHAR)'/';		//insert divider
				Tv_StrRt ++;
				*Tv_StrRt	=	0;
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
				DrStrIntToStrU16le( Tv_StrRt, (__s32)A_Year );
				Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
				break;
			default:
				DrStrIntToStrU16le( Tv_StrRt, (__s32)A_Year );
				Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
				*Tv_StrRt	=	(WCHAR)'/';		//insert divider
				Tv_StrRt ++;
				*Tv_StrRt	=	0;
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
				DrStrIntToStrU16le( Tv_StrRt, (__s32)A_Month );
				Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
				*Tv_StrRt	=	(WCHAR)'/';		//insert divider
				Tv_StrRt ++;
				*Tv_StrRt	=	0;
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
				DrStrIntToStrU16le( Tv_StrRt, (__s32)A_Day );
				Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
				break;
		}
}
//--------------------------------------------------------------------
void	DrStrTimeToStrU16le( WCHAR* A_StrRt, Def_DrTime A_Time, BOOL8 A_IsNoTime, BOOL8 A_IsNoDate,
											BOOL8 A_IsNoSec, __u8 A_GapDate, __u8 A_GapTime, __u8 A_DivGap, BOOL8 A_IsDivider,
											BOOL8 A_IsFix, BOOL8 A_IsApm, WCHAR* A_StrAm, WCHAR* A_StrPm, 
	WCHAR A_CharDivDate , WCHAR A_CharDivTime)
{
	// local -------------------
		__u16	Tv_Year;
		__u8	Tv_Mon;
		__u8	Tv_Day;
		__u8	Tv_Hour;
		__u8	Tv_Min;
		__u8	Tv_Sec;
		WCHAR*	Tv_StrRt;
	// code --------------------
		DrStrClear( A_StrRt );
		Tv_StrRt	=	A_StrRt;
		DrTimeDec( A_Time, &Tv_Year, &Tv_Mon, &Tv_Day, &Tv_Hour, &Tv_Min, &Tv_Sec );
		if ( !A_IsNoDate )
		{
			switch( V_DrStrToolDateDispType )
			{
				case	E_DrStrDateDispMmDdYy:
					DrStrIntToStrU16le( Tv_StrRt, (__s32)Tv_Mon, 2, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
					Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
					if ( A_IsDivider )
					{
						Tv_StrRt = DrStrAddWchar(Tv_StrRt, A_CharDivDate);
						Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
					}
					DrStrIntToStrU16le( Tv_StrRt, (__s32)Tv_Day, 2, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
					Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
					if ( A_IsDivider )
					{
						Tv_StrRt = DrStrAddWchar(Tv_StrRt, A_CharDivDate);
						Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
					}
					DrStrIntToStrU16le( Tv_StrRt, (__s32)Tv_Year, 4, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
					break;
				case	E_DrStrDateDispDdMmYy:
					DrStrIntToStrU16le( Tv_StrRt, (__s32)Tv_Day, 2, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
					Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
					if ( A_IsDivider )
					{
						Tv_StrRt = DrStrAddWchar(Tv_StrRt, A_CharDivDate);
						Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
					}
					DrStrIntToStrU16le( Tv_StrRt, (__s32)Tv_Mon, 2, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
					Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
					if ( A_IsDivider )
					{
						Tv_StrRt = DrStrAddWchar(Tv_StrRt, A_CharDivDate);
						Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
					}
					DrStrIntToStrU16le( Tv_StrRt, (__s32)Tv_Year, 4, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
					break;
				default:
					DrStrIntToStrU16le( Tv_StrRt, (__s32)Tv_Year, 4, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
					Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
					if ( A_IsDivider )
					{
						Tv_StrRt = DrStrAddWchar(Tv_StrRt, A_CharDivDate);
						Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
					}
					DrStrIntToStrU16le( Tv_StrRt, (__s32)Tv_Mon, 2, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
					Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
					if ( A_IsDivider )
					{
						Tv_StrRt = DrStrAddWchar(Tv_StrRt, A_CharDivDate);
						Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapDate );
					}
					DrStrIntToStrU16le( Tv_StrRt, (__s32)Tv_Day, 2, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
					break;
			}
			//insert gap
			if ( !A_IsNoTime )
			{
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_DivGap );
			}
		}
		//time
		if ( !A_IsNoTime )
		{

			if(A_IsApm)
			{
				// APM mark
				if(12 <= Tv_Hour)
				{
					if(NULL == A_StrPm)
					{
						Tv_StrRt	=	DrStrAddWchar(Tv_StrRt, 0x50);
						Tv_StrRt	=	DrStrAddWchar(Tv_StrRt, 0x4D);
					}
					else
					{
						Tv_StrRt	=	DrStrCopyU16(Tv_StrRt, A_StrPm);
					}
				}
				else
				{
					if(NULL == A_StrPm)
					{
						Tv_StrRt	=	DrStrAddWchar(Tv_StrRt, 0x41);
						Tv_StrRt	=	DrStrAddWchar(Tv_StrRt, 0x4D);
					}
					else
					{
						Tv_StrRt	=	DrStrCopyU16(Tv_StrRt, A_StrAm);
					}
				}

				if(0 != A_GapTime)
				{
					Tv_StrRt	=	DrStrAddWchar(Tv_StrRt, 0x20);
					if(1 < A_GapTime)
					{
						Tv_StrRt	=	DrStrFillSpaceU16le(Tv_StrRt, A_GapTime - 1);
					}
				}

				// hour
				if(0 == Tv_Hour)
				{
					Tv_Hour		=	12;
				}
				else if(12 <= Tv_Hour)
				{
					if(12 < Tv_Hour)
					{
						Tv_Hour	=	Tv_Hour - 12;
					}
				}
			}

			DrStrIntToStrU16le( Tv_StrRt, (__s32)Tv_Hour, 2, A_IsFix );
			Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
			Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapTime );
			if ( A_IsDivider )
			{
				Tv_StrRt = DrStrAddWchar(Tv_StrRt, A_CharDivTime);
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapTime );
			}
			DrStrIntToStrU16le( Tv_StrRt, (__s32)Tv_Min, 2, A_IsFix );
			if ( !A_IsNoSec )
			{
				Tv_StrRt	=	Tv_StrRt + DrStrLenU16( Tv_StrRt );
				Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapTime );
				if ( A_IsDivider )
				{
					Tv_StrRt = DrStrAddWchar(Tv_StrRt, A_CharDivTime);
					Tv_StrRt	=	DrStrFillSpaceU16le( Tv_StrRt, A_GapTime );
				}
				DrStrIntToStrU16le( Tv_StrRt, (__s32)Tv_Sec, 2, A_IsFix );
			}

		}

}
//--------------------------------------------------------------------
void		DrStrTimeToStrAsc( char* A_StrRt, Def_DrTime A_Time, BOOL8 A_IsNoTime, BOOL8 A_IsNoDate, BOOL8 A_IsNoSec,
												__u8 A_GapDate, __u8 A_GapTime, __u8 A_DivGap, BOOL8 A_IsDivider, BOOL8 A_IsFix,
												BOOL8 A_IsApm, char* A_StrAm, char* A_StrPm, char A_CharDivDate, char A_CharDivTime)
{
	// local -------------------
		__u16	Tv_Year;
		__u8	Tv_Mon;
		__u8	Tv_Day;
		__u8	Tv_Hour;
		__u8	Tv_Min;
		__u8	Tv_Sec;
		char*	Tv_StrRt;
	// code --------------------
		DrStrClear( A_StrRt );
		Tv_StrRt	=	A_StrRt;
		DrTimeDec( A_Time, &Tv_Year, &Tv_Mon, &Tv_Day, &Tv_Hour, &Tv_Min, &Tv_Sec );
		if ( !A_IsNoDate )
		{
			switch( V_DrStrToolDateDispType )
			{
				case	E_DrStrDateDispMmDdYy:
					DrStrIntToStrAsc( Tv_StrRt, (__s32)Tv_Mon, 2, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenAsc( Tv_StrRt );
					Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapDate );
					if ( A_IsDivider )
					{
						Tv_StrRt = DrStrAddChar(Tv_StrRt, A_CharDivDate);
						Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapDate );
					}
					DrStrIntToStrAsc( Tv_StrRt, (__s32)Tv_Day, 2, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenAsc( Tv_StrRt );
					Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapDate );
					if ( A_IsDivider )
					{
						Tv_StrRt = DrStrAddChar(Tv_StrRt, A_CharDivDate);
						Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapDate );
					}
					DrStrIntToStrAsc( Tv_StrRt, (__s32)Tv_Year, 4, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenAsc( Tv_StrRt );
					break;
				case	E_DrStrDateDispDdMmYy:
					DrStrIntToStrAsc( Tv_StrRt, (__s32)Tv_Day, 2, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenAsc( Tv_StrRt );
					Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapDate );
					if ( A_IsDivider )
					{
						Tv_StrRt = DrStrAddChar(Tv_StrRt, A_CharDivDate);
						Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapDate );
					}
					DrStrIntToStrAsc( Tv_StrRt, (__s32)Tv_Mon, 2, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenAsc( Tv_StrRt );
					Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapDate );
					if ( A_IsDivider )
					{
						Tv_StrRt = DrStrAddChar(Tv_StrRt, A_CharDivDate);
						Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapDate );
					}
					DrStrIntToStrAsc( Tv_StrRt, (__s32)Tv_Year, 4, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenAsc( Tv_StrRt );
					break;
				default:
					DrStrIntToStrAsc( Tv_StrRt, (__s32)Tv_Year, 4, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenAsc( Tv_StrRt );
					Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapDate );
					if ( A_IsDivider )
					{
						Tv_StrRt = DrStrAddChar(Tv_StrRt, A_CharDivDate);
						Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapDate );
					}
					DrStrIntToStrAsc( Tv_StrRt, (__s32)Tv_Mon, 2, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenAsc( Tv_StrRt );
					Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapDate );
					if ( A_IsDivider )
					{
						Tv_StrRt = DrStrAddChar(Tv_StrRt, A_CharDivDate);
						Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapDate );
					}
					DrStrIntToStrAsc( Tv_StrRt, (__s32)Tv_Day, 2, A_IsFix );
					Tv_StrRt	=	Tv_StrRt + DrStrLenAsc( Tv_StrRt );
					break;
			}
			//insert gap
			if ( !A_IsNoTime )
			{
				Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_DivGap );
			}
		}
		//time
		if ( !A_IsNoTime )
		{

			if(A_IsApm)
			{
				// APM mark
				if(12 <= Tv_Hour)
				{
					if(NULL == A_StrPm)
					{
						Tv_StrRt	=	DrStrCopyAsc(Tv_StrRt, "PM");
					}
					else
					{
						Tv_StrRt	=	DrStrCopyAsc(Tv_StrRt, A_StrPm);
					}
				}
				else
				{
					if(NULL == A_StrPm)
					{
						Tv_StrRt	=	DrStrCopyAsc(Tv_StrRt, "AM");
					}
					else
					{
						Tv_StrRt	=	DrStrCopyAsc(Tv_StrRt, A_StrAm);
					}
				}

				Tv_StrRt	=	DrStrFillSpaceAsc(Tv_StrRt, A_GapTime);

				// hour
				if(0 == Tv_Hour)
				{
					Tv_Hour		=	12;
				}
				else if(12 <= Tv_Hour)
				{
					if(12 < Tv_Hour)
					{
						Tv_Hour	=	Tv_Hour - 12;
					}
				}
			}

			DrStrIntToStrAsc( Tv_StrRt, (__s32)Tv_Hour, 2, A_IsFix );
			Tv_StrRt	=	Tv_StrRt + DrStrLenAsc( Tv_StrRt );
			Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapTime );
			if ( A_IsDivider )
			{
				Tv_StrRt = DrStrAddChar(Tv_StrRt, A_CharDivTime);
				Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapTime );
			}
			DrStrIntToStrAsc( Tv_StrRt, (__s32)Tv_Min, 2, A_IsFix );
			if ( !A_IsNoSec )
			{
				Tv_StrRt	=	Tv_StrRt + DrStrLenAsc( Tv_StrRt );
				Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapTime );
				if ( A_IsDivider )
				{
					Tv_StrRt = DrStrAddChar(Tv_StrRt, A_CharDivTime);
					Tv_StrRt	=	DrStrFillSpaceAsc( Tv_StrRt, A_GapTime );
				}
				DrStrIntToStrAsc( Tv_StrRt, (__s32)Tv_Sec, 2, A_IsFix );
			}

		}

}
//--------------------------------------------------------------------
Def_DrTime	DrStrStrToTimeU16le(WCHAR* A_Str)
{
	// local -------------------
		Def_DrTime	Tv_Result;
		__s32	Tv_ValTbl[6];
		__u32	Tv_ValIdx;

		WCHAR*	Tv_StrWk;
		__u32	Tv_Len;

		BOOL8	Tv_IsBad;
	// code --------------------
		Tv_Result		=	0;

		Tv_ValIdx		=	0;

		Tv_IsBad		=	FALSE;

		Tv_ValTbl[0]	=	0;
		Tv_ValTbl[1]	=	0;
		Tv_ValTbl[2]	=	0;
		Tv_ValTbl[3]	=	0;
		Tv_ValTbl[4]	=	0;
		Tv_ValTbl[5]	=	0;

		
		// find first number
		Tv_StrWk	=	DrStrSkipSpaceU16le(A_Str);
		while ((NULL != Tv_StrWk) && (Tv_ValIdx < 6))
		{
			// check number
			Tv_Len	=	DrStrGetDecNumU16le(Tv_StrWk);
			if(0 == Tv_Len)
			{
				break;
			}
			// convert value
			if(!DrStrStrToIntU16le(Tv_StrWk, &Tv_ValTbl[Tv_ValIdx], Tv_Len))
			{
				break;
			}
			// go next
			Tv_StrWk	=	Tv_StrWk + Tv_Len;
			// skip space
			Tv_StrWk	=	DrStrSkipSpaceU16le(Tv_StrWk);
			if(NULL == Tv_StrWk)
			{
				break;
			}
			// get delimiter
			if(L'/' == (*Tv_StrWk))
			{
				// check date index
				if(3 <= Tv_ValIdx)
				{
					//bad value
					Tv_IsBad	=	TRUE;
					break;
				}
				// go next
				Tv_StrWk ++;
				Tv_StrWk	=	DrStrSkipSpaceU16le(Tv_StrWk);
			}
			else if(L':' == (*Tv_StrWk))
			{
				if(0 == Tv_ValIdx)
				{
					// time mode
					Tv_ValTbl[3]	=	Tv_ValTbl[0];
					Tv_ValTbl[0]	=	0;
					Tv_ValIdx			=	3;
				}
				else if(3 > Tv_ValIdx)
				{
					Tv_IsBad	=	TRUE;
					break;
				}
				// go next
				Tv_StrWk ++;
				Tv_StrWk	=	DrStrSkipSpaceU16le(Tv_StrWk);
			}
			// next
			Tv_ValIdx ++;
		}	//	while ((NULL != Tv_StrWk) && (Tv_ValIdx < 6))

		// check bad
		if(!Tv_IsBad)
		{
			// check last index
			if(2 >= Tv_ValIdx)
			{
				if((0 == Tv_ValTbl[0]) || (0 == Tv_ValTbl[1]) || (0 == Tv_ValTbl[2]))
				{
					Tv_IsBad	=	TRUE;
				}
			}
		}

		if(!Tv_IsBad)
		{
			// check time only
			if((0 == Tv_ValTbl[0]) && (0 == Tv_ValTbl[1]) && (0 == Tv_ValTbl[2]))
			{
				// time only mode
				Tv_Result	=	(Def_DrTime)((E_DrTimeSecOfHour * Tv_ValTbl[3]) + (E_DrTimeSecOfMin * Tv_ValTbl[4]) + Tv_ValTbl[5]);
			}
			else
			{
				if((0 != Tv_ValTbl[0]) && (0 != Tv_ValTbl[1]) && (0 != Tv_ValTbl[2]))
				{
					switch(V_DrStrToolDateDispType)
					{
						case	E_DrStrDateDispMmDdYy:
							Tv_Result	=	DrTimeEnc((__u16)Tv_ValTbl[2], (__u8)Tv_ValTbl[0], (__u8)Tv_ValTbl[1], (__u8)Tv_ValTbl[3], (__u8)Tv_ValTbl[4], (__u8)Tv_ValTbl[5]);
							break;
						case E_DrStrDateDispDdMmYy:
							Tv_Result	=	DrTimeEnc((__u16)Tv_ValTbl[2], (__u8)Tv_ValTbl[1], (__u8)Tv_ValTbl[0], (__u8)Tv_ValTbl[3], (__u8)Tv_ValTbl[4], (__u8)Tv_ValTbl[5]);
							break;
						default:
							Tv_Result	=	DrTimeEnc((__u16)Tv_ValTbl[0], (__u8)Tv_ValTbl[1], (__u8)Tv_ValTbl[2], (__u8)Tv_ValTbl[3], (__u8)Tv_ValTbl[4], (__u8)Tv_ValTbl[5]);
							break;
					}
				}
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrStrGetDecNumU16le(WCHAR* A_Str)
{
	// local -------------------
		__u32		Tv_Result;
	// code --------------------
		Tv_Result	=	0;

		while( (0x30 <= A_Str[Tv_Result]) && (0x39 >= A_Str[Tv_Result]))
		{
			Tv_Result ++;
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
WCHAR*	DrStrSkipSpaceU16le(WCHAR* A_Str)
{
	// local -------------------
		WCHAR*	Tv_Result;
	// code --------------------
		Tv_Result	=	A_Str;

		while(0 != (*Tv_Result))
		{
			if((9 != (*Tv_Result)) && (0x20 != (*Tv_Result)))
			{
				break;
			}
			// go next
			Tv_Result ++;
		}

		// check finish
		if(0 == (*Tv_Result))
		{
			Tv_Result	=	NULL;
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
char*		DrStrSkipZeroAsc( char* A_Str )
{
	// local -------------------
		char*	Tv_Result;
	// code --------------------
		Tv_Result		=	A_Str;
		while ( 0 != (*Tv_Result) )
		{
			Tv_Result ++;
		}
		return	Tv_Result;
}
//--------------------------------------------------------------------
WCHAR*	DrStrSkipZeroU16( WCHAR* A_Str )
{
	// local -------------------
		WCHAR*	Tv_Result;
	// code --------------------
		Tv_Result		=	A_Str;
		while ( 0 != (*Tv_Result) )
		{
			Tv_Result ++;
		}
		return	Tv_Result;
}
//--------------------------------------------------------------------
void		DrStrU16leToUtf8( char* A_StrRt, WCHAR* A_StrSrc )
{
	// local -------------------
		__u16*		Tv_PtrSr;
		__u8*		Tv_PtrTg;
		__u16		Tv_Wval;
	// code --------------------
		Tv_PtrSr	=	(__u16*)A_StrSrc;
		Tv_PtrTg	=	(__u8*)A_StrRt;
		while ( 0 != (*Tv_PtrSr) )
		{
			Tv_Wval	=	*Tv_PtrSr;
			if ( 0x7F >= Tv_Wval )
			{
				*Tv_PtrTg	=	(__u8)Tv_Wval;
				Tv_PtrTg ++;
			}
			else if ( 0x7FF >= Tv_Wval )
			{
				*Tv_PtrTg	=	0xC0 | (__u8)( 0x1F & (Tv_Wval >> 6) );
				Tv_PtrTg ++;
				*Tv_PtrTg	=	0x80 | (__u8)( 0x3F & Tv_Wval );
				Tv_PtrTg ++;
			}
			else 
			{
				*Tv_PtrTg	=	0xE0 | (__u8)( 0x0F & (Tv_Wval >> 12) );
				Tv_PtrTg ++;
				*Tv_PtrTg	=	0x80 | (__u8)( 0x3F & (Tv_Wval >> 6) );
				Tv_PtrTg ++;
				*Tv_PtrTg	=	0x80 | (__u8)( 0x3F & Tv_Wval );
				Tv_PtrTg ++;
			}

			//next
			Tv_PtrSr ++;
		}
		*Tv_PtrTg	=	0;
}
//--------------------------------------------------------------------
void		DrStrChgSenseLowAsc( char* A_Str )
{
	// local -------------------
		char*	Tv_StrWk;
	// code --------------------
		Tv_StrWk	=	A_Str;
		while ( 0 != (*Tv_StrWk) )
		{
			//check case
			if ( ('A' <= (*Tv_StrWk)) && ('Z' >= (*Tv_StrWk)) )
			{
				//change
				*Tv_StrWk	=	(*Tv_StrWk) + 0x20;
			}
			//next
			Tv_StrWk ++;
		}
}
//--------------------------------------------------------------------
char*		DrStrFindAsc( char* A_StrDst, char* A_StrFind )
{
	// local -------------------
		char*	Tv_Result;
		char*	Tv_PtrPrc;
		char*	Tv_PtrCmpSr;
		char*	Tv_PtrCmpTg;
	// code --------------------
		Tv_Result	=	NULL;

		Tv_PtrPrc	=	A_StrDst;
		while ( 0 != (*Tv_PtrPrc) )
		{
			//compare
			if ( (*Tv_PtrPrc) == A_StrFind[0] )
			{
				Tv_PtrCmpSr	=	A_StrFind;
				Tv_PtrCmpTg	=	Tv_PtrPrc;
				while ( TRUE )
				{
					//check source zero
					if ( 0 == (*Tv_PtrCmpSr) )
					{
						//found
						Tv_Result	=	Tv_PtrPrc;
						break;
					}
					//check target zero
					if ( 0 == (*Tv_PtrCmpTg) )
					{
						//target already finish
						break;
					}
					//check change
					if ( (*Tv_PtrCmpSr) != (*Tv_PtrCmpTg) )
					{
						//diffrent string
						break;
					}
					//next
					Tv_PtrCmpSr ++;
					Tv_PtrCmpTg ++;
				}
				//found
				if ( NULL != Tv_Result )
				{
					break;
				}
			}
			//next
			Tv_PtrPrc ++;
		}
		return	Tv_Result;
}
//--------------------------------------------------------------------
WCHAR*	DrStrFindU16le( WCHAR* A_StrDst, WCHAR* A_StrFind, BOOL8 A_IsCaseSense )
{
	// local -------------------
		WCHAR*	Tv_Result;
		WCHAR*	Tv_Prgs;
		__s32			Tv_CmpCnt;
		BOOL8		Tv_IsOk;
	// code --------------------
		Tv_Result	=	NULL;

		if ( (NULL != A_StrDst) && (NULL != A_StrFind) )
		{
			if ( 0 != (*A_StrFind) )
			{
				Tv_Prgs	=	A_StrDst;
				while ( 0 != (*Tv_Prgs) )
				{
					// check compare
					Tv_CmpCnt	=	0;
					while ( TRUE )
					{
						// check finish
						if ( 0 == A_StrFind[Tv_CmpCnt] )
						{
							// found
							Tv_Result	=	Tv_Prgs;
							break;
						}
						// compare text
						if ( Tv_Prgs[Tv_CmpCnt] != A_StrFind[Tv_CmpCnt] )
						{
							if ( A_IsCaseSense )
							{
								break;
							}
							else
							{
								Tv_IsOk	=	FALSE;
								if ( (0x41 <= A_StrFind[Tv_CmpCnt]) && (0x5A >= A_StrFind[Tv_CmpCnt]) )
								{
									if ( (A_StrFind[Tv_CmpCnt] + 0x20) == Tv_Prgs[Tv_CmpCnt] )
									{
										Tv_IsOk	=	TRUE;
									}
								}
								else if ( (0x61 <= A_StrFind[Tv_CmpCnt]) && (0x7A >= A_StrFind[Tv_CmpCnt]) )
								{
									if ( (A_StrFind[Tv_CmpCnt] - 0x20) == Tv_Prgs[Tv_CmpCnt] )
									{
										Tv_IsOk	=	TRUE;
									}
								}
								if ( !Tv_IsOk )
								{
									break;
								}
							}
						}
						// check finish string
						if ( 0 == Tv_Prgs[Tv_CmpCnt] )
						{
							break;
						}
						// next
						Tv_CmpCnt ++;
					}
					// check finish
					if ( NULL != Tv_Result )
					{
						break;
					}
					// next
					Tv_Prgs ++;
				}
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u32	DrStrWcharCnt( WCHAR* A_Str, WCHAR A_Char, __u32 A_LenLim )
{
	// local -------------------
		__u32	Tv_Result;
		WCHAR*	Tv_PtrStr;
		__u32	Tv_Limite;
	// code --------------------
		Tv_Result	=	0;
		Tv_PtrStr	=	A_Str;
		Tv_Limite	=	A_LenLim;
		if ( 0 == Tv_Limite )
		{
			Tv_Limite	=	0xFFFFFFFF;
		}

		while ( 0 != (*Tv_PtrStr) )
		{
			if ( A_Char == (*Tv_PtrStr) )
			{
				Tv_Result ++;
			}
			// next
			Tv_PtrStr ++;
			Tv_Limite --;
			if ( 0 == Tv_Limite )
			{
				break;
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
__u8	DrStrRulePasswdU16le( WCHAR* A_Str, __u32 A_MinCnt )
{
	// local -------------------
		__u8	Tv_Result;
		__u8	Tv_Match;
		__u8	Tv_Type;
		BOOL8	Tv_IsOk;
		__u32	Tv_PassLen;
		__u32	Tv_WkIdx;
		WCHAR	Tv_LastChar;
		__u32	Tv_ChkIdx;
	// code --------------------
		// init
		Tv_Result	=	E_DrStrPasswdGood;		

		Tv_PassLen	=	(__u32)DrStrLenU16( A_Str );

		// check bad text
		if ( (E_DrStrPasswdGood == Tv_Result) && (0 != Tv_PassLen) )
		{
			for ( Tv_WkIdx=0;Tv_WkIdx < Tv_PassLen;Tv_WkIdx++ )
			{
				if ( (L'A' > A_Str[Tv_WkIdx]) || (L'Z' < A_Str[Tv_WkIdx]) )
				{
					if ( (L'a' > A_Str[Tv_WkIdx]) || (L'z' < A_Str[Tv_WkIdx]) )
					{
						if ( (L'0' > A_Str[Tv_WkIdx]) || (L'9' < A_Str[Tv_WkIdx]) )
						{
							Tv_IsOk	=	FALSE;
							for (Tv_ChkIdx=0;Tv_ChkIdx < E_DrStrToolPassSpcCnt;Tv_ChkIdx++)
							{
								if ( V_DrStrPassSpcU16leTbl[Tv_ChkIdx] == A_Str[Tv_WkIdx] )
								{
									Tv_IsOk	=	TRUE;
									break;
								}
							}
							
							if ( !Tv_IsOk )
							{
								Tv_Result	=	E_DrStrPasswdBadChar;
								break;
							}
						}
					}
				}
			}

		}

		// check simple type
		if ( (E_DrStrPasswdGood == Tv_Result) && (A_MinCnt <= Tv_PassLen) )
		{
			Tv_Match	=	0;
			Tv_Type		=	0;
			for ( Tv_WkIdx=0;Tv_WkIdx < Tv_PassLen;Tv_WkIdx++ )
			{
				if ( (L'A' <= A_Str[Tv_WkIdx]) && (L'Z' >= A_Str[Tv_WkIdx]) )
				{
					if ( 0 == (0x03 & Tv_Type) )
					{
						Tv_Type	=	Tv_Type | 0x01;
						Tv_Match ++;
					}
				}
				else if ( (L'a' <= A_Str[Tv_WkIdx]) && (L'z' >= A_Str[Tv_WkIdx]) )
				{
					if ( 0 == (0x03 & Tv_Type) )
					{
						Tv_Type	=	Tv_Type | 0x02;
						Tv_Match ++;
					}
				}
				else if ( (L'0' <= A_Str[Tv_WkIdx]) && (L'9' >= A_Str[Tv_WkIdx]) )
				{
					if ( 0 == (0x04 & Tv_Type) )
					{
						Tv_Type	=	Tv_Type | 0x04;
						Tv_Match ++;
					}
				}
				else
				{
					if ( 0 == (0x08 & Tv_Type) )
					{
						Tv_Type	=	Tv_Type | 0x08;
						Tv_Match ++;
					}
				}
			}	//	for ( Tv_WkIdx=0;Tv_WkIdx < Tv_PassLen;Tv_WkIdx++ )
			// check result
			if ( 3 > Tv_Match )
			{
				Tv_Result	=	E_DrStrPasswdSimple;
			}
		}

		// check equal text
		if ( (E_DrStrPasswdGood == Tv_Result) && (4 <= Tv_PassLen) )
		{
			for (Tv_WkIdx=0;Tv_WkIdx < (Tv_PassLen - 3);Tv_WkIdx++) 
			{
				if ( 4 <= DrStrWcharCnt( A_Str, A_Str[Tv_WkIdx] ) )
				{
					Tv_Result	=	E_DrStrPasswdSameChar;
					break;
				}
			}
		}

		// check series 
		if ( (E_DrStrPasswdGood == Tv_Result) && (4 <= Tv_PassLen) )
		{
			Tv_Match		=	0;
			Tv_Type			=	0;
			Tv_LastChar		=	A_Str[0];
			for ( Tv_WkIdx=1;Tv_WkIdx < Tv_PassLen;Tv_WkIdx++ )
			{
				if ( (Tv_LastChar + 1) == A_Str[Tv_WkIdx] )
				{
					if ( 1 != Tv_Type )
					{
						Tv_Match	=	0;
						Tv_Type		=	1;
					}
					// series
					Tv_Match ++;
				}
				else if ( (Tv_LastChar - 1) == A_Str[Tv_WkIdx] )
				{
					if ( 2 != Tv_Type )
					{
						Tv_Match	=	0;
						Tv_Type		=	2;
					}
					// series
					Tv_Match ++;
				}
				else
				{
					Tv_LastChar	=	A_Str[Tv_WkIdx];
					Tv_Match	=	0;
					Tv_Type		=	0;
				}
				// update last char
				Tv_LastChar	=	A_Str[Tv_WkIdx];
				// check found
				if ( 3 <= Tv_Match )
				{
					// found
					Tv_Result	=	E_DrStrPasswdSeries;
					break;
				}
			}	//	for ( Tv_WkIdx=0;Tv_WkIdx < Tv_PassLen;Tv_WkIdx++ )
		}

		// check length
		if ( (E_DrStrPasswdGood == Tv_Result) && (A_MinCnt > Tv_PassLen) )
		{
			Tv_Result	=	E_DrStrPasswdShort;
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrStrStripToIpV4Asc( char* A_Str, void* A_PtrRt )
{
	BOOL8		Tv_Result;

	Tv_Result	=	TRUE;

#if 0
	*((unsigned long*)A_PtrRt)	=	inet_addr(A_Str);
	if (0xFFFFFFFF == *((unsigned long*)A_PtrRt))
	{
		Tv_Result	=	FALSE;
	}
#endif

	if (0 == inet_pton(AF_INET, A_Str, A_PtrRt))
	{
		Tv_Result	=	FALSE;
	}
	
	return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrStrStripToIpV4U16le(WCHAR* A_Str, void* A_PtrRt)
{
	// local -------------------
		BOOL8	Tv_Result;
		__u8	Tv_DotCnt;
		__u32	Tv_NumCnt;
		WCHAR*	Tv_StrPrs;
		__s32	Tv_ValTbl[4];
		__u32	Tv_Add;
		__u8*	Tv_PtrBt;
	// code --------------------
		Tv_Result		=	FALSE;
		if(DrStrIsExist(A_Str))
		{
			Tv_Result			=	TRUE;
			Tv_DotCnt			=	0;
			Tv_NumCnt			=	0;
			Tv_ValTbl[0]	=	-1;
			Tv_ValTbl[1]	=	-1;
			Tv_ValTbl[2]	=	-1;
			Tv_ValTbl[3]	=	-1;

			Tv_StrPrs	=	A_Str;
			while(0 != *Tv_StrPrs)
			{
				// check string number
				if((0x30 <= *Tv_StrPrs) && (0x39 >= *Tv_StrPrs))
				{
					if(0 > Tv_ValTbl[Tv_DotCnt])
					{
						Tv_ValTbl[Tv_DotCnt]	= 0;
					}
					// number
					if(3 <= Tv_NumCnt)
					{
						Tv_Result	=	FALSE;
						break;
					}
					// value set
					Tv_Add	=	(__u32)(*Tv_StrPrs - 0x30);
					Tv_ValTbl[Tv_DotCnt]	=	(Tv_ValTbl[Tv_DotCnt] * 10) + Tv_Add;
					Tv_NumCnt ++;
				}
				else if(0x2E == *Tv_StrPrs)
				{
					// dot
					if(3 <= Tv_DotCnt)
					{
						Tv_Result	=	FALSE;
						break;
					}
					Tv_DotCnt ++;
					Tv_NumCnt	=	0;
				}
				else
				{
					// bad character
					Tv_Result	=	FALSE;
					break;
				}
				Tv_StrPrs ++;
			}

			if(3 != Tv_DotCnt)
			{
				Tv_Result	=	FALSE;
			}

			// make ip
			if(Tv_Result)
			{
				Tv_Result	=	FALSE;
				if((0 <= Tv_ValTbl[0]) && (0 <= Tv_ValTbl[1]) && (0 <= Tv_ValTbl[2]) && (0 <= Tv_ValTbl[3]))
				{
					if((256 > Tv_ValTbl[0]) && (256 > Tv_ValTbl[1]) && (256 > Tv_ValTbl[2]) && (256 > Tv_ValTbl[3]))
					{
						Tv_PtrBt	=	(__u8*)A_PtrRt;
						Tv_PtrBt[0]	=	(__u8)Tv_ValTbl[0];
						Tv_PtrBt[1]	=	(__u8)Tv_ValTbl[1];
						Tv_PtrBt[2]	=	(__u8)Tv_ValTbl[2];
						Tv_PtrBt[3]	=	(__u8)Tv_ValTbl[3];

						Tv_Result	=	TRUE;
					}
				}
			}
		}

		return	Tv_Result;

}
//--------------------------------------------------------------------
BOOL8		DrStrIsExist(char* A_Str)
{
	// local -------------------
		BOOL8	Tv_Result;
	// code --------------------
		Tv_Result	=	FALSE;
		if(NULL != A_Str)
		{
			if(0 != A_Str[0])
			{
				Tv_Result	=	TRUE;
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
BOOL8		DrStrIsExist(WCHAR* A_Str)
{
	// local -------------------
		BOOL8	Tv_Result;
	// code --------------------
		Tv_Result	=	FALSE;
		if(NULL != A_Str)
		{
			if(0 != A_Str[0])
			{
				Tv_Result	=	TRUE;
			}
		}

		return	Tv_Result;
}
//--------------------------------------------------------------------
char*		DrStrMacToStrAsc(char* A_StrRt, __u8* A_PtrMac, BOOL8 A_IsUpper, char A_Separater)
{
	// local -------------------
		char*	Tv_Result;
		__u32	Tv_WkIdx;
		__u8	Tv_Val;
	// code --------------------
		Tv_Result	=	A_StrRt;

		for( Tv_WkIdx = 0; Tv_WkIdx < 6; Tv_WkIdx++)
		{
			Tv_Val	=	(A_PtrMac[Tv_WkIdx] >> 4) & 0x0F;

			if(9 >= Tv_Val)
			{
				*Tv_Result	=	(char)(0x30 + Tv_Val);
			}
			else
			{
				if(A_IsUpper)
				{
					*Tv_Result	=	(char)(0x41 + Tv_Val - 10);
				}
				else
				{
					*Tv_Result	=	(char)(0x61 + Tv_Val - 10);
				}
			}
			Tv_Result ++;

			Tv_Val	=	A_PtrMac[Tv_WkIdx] & 0x0F;

			if(9 >= Tv_Val)
			{
				*Tv_Result	=	(char)(0x30 + Tv_Val);
			}
			else
			{
				if(A_IsUpper)
				{
					*Tv_Result	=	(char)(0x41 + Tv_Val - 10);
				}
				else
				{
					*Tv_Result	=	(char)(0x61 + Tv_Val - 10);
				}
			}
			Tv_Result ++;

			if((0 != A_Separater) && (5 != Tv_WkIdx))
			{
				*Tv_Result	=	A_Separater;
				Tv_Result ++;
			}
		}

		*Tv_Result	=	0;
		Tv_Result ++;

		return	Tv_Result;
}
//--------------------------------------------------------------------
WCHAR*	DrStrMacToStrU16le(WCHAR* A_StrRt, __u8* A_PtrMac, BOOL8 A_IsUpper, WCHAR A_Separater)
{
	// local -------------------
		WCHAR*	Tv_Result;
		__u32	Tv_WkIdx;
		__u8	Tv_Val;
	// code --------------------
		Tv_Result	=	A_StrRt;

		for(Tv_WkIdx = 0; Tv_WkIdx < 6; Tv_WkIdx++)
		{
			Tv_Val	=	(A_PtrMac[Tv_WkIdx] >> 4) & 0x0F;

			if(9 >= Tv_Val)
			{
				*Tv_Result	=	(WCHAR)(0x30 + Tv_Val);
			}
			else
			{
				if(A_IsUpper)
				{
					*Tv_Result	=	(WCHAR)(0x41 + Tv_Val - 10);
				}
				else
				{
					*Tv_Result	=	(WCHAR)(0x61 + Tv_Val - 10);
				}
			}
			Tv_Result ++;

			Tv_Val	=	A_PtrMac[Tv_WkIdx] & 0x0F;

			if(9 >= Tv_Val)
			{
				*Tv_Result	=	(WCHAR)(0x30 + Tv_Val);
			}
			else
			{
				if(A_IsUpper)
				{
					*Tv_Result	=	(WCHAR)(0x41 + Tv_Val - 10);
				}
				else
				{
					*Tv_Result	=	(WCHAR)(0x61 + Tv_Val - 10);
				}
			}
			Tv_Result ++;

			if((0 != A_Separater) && (5 != Tv_WkIdx))
			{
				*Tv_Result	=	A_Separater;
				Tv_Result ++;
			}
		}

		*Tv_Result	=	0;
		Tv_Result ++;

		return	Tv_Result;
}
//--------------------------------------------------------------------
void				DrStrStrToMacAdrAsc(__u8* A_PtrRt, char* A_StrMac)
{
	// local -------------------
		__u8	Tv_BitPos;
		__u8	Tv_Val;
		__u32	Tv_Cnt;
		char*	Tv_WkStr;
	// code --------------------
		Tv_Cnt		=	0;
		Tv_BitPos	=	0;
		Tv_WkStr	=	A_StrMac;

		// reset result
		A_PtrRt[0]		=	0;
		A_PtrRt[1]		=	0;
		A_PtrRt[2]		=	0;
		A_PtrRt[3]		=	0;
		A_PtrRt[4]		=	0;
		A_PtrRt[5]		=	0;

		while(6 > Tv_Cnt)
		{
			// check value
			if(0 == (*Tv_WkStr) )
			{
				break;
			}

			if((0x30 <= (*Tv_WkStr)) && (0x39 >= (*Tv_WkStr)))
			{
				// patch position
				if(8 <= Tv_BitPos)
				{
					Tv_Cnt ++;
					Tv_BitPos	=	0;
				}
				// number
				Tv_Val	=	(__u8)((*Tv_WkStr) - 0x30);
				A_PtrRt[Tv_Cnt]	=	(A_PtrRt[Tv_Cnt] << Tv_BitPos) | Tv_Val;

				Tv_BitPos	=	Tv_BitPos + 4;
			}
			else if((0x41 <= (*Tv_WkStr)) && (0x5A >= (*Tv_WkStr)))
			{
				// patch position
				if(8 <= Tv_BitPos)
				{
					Tv_Cnt ++;
					Tv_BitPos	=	0;
				}
				// down case
				Tv_Val	=	(__u8)((*Tv_WkStr) - 0x41 + 10);
				A_PtrRt[Tv_Cnt]	=	(A_PtrRt[Tv_Cnt] << Tv_BitPos) | Tv_Val;

				Tv_BitPos	=	Tv_BitPos + 4;
			}
			else if((0x61 <= (*Tv_WkStr)) && (0x7A >= (*Tv_WkStr)))
			{
				// patch position
				if(8 <= Tv_BitPos)
				{
					Tv_Cnt ++;
					Tv_BitPos	=	0;
				}
				// upper case
				Tv_Val	=	(__u8)((*Tv_WkStr) - 0x61 + 10);
				A_PtrRt[Tv_Cnt]	=	(A_PtrRt[Tv_Cnt] << Tv_BitPos) | Tv_Val;

				Tv_BitPos	=	Tv_BitPos + 4;
			}
			else
			{
				Tv_Cnt ++;
				Tv_BitPos	=	0;
			}
			// next
			Tv_WkStr ++;
		}

}
//--------------------------------------------------------------------
void		DrStrStrToMacAdrU16le(__u8* A_PtrRt, WCHAR* A_StrMac)
{
	// local -------------------
		__u8	Tv_BitPos;
		__u8	Tv_Val;
		__u32	Tv_Cnt;
		WCHAR*	Tv_WkStr;
	// code --------------------
		Tv_Cnt		=	0;
		Tv_BitPos	=	0;
		Tv_WkStr	=	A_StrMac;

		// reset result
		A_PtrRt[0]		=	0;
		A_PtrRt[1]		=	0;
		A_PtrRt[2]		=	0;
		A_PtrRt[3]		=	0;
		A_PtrRt[4]		=	0;
		A_PtrRt[5]		=	0;

		while(6 > Tv_Cnt)
		{
			// check value
			if(0 == (*Tv_WkStr))
			{
				break;
			}

			if((0x30 <= (*Tv_WkStr)) && (0x39 >= (*Tv_WkStr)))
			{
				// patch position
				if(8 <= Tv_BitPos)
				{
					Tv_Cnt ++;
					Tv_BitPos	=	0;
				}
				// number
				Tv_Val	=	(__u8)((*Tv_WkStr) - 0x30);
				A_PtrRt[Tv_Cnt]	=	(A_PtrRt[Tv_Cnt] << Tv_BitPos) | Tv_Val;

				Tv_BitPos	=	Tv_BitPos + 4;
			}
			else if((0x41 <= (*Tv_WkStr)) && (0x5A >= (*Tv_WkStr)))
			{
				// patch position
				if(8 <= Tv_BitPos)
				{
					Tv_Cnt ++;
					Tv_BitPos	=	0;
				}
				// down case
				Tv_Val	=	(__u8)((*Tv_WkStr) - 0x41 + 10);
				A_PtrRt[Tv_Cnt]	=	(A_PtrRt[Tv_Cnt] << Tv_BitPos) | Tv_Val;

				Tv_BitPos	=	Tv_BitPos + 4;
			}
			else if((0x61 <= (*Tv_WkStr)) && (0x7A >= (*Tv_WkStr)))
			{
				// patch position
				if(8 <= Tv_BitPos)
				{
					Tv_Cnt ++;
					Tv_BitPos	=	0;
				}
				// upper case
				Tv_Val	=	(__u8)((*Tv_WkStr) - 0x61 + 10);
				A_PtrRt[Tv_Cnt]	=	(A_PtrRt[Tv_Cnt] << Tv_BitPos) | Tv_Val;

				Tv_BitPos	=	Tv_BitPos + 4;
			}
			else
			{
				Tv_Cnt ++;
				Tv_BitPos	=	0;
			}
			// next
			Tv_WkStr ++;
		}

}
//--------------------------------------------------------------------
BOOL8 DrStrSubPtnMchU16le(Ptr_DrStrPtnMchU16 A_PtrCtl, __u32 A_PrgsSrc, __u32 A_PrgsPtn)
{
	__u32 Tv_PrgsSrc;
	__u32 Tv_PrgsPtn;
	BOOL8	Tv_IsWild;
	BOOL8 Tv_IsMtc;
	WCHAR Tv_WcSr;
	WCHAR Tv_WcTg;

	Tv_IsWild = FALSE;
	// match char
	Tv_PrgsSrc = A_PrgsSrc;
	Tv_PrgsPtn = A_PrgsPtn;

	while(Tv_PrgsPtn < A_PtrCtl->LenPtn)
	{
		// compare
		if(0x2A == A_PtrCtl->StrPtn[Tv_PrgsPtn])
		{
			// wildcard
			Tv_PrgsPtn ++;
			Tv_IsWild = TRUE;
		}
		else
		{
#if 0
			if ((Tv_PrgsSrc + 1) >= A_PtrCtl->LenSrc)
			{
				return FALSE;
			}
#endif

			if (Tv_IsWild)
			{
				while (Tv_PrgsSrc < A_PtrCtl->LenSrc)
				{
					if (DrStrSubPtnMchU16le(A_PtrCtl, Tv_PrgsSrc, Tv_PrgsPtn))
					{
						return TRUE;
					}
					// next
					Tv_PrgsSrc++;
				}
				return FALSE;
			}

			// normal work
			if (0x3F == A_PtrCtl->StrPtn[Tv_PrgsPtn])
			{
				// question mark
				Tv_PrgsSrc++;
				Tv_PrgsPtn++;
				continue;
			}

			// noraml text
			if(A_PtrCtl->StrSrc[Tv_PrgsSrc] != A_PtrCtl->StrPtn[Tv_PrgsPtn])
			{
				Tv_IsMtc = FALSE;
				if(!A_PtrCtl->IsCaseSense)
				{
					Tv_WcSr = A_PtrCtl->StrSrc[Tv_PrgsSrc];
					Tv_WcTg = A_PtrCtl->StrPtn[Tv_PrgsPtn];
					if((0x41 <= Tv_WcSr) && (0x5A >= Tv_WcSr))
					{
						Tv_WcSr = Tv_WcSr + 0x20;
					}
					if((0x41 <= Tv_WcTg) && (0x5A >= Tv_WcTg))
					{
						Tv_WcTg = Tv_WcTg + 0x20;
					}
					if(Tv_WcSr == Tv_WcTg)
					{
						Tv_IsMtc = TRUE;
					}
				}
				if(!Tv_IsMtc)
				{
					return FALSE;
				}
			}

			Tv_PrgsSrc++;
			Tv_PrgsPtn++;
		}
	}

	// check wildcard
	if (Tv_IsWild && (Tv_PrgsPtn >= A_PtrCtl->LenPtn))
	{
		return TRUE;
	}

	if ((Tv_PrgsSrc >= A_PtrCtl->LenSrc) && (Tv_PrgsPtn >= A_PtrCtl->LenPtn))
	{
		return TRUE;
	}

	return FALSE;
}
//--------------------------------------------------------------------
BOOL8 DrStrPatternMatchU16le(WCHAR* A_StrSrc, WCHAR* A_StrPtn, BOOL8 A_IsCaseSense)
{
	St_DrStrPtnMchU16 Tv_PtnCtl;

	Tv_PtnCtl.StrSrc = A_StrSrc;
	Tv_PtnCtl.LenSrc = (__u32)DrStrLenU16(A_StrSrc);
	Tv_PtnCtl.StrPtn = A_StrPtn;
	Tv_PtnCtl.LenPtn = (__u32)DrStrLenU16(A_StrPtn);
	Tv_PtnCtl.IsCaseSense = A_IsCaseSense;

	// compare pattern
	return DrStrSubPtnMchU16le(&Tv_PtnCtl,0,0);
}
//--------------------------------------------------------------------


