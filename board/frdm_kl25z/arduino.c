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


/* the worked for Serial.begin()
 * We allow baudrate as any number but only certain values are supported:
 * Supported baudrate values: 
 * 2400, 4800, 9600, 19200, 38400, 57600, 115200
 * If invalid value is given, the baudrate will be set to 9600.
 * */
static void  SerialBegin( uint32_t baudrate)
{     
	UART_speed_t baud = BD9600;	/* default value */
	
	switch(baudrate) {
	case 2400:
		baud = BD2400;
		break;
	case 4800:
		baud = BD4800;
		break;
	case 9600:
		baud = BD9600;
		break;
	case 19200:
		baud = BD19200;
		break;
	case 38400:
		baud = BD38400;
		break;
	case 57600:
		baud = BD57600;
		break;
	case 115200:
		baud = BD115200;
		break;
	
	}
	
	Driver_UART0.Initialize(baud, null);	
	
}

/* Implements Serial.end 
 * Disables the UART module. Does not touch the Rx, Tx pins - not needed.
 * To use the Tx, Rx pins in GPIO mode simply set the direction and use it as needed. */
static void  SerialEnd()
{   
	Driver_UART0.Uninitialize();	
}

/* Access structure for Serial "class" */
MSF_SERIAL_ARDUINO Serial = {
  SerialBegin,
  SerialEnd,
  msf_print,
  /* TODO: */    
};
