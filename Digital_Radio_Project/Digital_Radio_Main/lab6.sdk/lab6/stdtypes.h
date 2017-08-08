/************************************************************************/
/*																		*/
/*	stdtypes.h	--	Digilent Standard Type Declarations					*/
/*																		*/
/************************************************************************/
/*	Author:		Gene Apperson											*/
/*	Copyright 2005, Digilent Inc.										*/
/************************************************************************/
/*  File Description:													*/
/*																		*/
/* This header file contains declarations for standard Digilent data	*/
/* types and constants for use with avr-gcc.							*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	01/04/2005(GeneA): created											*/
/*	04/28/2009(MichaelA): modified to support PIC32MX processor line	*/
/*																		*/
/************************************************************************/

#if !defined(_STDTYPES_INC)
#define _STDTYPES_INC

#include <stdint.h>

/* ------------------------------------------------------------ */
/*					General Type Declarations					*/
/* ------------------------------------------------------------ */

#define	fFalse	0
#define	fTrue	(!fFalse)

#define	BOOL	uint32_t
#define	BYTE	uint8_t
#define HWORD	uint16_t
#define	WORD	uint32_t
#define DWORD	uint64_t

/* ------------------------------------------------------------ */
/*					Object Class Declarations					*/
/* ------------------------------------------------------------ */



/* ------------------------------------------------------------ */
/*					Variable Declarations						*/
/* ------------------------------------------------------------ */



/* ------------------------------------------------------------ */

#endif

/************************************************************************/
