/****************************************************************************
 * @file     frdm_kl25z.h
 * @brief    Main header file for the Freescale FRDM KL25Z board. 
 * @note     This file is included by msf_config.h, you do not need to
 * include it directly into your code!
 * It defines the board-specific features.     
 *
 ******************************************************************************/
#ifndef MSFL_FRDM_KL25Z_H
#define MSFL_FRDM_KL25Z_H

/* Include the configuration options for MSF lite for this MCU */
#include "msf_config_mkl25z.h" 

/* Definition of the pins as named on the board 
 * Note: It is possible to use also the MCU pin definitions directly 
 * which can be found in msf_mkl25z.h file.
 * */ 
#define		PIN_A0				(GPIO_A0)
#define		PIN_A1				(GPIO_A1)
#define		PIN_A2				(GPIO_A2)
#define		PIN_A3				(GPIO_A3)
#define		PIN_A4				(GPIO_A4)
#define		PIN_A5				(GPIO_A5)
#define		PIN_A12				(GPIO_A12)


/* Arduino pin definitions 
 * TODO: move to arduino.h
 * *
 */
#define		PIN_D0				(GPIO_A1)

/* TODO: define pin names as on the board with #define	enum_value  */


#endif /* MSFL_FRDM_KL25Z_H */
