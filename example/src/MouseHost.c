/*
 * @brief Mouse Host
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * Copyright(C) Dean Camera, 2011, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "MouseHost.h"
#include "math.h"
#include "stdlib.h"
#define TIMER0_IRQ_HANDLER				TIMER0_IRQHandler  // TIMER0 interrupt IRQ function name
#define TIMER0_INTERRUPT_NVIC_NAME		TIMER0_IRQn        // TIMER0 interrupt NVIC interrupt name


#define max(a,b) \
({ __typeof__ (a) _a = (a); \
   __typeof__ (b) _b = (b); \
 _a > _b ? _a : _b; })

#define min(a,b) \
({ __typeof__ (a) _a = (a); \
   __typeof__ (b) _b = (b); \
 _a < _b ? _a : _b; })

uint8_t pins[7] = {0,1,2,4,5,6,8};
bool smile [7][5] = { {0,0,0,0,0}
					, {0,0,0,0,0}
					, {0,0,0,0,0}
					, {0,0,0,0,0}
					, {0,0,0,0,0}
					, {0,0,0,0,0}
					, {0,0,0,0,0}};
int buttons = 0;
const int LEFT_BUTTON = 1, RIGHT_BUTTON = 2, MIDDLE_BUTTON = 4;

//Current mouse location
double cx = 0;
double cy = 0;
//Previous mouse location
double pcy = 0;
double pcx = 0;
//Goal location
int goalX = 0, goalY = 0;

int points = 0;

int current = 0;

int maxX = 6;
int maxY = 4;

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/** LPCUSBlib HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
static USB_ClassInfo_HID_Host_t Mouse_HID_Interface = {
		.Config =
				{ .DataINPipeNumber = 1, .DataINPipeDoubleBank = false,

				.DataOUTPipeNumber = 2, .DataOUTPipeDoubleBank = false,

				.HIDInterfaceProtocol = HID_CSCP_MouseBootProtocol,
						.PortNumber = 0, }, };

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

bool test = false;
void TIMER0_IRQHandler(void)
{
	Chip_TIMER_Disable(LPC_TIMER0);		  // Stop TIMER0
	Chip_TIMER_Reset(LPC_TIMER0);		  // Reset TIMER0
	Chip_TIMER_ClearMatch(LPC_TIMER0,0);  // Clear TIMER0 interrupt


	if(current == 0){
		Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, pins[6]);
	}else{
		Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, pins[current - 1]);
	}

	int i = 0;
//	for(; i < 5; i++){
//		if(smile[current][i]){
//			Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 9 + i);
//		}else{
//			Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 9 + i);
//		}
//
//		Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 9 + i);
//	}
//
//	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 9);

//	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, pins[current++]);


	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 9 + pcy);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, pins[(int)pcx]);

	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, pins[(int)cx]);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 9 + cy);

	test = !test;
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, pins[(int)cx]);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 9 + cy);
	current %= 7;
	Chip_TIMER_Enable(LPC_TIMER0);
}

void RespawnGoal(){
	do{
		goalX = rand() % (maxX + 1);
		goalY = rand() % (maxY + 1);
	}while(goalX == (int)cx && goalY == (int)cy);
}

/* Mouse management task */
static void MouseHost_Task(void) {

	if (USB_HostState[Mouse_HID_Interface.Config.PortNumber]
			!= HOST_STATE_Configured) {
		return;
	}

	if (HID_Host_IsReportReceived(&Mouse_HID_Interface)) {
		USB_MouseReport_Data_t MouseReport;
		HID_Host_ReceiveReport(&Mouse_HID_Interface, &MouseReport);

		pcx = cx;
		pcy = cy;

		cx += MouseReport.X / 128.0;
		cx = max(0, cx);
		cx = min(maxX, cx);

		cy += MouseReport.Y / 128.0;
		cy = max(0, cy);
		cy = min(maxY, cy);

		if(goalX == (int)cx && goalY == (int)cy){
			RespawnGoal();
			points++;
			printf("You've gotten %d points!\r", points);
		}

		buttons = MouseReport.Button;
	}
}

/* Configures the board hardware and chip peripherals for the demo's functionality */
static void SetupHardware(void) {
	SystemCoreClockUpdate();
	Board_Init();
	Chip_USB_Init();
	USB_Init(Mouse_HID_Interface.Config.PortNumber, USB_MODE_Host);
	/* Hardware Initialization */
	Board_Debug_Init();
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

void AudioSetup(){
	// Audio set up
	I2S_AUDIO_FORMAT_T audio_Confg;
	uint8_t bufferUART, continue_Flag = 1;
	audio_Confg.SampleRate = 48000;
	//Select audio data to be 1 channel (1 is mono, 2 is stereo)
	audio_Confg.ChannelNumber = 1;
	//Select audio data to be 32 bits
	audio_Confg.WordWidth = 32;


	Board_Audio_Init(LPC_I2S, UDA1380_LINE_IN);
	Chip_I2S_Init(LPC_I2S);
	Chip_I2S_RxConfig(LPC_I2S, &audio_Confg);
	Chip_I2S_TxConfig(LPC_I2S, &audio_Confg);

	Chip_I2S_TxStop(LPC_I2S);
	Chip_I2S_DisableMute(LPC_I2S);
	Chip_I2S_TxStart(LPC_I2S);

	Chip_I2S_Int_RxCmd(LPC_I2S, ENABLE, 4);
	Chip_I2S_Int_TxCmd(LPC_I2S, ENABLE, 4);
}

void LEDSetup(){
	Chip_IOCON_PinMux(LPC_GPIO, 2, 0, IOCON_FUNC0, IOCON_MODE_INACT);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 1, IOCON_FUNC0, IOCON_MODE_INACT);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 2, IOCON_FUNC0, IOCON_MODE_INACT);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 3, IOCON_FUNC0, IOCON_MODE_INACT);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 4, IOCON_FUNC0, IOCON_MODE_INACT);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 5, IOCON_FUNC0, IOCON_MODE_INACT);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 6, IOCON_FUNC0, IOCON_MODE_INACT);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 8, IOCON_FUNC0, IOCON_MODE_INACT);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 9, IOCON_FUNC0,  1 << 10);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 10, IOCON_FUNC0, 1 << 10);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 11, IOCON_FUNC0,  1 << 10);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 12, IOCON_FUNC0,  1 << 10);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 13, IOCON_FUNC0,  1 << 10);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 14, IOCON_FUNC0,  1 << 10);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 15, IOCON_FUNC0,  1 << 10);
	Chip_IOCON_PinMux(LPC_GPIO, 2, 16, IOCON_FUNC0,  1 << 10);

	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 2, 1UL << 0 | 1UL << 1 | 1UL << 2 | 1UL << 3 | 1UL << 4| 1UL << 5 | 1UL << 6 | 1UL << 8
			| 1UL << 9 | 1UL << 10 | 1UL << 11 | 1UL << 12 | 1UL << 13 | 1UL << 14 | 1UL << 15 | 1UL << 16);

	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 0);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 1);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 2);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 3);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 4);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 5);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 6);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, 2, 8);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 9);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 10);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 11);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 12);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 13);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 14);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 15);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 16);
}

void TimerSetup(){
	int PrescaleValue = 1;
	Chip_TIMER_Init(LPC_TIMER0);					   // Initialize TIMER0
	Chip_TIMER_PrescaleSet(LPC_TIMER0,PrescaleValue);  // Set prescale value
	Chip_TIMER_SetMatch(LPC_TIMER0,0,100000);		   // Set match value so it trigger every second
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 0);		   // Configure to trigger interrupt on match

	// Enable Interrupt
	NVIC_ClearPendingIRQ(TIMER0_INTERRUPT_NVIC_NAME);
	NVIC_EnableIRQ(TIMER0_INTERRUPT_NVIC_NAME);

	Chip_TIMER_Enable(LPC_TIMER0);
}

void SendAudioData(){
	if ((Chip_I2S_GetTxLevel(LPC_I2S) < 4)) {
		int time = Chip_TIMER_ReadCount(LPC_TIMER0);
		int data = 0;
		if((buttons & LEFT_BUTTON) == LEFT_BUTTON)
			data += (int)(0x0FFFFFFF*sin(time/6000.0));
		if((buttons & RIGHT_BUTTON) == RIGHT_BUTTON)
			data += (int)(0x0FFFFFFF*sin(time/5000.0));
		if((buttons & MIDDLE_BUTTON) == MIDDLE_BUTTON)
			data += (int)(0x0FFFFFFF*sin(time/4000.0));
		Chip_I2S_Send(LPC_I2S, data);
	}
}

/**
 * @brief	Main program entry point
 * @return	Nothing
 * @note	This routine configures the hardware required by the application,
 * then enters a loop to run the application tasks in sequence.
 */
int main(void) {
	SetupHardware();

	AudioSetup();

	LEDSetup();

	TimerSetup();

	RespawnGoal();

	DEBUGOUT("Mouse Host running.\r\n");

	for (;;) {
		SendAudioData();

		MouseHost_Task();

		HID_Host_USBTask(&Mouse_HID_Interface);
		USB_USBTask(Mouse_HID_Interface.Config.PortNumber, USB_MODE_Host);
	}
}

/* This indicates that a device has been attached to the host,
 and starts the library USB task to begin the enumeration and USB
 management process. */
void EVENT_USB_Host_DeviceAttached(const uint8_t corenum) {
	DEBUGOUT(("Device Attached on port %d\r\n"), corenum);
}

/* This indicates that a device has been removed from the host,
 and stops the library USB task management process. */
void EVENT_USB_Host_DeviceUnattached(const uint8_t corenum) {
	DEBUGOUT(("\r\nDevice Unattached on port %d\r\n"), corenum);
}

/* This indicates that a device has been successfully
 enumerated by the host and is now ready to be used by the
 application. */
void EVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum) {

	uint16_t ConfigDescriptorSize;
	uint8_t ConfigDescriptorData[512];
	if (USB_Host_GetDeviceConfigDescriptor(corenum, 1, &ConfigDescriptorSize,
			ConfigDescriptorData, sizeof(ConfigDescriptorData))
			!= HOST_GETCONFIG_Successful) {
		DEBUGOUT("Error Retrieving Configuration Descriptor.\r\n");

		return;
	}

	Mouse_HID_Interface.Config.PortNumber = corenum;
	if (HID_Host_ConfigurePipes(&Mouse_HID_Interface, ConfigDescriptorSize,
			ConfigDescriptorData) != HID_ENUMERROR_NoError) {
		DEBUGOUT("Attached Device Not a Valid Mouse.\r\n");

		return;
	}

	if (USB_Host_SetDeviceConfiguration(
			Mouse_HID_Interface.Config.PortNumber, 1)
			!= HOST_SENDCONTROL_Successful) {
		DEBUGOUT("Error Setting Device Configuration.\r\n");

		return;
	}

	if (HID_Host_SetBootProtocol(&Mouse_HID_Interface) != 0) {
		DEBUGOUT("Could not Set Boot Protocol Mode.\r\n");

		USB_Host_SetDeviceConfiguration(
				Mouse_HID_Interface.Config.PortNumber, 0);
		return;
	}

	DEBUGOUT("Mouse Enumerated.\r\n");
}

/* This indicates that a hardware error occurred while in host mode. */
void EVENT_USB_Host_HostError(const uint8_t corenum, const uint8_t ErrorCode) {
	USB_Disable(corenum, USB_MODE_Host);

	DEBUGOUT(("Host Mode Error\r\n"
			" -- Error port %d\r\n"
			" -- Error Code %d\r\n"), corenum, ErrorCode);

	/* Wait forever */
	for (;;) {
	}
}

/* This indicates that a problem occurred while enumerating an
 attached USB device. */
void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t corenum,
		const uint8_t ErrorCode, const uint8_t SubErrorCode) {
	DEBUGOUT(("Dev Enum Error\r\n"
			" -- Error port %d\r\n"
			" -- Error Code %d\r\n"
			" -- Sub Error Code %d\r\n"
			" -- In State %d\r\n"), corenum, ErrorCode, SubErrorCode,
			USB_HostState[corenum]);
}

/* Dummy callback function for HID Parser */
bool CALLBACK_HIDParser_FilterHIDReportItem(
		HID_ReportItem_t * const CurrentItem) {
	return true;
}
