/*
	Rnd2team - string tools

		uses
			DrTypeBase.h


*/

//--------------------------------------------------------------------
#ifndef	_EPC_DrStrTool
#define	_EPC_DrStrTool

//====================================================================
// uses

#include <DrPlatform.h>
#include <DrTypeBase.h>

//====================================================================
//constance


#define		E_DrStrOutOfString		0xFFFFFFFF
#define		E_DrStrLimitMask			0x7FFFFFF

//pars
#define		E_DrStrPrsOptCharCnt						16

//korean
#define		E_DrStrKorCmbJamoStartU16le			0x3131
#define		E_DrStrKorCmbJamoEndU16le				0x318E

#define		E_DrStrKorCodeStartU16le				0xAC00
#define		E_DrStrKorCodeEndU16le					0xD7A3

#define		E_DrStrKorFirStepU16le					0x24C		/* first step */
#define		E_DrStrKorLastStepU16le					0x1C		/* last step */

#define		E_DrStrKorFirCntU16le						0x13		/* first count */
#define		E_DrStrKorMidCntU16le						0x15		/* middle count */
#define		E_DrStrKorLastCntU16le					0x1C		/* last count */

#define		E_DrStrKorNoMidLastU16le				0x00		/* not exist middle or last */

//language
#define		E_DrStrLngEnglish							0
#define		E_DrStrLngJapan								1
#define		E_DrStrLngKorean							2
#define		E_DrStrLngGerman							3
#define		E_DrStrLngPrance							4
#define		E_DrStrLngRussia							5
#define		E_DrStrLngRussia							5
#define		E_DrStrLngItaly								6
#define		E_DrStrLngPortugal						7
#define		E_DrStrLngTurkey							8
#define		E_DrStrLngPoland							9

//date display type
#define		E_DrStrDateDispYyMmDd					0
#define		E_DrStrDateDispMmDdYy					1
#define		E_DrStrDateDispDdMmYy					2

// survelence
#define	E_DrStrPasswdGood					0
#define E_DrStrPasswdShort				1
#define E_DrStrPasswdSameChar			2
#define E_DrStrPasswdSeries				3
#define E_DrStrPasswdSimple				4
#define E_DrStrPasswdBadChar			5

#define E_DrStrPasswdErrCnt				5

//====================================================================
//global macro
#define		DrStrClear(A_Str)		(A_Str)[0]	=	0
//#define		DrStrIsExist(A_Str)	((BOOL)((A_Str)[0]))

#define		DrStrMaxLen(A_Str)	((sizeof((A_Str))/sizeof((A_Str)[0])) -1)

//====================================================================
//global type

typedef struct St_DrStrParsWordRngAsc
{
	char	St;
	char	Ed;
}	*Ptr_DrStrParsWordRngAsc;

typedef	struct St_DrStrParsCtrlAsc 
{
	char*		StrSrc;				// source string
	__u32		SrcLen;				// source length

	__u32		LineSt;				// line position
	__u32		LineLenCont;	// line length contents
	__u32		LineCrSize;		// line carridge return size

	__u32		WordPrgs;			// word progress
	__u32		WordLen;
}	*Ptr_DrStrParsCtrlAsc;

typedef	struct St_DrStrUniPars
{
	__u32		Last;			//last size
	WCHAR*	Ptr;			//working pointer
	__u32		DivCharCnt;
	WCHAR		DivTbl[E_DrStrPrsOptCharCnt];		//word only
	__u32		DrpCharCnt;
	WCHAR		DrpTbl[E_DrStrPrsOptCharCnt];		//word only
	__u32		IdpCharCnt;										
	WCHAR		IdpTbl[E_DrStrPrsOptCharCnt];		//word only
}	*Ptr_DrStrUniPars;

typedef struct St_DrStrPtnMchU16
{
	WCHAR* StrSrc;
	WCHAR* StrPtn;
	__u32 LenSrc;
	__u32 LenPtn;
	BOOL8 IsCaseSense;
}	*Ptr_DrStrPtnMchU16;

//====================================================================
//global function

BOOL8		DrStrIsExist(char* A_Str);
BOOL8		DrStrIsExist(WCHAR* A_Str);

Def_DrCalPtr		DrStrLenAsc(	const char* A_StrSrc, Def_DrCalPtr A_LenLim = 0 );
Def_DrCalPtr		DrStrLenU16( WCHAR* A_StrSrc, Def_DrCalPtr A_LenLim = 0 );

char*		DrStrCopyAsc( char* A_StrDst,const char* A_StrSrc, Def_DrCalPtr A_LenLim = 0 );		//return = next pointer
WCHAR*	DrStrCopyU16( WCHAR* A_StrDst,const WCHAR* A_StrSrc, Def_DrCalPtr A_LenLim = 0 );	//return = next pointer

__u32		DrStrParseGetWordLenAsc( char* A_Str, __u32 A_LenLim = 0 );

__u32		DrStrParseFindWordStartAsc( __u32 A_Mode, char* A_Str, __u32 A_LenLim = 0 );

__u32		DrStrGetOneLineSizeAsc( char* A_Str, __u32* A_PtrRtCrSize, __u32 A_LenLim = 0 );

__s32		DrStrCmpAsc( const char* A_StrSrc, const char* A_StrDst, BOOL8 A_IsCaseSense, __u32 A_LenLim = 0 );
__s32		DrStrCmpU16le(	WCHAR* A_StrSrc, WCHAR* A_StrDst, BOOL8 A_IsCaseSense, __u32 A_LenLim = 0 );

__u32		DrStrFindChar( char* A_StrDst, char A_FindChar, BOOL8 A_IsMultibyte = FALSE, __u32 A_LenLim = 0 );
__u32		DrStrFindWchar( WCHAR* A_StrDst, WCHAR A_FindChar, __u32 A_LenLim = 0 );

char*		DrStrFindAsc( char* A_StrDst, char* A_StrFind );
WCHAR*	DrStrFindU16le( WCHAR* A_StrDst, WCHAR* A_StrFind, BOOL8 A_IsCaseSense );

__u32		DrStrWcharCnt( WCHAR* A_Str, WCHAR A_Char, __u32 A_LenLim = 0 );	// get charactor count

char*		DrStrCatAsc( char* A_StrDst, char* A_StrSrc, __u32 A_LenLim = 0 );		// return = next pointer
WCHAR*	DrStrCatU16( WCHAR* A_StrDst, WCHAR* A_StrSrc, __u32 A_LenLim = 0 );	// return = next pointer

char*		DrStrAddChar(char* A_StrDst, char A_Char, __u32 A_LenLim = 0);	/* return = next pointer */
WCHAR*	DrStrAddWchar( WCHAR* A_StrDst, WCHAR A_Wchar, __u32 A_LenLim = 0 );	// return = next pointer

void		DrStrInsAsc( char* A_StrDst, char* A_StrIns, __u32 A_InsPos );
void		DrStrInsU16( WCHAR* A_StrDst, WCHAR* A_StrIns, __u32 A_InsPos );

void		DrStrDelAsc( char* A_StrDst, __u32 A_DelPos, __u32 A_DelSize );
void		DrStrDelU16( WCHAR* A_StrDst, __u32 A_DelPos, __u32 A_DelSize );

void		DrStrChgSenseLowAsc( char* A_Str );

BOOL8		DrStrFnGetDrvAsc( char* A_StrFn, char* A_StrRtDrv, __u32 A_LenLim = 0 );
BOOL8		DrStrFnGetDrvU16le(WCHAR* A_StrFn, WCHAR* A_StrRtDrv, __u32 A_LenLim = 0);

BOOL8		DrStrFnGetExtAsc( char* A_StrFn, char* A_RtStrExt, __u32 A_LenLim = 0 );
BOOL8		DrStrFnGetExtU16le(WCHAR* A_StrFn, WCHAR* A_RtStrExt, __u32 A_LenLim = 0);

char*		DrStrFnAddBkSlshAsc( char* A_StrFn, __u32 A_LenLim = 0 );
WCHAR*	DrStrFnAddBkSlshU16le( WCHAR* A_StrFn, __u32 A_LenLim = 0 );

BOOL8		DrStrFnGetPathAsc( char* A_StrFn, char* A_StrRtPath, __u32 A_LenLim = 0 );
BOOL8		DrStrFnGetPathU16le( WCHAR* A_StrFn, WCHAR* A_StrRtPath, __u32 A_LenLim = 0 );

BOOL8		DrStrFnGetNameOnlyAsc( char* A_StrFn, char* A_StrRtName, __u32 A_LenLim = 0 );
__u32		DrStrFnGetNameOnlyU16le(WCHAR* A_StrFn, WCHAR* A_StrRtName, __u32 A_LenLim = 0);

BOOL8		DrStrFnGetFileNameAsc( char* A_StrFn, char* A_StrRtName, __u32 A_LenLim = 0 );

BOOL8		DrStrFnReverseSlashAsc( char* A_StrFn, __u32 A_LenLim = 0 );
BOOL8		DrStrFnReverseSlashU16le( WCHAR* A_StrFn, __u32 A_LenLim = 0 );

BOOL8		DrStrFnIsSameExtAsc( char* A_StrFn, char* A_StrExt );

BOOL8		DrStrFnDivNameAndParaAsc( char* A_StrSr, char* A_StrRtFn, char* A_StrRtPara );

void		DrStrTrimLeftAsc( char* A_StrSrc, char* A_StrRt, __u32 A_LenLim = 0 );

void		DrStrTrimRightAsc( char* A_StrDst, __u32 A_LenLim = 0 );

void		DrStrTrimBothAsc( char* A_StrSrc, char* A_StrRt, __u32 A_LenLim = 0 );

void		DrStrIpToStrAsc( char* A_StrRt, __u32 A_Ip );
void		DrStrIpToStrU16le( WCHAR* A_StrRt, __u32 A_Ip );

BOOL8		DrStrStripToIpV4Asc( char* A_Str, void* A_PtrRt );
BOOL8		DrStrStripToIpV4U16le(WCHAR* A_Str, void* A_PtrRt);

char*		DrStrMacToStrAsc(char* A_StrRt, __u8* A_PtrMac, BOOL8 A_IsUpper, char A_Separater);
WCHAR*	DrStrMacToStrU16le(WCHAR* A_StrRt, __u8* A_PtrMac, BOOL8 A_IsUpper, WCHAR A_Separater);

void		DrStrStrToMacAdrAsc(__u8* A_PtrRt, char* A_StrMac);
void		DrStrStrToMacAdrU16le(__u8* A_PtrRt, WCHAR* A_StrMac);

void		DrStrStripQuotationAsc( char* A_StrSrc, char* A_StrRt, __u32 A_LenLim = 0 );

void		DrStrDressQuotationAsc( char* A_StrSrc, char* A_StrRt, __u32 A_LenLim = 0 );

void		DrStrStripQuotationAscThisAsc( char* A_Str, __u32 A_LenLim = 0 );

void		DrStrDressQuotationAscThisAsc( char* A_StrDst, __u32 A_LenLim = 0 );

char*		DrStrFillSpaceAsc(char* A_Str, __u8 A_Cnt);
WCHAR*	DrStrFillSpaceU16le( WCHAR* A_Str, __u8 A_Cnt );

char*		DrStrSkipZeroAsc( char* A_Str );
WCHAR*	DrStrSkipZeroU16( WCHAR* A_Str );

WCHAR*	DrStrSkipSpaceU16le(WCHAR* A_Str);

__u32	DrStrGetDecNumU16le(WCHAR* A_Str);

BOOL8		DrStrIsRelativeAsc(char* A_StrFn);

// wild card
BOOL8 DrStrPatternMatchU16le(WCHAR* A_StrSrc, WCHAR* A_StrPtn, BOOL8 A_IsCaseSense);

BOOL8 DrStrSubPtnMchU16le(Ptr_DrStrPtnMchU16 A_PtrCtl, __u32 A_PrgsSrc, __u32 A_PrgsPtn);

//number to string
BOOL8		DrStrIntToStrAsc( char* A_StrRt, __s32 A_Value, __u32 A_LenLim = 0, BOOL8 A_IsFix = FALSE, BOOL8 A_IsSign = TRUE );
BOOL8		DrStrIntToStrU16le( WCHAR* A_StrDst, __s32 A_Value, __u32 A_LenLim = 0,
											 BOOL8 A_IsFix = FALSE, BOOL8 A_IsSign = TRUE );

BOOL8		DrStrDwordToHexAsc( char* A_StrRt, __u32 A_Value, __u32 A_LenLim = 0, BOOL8 A_IsFix = FALSE );
BOOL8		DrStrDwordToHexU16le( WCHAR* A_StrRt, __u32 A_Value, __u32 A_LenLim = 0, BOOL8 A_IsFix = FALSE );

BOOL8		DrStrQwordToHexAsc( char* A_StrRt, __u64 A_Value, __u32 A_LenLim = 0, BOOL8 A_IsFix = FALSE );
BOOL8		DrStrQwordToHexU16le(WCHAR* A_StrRt, __u64 A_Value, __u32 A_LenLim = 0, BOOL8 A_IsFix = FALSE);

char* DrStrTempToStrAsc(char* A_StrTg, __s16 A_Temp);
WCHAR* DrStrTempToStrU16le(WCHAR* A_StrTg, __s16 A_Temp);

//string to number 
BOOL8    DrStrStrToIntAsc( char* A_StrNum, __s32* A_PtrRtVal, __u32 A_LenLim = 0 );
BOOL8    DrStrStrToIntU16le( WCHAR* A_StrNum, __s32* A_PtrRtVal, __u32 A_LenLim = 0 );

BOOL8		DrStrHexToDwordAsc( char* A_StrNum, __u32* A_PtrRtVal, __u32 A_LenLim = 0 );
BOOL8		DrStrHexToDwordU16le( WCHAR* A_StrNum, __u32* A_PtrRtVal, __u32 A_LenLim = 0 );

BOOL8		DrStrHexToQwordAsc( char* A_StrNum, __u64* A_PtrRtVal, __u32 A_LenLim = 0 );
BOOL8		DrStrHexToQwordU16le(WCHAR* A_StrNum, __u64* A_PtrRtVal, __u32 A_LenLim = 0);

BOOL8	DrStrTempToSwordAsc(char* A_StrTemp, __s16* A_PtrRtVal, __u32 A_LenLim = 0);
BOOL8	DrStrTempToSwordU16le(WCHAR* A_StrTemp, __s16* A_PtrRtVal, __u32 A_LenLim = 0);

//string parsing
void		DrStrPrsInitAsc(Ptr_DrStrParsCtrlAsc A_PtrPrs, char* A_StrSrc, __u32 A_SrcMax = 0);

BOOL8		DrStrPrsLineNext(Ptr_DrStrParsCtrlAsc A_PtrPrs );

char*		DrStrPrsWordSkipSpace(Ptr_DrStrParsCtrlAsc A_PtrPrs );
BOOL8		DrStrPrsWordGoMarkNext(Ptr_DrStrParsCtrlAsc A_PtrPrs, char A_Mark);
char*		DrStrPrsWordSkipByCount(Ptr_DrStrParsCtrlAsc A_PtrPrs, __u32 A_SkinCnt);
char*		DrStrPrsWordFindChars(Ptr_DrStrParsCtrlAsc A_PtrPrs, char* A_StrChars);
BOOL8		DrStrPrsWordGoNext(Ptr_DrStrParsCtrlAsc A_PtrPrs);
char*		DrStrPrsWordGet(Ptr_DrStrParsCtrlAsc A_PtrPrs, Ptr_DrStrParsWordRngAsc A_PtrRngTbl, __u32 A_RngTblCnt);

__u32		DrStrBase64Enc( char* A_StrSrc, char* A_StrRtEnc );

__u32		DrStrBase64Dec( char* A_StrSrc, char* A_StrRtDec );

//string utility
void		DrStrDateDispTypeSet( __u8 A_Mode );
__u8		DrStrDateDispTypeGet( void );
void		DrStrTimeToStrU16le( WCHAR* A_StrRt, Def_DrTime A_Time, BOOL8 A_IsNoTime = FALSE, BOOL8 A_IsNoDate = FALSE,
	BOOL8 A_IsNoSec = FALSE, __u8 A_GapDate = 1, __u8 A_GapTime = 1, __u8 A_DivGap = 2, BOOL8 A_IsDivider = TRUE, 
	BOOL8 A_IsFix = TRUE, BOOL8 A_IsApm = FALSE, WCHAR* A_StrAm = NULL, WCHAR* A_StrPm = NULL,
	WCHAR A_CharDivDate = 0x2F, WCHAR A_CharDivTime = 0x3A);
void		DrStrTimeToStrAsc( char* A_StrRt, Def_DrTime A_Time, BOOL8 A_IsNoTime = FALSE, BOOL8 A_IsNoDate = FALSE,
	BOOL8 A_IsNoSec = FALSE, __u8 A_GapDate = 1, __u8 A_GapTime = 1, __u8 A_DivGap = 2, BOOL8 A_IsDivider = TRUE, 
	BOOL8 A_IsFix = TRUE, BOOL8 A_IsApm = FALSE, char* A_StrAm = NULL, char* A_StrPm = NULL,
	char A_CharDivDate = 0x2F, char A_CharDivTime = 0x3A);

void		DrStrDateToStrU16le( WCHAR* A_StrRt, __u16 A_Year, __u8 A_Month, __u8 A_Day ,__u8 A_GapDate = 1 );

Def_DrTime	DrStrStrToTimeU16le(WCHAR* A_Str);

// survelence
__u8	DrStrRulePasswdU16le( WCHAR* A_Str, __u32 A_MinCnt = 8 );

//code converting
void		DrStrU16leToUtf8(char* A_StrRt, WCHAR* A_StrSrc);
void		DrStrU16leToAsc(char* A_StrRt, WCHAR* A_StrSrc, __u32 A_Language = E_DrStrLngEnglish);
void		DrStrAscToU16le(WCHAR* A_StrRt, char* A_StrSrc, __u32 A_Language = E_DrStrLngEnglish);

//korean
void		DrStrKorDecU16le( __u16 A_Code, __u8* A_PtrRtFirst, __u8* A_PtrRtMid, __u8* A_PtrRtLast );
__u16		DrStrKorEncU16le( __u8 A_First, __u8 A_Mid, __u8 A_Last );
__u8		DrStrKorJamoToFirstU16le( __u16 A_Code );
__u8		DrStrKorJamoToMiddleU16le( __u16 A_Code );
__u8		DrStrKorJamoToLastU16le( __u16 A_Code );

//====================================================================
//out Enviroment conditional
#endif
