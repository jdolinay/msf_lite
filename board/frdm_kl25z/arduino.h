/*
 Arduino-compatibility layer for MSF Lite.
 Include this file  in your application if you want to use the Arduino API.
 You do not need to include any other MSF files.
*/

#ifndef MSFL_ARDUINO_H
#define MSFL_ARDUINO_H

/* We include all the needed files here so that it is easy for the user - just include arduino.h in your source */

/* MSF configuration for this project */
#include "msf_config.h"

/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"

#include "msf.h"

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
#define		PD13			(GPIO_D1)
#define		PD14			(GPIO_E0)	/* Pins 14 and 15 are not available on older */
#define		PD15			(GPIO_E1)	/* version of Arduino Uno (before R3 pinout) */
/* Analog pins available also as digital I/O */
#define		PA0				(GPIO_B0)
#define		PA1				(GPIO_B1)
#define		PA2				(GPIO_B2)
#define		PA3				(GPIO_B3)
#define		PA4				(GPIO_C2)
#define		PA5				(GPIO_C1)

/* Number of pins on this board */
#define		MSF_ARDUINO_PINS	(22)

/* The array is defined in arduino.c */
extern const uint32_t g_msf_pins_arduino[];

/* Symbols for digital I/O */
#define		HIGH	(1)
#define		LOW		(0)
#define		INPUT	(0)
#define		OUTPUT	(1)
#define		INPUT_PULLUP	(2)

/* Arduino pin number to pin-code conversion */
/* Convert Ardiuno pin number into pin code used by MSF GPIO functions. 
 * @returns Pin code or GPIO_INVALID_PIN if the pin number is out of range */
static inline uint32_t pin2pincode(int pin)
{
	if ( pin < MSF_ARDUINO_PINS )
		return g_msf_pins_arduino[pin];
	else
		return GPIO_INVALID_PIN;
}


/* Arduino API function wrappers */ 
static inline void pinMode(int pin, int mode)
{
	uint32_t pin_code = pin2pincode(pin);
	switch ( mode) 
	{
	case INPUT:
		msf_pin_direction(pin_code, input);
		msf_pin_pullmode(pin_code, pull_none);
		break;
		
	case INPUT_PULLUP:
		msf_pin_direction(pin_code, input);
		msf_pin_pullmode(pin_code, pull_up);
		break;	
		
	case OUTPUT:
		msf_pin_direction(pin_code, output);
		break;
	}
		
}

static inline void digitalWrite(int pin, int val)
{
	msf_pin_write(pin2pincode(pin), (bool)val);	
}

static inline int digitalRead(int pin)
{
	return (msf_pin_read(pin2pincode(pin))) ? HIGH : LOW;		
}

/* Set the reference for ADC.
 * Arduino supports:
 * DEFAULT - e.g. 5V on Uno
 * INTERNAL - depends on MCU, e.g. 1.1 V for UNO (atmega328)
 * EXTERNAL - AREF pin.
 * and some other for Arduino Mega only. 
 * */
static inline void analogReference(int type)
{
	
}

/* The pin is 0 to 5 on Arduino Uno.
 * */
static inline int analogRead(int pin)
{
	
}


/* TODO: do analogWrite? */

static inline void delay(uint32_t millis)
{
	msf_delay_ms(millis);
}





#endif /* MSFL_ARDUINO_H */
