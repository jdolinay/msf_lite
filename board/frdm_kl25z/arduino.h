/*
 Arduino-compatibility layer for MSF Lite.
 Include this file AFTER the msf_config.h in your application if you 
 wish to use the Arduino API.
*/

#ifndef MSFL_ARDUINO_H
#define MSFL_ARDUINO_H

#ifndef MSF_CONFIG_H
    #error Please include msf_config.h before including this file.
#endif

/* Arduino pin definitions 
 * Digital pin n is named PDn etc.
 */
#define		PD0				(GPIO_A1)
#define		PD1				(GPIO_A2)
#define		PD2				(GPIO_D4)
#define		PD3				(GPIO_A12)
#define		PD4				(GPIO_A4)
#define		PD5				(GPIO_A5)
#define		PD6				(GPIO_C8)
#define		PD7				(GPIO_C9)
#define		PD8				(GPIO_A13)
#define		PD9				(GPIO_D5)
#define		PD10			(GPIO_D0)
#define		PD11			(GPIO_D2)
#define		PD12			(GPIO_D3)
#define		PD13			(GPIO_D4)
#define		PD14			(GPIO_E0)
#define		PD15			(GPIO_E1)
/* Analog pins available also as digital I/O*/
#define		PA0				(GPIO_B0)
#define		PA1				(GPIO_B1)
#define		PA2				(GPIO_B2)
#define		PA3				(GPIO_B3)
#define		PA4				(GPIO_C2)
#define		PA5				(GPIO_C1)


#endif /* MSFL_ARDUINO_H */
