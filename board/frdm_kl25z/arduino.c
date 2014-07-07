/*
 Arduino-compatibility layer for MSF Lite.
 
*/
#include "arduino.h"

#include <stdio.h>	/* for sprintf */


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
	PD15, PD16, PD17, PD18, PD19,
	PD20, PD21,
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

/** In this wariable we keep record of which timer channels were already set to
 * PWM mode (so we can only change the channel value in analogWrite) and do
 * not need to set the channel to PWM mode.
 * Each byte is reserved for 1 TPM module, byte 0 is TPM0, byte 1 is PTM2, etc.
 * Each bit within the byte is 1 channel of the timer, bit 0 in byte 0 is TPM0 channel 0,
 * bit 1 is TPM0 channel 1, etc. 
 */
uint32_t g_msf_awritechannels;

/** Prescaler value for timers according to user-defined PWM frequency 
 * in msf_config.h.
 * Prescaler values for frequency:
 * 128 - 244 Hz
 * 64 - 488 H
 * 32 - 977 Hz
 * 16 - 1953 Hz
 * 8 - 3906 Hz
 * 4 - 7.812 kHz
 * 2 - 15.625 kHz
 * 1 - 31.250 Khz
 * 
 * */
#if MSF_AWRITE_244HZ
	#define	MSF_AWRITE_PRESCALER	(MSF_TPM_PRESCALER_128)	/* freq. 244 Hz */
#elif MSF_AWRITE_500HZ
	#define	MSF_AWRITE_PRESCALER	(MSF_TPM_PRESCALER_64)	/* freq. 488 Hz */
#elif MSF_AWRITE_1KHZ
	#define	MSF_AWRITE_PRESCALER	(MSF_TPM_PRESCALER_32)	/* freq. 977 kHz */
#elif MSF_AWRITE_2KHZ
	#define	MSF_AWRITE_PRESCALER	(MSF_TPM_PRESCALER_16)	/* freq. 1.953 kHz */
#elif MSF_AWRITE_4KHZ
	#define	MSF_AWRITE_PRESCALER	(MSF_TPM_PRESCALER_8)	/* freq. 3.906 kHz */
#elif MSF_AWRITE_8KHZ
	#define	MSF_AWRITE_PRESCALER	(MSF_TPM_PRESCALER_4)	/* freq. 7.812 kHz */
#elif MSF_AWRITE_16KHZ
	#define	MSF_AWRITE_PRESCALER	(MSF_TPM_PRESCALER_2)	/* freq. 15.625 kHz */
#elif MSF_AWRITE_32KHZ
	#define	MSF_AWRITE_PRESCALER	(MSF_TPM_PRESCALER_1)	/* freq. 31.250 Hz */
#else
	#define	MSF_AWRITE_PRESCALER	(MSF_TPM_PRESCALER_64)	/* default freq. 488 Hz */
#endif
/** Internal function which must be called by main before calling setup().
 * It will initialize the Arduino-compatibility layer.
 * NOTE: this assumes the timer clock is 8 MHz, which is true for CLOCK_SETUP = 1
 * and  2 (see msf_config.h).
 * For the other clock setups the values must be re-calculated.
 * See msf_<device>.h, the "TPM timer definitions" section for the 
 * timer clocks for all clock options.
 * */
void arduino_init()
{
	g_msf_awritechannels = 0;
	
	// Initialize the timers for analogWrite
	// MOD = Ft/Fo - 1 = Fsrc / (Fo . PRESCALER) - 1
	// If we want Fo = 500 Hz
	// For timer clock Fsrc = 8 MHz:
	// PRSC = 128: Fo = 62500; MOD = 124	(too little resolution)
	// PRSC = 64: Fo = 125000; MOD = 250	(ok if we change the frequency a little)
	// Fo = Ft/(MOD + 1) = 125000/256 = 488.2 Hz
	// Fo = Fsrc/(MOD+1)*PRESCALER
	Driver_TPM0.Initialize(null);	
	Driver_TPM0.Control(MSF_TPM_PRESCALER_SET, MSF_AWRITE_PRESCALER);
	Driver_TPM0.Control(MSF_TPM_MOD_VALUE, 255);	
	
	Driver_TPM1.Initialize(null);	
	Driver_TPM1.Control(MSF_TPM_PRESCALER_SET, MSF_AWRITE_PRESCALER);
	Driver_TPM1.Control(MSF_TPM_MOD_VALUE, 255);	
		
	Driver_TPM2.Initialize(null);	
	Driver_TPM2.Control(MSF_TPM_PRESCALER_SET, MSF_AWRITE_PRESCALER);
	Driver_TPM2.Control(MSF_TPM_MOD_VALUE, 255);	
	
}


/* ---------------------- Analog write ------------------------ */
/* internal function for analogWrite
 * Convert value 0 - 255 used in Arduino for analogWrite to
 * the value which is written to TPM register CnV (depends on modulo)
 * */
static inline uint32_t val_to_cnval(int value)
{
	// See arduino_init(); We set up the timer so that there 
	// is no conversion needed.
	return value;
}


/** Start generating PWM signal on given pin.  
 * @param Arduino pin number 0 to 21 (not all pins supported, see note below!). You do not need to send
 * 	the pin to output mode using pinMode.
 * @param value between 0 and 255; 0 means full time low, 255 means full time high on the pin.
 * @return none
 * @note The PWM frequency is 488 Hz by default but can be changed in msf_config.h 
 * Not all pins can be used to really generate PWM (only those connected to timer can).
 * For pins 14 and 15 the function does nothing. This is different behaviour than on Arduino, where
 * the pin which is not available on timer will be all time low for values 0 - 127 and all time high
 * for values 128 - 255. This makes the code more complicated (handle pin mode and direction) and
 * makes little sense to do. On the other hand, almost all pins can be used for PWM here while on Arduino
 * there are only few. :)  
 * 
 * Pins with PWM support:
 * 0 (PTA1) - TMP2_ch0
 * 1 (PTA2) - TMP2_ch1 
 * 2 (PTD4) - TMP0_ch4
 * 3 (PTA12) - TMP1_ch0
 * 4 (PTA4) - TMP0_ch1
 * 5 (PTA5) - TMP0_ch2
 * 6 (PTC8) - TMP0_ch4	(same channel as pin 2!)
 * 7 (PTC9) - TMP0_ch5
 * 8 (PTA13) - TMP1_ch1
 * 9 (PTD5) - TMP0_ch5	(same channel as pin 7!)
 * 10 (PTD0) - TMP0_ch0
 * 11 (PTD2) - TMP0_ch2	(same channel as pin 5!)
 * 12 (PTD3) - TMP0_ch3
 * 13 (PTD1) - TMP0_ch1	(same channel as pin 4!)
 * 14 (PTE0) - N/A
 * 15 (PTE1) - N/A
 * 16 (PTB0) - TMP1_ch0 (same channel as pin 3!)
 * 17 (PTB1) - TMP1_ch1 (same channel as pin 8!)
 * 18 (PTB2) - TMP2_ch0 (same channel as pin 0!)
 * 19 (PTB3) - TMP2_ch1 (same channel as pin 1!)
 * 20 (PTC2) - TMP0_ch1 (same channel as pin 4!)
 * 21 (PTC1) - TMP0_ch0 (same channel as pin 10!)
 * 
 * Pins which are defined as default for analogWrite. No need to change msf_config.h to use these pins with 
 * analogWrite:
 * 2,3,4,5,7,8,10,12,18,19. 
 * 
 * Note about pins which use the same timer channel (e.g. pin 6 and pin 2):
 * Only one of the pins can be used at a time. This is defined in msf_config.h, see the 
 * "Pin configuration for the timer TPMn drivers." section. The pins with lower numbers are
 * generally pre-defined, except for pins 0 and 1. The alternate pin definitions are given in comment. 
 * IMPORTANT: if you do not configure the pin in msf_config.h, analogWrite on this pin will not work!
 * 
 * Note on using the same pin for analogWrite and digital read/write:
 * This should be avoided...
 * If you use a pin for analogWrite and then want to use it in digital mode, set it to input/output 
 * mode using pinMode and then you can use digital Read/Write. But note that the pin will not work
 * with analogWrite again, because the analogWrite will not attach the pin to the timer channel, because 
 * it will still think it is already attached (see g_msf_awritechannels).
 * 
 * Note about implementation: Arduino initializes all the timers to fixed frequency and analogWrite just connects
 * timer pins to the timer as required. 
 * Our implementation also initializes the timers to freq. about 488 Hz and 
 * does not set channels to PWM mode (this would attach the pin).
 * 
 * 
 * 
 * */
void analogWrite(int pin, int value)
{
	MSF_DRIVER_TPM* pDrv = null;
	int channel = -1;
	uint32_t channel_mask, module_pos;
	
	// We use direct mapping of pin-number to timer "object" and channel using this switch...
	// This should allow fast code for const pins
	switch ( pin )
	{
	case 0:
	case 18:
		pDrv = &Driver_TPM2;
		channel = 0;
		module_pos = 2;
		break;
	case 1:
	case 19:
		pDrv = &Driver_TPM2;
		channel = 1;
		module_pos = 2;
		break;
	case 2:
	case 6:
		pDrv = &Driver_TPM0;
		channel = 4;
		module_pos = 0;
		break;
	case 3:
	case 16:
		pDrv = &Driver_TPM1;
		channel = 0;
		module_pos = 1;
		break;
	case 4:
	case 13:
	case 20:
		pDrv = &Driver_TPM0;
		channel = 1;
		module_pos = 0;
		break;
	case 5:
	case 11:
		pDrv = &Driver_TPM0;
		channel = 2;
		module_pos = 0;
		break;
	case 7:
	case 9:
		pDrv = &Driver_TPM0;
		channel = 5;
		module_pos = 0;
		break;	
	case 8:
	case 17:
		pDrv = &Driver_TPM1;
		channel = 1;
		module_pos = 1;
		break;	
	case 10:
	case 21:
		pDrv = &Driver_TPM0;
		channel = 0;
		module_pos = 0;
		break;	
	case 12:
		pDrv = &Driver_TPM0;
		channel = 3;
		module_pos = 0;
		break;
		
	default:			
		channel = -1;
	
	}
	
	if ( channel >= 0 )
	{
		channel_mask = (1UL << (8*module_pos+channel));	// see g_msf_awritechannels comment
		if ( (channel_mask &  g_msf_awritechannels) == 0 )
		{
			g_msf_awritechannels |= channel_mask;
			pDrv->ChannelSetMode(channel, PWM_edge_hightrue, 0);
		}
		// write the new value
		pDrv->ChannelWrite(channel, val_to_cnval(value));
	}
}


/* ---------------------- end Analog write ------------------------ */

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
