/****************************************************************************
 * @file     msf_config_<device>.h
 * @brief    User configuration for the MSF library 
 * @version  2
 * @date     22. Apr 2014
 *
 * @note     This file should contain settings which are fixed for given board
 *    
 *
 ******************************************************************************/
#ifndef MSF_CONFIG_MKL25Z_H
#define	MSF_CONFIG_MKL25Z_H

/*********************************************
*    Select your platform, e.g. KINETIS
********************************************/
#define     MSF_KINETIS     1


/*********************************************
*    Select your device from the platform 
********************************************/
#define   MSF_KINETIS_MKL25Z      1 


/*********************************************
*    Define the compiler to be used 
********************************************/
#define     CW_10    1


/* TODO: moved to project-private file msf_config.h*/
/*********************************************
*    Define the standard I/O channel 
********************************************/
//#define   MSF_USE_STDIO     1   /* Using conio driver (UART) for std output */
/* To disable initializing UART in msf_init use:
#define   MSF_USE_STDIO     0 
*/

/*********************************************
*    Define whether we want to use analog inputs
*    If nonzero; ADC is initialized. This means 
*    little more power consumption, otherwise no
*    harm is done by enabling the analog inputs even
*    if not used 
********************************************/
//#define MSF_USE_ANALOG      1


/*********************************************
*    Define which drivers should be created (used) 
*    It may save some memory to define drivers which 
*    your applicaiton will not use with 0.
********************************************/
//#define MSF_DRIVER_UART0    1   /* Use UART0 driver (create UART0 driver instance) */
//#define MSF_DRIVER_ADC0     1	/* Use ADC driver */

/*********************************************
*    Define inline option for functions
********************************************/
#define     MSF_USE_INLINE  1
/* NOTE: it is not possible to define macro for pragma, e.g.
    #define     MSF_INLINE  #pragma INLINE
    except for C99 compilers, where is should be possible to use:
    _Pragma("argument") which is equivalent to #pragma argument 
*/

/*********************************************
*    Define the MSF API components
********************************************/
/* Note: this is defined by MSF implementor for given device.
    The supported group of fucntions should be defined as 1,
    not-supported as 0.
*/ 
#define     MSF_TIME_API               1
#define     MSF_SHORT_DELAY_API        1
#define     MSF_PRINT_API              1
#define     MSF_ANALOG_API             1 
#define     MSF_SWTIMER_API            0


/*********************************************
*    Default options for MSF drivers
********************************************/
/* Note: these are settings applied to the driver when it is initialized.
    For example: resolution of ADC.
    It can be changed with care. The values are not checked in the code and
    the driver may stop working.
    You can change some options of the drivers also in runtime, see the <driver>.Control(). 
*/

/******************** ADC driver options *************************/
/** ADC resolution bit field value
0 = 8-bit
1 = 12-bit
2 = 10-bit
3 = 16-bit
@note this is default value; any value can be set by driver Control function.  
*/
#define WMSF_ADC_RESOLUTION     (3)
 
/** ADC result averaging
 0 = 4 samples
 1 = 8 samples
 2 = 16 samples
 3 = 32 samples
 F = no averaging (internal value used in the driver )
 @note this is default value; any value can be set by driver Control function. 
 */
#define	WMSF_ADC_AVERAGE		(0)
 
/******************** End ADC driver options *************************/

/* Check if there is valid F_CPU defined in msf-config.h */
#if !( (F_CPU == 41943040) ||  (F_CPU == 48000000) || (F_CPU == 8000000) || (F_CPU == 20900000) || (F_CPU == 4000000))
	#error Please define valid F_CPU in msf_config.h. It is possible that the clock speed is not supported.
#endif

/* TODO: remove the block below
 * User will define F_CPU directly in msf_config.h.
 * This way our code will not be broken if the MCU vendor changes the meaning of
 * the CLOCK_SETUP macro. */
/* @note This constant is used internally by the MSF lite. It will be set
 * automatically based on the CLOCK_SETUP value in msf_config.h.
 * Do not change the F_CPU directly here! It will not actually change the
 * CPU settings!
*/
#if 0
#if (CLOCK_SETUP == 0)
	#define   F_CPU   (41943040)	/* clock option 0 in system_MKL25Z4.c */
#elif (CLOCK_SETUP == 1)
	#define   F_CPU   (48000000)	/* clock option 1 in system_MKL25Z4.c */
#elif (CLOCK_SETUP == 2)
	#define   F_CPU   (8000000)		/* clock option 2 in system_MKL25Z4.c */
#elif (CLOCK_SETUP == 3)
	#define   F_CPU   (20900000) 	/* clock option 3 in system_MKL25Z4.c */
#else
	#error Please define valid CLOCK_SPEED in msf_config.h
#endif
#endif


#endif /* MSF_CONFIG_MKL25Z_H */
