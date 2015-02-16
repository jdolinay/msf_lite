/* waveout example.
 *
 * Demonstrate usage of the waveout high-level driver for generating signals.
 *
 * The driver can be used with any or all of the TPM0, TPM1 or TPM2 low-level timer drivers.
 *
 * This project contains several example use-cases in separate functions.
 * Uncomment one of the functions at a time to test it.
 *
 * Testing the program:
 * - run the example program
 * - measure the signal on the selected output pin with oscilloscope or multi-meter
 * with frequency and duty measurement.
 *
 *
 * The waveout driver supports all the clock options available as of KDS 1.1.1
 * and automatically sets the timer to 1 MHz clock so that the time-values used
 * are in microseconds (us).
 *
 * Table of the pins which can be used as output:
 * waveout channel    timer channel   default pin (see msf_config.h)
 * WAVEIO_C0		TPM0 channel 0	D0  (Arduino 10)
 * WAVEIO_C1		TPM0 channel 1	A4	(Arduino 4 )
 * WAVEIO_C2		TPM0 channel 2	A5	(Arduino 5)
 * WAVEIO_C3		TPM0 channel 3	D3	(Arduino 12)
 * WAVEIO_C4		TPM0 channel 4	D4	(Arduino 2)
 * WAVEIO_C5		TPM0 channel 5	C9	(Arduino 7)
 * WAVEIO_C6		TPM1 channel 0  A12	(Arduino 3)
 * WAVEIO_C7		TPM1 channel 1  A13	(Arduino 8)
 * WAVEIO_C8		TPM2 channel 0  B2	(Arduino 16 (A2))
 * WAVEIO_C9		TPM2 channel 1  B3	(Arduino 17 (A3))
 *
 * Notes about the usage of the waveout driver:
 * The values given to waveout_channel_start are length of the half-waves
 * of the signal we want to generate in microseconds (us).
 * Example: 1 and 1 means wave length 2 us (signal period = 2 us)
 * frequency_in_Hz = 1/period_in_second = 1 000 000 / period_in_us
 *  frequency_in_kHz = 1 000 / period_in_us
 *  period_in_us = 1 000 / frequency_in_kHz
 *	 Example:
 *	want to obtain 10 kHz: period = 1000/10=100
 *	For square wave signal the half-wave values are 50 and 50
 * Experimental: (48 MHz CPU)
 * TPM0 max frequency about 35 KHz (half-waves: 14,14) for 1 channel active
 *		for 5 channels max 10 kHz
 * TPM1 or TPM2 max frequency about 45 KHz (half-waves: 11,11)
 *
 *
 */

/* MSF configuration for this project */
#include "msf_config.h"
/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"
#include "msf.h"

//#include "waveout.h"	// waveout driver
#include "waveio.h"	// waveio driver

/* Defines for handling LEDs easier
LED connections:
 B18 	- Red LED
 B19 	- Green LED
 D1	- Blue LED (Arduino pin D13)
 All LEDs are on, if the pin is low and off if it is high.
 */
#define	RED_LED			PIN_B18		// or GPIO_B18
#define	GREEN_LED		PIN_B19		// or GPIO_B19

void test_waveio_driver_single(void);
void test_waveio_driver_all(void);
void test_waveio_driver_servo(void);

void test_waveio_driver_wave(void);
void test_waveio_driver_pulse(void);
void test_waveio_driver_multichannel(void);
void test_waveio_driver_rc(void);
void test_waveio_driver_sonar(void);
void display_channel(uint8_t channel);

/*
void test_waveout_driver_single(void);
void test_waveout_driver_all(void);
void test_waveout_driver_servo(void);
*/

static int gi = 0;

int main(void)
{

	// Always initialize the MSF library
	msf_init(0);

	// Init pins for LEDs
	msf_pin_direction(RED_LED, output);
	msf_pin_direction(GREEN_LED, output);
	msf_pin_write(RED_LED, true);
	msf_pin_write(GREEN_LED, true);

	// Uncomment one of these functions at a time...
	//test_waveio_driver_servo();
	//test_waveio_driver_single();
	//test_waveio_driver_all();

	//test_waveio_driver_wave();
	//test_waveio_driver_pulse();
	test_waveio_driver_multichannel();
	//test_waveio_driver_rc();
	//test_waveio_driver_sonar();

	// Uncomment one of these functions at a time...
	//test_waveout_driver_single();
	//test_waveout_driver_all();
	//test_waveout_driver_servo();


    /* This for loop should be replaced. By default this loop allows a single stepping. */
    for (;;) {
        gi++;
    }
    /* Never leave main */
    return 0;
}

/* test_waveio_driver_single
 * Generate signal on single output pin (single waveio channel).
 * Start the program and connect oscilloscope or multimeter to the output pin.
 * You should see 1 kHz signal on this pin.
 * */
void test_waveio_driver_single(void)
{
	// Initialize waveio driver telling it which channel(s) we will use.
	// The channels are given in ranges as follows from the hardware underneath
	// - the TPM timer modules which are TPM0 with 6 channels, TPM1 and TPM2 each
	// with 2 channels.
	// See also the pin table at the top of this file.
	waveio_init(WAVEIO_RANGE_0_5);


	while(1)
	{
		// Start generating the signal
		// Note that the function does not block; the program continues execution!
		waveio_out_start(0, 500, 500);

		msf_pin_write(RED_LED, true);		// red off
		msf_pin_write(GREEN_LED, false);	// green on

		msf_delay_ms(10000);

		msf_pin_write(GREEN_LED, true);	// green off
		msf_pin_write(RED_LED, false);		// red on

		waveio_out_stop(0);

		msf_delay_ms(5000);
	}
}

/* test_waveio_driver_all
 * Generate signal on all output channels at once.
 * Start the program and connect oscilloscope or multimeter to
 * any of the supported output pins - see the pin table at the top of this file.
 * You should see signals with various frequency and duty, see the code.
 *
 * */
void test_waveio_driver_all(void)
{
	int i;

	// Initialize waveout driver telling it which channel(s) we will use
	waveio_init(WAVEIO_RANGE_0_5 | WAVEIO_RANGE_6_7 | WAVEIO_RANGE_8_9);

	// Start generating the signal
	waveio_out_start(0, 500, 500);	// 1 kHz, 50% duty; D0  (Arduino 10)
	waveio_out_start(1, 100, 900);	// 1 kHz, 10% duty; A4	(Arduino 4)
	waveio_out_start(2, 900, 100);	// 1 kHz, 90% duty; A5	(Arduino 5 )
	waveio_out_start(3, 250, 250);	// 2 kHz, 50% duty; D3	(Arduino 12)
	waveio_out_start(4, 5000, 5000);	// 100 Hz, 50% duty; D4	(Arduino 2)
	waveio_out_start(5, 1500, 18500);	// servo, 50 Hz, 1.5 ms pulse; C9	(Arduino 7)
	waveio_out_start(6, 250, 750);	// A12	(Arduino 3)
	waveio_out_start(7, 500, 500);	// A13	(Arduino 8)
	waveio_out_start(8, 750, 250);	// B2  (Arduino 16 (A2))
	waveio_out_start(9, 600, 400);	// B3	(Arduino 17 (A3))

	// Do not leave this function.
	// Note that the waveout_channel_start does not block the caller.
	for (;;) {
	     i++;
	}
}

/* test_waveio_driver_servo
 * Demonstrate control of a RC servo motor.
 * Sweep the servo from one end position to the other
 * */
void test_waveio_driver_servo(void)
{
	uint16_t angle = 0;
	int direction = 1;

	// Initialize waveout driver telling it which channel(s) we will use
	waveio_init(WAVEIO_RANGE_0_5 );	// | WAVEIO_RANGE_6_7 | WAVEIO_RANGE_8_9);

	while(1)
	{
		for ( angle = 0; angle < 180; angle++ )
		{
			// generate servo signal on channel 0 = pin D0 (arduino pin 10)
			waveio_out_servo(0, angle);
			waveio_out_servo(1, angle);
			waveio_out_servo(2, angle);
			msf_delay_ms(20);
		}

		for ( angle = 180; angle > 0; angle-- )
		{
			// generate servo signal on channel 0 = pin D0 (arduino pin 10)
			waveio_out_servo(0, angle);
			waveio_out_servo(1, angle);
			waveio_out_servo(2, angle);
			msf_delay_ms(20);
		}

		/*
		if ( direction )
		{
			angle +=1;
			if ( angle >= 180 )
			{
				direction = 0;
				angle = 180;
			}
		}
		else
		{
			if (angle >= 1)
			{
				angle -= 1;
			}
			else
			{
				angle = 0;
				direction = 1;
			}
		}*/

	}

}

// Define the channel for test_wavein_driver_wave() and test_wavein_driver_pulse().
#define		INPUT_CHANNEL	(WAVEIO_C0)

/* test_waveio_driver_wave
 * Measure the half-waves of the signal.
 * Connect the signal to one of the supported pins (see table at the top of this file)
 * and set the INPUT_CHANNEL constant above.
 * Start the program and connect to the OpenSDA port with terminal. (baud 19200).
 * You should see the lengths of the half-waves of the signal.
 * */
void test_waveio_driver_wave(void)
{
	uint16_t a, b;
	uint8_t err;

	// Initialize the waveio driver
	waveio_init(WAVEIO_RANGE_0_5 | WAVEIO_RANGE_6_7 | WAVEIO_RANGE_8_9);

	// Configure the channel we will use as input
	waveio_in_attach(INPUT_CHANNEL);

	// Start measuring on this channel
	waveio_in_start(INPUT_CHANNEL);
	while(1)
	{
		// Read measured values
		err = waveio_in_read(INPUT_CHANNEL, &a, &b);
		if ( err == WAVEIO_NO_ERROR )
		{
			msf_print("Half-wave 1: ");
			msf_printnum(a);
			msf_print(" us  Half-wave 2: ");
			msf_printnum(b);
			msf_print("\n");
		}
		else
		{
			msf_print("Error : ");
			msf_printnum(err);
			// see waveio.h for error codes; most common will be 102 = WAVEIO_ERROR_NOINPUT
			msf_print("\n");
		}

		msf_delay_ms(1500);
	}

}

/* test_waveio_driver_pulse
 * Measure pulse on input.
 * Connect the signal to one of the supported pins (see table at the top of this file)
 * and set the INPUT_CHANNEL constant above.
 * Start the program and connect to the OpenSDA port with terminal. (baud 19200).
 * You should see the length of the pulse detected on the input. If no pulse
 * is detected for 1 second, the program will display "timeout" message.
 * */
void test_waveio_driver_pulse(void)
{
	uint16_t a;

	waveio_init(WAVEIO_RANGE_0_5 | WAVEIO_RANGE_6_7 | WAVEIO_RANGE_8_9);

	waveio_in_attach(INPUT_CHANNEL);
	// Note: no need to start measurement by calling waveio_channel_start(n);
	while(1)
	{
		// Wait for a pulse on the input with 1000 ms timeout
		a = waveio_in_pulse_wait(INPUT_CHANNEL, 1000);
		if (a == 0)
			msf_print("Timeout.\n");
		else
		{
			msf_print("Pulse: ");
			msf_printnum(a);
			msf_print(" us \n");
		}

		msf_delay_ms(1000);
	}

}

/* test_waveio_driver_multichannel
 * Measure the half-waves of the signal on multiple pins at once.
 * Connect the input signal to one or more of the supported pins (see table above).
 * Start the program and connect to the OpenSDA port with terminal. (baud 19200).
 * You should see the lengths of half-waves detected on each of the active pins.
 * For unconnected pins the output will say Error 102, which means no signal.
 * See waveio.h for error codes.
 *
 * */
void test_waveio_driver_multichannel(void)
{
	uint8_t i;

	// Initialize the waveio driver
	waveio_init(WAVEIO_RANGE_0_5 | WAVEIO_RANGE_6_7 | WAVEIO_RANGE_8_9);

	// Connect the channels
	waveio_in_attach(WAVEIO_C0);
	waveio_in_attach(WAVEIO_C1);
	waveio_in_attach(WAVEIO_C2);
	waveio_in_attach(WAVEIO_C3);
	waveio_in_attach(WAVEIO_C4);
	waveio_in_attach(WAVEIO_C5);

	// Start measuring on these channels
	waveio_in_start(WAVEIO_C0);
	waveio_in_start(WAVEIO_C1);
	waveio_in_start(WAVEIO_C2);
	waveio_in_start(WAVEIO_C3);
	waveio_in_start(WAVEIO_C4);
	waveio_in_start(WAVEIO_C5);

	while(1)
	{
		// Read values from all channels
		for ( i = 0; i<6; i++ )
		{
			display_channel(i);
			msf_delay_ms(100);	// delay needed so that the transmit buffer
			// for serial line does not overflow.
		}

		msf_print("\n");
		msf_delay_ms(2000);
	}

}

/* helper for test_wavein_driver_multichannel
 * Read and send to serial port the info about one channel
 * */
void display_channel(uint8_t channel)
{
	uint8_t err;
	uint16_t a, b;

	msf_print("CH ");
	msf_printnum(channel);

	err = waveio_in_read((WAVEIO_channel)channel, &a, &b);
	if ( err == WAVEIO_NO_ERROR )
	{

		msf_print(": Half-wave 1: ");
		msf_printnum(a);
		msf_print(" us Half-wave 2: ");
		msf_printnum(b);
		msf_print("\n");
	}
	else
	{
		msf_print(":  Error: ");
		msf_printnum(err);
		msf_print("\n");
	}

}


/* test_wavein_driver_rc
 * Show how to use the wavein driver for processing input from
 * RC receiver (radio controlled models).
 * The signal is a pulse between 1 and 2 ms repeating every 20 ms.
 * */
#define		RC_NUM_CHANNELS	(6)		// up to 6 for TPM0; max. 2 for TPM1 or TPM2

void test_waveio_driver_rc(void)
{
	uint16_t inputs[RC_NUM_CHANNELS];	// values for the pulse on each channel
	uint16_t pulse;

	uint8_t channel;
	uint16_t a, b;

	// Initialize the wavein driver
	waveio_init(WAVEIO_RANGE_0_5);

	// Connect the channels
	for ( channel = 0; channel < RC_NUM_CHANNELS; channel++ )
	{
		waveio_in_attach(channel);
	}

	// Start measuring on these channels
	for ( channel = 0; channel < RC_NUM_CHANNELS; channel++ )
	{
		waveio_in_start(channel);
	}

	while(1)
	{
		//
		// 1) read the inputs
		//
		// do not need to check for error, the measured results
		// are guaranteed to be 0 in case of error
		for ( channel = 0; channel < RC_NUM_CHANNELS; channel++ )
		{
			inputs[channel] = 0;	// preset invalid pulse value

			waveio_in_read(channel, &a, &b);

			if ( a != 0 && b != 0 )
			{
				// Note that we do not know which part of the wave is the
				// pulse and which is the "space". So take the shorter part...
				if ( a < b )
					pulse = a;
				else
					pulse = b;
				// Check if the pulse length is valid
				if ( pulse > 800 && pulse < 2200 )
				{
					inputs[channel] = pulse;
				}

			}
		}


		//
		// 2) Display the inputs
		//
		for ( channel = 0; channel < RC_NUM_CHANNELS; channel++ )
		{
			msf_print("CH ");
			msf_printnum(channel);
			if ( inputs[channel] != 0 )
			{
				msf_print(": ");
				msf_printnum(inputs[channel]);
				msf_print(" us \n");
			}
			else
			{
				msf_print(" - \n");
			}
			// short delay to allow the UART driver to send the data; otherwise the
			// buffer for transmit could be overwritten if we send too many characters
			// too quickly
			msf_delay_ms(50);
		}

		msf_print("\n");
		msf_delay_ms(1000);

	}

}

/* test_wavein_driver_sonar
 * Show how to use the Parallax Ping ultrasonic sensor.
 * To start measurement, send 5 us pulse.
 * After 750 ms the sensor will respond with pulse with lenght
 * proportional to distance. The pulse is 115 us to 18.5 ms long.
 *
 * Uses MCU pin PTD0 (Arduino pin 10), which is also channel 0 for wavein driver.
 *
 * */
void test_waveio_driver_sonar(void)
{
	uint16_t a;

	// Initialize the wavein driver
	waveio_init(WAVEIO_RANGE_0_5);

	while(1)
	{
		// Generate start pin
		msf_pin_direction(GPIO_D0, output);
		msf_pin_write(GPIO_D0, true);
		msf_delay_us(5);
		msf_pin_write(GPIO_D0, false);

		// measure the pulse
		waveio_in_attach(0);
		a = waveio_in_pulse_wait(0, 200);
		if (a == 0)
			msf_print("Timeout.\n");
		else
		{
			msf_printnum(a);
			msf_print(" us, ");
			// print also distance in cm and inches
			// The speed of sound is 340 m/s or 29 microseconds per centimeter.
			// The ping travels out and back, so to find the distance of the
			// object we take half of the distance traveled.
			msf_printnum(a/29/2);
			msf_print(" cm, ");
			// 73.746 microseconds per inch...
			msf_printnum(a/74/2);
			msf_print(" inches \n");
		}
		waveio_in_detach(0);

		msf_delay_ms(1000);
	}

}




/* waveout */
#if 0
/* test_waveout_driver_single
 * Generate signal on single output pin (single waveout channel).
 * Start the program and connect oscilloscope or multimeter to the output pin.
 * You should see 1 kHz signal on this pin.
 * */
void test_waveout_driver_single(void)
{
	// Initialize waveout driver telling it which channel(s) we will use.
	// The channels are given in ranges as follows from the hardware underneath
	// - the TPM timer modules which are TPM0 with 6 channels, TPM1 and TPM2 each
	// with 2 channels.
	// See also the pin table at the top of this file.
	waveout_init(WAVEOUT_RANGE_0_5);

	// Attach the channel (pin) we will use
	//waveout_channel_attach(0);



	while(1)
	{
		// Start generating the signal
		// Note that the function does not block; the program continues execution!
		waveout_channel_start(0, 500, 500);

		msf_pin_write(RED_LED, true);		// red off
		msf_pin_write(GREEN_LED, false);	// green on

		msf_delay_ms(10000);

		msf_pin_write(GREEN_LED, true);	// green off
		msf_pin_write(RED_LED, false);		// red on

		waveout_channel_stop(0);

		msf_delay_ms(5000);
	}
}



/* test_waveout_driver_all
 * Generate signal on all output channels at once.
 * Start the program and connect oscilloscope or multimeter to
 * any of the supported output pins - see the pin table at the top of this file.
 * You should see signals with various frequency and duty, see the code.
 *
 * */
void test_waveout_driver_all(void)
{
	int i;

	// Initialize waveout driver telling it which channel(s) we will use
	waveout_init(WAVEOUT_RANGE_0_5 | WAVEOUT_RANGE_6_7 | WAVEOUT_RANGE_8_9);

	// Attach the channels (switch pin to timer mode)
	/*for (i=0; i<10; i++ )
	{
		waveout_channel_attach(i);
	}*/

	// Start generating the signal
	waveout_channel_start(0, 500, 500);	// 1 kHz, 50% duty; D0  (Arduino 10)
	waveout_channel_start(1, 100, 900);	// 1 kHz, 10% duty; A4	(Arduino 4)
	waveout_channel_start(2, 900, 100);	// 1 kHz, 90% duty; A5	(Arduino 5 )
	waveout_channel_start(3, 250, 250);	// 2 kHz, 50% duty; D3	(Arduino 12)
	waveout_channel_start(4, 5000, 5000);	// 100 Hz, 50% duty; D4	(Arduino 2)
	waveout_channel_start(5, 1500, 18500);	// servo, 50 Hz, 1.5 ms pulse; C9	(Arduino 7)
	waveout_channel_start(6, 250, 750);	// A12	(Arduino 3)
	waveout_channel_start(7, 500, 500);	// A13	(Arduino 8)
	waveout_channel_start(8, 750, 250);	// B2  (Arduino 16 (A2))
	waveout_channel_start(9, 600, 400);	// B3	(Arduino 17 (A3))

	// Do not leave this function.
	// Note that the waveout_channel_start does not block the caller.
	for (;;) {
	     i++;
	}
}

/* test_waveout_driver_servo
 * Demonstrate control of a RC servo motor.
 * Sweep the servo from one end position to the other
 * */
void test_waveout_driver_servo(void)
{
	uint16_t angle = 0;
	int direction = 1;

	// Initialize waveout driver telling it which channel(s) we will use
	waveout_init(WAVEOUT_RANGE_0_5 );	// | WAVEOUT_RANGE_6_7 | WAVEOUT_RANGE_8_9);

	while(1)
	{
		for ( angle = 0; angle < 180; angle++ )
		{
			// generate servo signal on channel 0 = pin D0 (arduino pin 10)
			waveout_servo(0, angle);
			waveout_servo(1, angle);
			waveout_servo(2, angle);
			msf_delay_ms(20);
		}

		for ( angle = 180; angle > 0; angle-- )
		{
			// generate servo signal on channel 0 = pin D0 (arduino pin 10)
			waveout_servo(0, angle);
			waveout_servo(1, angle);
			waveout_servo(2, angle);
			msf_delay_ms(20);
		}

		/*
		if ( direction )
		{
			angle +=1;
			if ( angle >= 180 )
			{
				direction = 0;
				angle = 180;
			}
		}
		else
		{
			if (angle >= 1)
			{
				angle -= 1;
			}
			else
			{
				angle = 0;
				direction = 1;
			}
		}*/

	}

}
#endif

/* handler for the non-maskable (NMI) interrupt.
 * The PTA4 pin (Arduino pin 4) is by default used as NMI input and
 * connecting low signal to this pin causes NMI interrupt - the defualt handler
 * is just endless loop. This is no problem if we use the PTA4 pin, because we
 * configure it to a different mode than NMI and all works fine.
 * However, if we by mistake connect the signal to this pin while it is not used
 * in the program, the program will hang (go into the default handler).
*/
void NMI_Handler(void)
{
	// do nothing, just override the default handler which contains endless loop.
}


////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
