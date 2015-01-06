/****************************************************************************
 * @file     msf_mkl25z.h
 * @brief    device definitions for Kinetis M0+ MKL25Z MCU  
 * @version  1
 * @date     29. Apr 2014
 *
 * @note     Contains definitions specific for this device.
 *          If more compilers should be supported, use #ifdef,...   
 *  For clock initialization we use CMSIS SystemInit() implemented in
 *  Freescale's system_MKL25Z4.c which supports 3 clock options:
 *  1) internal osc; Core clock = 41.94MHz, BusClock = 13.98MHz
 *  2) external 8 MHz crystal (present on FRDM-KL25Z board),
 *     Core clock = 48MHz, BusClock = 24MHz
 *  3) external crystal directly,  Core clock = 8MHz, BusClock = 8MHz
 *
 *  If no clock initialization is used (such as with the standard startup
 *  code generated by CW project wizard, the default CPU clock after reset 
 *  is Core clock 20.9 MHZ (F_CPU = 20900000)
 *
 ******************************************************************************/
#ifndef MSF_MKL25Z_H
#define MSF_MKL25Z_H


#ifdef __cplusplus
extern "C" {
#endif


  
/* ------------  GPIO Pin configuration for the Kinetis derivatives--------- */
/* MKL25Z has ports A through E 
 * We encode the pin as port number (A=0, B=1,...) and pin number (0 - 31) 
 * */
typedef enum {
	GPIO_INVALID_PIN = 0x0000,
	GPIO_A0 = GPIO_MAKE_PINCODE(PORT_A, 0),
	GPIO_A1 = GPIO_MAKE_PINCODE(PORT_A, 1),
	GPIO_A2 = GPIO_MAKE_PINCODE(PORT_A, 2),
	/* note that PTA0 and PTA2 have non-standard pull resistor reset value,
	 see Signal multiplexing chapter in the data sheet. */
	GPIO_A3 = GPIO_MAKE_PINCODE(PORT_A, 3),
	GPIO_A4 = GPIO_MAKE_PINCODE(PORT_A, 4),
	GPIO_A5 = GPIO_MAKE_PINCODE(PORT_A, 5),
	/* Pins PTA6 - PTA11 are not available in any package */	
	GPIO_A12 = GPIO_MAKE_PINCODE(PORT_A, 12),
	GPIO_A13 = GPIO_MAKE_PINCODE(PORT_A, 13),
	GPIO_A14 = GPIO_MAKE_PINCODE(PORT_A, 14),
	GPIO_A15 = GPIO_MAKE_PINCODE(PORT_A, 15),
	GPIO_A16 = GPIO_MAKE_PINCODE(PORT_A, 16),
	GPIO_A17 = GPIO_MAKE_PINCODE(PORT_A, 17),
	GPIO_A18 = GPIO_MAKE_PINCODE(PORT_A, 18),
	GPIO_A19 = GPIO_MAKE_PINCODE(PORT_A, 19),
	GPIO_A20 = GPIO_MAKE_PINCODE(PORT_A, 20),
	/* no more pins on port A */
	GPIO_B0 = GPIO_MAKE_PINCODE(PORT_B, 0),	
	GPIO_B1 = GPIO_MAKE_PINCODE(PORT_B, 1),
	GPIO_B2 = GPIO_MAKE_PINCODE(PORT_B, 2),
	GPIO_B3 = GPIO_MAKE_PINCODE(PORT_B, 3),
	/* pins PTB4 - PTB7 N/A... */
	GPIO_B8 = GPIO_MAKE_PINCODE(PORT_B, 8),
	GPIO_B9 = GPIO_MAKE_PINCODE(PORT_B, 9),
	GPIO_B10 = GPIO_MAKE_PINCODE(PORT_B, 10),
	GPIO_B11 = GPIO_MAKE_PINCODE(PORT_B, 11),
	/* pins PTB12 - PTB15 N/A... */
	GPIO_B16 = GPIO_MAKE_PINCODE(PORT_B, 16),
	GPIO_B17 = GPIO_MAKE_PINCODE(PORT_B, 17),
	GPIO_B18 = GPIO_MAKE_PINCODE(PORT_B, 18),	/* this is red LED on FRDM-KL25Z */
	GPIO_B19 = GPIO_MAKE_PINCODE(PORT_B, 19),	/* this is green LED on FRDM-KL25Z */
	/* no more pins on port B */
	GPIO_C1 = GPIO_MAKE_PINCODE(PORT_C, 1),
	GPIO_C2 = GPIO_MAKE_PINCODE(PORT_C, 2),
	GPIO_C3 = GPIO_MAKE_PINCODE(PORT_C, 3),
	GPIO_C4 = GPIO_MAKE_PINCODE(PORT_C, 4),
	GPIO_C5 = GPIO_MAKE_PINCODE(PORT_C, 5),
	GPIO_C6 = GPIO_MAKE_PINCODE(PORT_C, 6),
	GPIO_C7 = GPIO_MAKE_PINCODE(PORT_C, 7),
	GPIO_C8 = GPIO_MAKE_PINCODE(PORT_C, 8),
	GPIO_C9 = GPIO_MAKE_PINCODE(PORT_C, 9),
	GPIO_C10 = GPIO_MAKE_PINCODE(PORT_C, 10),
	GPIO_C11 = GPIO_MAKE_PINCODE(PORT_C, 11),
	GPIO_C12 = GPIO_MAKE_PINCODE(PORT_C, 12),
	GPIO_C13 = GPIO_MAKE_PINCODE(PORT_C, 13),
	/* pins PTC14 - PTC15 N/A... */
	GPIO_C16 = GPIO_MAKE_PINCODE(PORT_C, 16),
	GPIO_C17 = GPIO_MAKE_PINCODE(PORT_C, 17),
	/* no more pins on port C */
	GPIO_D0 = GPIO_MAKE_PINCODE(PORT_D, 0),	
	GPIO_D1 = GPIO_MAKE_PINCODE(PORT_D, 1),	/* this is blue LED on FRDM-KL25Z */
	GPIO_D2 = GPIO_MAKE_PINCODE(PORT_D, 2),	
	GPIO_D3 = GPIO_MAKE_PINCODE(PORT_D, 3),
	GPIO_D4 = GPIO_MAKE_PINCODE(PORT_D, 4),
	GPIO_D5 = GPIO_MAKE_PINCODE(PORT_D, 5),
	GPIO_D6 = GPIO_MAKE_PINCODE(PORT_D, 6),
	GPIO_D7 = GPIO_MAKE_PINCODE(PORT_D, 7),
	/* no more pins on port D */
	GPIO_E0 = GPIO_MAKE_PINCODE(PORT_E, 0),
	GPIO_E1 = GPIO_MAKE_PINCODE(PORT_E, 1),
	GPIO_E2 = GPIO_MAKE_PINCODE(PORT_E, 2),
	GPIO_E3 = GPIO_MAKE_PINCODE(PORT_E, 3),
	GPIO_E4 = GPIO_MAKE_PINCODE(PORT_E, 4),
	GPIO_E5 = GPIO_MAKE_PINCODE(PORT_E, 5),
	/* pins PTE6 - PTE19 N/A... */
	GPIO_E20 = GPIO_MAKE_PINCODE(PORT_E, 20),
	GPIO_E21 = GPIO_MAKE_PINCODE(PORT_E, 21),
	GPIO_E22 = GPIO_MAKE_PINCODE(PORT_E, 22),
	GPIO_E23 = GPIO_MAKE_PINCODE(PORT_E, 23),
	GPIO_E24 = GPIO_MAKE_PINCODE(PORT_E, 24),
	GPIO_E25 = GPIO_MAKE_PINCODE(PORT_E, 25),
	/* pins PTE26 - PTE28 N/A... */
	GPIO_E29 = GPIO_MAKE_PINCODE(PORT_E, 29),
	GPIO_E30 = GPIO_MAKE_PINCODE(PORT_E, 30),
	GPIO_E31 = GPIO_MAKE_PINCODE(PORT_E, 31),
	/* no more pins */
	GPIO_PINS_END = 0xFFFF,	/* */

} MCU_pin_type; 



/* -------------- End Pin definitions  --------------- */


/* -------------- UART definitions  --------------- */

/** @addtogroup group_uart  
 * @{    
 */ 

/* Clocks for UARTs:
 * UART0 - the clock can be MCGIRCLK, OSCERCLK, MCGFLLCLK, MCGPLLCLK/2
 * UART1, UART2 - the clock is always bus clock
 *
 * Note that the baud rate is different for UART0 than for UART1/2
 * UART0 Baud rate is:
 * baud = uart_clock / ((OSR+1)xBR)
 * BR is baud rate divisor set in baud register UART_BDH (1 - 8191)
 * OSR is oversampling set in UART_C4 register ( 4 - 32)
 * We define the enum value as the BD in lower 13-bits; OSR in bits 13-17 
 * and BR for or UART1/2 in bits 18-30, so that we can use the same enum for all UARTs
 * UART1/2 clock is BUS clock (F_BUS)
 * BR = BUSCLK / (16 x baud)
 * */
/** macro which creates 32-bit value by combining OSR and BR values.
 * osr is the real OSR value; the register C4 stores osr-1 (value %11 (3) in OSR means real osr = 4) */
#define		UART_MAKE_BDVAL(osr, br, uart1_br )	((br & 0x00001FFF) | ((osr & 0x1F) << 13) \
														| ((uart1_br & 0x00001FFF) << 18))
#define		UART_GET_BR(baud_val)		(baud_val & 0x00001FFF)				/* the BR is actually only 13-bit long*/
#define		UART_GET_OSR(baud_val)		((baud_val >> 13) & 0x0000001F)		/* OSR is only 5-bits */
#define		UART_GET_BR_UART1(baud_val)	((baud_val & 0x7FFC0000) >> 18)			/* the BR for UART1/2, 13-bit long*/

#if F_CPU == 48000000
/* CLOCK_SETUP = 1 or 4 in system_MKL25Z4.c (CMSIS);
  the UART0 must be clocked from OSCERCLK, because
  PLLFLLCLK is disabled; the UART0 clock is 8 MHz (external crystal) */ 
typedef enum  
 {
        BD_INVALID = 0,
        BD2400 = UART_MAKE_BDVAL(14, 238, 625),	 
        BD4800 = UART_MAKE_BDVAL(14, 119, 314),	
        BD9600 = UART_MAKE_BDVAL(13, 64, 156),	        
        BD19200 = UART_MAKE_BDVAL(13, 32, 78),
        BD38400 = UART_MAKE_BDVAL(13, 16, 39),
        BD57600 = UART_MAKE_BDVAL(14, 10, 26),
        BD115200 = UART_MAKE_BDVAL(14, 5, 13),        
 } UART_baudrate_type;
 
/** The value of the UART0SRC bitfield in SIM_SOPT2 */
#define	MSF_UART0_CLKSEL	(2) /* OSCERCLK as UART0 clock source */
 
#elif F_CPU == 4000000
 /* There are 2 options for 4 MHz F_CPU with different clock source and different F_BUS */
 #if F_BUS == 800000
	 /* CLOCK_SETUP = 2 in system_MKL25Z4.c (CMSIS);
	 Multipurpose Clock Generator (MCG) in Bypassed Low Power Internal (BLPI) mode.
	 Core clock/Bus clock derived directly from an fast internal clock 4MHz with no multiplication
	 Core clock = 4MHz, BusClock = 0.8MHz
	 UART0 must be clocked from MCGIRCLK, UART0 clock is 4 MHz
	 */
	 typedef enum
	 {
		 BD_INVALID = 0,
		 BD2400 = UART_MAKE_BDVAL(17, 98, 21),
		 BD4800 = UART_MAKE_BDVAL(17, 49, 0),		/* BR for UART1: 10.42, Error 4.2% */
		 BD9600 = UART_MAKE_BDVAL(16, 26, 0),		/* BR for UART1: 5.21, Error 4.2% */
		 BD19200 = UART_MAKE_BDVAL(16, 13, 0),  	/* BR for UART1: 2.6 */
		 BD38400 = UART_MAKE_BDVAL(8, 13, 0),  		/* BR for UART1: 1.3 */
		 BD57600 = UART_MAKE_BDVAL(10, 7, 0), 	/* BR for UART1: ? */
		 BD115200 = UART_MAKE_BDVAL(7, 5, 0), 	/* BR for UART1: 0.43 */
	 } UART_baudrate_type;

	 /** The value of the UART0SRC bitfield in SIM_SOPT2 */
	 #define	MSF_UART0_CLKSEL	(3)	/* MCGIRCLK as UART0 clock source */

  #elif F_BUS == 1000000
	  /* CLOCK_SETUP = 3 in system_MKL25Z4.c (CMSIS);
	  Multipurpose Clock Generator (MCG) in Bypassed Low Power External (BLPE) mode
	  Core clock/Bus clock derived directly from the external crystal 8MHz with no multiplication
	  The clock settings is ready for Very Low Power Run mode.
	  Core clock = 4MHz, BusClock = 1MHz
	  UART0 clock is 8 MHz (external crystal)
	  */
	  typedef enum
	  {
		 BD_INVALID = 0,
		 BD2400 = UART_MAKE_BDVAL(14, 238, 26),
		 BD4800 = UART_MAKE_BDVAL(14, 119, 13),
		 BD9600 = UART_MAKE_BDVAL(13, 64, 0), /* BR for UART1: 6.51 */
		 BD19200 = UART_MAKE_BDVAL(13, 32, 0), /* BR for UART1: 3.26 */
		 BD38400 = UART_MAKE_BDVAL(13, 16, 0), /* BR for UART1: 1.63 */
		 BD57600 = UART_MAKE_BDVAL(14, 10, 0), /* BR for UART1: 1.09, Error 10% */
		 BD115200 = UART_MAKE_BDVAL(14, 5, 0), /* BR for UART1: 0.54 */
	  } UART_baudrate_type;

	  /** The value of the UART0SRC bitfield in SIM_SOPT2 */
	  #define	MSF_UART0_CLKSEL	(2) /* OSCERCLK as UART0 clock source */

 #else
	#error The F_BUS clock for F_CPU 4 MHz is not supported.
 #endif

#elif F_CPU == 20970000
/* CLOCK_SETUP = 0 in system_MKL25Z4.c (CMSIS);
 * Multipurpose Clock Generator (MCG) in FLL Engaged Internal (FEI) mode
   Default  part configuration.
   Reference clock source for MCG module is the slow internal clock source 32.768kHz
   Core clock = 20.97MHz, BusClock = 20.97MHz
   UART0 clock is 20.97MHz
 */
  typedef enum
  {
		 BD_INVALID = 0,
		 BD2400 = UART_MAKE_BDVAL(18, 485, 546),
		 BD4800 = UART_MAKE_BDVAL(18, 242, 273),
		 BD9600 = UART_MAKE_BDVAL(11, 198, 137),
		 BD19200 = UART_MAKE_BDVAL(11, 99, 68),
		 BD38400 = UART_MAKE_BDVAL(17, 32, 34),
		 BD57600 = UART_MAKE_BDVAL(11, 33, 23),
		 BD115200 = UART_MAKE_BDVAL(13, 14, 0),	/* UART1 BR = 11.4 error is 3.6% with BR = 11*/
  } UART_baudrate_type;

  /** The value of the UART0SRC bitfield in SIM_SOPT2 */
  #define	MSF_UART0_CLKSEL	(1)	/* PLLFLLCLK as UART0 clock source */

 
/* *******************************
 * IMPORTANT NOTE:
 * the F_CPU below were valid with CMSIS system_MKL25Z4.c file CLOCK_SETUP definitions
 * provided with KDS/CodeWarrior earlier; in KDS 1.1.1 the settings for the clock options
 * changed. The comments below with CLOCK_SETUP values are NOT valid for KDS 1.1.1 */
#elif F_CPU == 2090000		/* default for frdm-kl25z */
/* CLOCK_SETUP = 3 
 * The default clock if no clock init is performed. We can clock UART from PLLFLLCLK which is 20.9 MHz
 * Note that we assume PLLFLLSEL = 0 (MCGFLLCLK clock)!  
 BUS clock 13,98 MHz */
 typedef enum  
 {
        BD_INVALID = 0,
        BD2400 = UART_MAKE_BDVAL(18, 484, 364),	 
        BD4800 = UART_MAKE_BDVAL(18, 242, 182),	
        BD9600 = UART_MAKE_BDVAL(11, 198, 91),
        BD19200 = UART_MAKE_BDVAL(11, 99, 46),
        BD38400 = UART_MAKE_BDVAL(17, 32, 23),
        BD57600 = UART_MAKE_BDVAL(11, 33, 15),
        BD115200 = UART_MAKE_BDVAL(13, 14, 0),	/* UART1 error is 5.5% with BR = 8*/        
 } UART_baudrate_type;

 /** The value of the UART0SRC bitfield in SIM_SOPT2 */
 #define	MSF_UART0_CLKSEL	(1)	/* PLLFLLCLK as UART0 clock source */
 
 
 
#elif F_CPU == 8000000		
 /* CLOCK_SETUP = 2 in system_MKL25Z4.c (CMSIS); the UART0 must be clocked from OSCERCLK, because 
   PLLFLLCLK is disabled; the UART0 clock is 8 MHz (external crystal)  
 BUS clock 8 MHz */
 typedef enum  
 {
	 /* same values as for the 48 MHz F_CPU */
	 BD_INVALID = 0,
	 BD2400 = UART_MAKE_BDVAL(14, 238, 208),	 
	 BD4800 = UART_MAKE_BDVAL(14, 119, 104),	
	 BD9600 = UART_MAKE_BDVAL(13, 64, 52),	        
	 BD19200 = UART_MAKE_BDVAL(13, 32, 26),
	 BD38400 = UART_MAKE_BDVAL(13, 16, 13),
	 BD57600 = UART_MAKE_BDVAL(14, 10, 0), /* 9, Error is 3.7% */
	 BD115200 = UART_MAKE_BDVAL(14, 5, 0), /* 4, Error is 8% */         
 } UART_baudrate_type;

 /** The value of the UART0SRC bitfield in SIM_SOPT2 */
 #define	MSF_UART0_CLKSEL	(2)	/* OSCERCLK as UART0 clock source */
 

/* Core clock = 41.94MHz */
#elif F_CPU == 41943040		
 /* CLOCK_SETUP = 0 in system_MKL25Z4.c (CMSIS); 
  * UART0 clock = F_CPU, is clocked from PLLFLLCLK 
  * Note that we assume PLLFLLSEL = 0 (MCGFLLCLK clock)! 
  * Bus clock 5.225 MHz */  
 typedef enum  
 {	 
	 BD_INVALID = 0,
	 BD2400 = UART_MAKE_BDVAL(32, 546, 136 ),	 
	 BD4800 = UART_MAKE_BDVAL(32, 273, 68),	 
	 BD9600 = UART_MAKE_BDVAL(16, 273, 34),	 
	 BD19200 = UART_MAKE_BDVAL(14, 156, 17 ),
	 BD38400 = UART_MAKE_BDVAL(14, 78, 0 ),
	 BD57600 = UART_MAKE_BDVAL(14, 52, 0),
	 BD115200 = UART_MAKE_BDVAL(13, 28, 0),
	 BD230400 = UART_MAKE_BDVAL(14, 13, 0),
 } UART_baudrate_type;
 
 /** The value of the UART0SRC bitfield in SIM_SOPT2 */
 #define	MSF_UART0_CLKSEL	(1)	/* PLLFLLCLK as UART0 clock source */
 
 
#else
	#error The CPU clock defined by F_CPU is not supported.
#endif


/** @}*/
/* -------------- End UART definitions  --------------- */


/* -------------- Main "system" timer definitions  --------------- */
/** The number in VAL register which equals to 1 us.
 * This is actually also the number of clock cycles which equals to 1 us.
 * We assume the F_CPU is always >= 1 MHz and so this value is always >= 1.
 * We assume SysTick interrupt every ms.
 * Example: F_CPU = 8 MHz, the counter counts from 8000 to 0 to generate interrupt
 * every ms. The MSF_SYSTICK_VALINUS = 8
 * Note1: that we do not need to use #if for F_CPU since CMSIS provides F_CPU in a variable. 
 * Note2: this will not be exact for F_CPU which is not in whole MHz*/
#define		MSF_SYSTICK_VALINUS		(SystemCoreClock/1000000u)
 

#if F_CPU == 41943040
	#undef MSF_SYSTICK_VALINUS
	#define MSF_SYSTICK_VALINUS		(42)	/* much better than value computed by F_CPU/x with integers */
#elif ((F_CPU == 20900000) || (F_CPU == 20970000))
	#undef MSF_SYSTICK_VALINUS
	#define MSF_SYSTICK_VALINUS		(21)	
#endif 
 
 
/** how many clocks there are in one microsecond 
 * NOTE that we need this as compile-time constant to make the error in msf_delay_us
 * as small as possible */
#if ((F_CPU == 20900000) || (F_CPU == 20970000))
	#define	MSF_CLOCKS_PER_US		(21)
	#define	WMSF_DELAYUS_OVERHEAD	(6)
#elif F_CPU == 41943040	
	#define	MSF_CLOCKS_PER_US		(42)
	#define	WMSF_DELAYUS_OVERHEAD	(3)
#elif F_CPU == 48000000
	#define	MSF_CLOCKS_PER_US		(48)
	#define WMSF_DELAYUS_OVERHEAD  (2)
#elif F_CPU == 4000000
	#define	MSF_CLOCKS_PER_US		(4)
	#define WMSF_DELAYUS_OVERHEAD  (30)	/* TODO: this is not tested! */
#endif 

/* -------------- End Main "system" timer definitions  --------------- */


/* -------------- ADC definitions  --------------- */
 /** Analog pin definitions 
  * @note This is MSF-global rather then ADC driver specific so that it can be used
  * in the global function msf_analog_read.
  * Pin name is "analog input" = AIN _ real physical pin on the MCU (e.g. E20 means PTE20) 
  * Naming convention in the KL25Z manual: 
  * DAD = Differential Analog Input
  * DADMx and DADPx are pair of pins used in differential 
  * mode (which MSF driver does not support).
  * In single-ended mode (MSF supported), each DADPx can be used (is an input), SOME DADMx also.
  * The signals for these pins are named ADC0_DPx and ADC0_DMx.
  * For single ended inputs the signals are ADC0_SEx
  * IMPORTANT: There may be 2 pins for the same channel number; which one is used depends on the
  * selection of ADC A or B. For example, the channel number 5 is connected to pin SE5A and SE5B
  * The pins available only in A or B converter have "a" and "b" in name; e.g. "SE5b".
  * Pins available both in A and B have just the number in the name, e.g. SE0  
  * In the enum we use 2 bits to indicate whether the channel is available in A, B or both.
  * These bits must be cleared to obtain the ADC channel number to write to ADC register. 
  * NOTE that B mode does not work with software trigger (see manual: "None of the SC1B-SC1n 
  * registers are used for software trigger operation and therefore writes to the SC1B�SC1n 
  * registers do not initiate a new conversion."
  **/
 
/* Helper macros to indicate whether the ADC channel is available in ADC A, ADC B or both */
#define		MSF_ADCA_ONLY	(0x0080)	
#define		MSF_ADCB_ONLY	(0x0100)
#define		MSF_ADC_BOTH	(MSF_ADCA_ONLY | MSF_ADCB_ONLY) 
 /** Macro to check if channel is available in A converter mode*/
#define		MSF_ADC_ISA_CHANNEL(ain_code)	(ain_code & MSF_ADCA_ONLY) 
#define		MSF_ADC_ISB_CHANNEL(ain_code)	(ain_code & MSF_ADCB_ONLY)
 
/** Macro to create analog-pin code from plain channel number and bits indicating 
 * availability of the channel in A and B converter. */ 
#define		MSF_ADC_MAKECODE(adc_ab, channel_no)	((uint16_t)adc_ab | (uint16_t)channel_no) 
 
 /* The enum with analog pin definitions */
typedef enum
{
     AIN_E20 = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 0),		/* ADC0_SE0 */
     /*AIN_SE1,	*/		/* ADC0_SE1; not present on any KL25Z */
     /*AIN_SE2, */		/* ADC0_SE1; not present on any KL25Z */
     AIN_E22 = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 3),		/* ADC0_SE3 */
     AIN_E21 = MSF_ADC_MAKECODE(MSF_ADCA_ONLY, 4),		/* ADC0_SE4a; Only available in 'A converter' */
     AIN_E29 = MSF_ADC_MAKECODE(MSF_ADCB_ONLY, 4),		/* ADC0_SE4b; Only available in 'B converter' */
     /*AIN_SE5A,*/		/* SE5a is not present on any KL25Z... */
     AIN_D1 = MSF_ADC_MAKECODE(MSF_ADCB_ONLY, 5),		/* SE5b is available in 'B converter'  */
     /*AIN_SE6A, */		/* SE6a, N/A */
     AIN_D5 = MSF_ADC_MAKECODE(MSF_ADCB_ONLY, 6),		/* SE6b */
     AIN_E23 = MSF_ADC_MAKECODE(MSF_ADCA_ONLY, 7),		/* SE7a */
     AIN_D6 = MSF_ADC_MAKECODE(MSF_ADCB_ONLY, 7),		/* SE7b */
     AIN_B0 = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 8),		/* SE8 */
     AIN_B1 = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 9),		/* SE9 */
     /* There is no SE10 input */
     AIN_C2 = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 11),		/* SE11 */
     AIN_B2 = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 12),		/* SE12 */
     AIN_B3 = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 13),		/* SE13 */
     AIN_C0 = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 14),		/* SE14 */
     AIN_C1 = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 15),		/* SE15 */
     /* no more channels available on any KL25Z */
     AIN_E30 = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 23),		/* SE23, also DAC0 output */
     AIN_TEMPERATURE = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 26),	/* Temperature sensor */
     /*AIN_BANDGAP = 27,*/	/* Bandgap 1V, needs enabling buffer first; not supported */
     AIN_VREFH = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 29),
     AIN_VREFL = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 30),
     AIN_ADC_DISALED = MSF_ADC_MAKECODE(MSF_ADC_BOTH, 31),	/* The value which disables ADC module*/
     
     
  }  Analog_pin_t;
  
  
 /** Macro for obtaining ADC module channel number for given ANALOG pin code.
  * Need to clear the bits indicating availability of the channel in A and B converter. */
 #define    MSF_PIN2CHANNEL(pin)    ((uint8_t)(pin & ~MSF_ADC_BOTH))
  
 /** Macro for obtaining ADC module number from ANALOG pin.
  * Currently, only one ADC module is supported but in future 
  * it may be possible to support more than one module.  
  */
 #define    MSF_PIN2ADC(pin)        (0)
  
/* The ADC clock must be 2 - 12 MHz for 16-bit resolution;
  or 1 - 18 MHz for <= 13 bit mode.
  For calibration <= 4 MHz is recommended.
*/  
/** Prescaler bit field value for given F_CPU
 * This is the value which goes into register, not the prescaler value itself
 * ADC clock must be within max and min ADC clock range. We set it for <= 4 MHz */
#if F_CPU == 48000000   /*  bus 24 MHz */
    #define WMSF_ADC_PRESCALER      (3)     /* presc = 8; ADC clock 3 MHz */        
#elif ((F_CPU == 20900000) || (F_CPU == 20970000))    /* bus 20.9 MHz */
    #define WMSF_ADC_PRESCALER      (3)     /* presc = 8; ADC clock 2.6 MHz */ 
#elif  F_CPU == 8000000     /* bus 8 MHz */
    #define WMSF_ADC_PRESCALER      (1)     /* presc = 2; ADC clock 4 MHz */ 
#elif  F_CPU == 4000000     /* bus 1 MHz */
    #define WMSF_ADC_PRESCALER      (1)     /* presc = 2; ADC clock 0.5 MHz */
#elif F_CPU == 41943040     /* bus 14 MHz */
    #define WMSF_ADC_PRESCALER      (2)     /* presc = 4; ADC clock 3.5 MHz */ 
#endif

 /* ADC clock options 
  0 = bus
  1 = bus/2
  2 = alternate clock
  3 = asynchronous clock (ADACK)
  IMPORTANT: if you change this option you need to change the  WMSF_ADC_PRESCALER
  values above so that the ADC clock is within the operating range!
  */
#define		WMSF_ADC_CLOCK	(0)
 
 
 /** Macro which check for ADC conversion completion.
   Evaluates to true if conversion is complete. */
#define     WMSF_ADCA_COMPLETE(reg)       ( (reg->SC1[0] & ADC_SC1_COCO_MASK) != 0)
#define     WMSF_ADCB_COMPLETE(reg)       ( (reg->SC1[1] & ADC_SC1_COCO_MASK) != 0)
       
/* -------------- End ADC definitions  --------------- */

  
/* -------------- TPM timer definitions  --------------- */  
  
/** Source of the internal clock for timer TPMn 
 * The value of the TPMSRC bit field in SIM_SOPT2  - internal clock for timers TPMn
 * The clock for F_CPU is:
 * F_CPU		TPM clock
 * 48 MHz 	- 8 MHz
 * 20.9 MHz - 20.9 MHz 
 * 8 MHz 	- 8 MHz
 * 41.9 MHz -  41943040 Hz 
 * */ 
#define	MSF_TPM_CLKSEL		MSF_UART0_CLKSEL	/* use the same value as UART0 */

/* -------------- End TPM timer definitions  --------------- */

#ifdef __cplusplus
}
#endif

#endif  /* MSF_MKL25Z_H */
