/************************************************************************/
/*																		*/
/*	ClpLib.c -- Common Utility Procedures for the PIC32MX                 */
/*																		*/
/************************************************************************/
/*	Author: Cristian Fatu												*/
/*	Copyright 2011, Digilent Inc.										*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*	This module contains implementations for common functions needed to */
/*	manage a CLP PMOD.													*/
/*	This file must be included in MPLAB project whenever CLP functions 	*/
/*  are needed. No modifications in this file are required.				*/
/*  This file includes ClpLib.h (CLP definitions) and					*/
/*  ClpLib_config.h (CLP pins definitions)								*/
/************************************************************************/
/*  Revision History:						                			*/
/*											                        	*/
/*	10/05/2011 (CristianF): created			                			*/
/*											                        	*/
/************************************************************************/

/*  MODIFIED Nov 2016 by E.W. Hansen for use in Xilinx SDK (MicroBlaze)
 *  The memory-mapped IO calls to the PIC pins are replaced by direct
 *  writes to IP Integrator GPIO ports.  The PIC-specific code has been
 *  removed.
 *
 *  These functions, which perform basic I/O, were modified:
 *  	ClpPinsConfigure()
 *  	ClpSetBackLight()
 *  	ClpReadByte()
 *  	ClpWriteByte()
 *  	ClpReadStatus()
 *  	ClpWaitUntilNotBusy()
 *  	ClpWriteCommand()
 *  	ClpWriteDataByte()
 *  	ClpInit()
 *  The remaining functions use the basic I/O functions, and are OK as is:
 *  	ClpDisplaySet()
 *  	ClpDisplayClear()
 *  	ClpReturnHome()
 *  	ClpSetWriteDdramPosition()
 *  	ClpWriteStringAtPos()
 *  	ClpSetWriteCgramPosition()
 *  	ClpWriteBytesAtPosCgram()
 *  	ClpDisplayShift()
 *  	ClpCursorShift()
 */

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "stdtypes.h"
#include "ClpLib.h"
//#include "util.h"				// Keep this around in case I want to rewrite it for MicroBlaze

#include "xgpio.h"
#include "xparameters.h"
// #include "microblaze_sleep.h"	// Can't use this with Zynq!

/* ------------------------------------------------------------ */
/*				Local Type Definitions							*/
/* ------------------------------------------------------------ */

#define CLP_DATA_BUS 1			// Subchannels in the GPIO
#define CLP_CONTROL_BUS 2

#define ROUGH_1MS_DELAY	100000
/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Local Variables									*/
/* ------------------------------------------------------------ */

XGpio GPIO_CLP;					// the GPIO port for the CLP
XGpio_Config GPIO_CLP_conf;

/* ------------------------------------------------------------ */
/*				Forward Declarations							*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Procedure Definitions							*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*        ClpPinsConfigure
**
**        Synopsis:
**				ClpPinsConfigure();
**        Parameters:
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**			Configures pins involved in CLP management as outputs. 
**				Data pins are not configured here. They will be configured as output/input 
**				in the corresponding write / read functions.
**
*/
void ClpPinsConfigure()
{
	// Configure the GPIO port for the CLP:
	// This is the only "machine dependent" part of this code.
	// The XPAR values are picked up from xparameters.h, must match the name of the AXI GPIO
	// block in IP Integrator. So that this library can be reused, when the hardware is set up
	// with IP Integrator, rename the AXI GPIO block from "axi_gpio_n" to "axi_gpio_clp".
	// The change will propagate into xparameters.h when the board support package is created by SDK.
	GPIO_CLP_conf.BaseAddress = XPAR_AXI_GPIO_CLP_BASEADDR;
	GPIO_CLP_conf.DeviceId = XPAR_AXI_GPIO_CLP_DEVICE_ID;
	GPIO_CLP_conf.InterruptPresent = XPAR_AXI_GPIO_CLP_INTERRUPT_PRESENT;
	GPIO_CLP_conf.IsDual = XPAR_AXI_GPIO_CLP_IS_DUAL;

	// Initialize the GPIO port for the CLP
	XGpio_CfgInitialize(&GPIO_CLP, &GPIO_CLP_conf, GPIO_CLP_conf.BaseAddress);

	// Data pins will be set as output or input in the corresponding write and read functions
}


/* ------------------------------------------------------------ */
/*        ClpSetBackLight
**
**        Synopsis:
**				ClpSetBackLight(fBackLight);
**        Parameters:
**				BOOL fBl - Backlight option
**						- fTrue in order to set the backlight ON	
**						- fFalse in order to set the backlight OFF
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Sets the backlight. 
**				It appears that, despite what the schematic says, not all Pmod-CLPs
**				have backlight functionality.
**
*/

void ClpSetBackLight(BOOL fBl)
{
	// The four CLP control bits are the four LSBs of channel 2 of the GPIO port.
	// The AXI_GPIO is 32 bits wide.  We use only the four lsbs, the others are 0.

	if(fBl)
	{
		XGpio_DiscreteWrite(&GPIO_CLP, CLP_CONTROL_BUS, 0x00000008);	// Set bit 3
	}
	else
	{
		XGpio_DiscreteWrite(&GPIO_CLP, CLP_CONTROL_BUS, 0x00000000);	// Clear bit 3
	}
}


/* ------------------------------------------------------------ */
/*        ClpReadByte
**
**        Synopsis:
**
**        Parameters:
**				  BOOL rMode - 0 to read status register, 1 to read data register
**
**        Return Values:
**                BYTE - the byte that was read
**
**        Errors:
**
**
**        Description:
**				Implements a CLP read sequence. 
**				This is a lowest level function that deals with timings of the read cycle.
**				Additional delay is inserted to give the bus time to settle before E bit is dropped.
*/
BYTE ClpReadByte(BOOL rMode)
{
	// Set data bits for reading.
	// The data bits are the 8 lsbs of the 32-bit AXI_GPIO port, channel 1.
	XGpio_SetDataDirection(&GPIO_CLP, CLP_DATA_BUS, 0x000000FF);

	// The control bits (K E R/W RS) are the 4 lsbs of the 32-bit AXI_GPIO port, channel 2.
	BYTE bControl;
	if (rMode){
		bControl = 0x03;			// Read data register
	}
	else {
		bControl = 0x02;			// Read status register
	}

	// Set mode bits, then raise the E bit
	XGpio_DiscreteWrite(&GPIO_CLP, CLP_CONTROL_BUS, ((WORD) bControl));			// E=0
	XGpio_DiscreteWrite(&GPIO_CLP, CLP_CONTROL_BUS, ((WORD) (bControl | 0x04)));	// E=1

	BYTE bData;
	// Read 32 bits, mask off the 24 MSBs and cast as a byte
	bData = (BYTE)(XGpio_DiscreteRead(&GPIO_CLP, 1) & 0x0000000F);

	// MB_Sleep(1);
	int i;
	for(i=0; i<ROUGH_1MS_DELAY; i++); 	// Rough time delay

	// Drop the E bit, then return to idle
	XGpio_DiscreteWrite(&GPIO_CLP, CLP_CONTROL_BUS, ((WORD) bControl));			// E=0
	XGpio_DiscreteWrite(&GPIO_CLP, CLP_CONTROL_BUS, 0x00000000);

	return bData;
}

/* ------------------------------------------------------------ */
/*        ClpWriteByte
**
**        Synopsis:
**
**        Parameters:
**        		BOOL wMode		0 for instruction byte, 1 for data byte
**				BYTE bData
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Implements a CLP write sequence. 
**				This is a lowest level function that deals with timings of the write cycle.
**				Writes in DDRAM or CGRAM according to the last set write position.
**				For MicroBlaze, must insert delay to allow bus data to become valid before dropping the E bit.
*/
void ClpWriteByte(BOOL wMode, BYTE bData)
{
	// Set data bits for writing.
	// The data bits are the 8 lsbs of the 32-bit AXI_GPIO port, channel 1.
	XGpio_SetDataDirection(&GPIO_CLP, CLP_DATA_BUS, 0x00000000);

	// The control bits (K E R/W RS) are the 4 lsbs of the 32-bit AXI_GPIO port, channel 2.
	BYTE bControl;
	if (wMode){
		bControl=0x01;		// Data byte coming
	}
	else
	{
		bControl=0x00;		// Instruction byte coming
	}

	// Set the mode bits, then raise the E bit
	XGpio_DiscreteWrite(&GPIO_CLP, CLP_CONTROL_BUS, ((WORD) bControl));	// E=0
	XGpio_DiscreteWrite(&GPIO_CLP, CLP_CONTROL_BUS, ((WORD) (bControl | 0x04)));	// E=1

	// Cast data byte as 32 bits, write to port
	XGpio_DiscreteWrite(&GPIO_CLP, CLP_DATA_BUS, ((WORD) bData));
    // MB_Sleep(1);			// Need a delay here so data is stable when E drops
	int i;
	for(i=0; i<ROUGH_1MS_DELAY; i++); 	// Rough time delay

	// Drop the E bit, then return to idle
	XGpio_DiscreteWrite(&GPIO_CLP, CLP_CONTROL_BUS, ((WORD) bControl));	// E=0
	XGpio_DiscreteWrite(&GPIO_CLP, CLP_CONTROL_BUS, 0x00000000);	// R/W, RS = 0, E=0
}


/* ------------------------------------------------------------ */
/*        ClpReadStatus
**
**        Synopsis:
**
**        Parameters:
**
**        Return Values:
**                BYTE - the byte rhat was read.
**
**        Errors:
**
**        Description:
**				Reads the status of the CLP. It clears the RS and calls ClpReadByte() function.
**
*/
BYTE ClpReadStatus()
{
	BYTE bStatus = ClpReadByte(0);
	return bStatus;
}


/* ------------------------------------------------------------ */
/*        ClpWaitUntilNotBusy
**
**        Synopsis:
**				ClpWaitUntilNotBusy()
**        Parameters:
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Waits until the status of the CLP is not busy. This function relies on ClpReadStatus().
**
*/
void ClpWaitUntilNotBusy()
{
	BYTE bStatus;
	bStatus = ClpReadStatus(0);
	while (bStatus & mskBStatus)
	{
//		DelayUs(10);
		// MB_Sleep(1);	// 1 ms delay
		int i;
		for(i=0; i<ROUGH_1MS_DELAY; i++); 	// Rough time delay

		bStatus = ClpReadStatus(0);
	}
}

/* ------------------------------------------------------------ */
/*        ClpWriteCommand
**
**        Synopsis:
**				ClpWriteCommand(cmdLcdClear);
**        Parameters:
**				BYTE bCmd	- the command code byte
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Writes the specified byte as command. When the device is ready it clears the RS and writes byte.
**
*/
void ClpWriteCommand(BYTE bCmd)
{
	// Wait until LCD is not busy
	ClpWaitUntilNotBusy();

	// Write command byte
	ClpWriteByte(0, bCmd);
}


/* ------------------------------------------------------------ */
/*        ClpWriteDataByte
**
**        Synopsis:
**				ClpWriteDataByte(pBytes[idx]);
**        Parameters:
**				BYTE bData		- the data byte
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Writes the specified byte as data. When the device is ready it sets the RS and writes byte.
**
*/
void ClpWriteDataByte(BYTE bData)
{
	// Wait until LCD is not busy
	ClpWaitUntilNotBusy();

	// Write data byte
	ClpWriteByte(1, bData);
}

/* ------------------------------------------------------------ */
/*        ClpDisplaySet
**
**        Synopsis:
**				ClpDisplaySet(displaySetOptionDisplayOn | displaySetOptionCursorOn);
**        Parameters:
**				BYTE bDisplaySetOptions	- display options
**					Possible options (to be OR-ed)
**						displaySetOptionDisplayOn - display ON
**						displaySetOptionCursorOn - cursor ON
**						displaySetBlinkOn - cursor blink ON
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Sets the display options. 
*/
void ClpDisplaySet(BYTE bDisplaySetOptions)
{
	ClpWriteCommand(cmdLcdCtlInit | bDisplaySetOptions);
}


/* ------------------------------------------------------------ */
/*        ClpDisplayClear
**
**        Synopsis:
**				ClpDisplayClear();
**        Parameters:
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Clears the display and returns the cursor home (upper left corner).
*/
void ClpDisplayClear()
{
	ClpWriteCommand(cmdLcdClear);
}



/* ------------------------------------------------------------ */
/*        ClpReturnHome
**
**        Synopsis:
**				ClpReturnHome();
**        Parameters:
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Returns the cursor home (upper left corner).
*/
void ClpReturnHome()
{
	ClpWriteCommand(cmdLcdRetHome);
}




/* ------------------------------------------------------------ */
/*        ClpInit
**
**        Synopsis:
**				ClpInit(displaySetOptionDisplayOn);
**        Parameters:
**				BYTE bDisplaySetOptions	- display options
**					Possible options (to be OR-ed)
**						displaySetOptionDisplayOn - display ON
**						displaySetOptionCursorOn - cursor ON
**						displaySetBlinkOn - cursor blink ON
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**			Performs the initializing (startup) sequence. It calls ClpDisplaySet providing bDisplaySetOptions argument.
*/
void ClpInit(BYTE bDisplaySetOptions)
{
	//	wait 20 ms;
	//  MB_Sleep(20);
	int i;
	for(i=0; i<20*ROUGH_1MS_DELAY; i++); 	// Rough time delay
	// Set function
	ClpWriteCommand(cmdLcdFcnInit);
	// Wait 37 us
	// MB_Sleep(1);
	for(i=0; i<ROUGH_1MS_DELAY; i++); 	// Rough time delay
	// Display Set
	ClpDisplaySet(bDisplaySetOptions);
	// Wait 37 us
	// MB_Sleep(1);
	for(i=0; i<ROUGH_1MS_DELAY; i++); 	// Rough time delay
	// Display Clear
	ClpDisplayClear();
	// Wait 1.52 ms
	// MB_Sleep(2);
	for(i=0; i<2*ROUGH_1MS_DELAY; i++); 	// Rough time delay
}



/* ------------------------------------------------------------ */
/*        ClpSetWriteDdramPosition
**
**        Synopsis:
**				ClpSetWriteDdramPosition(bAddrOffset);
**        Parameters:
**				BYTE bAdr - the write location. The position in DDRAM where the next data writes will put bytes.
**					0x00-0x27 refer to the first row
**					0x40-0x67 refer to the second row
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Sets the DDRAM write position. This is the location where the next data write will be performed.
**				Be aware that writing to a location auto-increments the write location.
*/
void ClpSetWriteDdramPosition(BYTE bAdr)
{
	BYTE bCmd = cmdLcdSetDdramPos | bAdr;
	ClpWriteCommand(bCmd);
}

/* ------------------------------------------------------------ */
/*        ClpWriteStringAtPos
**
**        Synopsis:
**				ClpWriteStringAtPos(szInfo1, 0, 0);
**        Parameters:
**				char *szLn	- string to be written
**				int idxLine	- line where the string will be written (0 or 1)
**				BYTE idxPos	- the starting position of the string within the line (0 to 39)
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Writes the specified string at the specified position on the specified line. 
**				It sets the corresponding write position and then writes data bytes as the device is ready.
**				Strings longer than 40 characters (0x27) are trimmed.
*/
void ClpWriteStringAtPos(char *szLn, int idxLine, BYTE idxPos)
{
	// crop string to 0x27 chars
	int len = strlen(szLn);
	if(len > 0x27)
	{
	szLn[0x27] = 0; // trim the string so it contains 40 characters 
		len = 0x27;
	}

	// Set write position
	BYTE bAddrOffset = (idxLine == 0 ? 0: 0x40) + idxPos;
	ClpSetWriteDdramPosition(bAddrOffset);

	BYTE bIdx = 0;
	while(bIdx < len)
	{
		ClpWriteDataByte(szLn[bIdx]);
		bIdx++;
	}
}

/* ------------------------------------------------------------ */
/*        ClpSetWriteCgramPosition
**
**        Synopsis:
**				ClpSetWriteCgramPosition(bAdr);
**        Parameters:
**				BYTE bAdr	- the write location. The position in CGRAM where the next data writes will put bytes.
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Sets the DDRAM write position. This is the location where the next data write will be performed.
**				Be aware that writing to a location auto-increments the write location.
*/
void ClpSetWriteCgramPosition(BYTE bAdr)
{
	BYTE bCmd = cmdLcdSetCgramPos | bAdr;
	ClpWriteCommand(bCmd);
}


/* ------------------------------------------------------------ */
/*        ClpWriteBytesAtPosCgram
**
**        Synopsis:
**				ClpWriteBytesAtPosCgram(userDefArrow, 8, posCgramChar0);
**        Parameters:
**				BYTE *pBytes	- pointer to the string of bytes
**				BYTE len		- the number of bytes to be written
**				BYTE bAdr		- the position in CGRAM where bytes will be written
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Writes the specified number of bytes to CGRAM starting at the specified position.
**				It sets the corresponding write position and then writes data bytes as the device is ready.
*/
void ClpWriteBytesAtPosCgram(BYTE *pBytes, BYTE len, BYTE bAdr)
{
	// Set write position
	ClpSetWriteCgramPosition(bAdr);

	// Write the string of bytes that define the character to CGRAM
	BYTE idx = 0;
	while(idx < len)
	{
		ClpWriteDataByte(pBytes[idx]);
		idx++;
	}
}


/* ------------------------------------------------------------ */
/*        ClpDisplayShift
**
**        Synopsis:
**				ClpDisplayShift(fBtn1Process);
**        Parameters:
**				BOOL fRight
**						- fTrue in order to shift right
**						- fFalse in order to shift left
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Shifts the display one position right or left, depending on the fRight parameter.
**
*/
void ClpDisplayShift(BOOL fRight)
{
	BYTE bCmd = cmdLcdDisplayShift | (fRight != fFalse ? mskShiftRL: 0);
	ClpWriteCommand(bCmd);
}


/* ------------------------------------------------------------ */
/*        ClpCursorShift
**
**        Synopsis:
**				ClpCursorShift(fBtn1Process);
**        Parameters:
**				BOOL fRight
**						- fTrue in order to shift right
**						- fFalse in order to shift left
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				Shifts the cursor one position right or left, depending on the fRight parameter.
**
*/
void ClpCursorShift(BOOL fRight)
{
	BYTE bCmd = cmdLcdCursorShift | (fRight != fFalse ? mskShiftRL: 0);
	ClpWriteCommand(bCmd);
}

/* ------------------------------------------------------------ */


