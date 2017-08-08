/************************************************************************/
/*																		*/
/*	ClpLib.h -- Common Utility Procedures for the PIC32MX             */
/*																		*/
/************************************************************************/
/*	Author: Cristian Fatu												*/
/*	Copyright 2011, Digilent Inc.										*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*	This header contains declarations for common functions needed to 	*/
/*	manage a CLP PMOD. It also contains other definitions used by 		*/
/*  CLP module.															*/
/*	This file must be included in MPLAB project whenever CLP functions 	*/
/*  are needed. No modifications in this file are required				*/
/************************************************************************/
/*  Revision History:						                			*/
/*											                        	*/
/*	10/15/2011 (CristianF): created			                			*/
/*											                        	*/
/************************************************************************/

#if !defined(_CLPLIB_INC)
#define	_CLPLIB_INC

#include "stdtypes.h"


/* ------------------------------------------------------------ */
/*				    	Macro Declarations			    		*/
/* ------------------------------------------------------------ */



/* ------------------------------------------------------------ */
/*					General Type Declarations					*/
/* ------------------------------------------------------------ */



/* ------------------------------------------------------------ */
/*					Object Class Declarations					*/
/* ------------------------------------------------------------ */




/* ------------------------------------------------------------ */
/*					Variable Declarations						*/
/* ------------------------------------------------------------ */

#define cmdLcdFcnInit 	0x38	// function set command, (8-bit interface, 2 lines, and 5x8 dots)
#define cmdLcdCtlInit 	0x08	// display control set command
#define cmdLcdClear		0x01		// clear display command
#define cmdLcdRetHome	0x02		// return home command
#define cmdLcdDisplayShift 0x18		// shift display command
#define cmdLcdCursorShift  0x10		// shift cursor command
#define cmdLcdSetDdramPos	0x80	// set DDRAM position command
#define cmdLcdSetCgramPos	0x40	// set CGRAM position command
#define mskBStatus 0x80		// bit busy 

#define mskShiftRL 0x04		// bit busy 
#define displaySetOptionDisplayOn	0x4 // Set Display On option
#define	displaySetOptionCursorOn 	0x2 // Set Cursor On option
#define	displaySetBlinkOn 			0x1 // Set Blink On option


#define posCgramChar0 0		// position in CGRAM for character 0
#define posCgramChar1 8		// position in CGRAM for character 1
#define posCgramChar2 16	// position in CGRAM for character 2
#define posCgramChar3 24	// position in CGRAM for character 3
#define posCgramChar4 32	// position in CGRAM for character 4
#define posCgramChar5 40	// position in CGRAM for character 5
#define posCgramChar6 48	// position in CGRAM for character 6
#define posCgramChar7 56	// position in CGRAM for character 7
/* ------------------------------------------------------------ */
/*					Function Declarations						*/
/* ------------------------------------------------------------ */

void ClpPinsConfigure();
void ClpInit(BYTE bDisplaySetOptions);
void ClpDisplaySet(BYTE bDisplaySetOptions);
void ClpDisplayClear();
BYTE ClpReadByte(BOOL rMode);				// mode bit added by EWH
BYTE ClpReadStatus();
void ClpWaitUntilNotBusy();
void ClpWriteByte(BOOL wMode, BYTE bData);	// mode bit added by EWH
void ClpSetWriteDdramPosition(BYTE bAdr);
void ClpSetWriteCgramPosition(BYTE bAdr);
void ClpWriteStringAtPos(char *szLn, int idxLine, BYTE bAdr);
void ClpWriteBytesAtPosCgram(BYTE *pBytes, BYTE len, BYTE bAdr);
void ClpWriteCommand(BYTE bCmd);
void ClpWriteDataByte(BYTE bData);
void ClpSetBackLight(BOOL fBl);
void ClpDisplayShift(BOOL fRight);
void ClpCursorShift(BOOL fRight);
void ClpReturnHome();

/* ------------------------------------------------------------ */


#endif	//_CLPLIB_INC
