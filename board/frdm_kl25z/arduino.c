/*
 Arduino-compatibility layer for MSF Lite.
 
*/
#include "arduino.h"


/* Array which maps the pin numbers used in Arduino to
 * codes of pins used in this board. 
 * The Arduino pin number is used as index into this array. 
 * Value at given index N is the pin code for the Arduino pin N.
 * */
const uint32_t g_msf_pins_arduino[] =
{
	PD0, PD1, PD2, PD3, PD4,
	PD5, PD6, PD7, PD8, PD9,
	PD10, PD11, PD12, PD13, PD14,
	PD15, 
	PD16,  
};

/* Array which maps the analog pin numbers used in Arduino to
 * codes of analog pins used in this board. 
 * The Arduino pin number is used as index into this array. 
 * Value at given index N is the pin code for the Arduino pin N.
 * */ 
const uint32_t g_msf_analogpins_arduino[] =
{
	PA0, PA1, PA2, PA3, PA4, PA5,	 
};


/* Access structure for Serial "class" */
MSF_SERIAL_ARDUINO Serial = {
  SerialBegin,
  SerialPrint,
  /* TODO: */    
};
