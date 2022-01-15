//--------------------------------------------------------------------
//Enviroment conditional
#ifndef	_EPC_DrTypeBase
#define	_EPC_DrTypeBase

//====================================================================
// uses

#ifdef LINUX_APP

#ifdef LINUX_DRV

#ifdef DR_64BIT

#include <asm-generic/int-ll64.h>

#endif

#else

#include <asm/types.h>

#endif

#endif


//====================================================================
//constance

#if defined(_WIN32) || defined(_WIN64)
#define	__gccna
#else
#define	__gccna	__attribute__((packed))
#endif

#ifdef DR_64BIT

#define	Def_DrCalPtr		__u64

#else

#define	Def_DrCalPtr		__u32

#endif


#if defined(_WIN32) && !defined(_WIN64)

#define VOID void
typedef	void	*PVOID;

#endif

#ifndef	NULL
#define	NULL	0
#endif

#define TRUE	1
#define FALSE	0

#define E_DrFnFullPathLen				1024
#define E_DrFnLongLen						128
#define E_DrFnLongWithPathLen		256
#define E_DrFnShortPathLen			128
#define E_DrFnShortLen					16

#define E_DrNumStrLen						16
#define E_DrNameStrLen					32
#define E_DrTagStrLen						64
#define E_DrShortStrLen					256
#define E_DrLongStrLen					1024
#define E_DrHugeStrLen					4096

// variable size
#define E_DrVarSize1byte				0
#define E_DrVarSize2byte				1
#define E_DrVarSize4byte				2

// align
#define E_DrAlignLeft						0
#define E_DrAlignRight					1
#define E_DrAlignCenter					2
#define E_DrAlignTop						0
#define E_DrAlignBottom					1

// direction
#define E_DrWayLeft							0
#define E_DrWayUp								1
#define E_DrWayRight						2
#define E_DrWayDown							3
#define E_DrWayCnt							4

// temp
#define	E_DrTempSensTypeNone				0
#define	E_DrTempSensTypeRoi					1
#define	E_DrTempSensTypePoly				2

#define	E_DrTempRoiTypeNone					0
#define	E_DrTempRoiTypePixel				1
#define	E_DrTempRoiTypeLine					2
#define	E_DrTempRoiTypeRect					3

#define	E_DrTempEvtRoiWarn					1
#define	E_DrTempEvtRoiCaus					2
#define	E_DrTempEvtRoiDang					3
#define	E_DrTempEvtRoiUnder					0x8

#define	E_DrTempEvtGruWarn					0x10
#define	E_DrTempEvtGruCaut					0x20
#define	E_DrTempEvtGruDang					0x30

#define	E_DrTempGroupRoiCnt					3
#define	E_DrTempPolyPosCnt					29


// variable attribute
#define E_DrVarLenVar						0
#define E_DrVarLen1Byte					1
#define E_DrVarLen2Byte					2
#define E_DrVarLen4Byte					3
#define E_DrVarLen8Byte					4

#define E_DrVarAtbBitPos				3

#define E_DrVarAtbUnsigned						(0x0000 << E_DrVarAtbBitPos)		/* unsigned number */
#define E_DrVarAtbSigned							(0x0001 << E_DrVarAtbBitPos)		/* signed number */
#define E_DrVarAtbHex									(0x0002 << E_DrVarAtbBitPos)		/* hex unsigned number */
#define E_DrVarAtbStr									(0x0003 << E_DrVarAtbBitPos)		/* text */
#define E_DrVarAtbWstr								(0x0004 << E_DrVarAtbBitPos)		/* unicode text */
#define E_DrVarAtbBool								(0x0005 << E_DrVarAtbBitPos)		/* bool */
#define E_DrVarAtbBitmap0							(0x0006 << E_DrVarAtbBitPos)		/* bitmap 0*/
#define E_DrVarAtbBitmap1							(0x0007 << E_DrVarAtbBitPos)		/* bitmap 1*/
#define E_DrVarAtbColor								(0x0008 << E_DrVarAtbBitPos)		/* color */
#define E_DrVarAtbListFixDec					(0x0009 << E_DrVarAtbBitPos)		/* list fix decimal */
#define E_DrVarAtbListFixHex					(0x000A << E_DrVarAtbBitPos)		/* list fix hex */
#define E_DrVarAtbListRangeDec				(0x000B << E_DrVarAtbBitPos)		/* list range decimal */
#define E_DrVarAtbListRangeHex				(0x000C << E_DrVarAtbBitPos)		/* list range hex */
#define E_DrVarAtbIp									(0x000D << E_DrVarAtbBitPos)		/* ip address */
#define E_DrVarAtbBitBool							(0x000E << E_DrVarAtbBitPos)		/* bit bool */
#define E_DrVarAtbPallete							(0x000F << E_DrVarAtbBitPos)		/* pallete */
#define E_DrVarAtbTime								(0x0010 << E_DrVarAtbBitPos)		/* time */
#define E_DrVarAtbAlignHz							(0x0011 << E_DrVarAtbBitPos)		/* align horizontal */
#define E_DrVarAtbAlignVt							(0x0012 << E_DrVarAtbBitPos)		/* align vertical */
#define E_DrVarAtbDirection						(0x0013 << E_DrVarAtbBitPos)		/* direction */
#define E_DrVarAtbVarSize							(0x0014 << E_DrVarAtbBitPos)		/* variable size */
#define E_DrVarAtbMacAdr							(0x0015 << E_DrVarAtbBitPos)		/* mac address */
#define E_DrVarAtbRotate							(0x0016 << E_DrVarAtbBitPos)		/* rotate */
#define E_DrVarAtbPathWstr						(0x0017 << E_DrVarAtbBitPos)		/* path wstr */
#define E_DrVarAtbFnWstr							(0x0018 << E_DrVarAtbBitPos)		/* file name wstr */
#define E_DrVarAtbCheckBool						(0x0019 << E_DrVarAtbBitPos)		/* Check bool */
#define E_DrVarAtbPasswdStr						(0x001A << E_DrVarAtbBitPos)		/* password str */
#define E_DrVarAtbTempK								(0x001B << E_DrVarAtbBitPos)		/* temprature kelvin */
#define E_DrVarAtbTempC								(0x001C << E_DrVarAtbBitPos)		/* temprature celceus */
#define E_DrVarAtbTempF								(0x001D << E_DrVarAtbBitPos)		/* temprature farenhiet */
#define E_DrVarAtbChkBit							(0x001E << E_DrVarAtbBitPos)		/* checkbox bit */
#define E_DrVarAtbTempRoiType					(0x001F << E_DrVarAtbBitPos)		/* temprature roi type */
#define E_DrVarAtbTempSensType				(0x0020 << E_DrVarAtbBitPos)		/* temprature sense type */

#define E_DrVarAtbUserStart						0x1000


// var type
#define E_DrVarTypeNone								0		/* none */

#define	E_DrVarTypeS8								(E_DrVarAtbSigned | E_DrVarLen1Byte)
#define	E_DrVarTypeS16							(E_DrVarAtbSigned | E_DrVarLen2Byte)
#define	E_DrVarTypeS32							(E_DrVarAtbSigned | E_DrVarLen4Byte)	
#define	E_DrVarTypeS64							(E_DrVarAtbSigned | E_DrVarLen8Byte)	

#define	E_DrVarTypeU8								(E_DrVarAtbUnsigned | E_DrVarLen1Byte)
#define	E_DrVarTypeU16							(E_DrVarAtbUnsigned | E_DrVarLen2Byte)
#define	E_DrVarTypeU32							(E_DrVarAtbUnsigned | E_DrVarLen4Byte)
#define	E_DrVarTypeU64							(E_DrVarAtbUnsigned | E_DrVarLen8Byte)

#define	E_DrVarTypeHexU8						(E_DrVarAtbHex | E_DrVarLen1Byte)
#define	E_DrVarTypeHexU16						(E_DrVarAtbHex | E_DrVarLen2Byte)
#define	E_DrVarTypeHexU32						(E_DrVarAtbHex | E_DrVarLen4Byte)
#define	E_DrVarTypeHexU64						(E_DrVarAtbHex | E_DrVarLen8Byte)

#define	E_DrVarTypeStr							(E_DrVarAtbStr | E_DrVarLenVar)			/* char string */

#define	E_DrVarTypeWchar						(E_DrVarAtbWstr | E_DrVarLen2Byte)		/* wchar one */
#define	E_DrVarTypeWstr							(E_DrVarAtbWstr | E_DrVarLenVar)		/* wchar string */

#define	E_DrVarTypeBool8						(E_DrVarAtbBool | E_DrVarLen1Byte)

#define	E_DrVarTypeBit0U8						(E_DrVarAtbBitmap0 | E_DrVarLen1Byte)
#define	E_DrVarTypeBit1U8						(E_DrVarAtbBitmap1 | E_DrVarLen1Byte)

#define	E_DrVarTypeColor						(E_DrVarAtbColor | E_DrVarLen4Byte)

#define	E_DrVarTypePallete					(E_DrVarAtbPallete | E_DrVarLen4Byte)

#define	E_DrVarTypeIpV4							(E_DrVarAtbIp | E_DrVarLen4Byte)

#define	E_DrVarTypeMacAdr						(E_DrVarAtbMacAdr | E_DrVarLen8Byte)

#define	E_DrVarTypeLstFixDecU8			(E_DrVarAtbListFixDec | E_DrVarLen1Byte)
#define	E_DrVarTypeLstFixDecU16			(E_DrVarAtbListFixDec | E_DrVarLen2Byte)
#define	E_DrVarTypeFixFixDecU32			(E_DrVarAtbListFixDec | E_DrVarLen4Byte)

#define	E_DrVarTypeLstFixHexU8			(E_DrVarAtbListFixHex | E_DrVarLen1Byte)
#define	E_DrVarTypeLstFixHexU16			(E_DrVarAtbListFixHex | E_DrVarLen2Byte)
#define	E_DrVarTypeFixFixHexU32			(E_DrVarAtbListFixHex | E_DrVarLen4Byte)

#define E_DrVarTypeBitBoolBit0			(E_DrVarAtbBitBool | 0)
#define E_DrVarTypeBitBoolBit1			(E_DrVarAtbBitBool | 1)
#define E_DrVarTypeBitBoolBit2			(E_DrVarAtbBitBool | 2)
#define E_DrVarTypeBitBoolBit3			(E_DrVarAtbBitBool | 3)
#define E_DrVarTypeBitBoolBit4			(E_DrVarAtbBitBool | 4)
#define E_DrVarTypeBitBoolBit5			(E_DrVarAtbBitBool | 5)
#define E_DrVarTypeBitBoolBit6			(E_DrVarAtbBitBool | 6)
#define E_DrVarTypeBitBoolBit7			(E_DrVarAtbBitBool | 7)

#define	E_DrVarTypeAlignHz					(E_DrVarAtbAlignHz | E_DrVarLen1Byte)
#define	E_DrVarTypeAlignVt					(E_DrVarAtbAlignVt | E_DrVarLen1Byte)

#define	E_DrVarTypeDirection				(E_DrVarAtbDirection | E_DrVarLen1Byte)
#define	E_DrVarTypeTime							(E_DrVarAtbTime | E_DrVarLen4Byte)
#define	E_DrVarTypeVarSize					(E_DrVarAtbVarSize | E_DrVarLen1Byte)
#define	E_DrVarTypeRotate						(E_DrVarAtbRotate | E_DrVarLen1Byte)
#define	E_DrVarTypePathWstr					(E_DrVarAtbPathWstr | E_DrVarLenVar)
#define	E_DrVarTypeFnWstr						(E_DrVarAtbFnWstr | E_DrVarLenVar)

#define E_DrVarTypeCheckBool8				(E_DrVarAtbCheckBool | E_DrVarLen1Byte)		/* Check bool */
#define E_DrVarTypePasswdStr				(E_DrVarAtbPasswdStr | E_DrVarLenVar)

#define	E_DrVarTypeTempKs16					(E_DrVarAtbTempK | E_DrVarLen2Byte)
#define	E_DrVarTypeTempCs16					(E_DrVarAtbTempC | E_DrVarLen2Byte)
#define	E_DrVarTypeTempFs16					(E_DrVarAtbTempF | E_DrVarLen2Byte)

#define E_DrVarTypeChkBit0					(E_DrVarAtbChkBit | 0)
#define E_DrVarTypeChkBit1					(E_DrVarAtbChkBit | 1)
#define E_DrVarTypeChkBit2					(E_DrVarAtbChkBit | 2)
#define E_DrVarTypeChkBit3					(E_DrVarAtbChkBit | 3)
#define E_DrVarTypeChkBit4					(E_DrVarAtbChkBit | 4)
#define E_DrVarTypeChkBit5					(E_DrVarAtbChkBit | 5)
#define E_DrVarTypeChkBit6					(E_DrVarAtbChkBit | 6)
#define E_DrVarTypeChkBit7					(E_DrVarAtbChkBit | 7)

#define E_DrVarTypeTempRoiType			(E_DrVarAtbTempRoiType | E_DrVarLen1Byte)
#define E_DrVarTypeTempSensType			(E_DrVarAtbTempSensType | E_DrVarLen1Byte)


//====================================================================
//global macro

#define Mac_DrMakeFcc(ch0, ch1, ch2, ch3)	\
				((__u32)(__u8)(ch0) | ((__u32)(__u8)(ch1) << 8) | \
				((__u32)(__u8)(ch2) << 16) | ((__u32)(__u8)(ch3) << 24 ))

#define Mac_DrMakeTcc(ch0, ch1)	\
	((__u16)(__u8)(ch0) | ((__u16)(__u8)(ch1) << 8))

//====================================================================
//global type

#ifndef	_WINDEF_

//typedef int									BOOL;

#ifdef LINUX_DRV
	typedef unsigned short			WCHAR;
#else
#ifdef LINUX_APP
#ifdef DFLT_WCHAR
	typedef wchar_t	WCHAR;
#else
	typedef unsigned short			WCHAR;
#endif
#else
	typedef wchar_t	WCHAR;
#endif
#endif

#endif

	// new standard type
#if defined(_WIN32) || defined(_WIN64)
 typedef unsigned __int8 __u8;
 typedef unsigned __int16 __u16;
 typedef unsigned __int32 __u32;
 typedef unsigned __int64 __u64;
 typedef __int8 __s8;
 typedef __int16 __s16;
 typedef __int32 __s32;
 typedef __int64 __s64;
#endif

	typedef	__u8	BOOL8;

	//file name
	typedef	char	Def_StrDrFnPath[E_DrFnLongWithPathLen];
	typedef	char	Def_StrDrFnPathLong[E_DrFnFullPathLen];
	typedef	char	Def_StrDrFnPathShort[E_DrFnShortPathLen];

	typedef	WCHAR	Def_WstrDrFnPath[E_DrFnLongWithPathLen];
	typedef	WCHAR	Def_WstrDrFnPathLong[E_DrFnFullPathLen];
	typedef	WCHAR	Def_WstrDrFnPathShort[E_DrFnShortPathLen];

	//standard string
	typedef	char	Def_StrNum[E_DrNumStrLen];
	typedef	char	Def_StrName[E_DrNameStrLen];
	typedef	char	Def_StrTag[E_DrTagStrLen];
	typedef	char	Def_StrShort[E_DrShortStrLen];
	typedef	char	Def_StrLong[E_DrLongStrLen];
	typedef	char	Def_StrHuge[E_DrHugeStrLen];

	typedef	WCHAR	Def_WstrNum[E_DrNumStrLen];
	typedef	WCHAR	Def_WstrName[E_DrNameStrLen];
	typedef	WCHAR	Def_WstrTag[E_DrTagStrLen];
	typedef	WCHAR	Def_WstrShort[E_DrShortStrLen];
	typedef	WCHAR	Def_WstrLong[E_DrLongStrLen];
	typedef	WCHAR	Def_WstrHuge[E_DrHugeStrLen];

	//time
	typedef	__u32	Def_DrTime, *Ptr_DrTime;

	typedef	struct St_DrTimeRng
	{
		Def_DrTime	TimeSt;			//start time
		Def_DrTime	TimeEd;			//end time
	}	*Ptr_DrTimeRng;

	typedef	struct	St_DrDate
	{
		__u8		Day;
		__u8		Month;
		__u16		Year;
	}	*Ptr_DrDate;

	typedef struct St_DrDateTime
	{
		__u8	Sec;
		__u8	Min;
		__u8	Hour;
		__u8	_rsvd0;
		__u8		Day;
		__u8		Month;
		__u16		Year;
	}	*Ptr_DrDateTime;

	//physical memory mapper
	typedef	struct St_DrPhyMemMap
	{
		__u32		Phys;			//physical address
		void*		PtrApp;		//virtual address
		void*		PtrDrv;		//driver address
		__u32		Size;			//size
	}	*Ptr_DrPhyMemMap;

	//point position
	typedef	struct	St_DrPos
	{
		__s32			x;
		__s32			y;
	}	*Ptr_DrPos;
	//rect position
	typedef	struct	St_DrRect
	{
		__s32		left;
		__s32		top;
		__s32		right;
		__s32		bottom;

	} *Ptr_DrRect;

	typedef	struct St_DrByteRect
	{
		__u8	left;
		__u8	top;
		__u8	right;
		__u8	bottom;
	}	*Ptr_DrByteRect;

	typedef	struct St_DrWordRect
	{
		__u16	left;
		__u16	top;
		__u16	right;
		__u16	bottom;
	}	*Ptr_DrWordRect;

	typedef	struct St_DrSwordRect
	{
		__s16	left;
		__s16	top;
		__s16	right;
		__s16	bottom;
	}	*Ptr_DrSwordRect;

	typedef	struct St_DrWordPos
	{
		__u16	x;
		__u16	y;
	}	*Ptr_DrWordPos;

	typedef	struct St_DrSwordPos
	{
		__s16	x;
		__s16	y;
	}	*Ptr_DrSwordPos;

	typedef	struct St_DrBytePos
	{
		__u8	x;
		__u8	y;
	}	*Ptr_DrBytePos;

	typedef	struct St_DrLinePos 
	{
		struct St_DrPos	St;		//start position
		struct St_DrPos	Ed;		//end positon
	}	*Ptr_DrLinePos;

	typedef	struct St_DrPosSz
	{
		__s32		x;
		__s32		y;
		__s32		Sx;
		__s32		Sy;
	}	*Ptr_DrPosSz;

	// relate positoin
	typedef struct St_DrR7Val
	{
		__u8	Ratio;
		__u8	Para;
		__s16	Val;
	}	*Ptr_DrR7Val;

	typedef struct St_DrR7Pos
	{
		struct St_DrR7Val	x;
		struct St_DrR7Val	y;
	}	*Ptr_DrR7Pos;

	typedef struct St_DrR7Rect
	{
		struct St_DrR7Val	Left;
		struct St_DrR7Val	Top;
		struct St_DrR7Val	Right;
		struct St_DrR7Val	Bottom;
	}	*Ptr_DrR7Rect;

	typedef struct St_DrTempPos
	{
		__u16	Temp;
		__u16	_rsvd0;
		__u16	X;
		__u16	Y;
	} *Ptr_DrTempPos;

	typedef struct St_DrTempRoiCfg
	{
		__u8	RoiType;
		BOOL8	IsUnder;
		__u16	TempWarn;
		__u16	TempCaus;
		__u16	TempDang;
		struct St_DrWordRect	Rc;
		/* size 16 byte */
	}	*Ptr_DrTempRoiCfg;

	typedef struct St_DrTempRoiEvt
	{
		__u8	SenType;
		__u8	SubType;
		__u8	Idx;
		__u8	Atb;
		__u16	TempHigh;
		__u16	TempLow;
		struct St_DrWordPos	PosHigh;
		struct St_DrWordPos	PosLow;
	}	*Ptr_DrTempRoiEvt;

	typedef struct St_DrTempGroupElemInfo
	{
		__u8	SenType;
		__u8	Idx;
	}	*Ptr_DrTempGroupElemInfo;

	typedef struct St_DrTempGroupCfg
	{
		BOOL8	IsUse;
		__u8	_rsvd0;
		struct St_DrTempGroupElemInfo	Elems[E_DrTempGroupRoiCnt];
		__u16	DifWarn;	// 8
		__u16	DifCaution;
		__u16	DifDanger;
		__u16	_rsvd1;

		/* size 16 */
	}	*Ptr_DrTempGroupCfg;

	typedef struct St_DrTempPolyCfg
	{
		struct St_DrWordPos	PosTbl[E_DrTempPolyPosCnt];
		BOOL8	IsUnder;
		__u8	_rsvd0;
		__u16	TempWarn;
		__u16	TempCaus;
		__u16	TempDang;
		__u32	Cnt;

		/* size 128 */
	}	*Ptr_DrTempPolyCfg;

	typedef struct St_DrMacRng
	{
		__u64	Begin;
		__u64	End;
	}	*Ptr_DrMacRng;

	typedef struct St_DrIpV4Rng
	{
		__u32	Begin;
		__u32	End;
	}	*Ptr_DrIpV4Rng;

	typedef struct St_DrU16Rng
	{
		__u16	Begin;
		__u16	End;
	}	*Ptr_DrU16Rng;

	typedef struct St_DrMemInfo
	{
		void*	Ptr;
		__u32	Size;
	}	*Ptr_DrMemInfo;

//====================================================================
//global function


//====================================================================
//out Enviroment conditional
#endif
