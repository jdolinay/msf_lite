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

/* OPTION 1: Define the clock speed you want to use in your application 
 * Note that only the options selected below are supported! 
 * */
#define CLOCK_SETUP     1

/* Predefined clock setups
   0 ... Multipurpose Clock Generator (MCG) in FLL Engaged Internal (FEI) mode
         Reference clock source for MCG module is the slow internal clock source 32.768kHz
         Core clock = 41.94MHz, BusClock = 13.98MHz
   1 ... Multipurpose Clock Generator (MCG) in PLL Engaged External (PEE) mode
         Reference clock source for MCG module is an external crystal 8MHz
         Core clock = 48MHz, BusClock = 24MHz
   2 ... Multipurpose Clock Generator (MCG) in Bypassed Low Power External (BLPE) mode
         Core clock/Bus clock derived directly from an external crystal 8MHz with no multiplication
         Core clock = 8MHz, BusClock = 8MHz
   3 ... Default settings: internal clock source 32.768kHz and FLL mode
   	   	 Core clock = 20.9MHz, BusClock = ?
*/

 /* Include the header file for our board */
#include "frdm_kl25z.h" 

#endif  /* MSF_CONFIG_H */
