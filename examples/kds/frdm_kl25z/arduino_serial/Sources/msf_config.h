/****************************************************************************
 * @file     msf_config.h
 * @brief    User configuration for this application 
 * @note     This is the file included by user application. It specifies 
 * the configuration for this project and should be created separately for
 * each project.
 * The options which can easily be configured are located in this file directly.
 * Other options which can be configured with care are located in the
 * msf_config_mkl25z.h file included from the frdm_kl25z.h     
 *
 ******************************************************************************/
#ifndef MSF_CONFIG_H
	#define MSF_CONFIG_H

/* OPTION 1: Define the CPU frequency and BUS frequency.
 * To do this, look into Project_Settings\Startup_Code\system_MKL25Z4.c
 * and set the CLOCK_SETUP macro there to select one of the available CPU
 * frequencies. Then define the F_CPU below to match the selected frequency.
 *
 * Note: These CLOCK_SETUP values are supported: (values as defined in KDS 1.1.1)
 * CLOCK_SETUP =  1 or 4.
 * CLOCK_SETUP =  2
 * */
/* CLOCK_SETUP = 1; default value in new project
 * CLOCK_SETUP = 4; */
/*#define F_CPU		(48000000)
#define	F_BUS		(24000000)
*/

/* CLOCK_SETUP = 2; */
/*
#define F_CPU		(4000000)
#define	F_BUS		(800000)
*/
/* CLOCK_SETUP = 3; */
/*
#define F_CPU		(4000000)
#define	F_BUS		(1000000)
*/

/* CLOCK_SETUP = 0; */
#define F_CPU		(20970000)
#define	F_BUS		(20970000)


// TODO: Add support for 20970000 and 4 MHZ clock into MSF
// Older values supported by MSF but not available in system_MKL25Z4.c anymore:
// #define F_CPU == 20900000   /* BUS = 13,98 MHz */
	/*The default clock if no clock init is performed. We can clock UART from PLLFLLCLK which is 20.9 MHz
	 * Note that we assume PLLFLLSEL = 0 (MCGFLLCLK clock)!
 	 * BUS clock 13,98 MHz
 	 */
//#define   F_CPU   (41943040) 	/* not available in CLOCK_SETUP in KDS 1.1.1 */
	/* The F_CPU 41943040 was available before and is supported by MSF,
	 * so I keep it here for possible use.
	 * The settings were:
	 * Multipurpose Clock Generator (MCG) in FLL Engaged Internal (FEI) mode
       Reference clock source for MCG module is the slow internal clock source 32.768kHz
       Core clock = 41.94MHz, BusClock = 13.98MHz
	 */
//#define #define   F_CPU   (8000000)	/* not available in CLOCK_SETUP in KDS 1.1.1 */
/* The F_CPU 8000000 was available before and is supported by MSF,
	 * so I keep it here for possible use.
	 * The settings were:
		Multipurpose Clock Generator (MCG) in Bypassed Low Power External (BLPE) mode
         Core clock/Bus clock derived directly from an external crystal 8MHz with no multiplication
         Core clock = 8MHz, BusClock = 8MHz
*/


/*********************************************
*    Define the standard I/O channel 
********************************************/
#define   MSF_USE_STDIO     1   /* Using conio driver (UART) for std output */
/* To disable initialising UART in msf_init use:
#define   MSF_USE_STDIO     0 
*/
/* Define the default baudrate used by console I/O (conio).
 * This must be one of the values from the enum defined in msf_<device>.h. */
#define	 MSF_STDIO_BAUDRATE		(BD19200)

/*********************************************
*    Define whether we want to use analog inputs
*    If nonzero; ADC is initialised. This means 
*    little more power consumption, otherwise no
*    harm is done by enabling the analog inputs even
*    if not used 
********************************************/
#define MSF_USE_ANALOG      1

/*********************************************
*    Define which drivers should be created (used) 
*    It may save some memory to define drivers which 
*    your applicaiton will not use with 0.
********************************************/
#define MSF_DRIVER_UART0    1   /* Use UART0 driver (create UART0 driver instance) */
#define	MSF_DRIVER_UART1	1	/* Use UART1 driver */
#define	MSF_DRIVER_UART2	1	/* Use UART2 driver */
#define MSF_DRIVER_ADC0     1	/* Use ADC driver */
#define	MSF_DRIVER_TPM0		1	/* Use TPM0 driver */
#define	MSF_DRIVER_TPM1		1	/* Use TPM1 driver */
#define	MSF_DRIVER_TPM2		1	/* Use TPM2 driver */

/*********************************************
* Here you can select which pin is used for each UART Rx and Tx from
* the available pins for that UART.
* You will need to look into the datasheet into the Pinout chapter
* (in Signal Multiplexing and Signal Descriptions) to find out which
* pins are available for each UART.
* Or check the comment for each pin here :)
* Define the name of the pin as defined in msf_<device>.h for you MCU
* and the number of the ALTx function as found in the pinout table in
* the datasheet.
**********************************************/

/* UART0 pins */
#define		MSF_UART0_RX_PIN	(GPIO_A1)	/* PTA1/2; PTA15/3; PTB16/3; PTD6/3; PTE21/4; */
#define		MSF_UART0_RX_ALT	(2)
#define		MSF_UART0_TX_PIN	(GPIO_A2)	/* PTA2/2; PTA14/3; PTB17/3; PTD7/3; PTE20/4; */
#define		MSF_UART0_TX_ALT	(2)

/* UART1 pins */
#define		MSF_UART1_RX_PIN	(GPIO_E1)	/* PTC3/3; PTA18/3; PTE1/3; */
#define		MSF_UART1_RX_ALT	(3)
#define		MSF_UART1_TX_PIN	(GPIO_E0)	/* PTC4/3; PTA19/3; PTE0/3; */
#define		MSF_UART1_TX_ALT	(3)

/* UART2 pins */
#define		MSF_UART2_RX_PIN	(GPIO_E1)	/* PTD2/3; PTD4/3; PTE23/4; */
#define		MSF_UART2_RX_ALT	(3)
#define		MSF_UART2_TX_PIN	(GPIO_E0)	/* PTD3/3; PTD5/3; PTE22/4; */
#define		MSF_UART2_TX_ALT	(3)

/*********************************************
*   Define the PWM frequency for Arduino analogWrite
*	Define one of the values as 1, all others as 0
*	Note that the values in the macros are approximate (rounded), the
*	exact values are given in comment to analogWrite in Arduino.c 
********************************************/
#define	MSF_AWRITE_244HZ	0
#define	MSF_AWRITE_500HZ	1
#define	MSF_AWRITE_1KHZ		0
#define	MSF_AWRITE_2KHZ		0
#define	MSF_AWRITE_4KHZ		0
#define	MSF_AWRITE_8KHZ		0
#define	MSF_AWRITE_16KHZ	0
#define	MSF_AWRITE_32KHZ	0


/*********************************************
* Pin configuration for the timer TPMn drivers.
* Here you can select which pin is used for each channel from
* the available pins for this channel.
* You will need to look into the datasheet into the Pinout chapter
* (in Signal Multiplexing and Signal Descriptions) to find out which
* pins are available for each channel.
* Define the name of the pin as defined in msf_<device>.h for you MCU
* and the number of the ALTx function as found in the pinout table in 
* the datasheet.
**********************************************/

/* Arduino note:
 * For the channels which are available on multiple Arduino-pinout pins,
 * the lower pin numbers are pre-defined here. 
 * Definitions for the other pins are given in comments.
 * Exceptions are pins 0 and 1 which are used also for UART so we pre-define
 * the alternate pins 18 and 19 as PWM pins. If you want to use pin 0 and 1 
 * for PWM, un-comment the definitions for these pins below.
 * */

/* Timer TPM0 pins (6 channels are available) */
// Timer 0, channel 0
#define		MSF_TPM0_CH0_PIN	(GPIO_D0)	// Arduino pin 10 
#define		MSF_TPM0_CH0_ALT	(4)
/*
#define		MSF_TPM0_CH0_PIN	(GPIO_C1)	// Arduino pin 21
#define		MSF_TPM0_CH0_ALT	(4)
*/

// Timer 0, channel 1
#define		MSF_TPM0_CH1_PIN	(GPIO_A4)	// Arduino pin 4
#define		MSF_TPM0_CH1_ALT	(3)
/*
#define		MSF_TPM0_CH1_PIN	(GPIO_D1)	// Arduino pin 13
#define		MSF_TPM0_CH1_ALT	(4)

#define		MSF_TPM0_CH1_PIN	(GPIO_C2)	// Arduino pin 20
#define		MSF_TPM0_CH1_ALT	(4)
*/

// Timer 0, channel 2
#define		MSF_TPM0_CH2_PIN	(GPIO_A5)	// Arduino pin 5
#define		MSF_TPM0_CH2_ALT	(3)
/*
#define		MSF_TPM0_CH2_PIN	(GPIO_D2)	// Arduino pin 11
#define		MSF_TPM0_CH2_ALT	(4)
*/

// Timer 0, channel 3
#define		MSF_TPM0_CH3_PIN	(GPIO_D3)	// Arduino pin 12
#define		MSF_TPM0_CH3_ALT	(4)

// Timer 0, channel 4
#define		MSF_TPM0_CH4_PIN	(GPIO_D4)	// Arduino pin 2
#define		MSF_TPM0_CH4_ALT	(4)
/*
#define		MSF_TPM0_CH4_PIN	(GPIO_C8)	// Arduino pin 6
#define		MSF_TPM0_CH4_ALT	(3)
*/

// Timer 0, channel 5
#define		MSF_TPM0_CH5_PIN	(GPIO_C9)	// Arduino pin 7
#define		MSF_TPM0_CH5_ALT	(3)
/*
#define		MSF_TPM0_CH5_PIN	(GPIO_D5)	// Arduino pin 9
#define		MSF_TPM0_CH5_ALT	(4)
*/


/* Timer TPM1 pins (only 2 channels on KL25) */
// Timer 1, channel 0
#define		MSF_TPM1_CH0_PIN	(GPIO_A12)	// Arduino pin 3 /*PTE20/3 ;PTA12/3; PTB0/3*/
#define		MSF_TPM1_CH0_ALT	(3)
// Timer 1, channel 1
#define		MSF_TPM1_CH1_PIN	(GPIO_A13)	// Arduino pin 8 /*PTE21/3; PTA13/3; PTB1/3*/
#define		MSF_TPM1_CH1_ALT	(3)

/* Timer TPM2 pins (only 2 channels on KL25) */
// Timer 2, channel 0
#define		MSF_TPM2_CH0_PIN	(GPIO_B2)	// Arduino pin 18  /*B18/3; PTA1/3; PTB2/3; PTE22/3*/
#define		MSF_TPM2_CH0_ALT	(3)
/*
#define		MSF_TPM2_CH0_PIN	(GPIO_A1)	// Arduino pin 0
#define		MSF_TPM2_CH0_ALT	(3)
*/
// Timer 2, channel 1
#define		MSF_TPM2_CH1_PIN	(GPIO_B3)  // Arduino pin 19  /*B19/3; PTA2/3; PTB3/3; PTE22/3*/
#define		MSF_TPM2_CH1_ALT	(3)
/*
#define		MSF_TPM2_CH1_PIN	(GPIO_A2)  // Arduino pin 1
#define		MSF_TPM2_CH1_ALT	(3)
*/


 /* Include the header file for our board */
#include "frdm_kl25z.h" 

#endif  /* MSF_CONFIG_H */
