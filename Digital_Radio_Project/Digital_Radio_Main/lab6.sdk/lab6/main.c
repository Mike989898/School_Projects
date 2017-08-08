/*
 * Lab 3
 * Mike Kilgore and Josh Feiber, Engs 128 17S
 *
 * Operating instructions:
 *
 * Connect DAC to Pmod JD, ADC to Pmod JE, and LCD to usual connections (JB and JC).
 *
 * BTN0 increments the frequency, BTN1 decrements the frequency, and BTN2 sets the frequency to 0.
 *
 * SW0 in the off position sets the frequency step size to 10, SW0 in the on position sets the frequency step
 * size to 100.
 *
 */


/*
 * VERSION HISTORY
********************************************************************************************
* 	v1.0: 29 March 2017
*		First version created.
*******************************************************************************************/

#include "xparameters.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"

#include <string.h>
#include "stl_printf.h"				// Tiny footprint (tfp) printf and sprintf
#include "stdtypes.h"				// Used by ClpLib
#include "ClpLib.h"	 				// Interface to LCD
#include "s_axis_mm2s.h"

/* Definitions */
#define COUNTER_DELAY 20000000		// Software delay length

// Parameter definitions
#define INTC_DEVICE_ID 		XPAR_PS7_SCUGIC_0_DEVICE_ID
#define BTNS_SWS_DEVICE_ID		XPAR_AXI_GPIO_0_DEVICE_ID
#define INTC_GPIO_INTERRUPT_ID XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR
#define S_AXIS_MM2S_BASE XPAR_DDS_S_AXIS_MM2S_0_S00_AXI_BASEADDR

#define BTN_CHAN 1
#define SWS_CHAN 2

#define BTN_INT 			XGPIO_IR_CH1_MASK // CH1 corresponds to the buttons. CH2 would be the switches.
#define BTN0_MASK 0x1
#define BTN1_MASK 0x2
#define BTN2_MASK 0x4

#define SW0_MASK 0x1

#define FREQ_UPPER_BOUND 200000
#define FREQ_LOWER_BOUND 0
#define COARSE_INCREMENT 100000
#define FINE_INCREMENT 100

#define two_thirty_two 0xFFFFFFFF
#define F_CLK 960000


XGpio BTNSWSInst;
XScuGic INTCInst;
static int freq = 0;

/************************** Variable Definitions *****************************/

//----------------------------------------------------
// PROTOTYPE FUNCTIONS
//----------------------------------------------------

static void BTN_Intr_Handler(void *baseaddr_p);
static int InterruptSystemSetup(XScuGic *XScuGicInstancePtr);
static int IntcInitFunction(u16 DeviceId, XGpio *GpioInstancePtr);


//----------------------------------------------------
// INTERRUPT HANDLER FUNCTIONS
//----------------------------------------------------

// Handles the button interrupt.
void BTN_Intr_Handler(void *InstancePtr)
{
	// Disable GPIO interrupts
	XGpio_InterruptDisable(&BTNSWSInst, BTN_INT);
	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus(&BTNSWSInst) & BTN_INT) !=
			BTN_INT) {
			return;
		}
	// Read the value of the button
	int btn_val = XGpio_DiscreteRead(&BTNSWSInst, BTN_CHAN);
	// Read the value of the switch
	int switch_val = XGpio_DiscreteRead(&BTNSWSInst, SWS_CHAN);
	// If the switch toggle is pulled toward the bottom of the board increment by 100, otherwise by 10
	int switch_inc_val = (switch_val & SW0_MASK) ? COARSE_INCREMENT : FINE_INCREMENT;

	// If btn 0 was pressed increment if the counter isn't saturated
	if ((btn_val & BTN0_MASK) && (freq + switch_inc_val) <= FREQ_UPPER_BOUND) freq += switch_inc_val;
	// If btn 1 was pressed, decrement the counter if doing so doesn't make freq go below FREQ_LOWER_BOUND
	else if ((btn_val & BTN1_MASK) && (freq - switch_inc_val) >= FREQ_LOWER_BOUND) freq -= switch_inc_val;
	// If btn 2 was pressed, reset the freq to FREQ_LOWER_BOUND
	else if (btn_val & BTN2_MASK) freq = FREQ_LOWER_BOUND;

	// Convert freq to the 32-bit phase increment
	uint32_t p = (two_thirty_two/F_CLK)*freq;

	// Write the phase increment to the s_axis_mm2s block
	S_AXIS_MM2S_mWriteReg(S_AXIS_MM2S_BASE, 0, p);


    (void)XGpio_InterruptClear(&BTNSWSInst, BTN_INT);
    // Enable GPIO interrupts
    XGpio_InterruptEnable(&BTNSWSInst, BTN_INT);
}


//----------------------------------------------------
// MAIN FUNCTION
//----------------------------------------------------
int main (void)
{
  int status;

  //-------------------------------------------------
  // INITIALIZE THE PERIPHERALS
  //-------------------------------------------------
  // Set up the LCD display
  ClpPinsConfigure();
  ClpInit(displaySetOptionDisplayOn);

  // Initialise Push Buttons
  status = XGpio_Initialize(&BTNSWSInst, BTNS_SWS_DEVICE_ID);
  if(status != XST_SUCCESS) return XST_FAILURE;
  // Set all buttons direction to inputs
  XGpio_SetDataDirection(&BTNSWSInst, BTN_CHAN, 0xFF);

  //Set all switch direction to inputs
  XGpio_SetDataDirection(&BTNSWSInst, SWS_CHAN, 0xFF);

  // Initialize interrupt controller
  status = IntcInitFunction(INTC_DEVICE_ID, &BTNSWSInst);
  if(status != XST_SUCCESS) return XST_FAILURE;

  ClpWriteStringAtPos("Engs 128, Lab 2", 0, 0);
  
  //------------------------------------------------
  // INFINITE EVENT LOOP
  //------------------------------------------------
  char CtrString[40];
  int Delay;
  while(1) {
		tfp_sprintf(CtrString, "Freq: %u%02u.%ukHz", freq/100000, (freq % 100000)/1000, ((freq % 100000) % 1000)/100);		// Format counter
		ClpWriteStringAtPos(CtrString, 1, 0);				// Write count to LCD
		for (Delay = 0; Delay < COUNTER_DELAY; Delay++);	// and wait a little
  }

  return XST_SUCCESS;
}

//----------------------------------------------------
// INITIAL SETUP FUNCTIONS
//----------------------------------------------------

int InterruptSystemSetup(XScuGic *XScuGicInstancePtr)
{
	// Enable interrupt
	XGpio_InterruptEnable(&BTNSWSInst, BTN_INT);
	XGpio_InterruptGlobalEnable(&BTNSWSInst);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 	 	 	 	 	 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
			 	 	 	 	 	 XScuGicInstancePtr);
	Xil_ExceptionEnable();


	return XST_SUCCESS;

}

int IntcInitFunction(u16 DeviceId, XGpio *GpioInstancePtr)
{
	XScuGic_Config *IntcConfig;
	int status;

	// Interrupt controller initialisation
	IntcConfig = XScuGic_LookupConfig(DeviceId);
	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig, IntcConfig->CpuBaseAddress);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Call to interrupt setup
	status = InterruptSystemSetup(&INTCInst);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Connect GPIO interrupt to handler
	status = XScuGic_Connect(&INTCInst,
					  	  	 INTC_GPIO_INTERRUPT_ID,
					  	  	 (Xil_ExceptionHandler)BTN_Intr_Handler,
					  	  	 (void *)GpioInstancePtr);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Enable GPIO interrupts interrupt
	XGpio_InterruptEnable(GpioInstancePtr, 1);
	XGpio_InterruptGlobalEnable(GpioInstancePtr);

	// Enable GPIO and timer interrupts in the controller
	XScuGic_Enable(&INTCInst, INTC_GPIO_INTERRUPT_ID);

	return XST_SUCCESS;
}

