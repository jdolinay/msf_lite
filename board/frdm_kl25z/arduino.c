/*
 Arduino-compatibility layer for MSF Lite.
 
*/
#include "arduino.h"

#include <stdlib.h>	/* for sprintf */


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



/* ---------------------- Serial ------------------------ */

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
	
	coniob_init(baud);
	
}

/* Implements Serial.end 
 * Disables the UART module. Does not touch the Rx, Tx pins - not needed.
 * To use the Tx, Rx pins in GPIO mode simply set the direction and use it as needed. */
static void  SerialEnd(void)
{   
	// do nothing for now
	//Driver_UART0.Uninitialize();	
}

/** Print text to serial line */
static void SerialPrint(const char* str)
{
	coniob_puts(str);	
}

/* print a string and move to new line.
 * Note that you can also write "\n" in the string and use Serial.print
 * example: Serial.print("hello\n"); == Serial.println("hello\n"); */
static void SerialPrintLn(const char* str)
{
	coniob_puts(str);
	coniob_putch('\n');
}

/** implements Serial.available
 * Get the number of bytes (characters) available for reading from the serial port. 
 * This is data that's already arrived and stored in the serial receive buffer (which holds 64 bytes).
 *  */
static int SerialAvailable(void)
{
	return coniob_kbhit();
}

/* Waits for the transmission of outgoing serial data to complete
 * */
static void SerialFlush(void)
{
	/* This is not available in coniob API, so we have to use the
	 * UART driver directly; but use the symbolic name defined in coniob.h
	 * so that we access the same driver which conio uses */
	while (CONIOB_UART_DRIVER.GetTxCount() > 0) 
		msf_delay_ms(1);
}

/* Returns the next byte (character) of incoming serial data without removing it from the internal serial buffer. 
 * That is, successive calls to peek() will return the same character, as will the next call to read().
 * @return the first byte of incoming serial data available (or -1 if no data is available) 
 * */
static int SerialPeek(void)
{
	int n = (int)coniob_peek();
	if (n == 0 )
		return -1;
	else
		return n;	
}

/** Reads incoming serial data. 
 * @return the first byte of incoming serial data available (or -1 if no data is available) */
static int SerialRead(void)
{
	int n = (int)coniob_getch();
	if (n == 0)
		return -1;
	else
		return n;
}

/** Reads characters from the internal buffer of serial driver into given buffer. 
 * The function terminates if the determined length has been read.
 * @return  number of characters placed in the buffer. A 0 means no valid data was found.
 * */
static int SerialReadBytes(char* buffer, int length)
{
	return (int)coniob_gets(buffer, length, 0);
}

/** Reads characters from the internal buffer of serial driver into given buffer. 
 * The function terminates if given terminator character is found or
 * if the given length has been read.
 * @return  number of characters placed in the buffer. A 0 means no valid data was found.
 * */
static int SerialReadBytesUntil(char terminator, char* buffer, int length)
{
	return (int)coniob_gets(buffer, length, 0);
}

/** Write byte to serial line.
 * This is limited version of the Arduino function; it just sends 1 byte.
 * If you want to send a string, use Serial.print(). 
 * */
static void SerialWrite(int val)
{
	coniob_putch((char)val);
}

/** Prints integer number in given format: DEC, HEX, OCT, BIN 
 * Note that without C++ features we cannot have function of the same name (print)
 * We do not support binary format as standard C print doe snot have it.
 */
static void SerialPrintInt(int val, int format)
{
	/* We have to use a temporary buffer for the string. 
	 * If the buffer is local variable (on the stack) this function could be 
	 * re-entrant. If it is global, it would not.
	 * But the UART driver itself is not reentrant... */
	char tmp[12];	
	sprintf(tmp, ARDUINO_FORMAT_2_PRINTF(format), val);
	coniob_puts(tmp);	
}

/** Prints float number with given number of decimal places.
 * Note that the floating point numbers must be enabled for (s)printf in 
 * C/C++ Build > Settings > Librarian set model to: "ewl" 
 * and select print formats: "int_FP" 
 *  */
static void SerialPrintFloat(float val, int decplaces)
{
	char fc;
	char tmp[12];
	char format[5] = "%.nf";
	if ( decplaces > 9 )
		decplaces = 9;
	if (decplaces < 0 )
		decplaces = 0;
	format[2] = decplaces + '0';
	
	sprintf(tmp, format, val);
	coniob_puts(tmp);	
}

/* Access structure for the Serial "class" */
MSF_SERIAL_ARDUINO Serial = {
  SerialBegin,
  SerialEnd,
  SerialPrint,
  SerialPrintInt,
  SerialPrintFloat,    
  SerialPrintLn,
  SerialAvailable,
  SerialRead,
  SerialReadBytes,
  SerialReadBytesUntil,
  SerialFlush,
  SerialPeek,
  SerialWrite,
      
};
