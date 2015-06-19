/* waveio example.
 *
 * Demonstrate usage of the waveio high-level driver for measuring and generating signals.
 *
 * The driver can be used with any or all of the TPM0, TPM1 or TPM2 low-level timer drivers.
 *
 * This project contains several example use-cases in separate functions.
 * Un-comment one of the functions at a time to test it.
 *
 * Testing the program:
 * - run the example program
 * - measure the signal on the selected output pin with oscilloscope or multi-meter
 * with frequency and duty measurement.
 * - or connect the input signal to selected pins and see the output of the program
 * to the OpenSDA serial port in a terminal:
 *		- open terminal emulator on your PC (for example, Tera Term or Putty)
 *		and open the OpenSDA COM port and set baudrate to 19200
 *		- run example program
 *		- connect input signal to the selected pin of your FRDM board (see below)
 *		You should see the output of the program in your terminal window.
 *
 *
 * The waveio driver supports all the clock options available as of KDS 1.1.1
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
 * Notes about the usage of the waveio driver:
 * The values given to/read from the driver are length of the half-waves
 * of the signal in microseconds (us).
 * Example: 1 and 1 means wave length 2 us (signal period = 2 us)
 * frequency_in_Hz = 1/period_in_second = 1 000 000 / period_in_us
 *  frequency_in_kHz = 1 000 / period_in_us
 *  period_in_us = 1 000 / frequency_in_kHz
 *	 Example:
 *	We want to obtain 10 kHz output:
 *	period = 1000/10 = 100
 *	For square wave signal the half-wave values are 50 and 50
 * The limits for the signal are described in the waveio header.
 * For output (48 MHz CPU)
 * TPM0 max frequency about 35 KHz (half-waves: 14,14) for 1 channel active
 *		for 5 channels max. about 10 kHz
 * TPM1 or TPM2 max frequency about 45 KHz (half-waves: 11,11)
 *
 */

/* MSF configuration for this project */
#include "msf_config.h"
/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"
#include "msf.h"

#include "waveio.h"		/* The waveio driver */

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
void test_waveio_rc_follow(void);


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


	//
	// Uncomment one of these functions at a time...
	//
	// outputs:
	//test_waveio_driver_single();
	//test_waveio_driver_all();
	//test_waveio_driver_servo();
	//
	// inputs:
	//test_waveio_driver_wave();
	//test_waveio_driver_pulse();
	//test_waveio_driver_multichannel();
	test_waveio_driver_rc();
	//test_waveio_driver_sonar();
	//
	// input and output together:
	//test_waveio_rc_follow();



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
 * You should see 1 kHz signal on this pin while the green LED is on.
 * While the red LED is on, there should be no signal.
 *
 * Channel used: WAVEIO_C0 = pin PTD0 (Arduino pin 10).
 * */
void test_waveio_driver_single(void)
{
	// Initialize waveio driver telling it which channel(s) we will use.
	// The channels are given in ranges as follows from the hardware underneath
	// - the TPM timer modules which are TPM0 with 6 channels, TPM1 and TPM2 each
	// with 2 channels.
	// See also the pin table at the top of this file.
	waveio_init(WAVEIO_RANGE_0_5);	// | WAVEIO_RANGE_6_7 | WAVEIO_RANGE_8_9);


	while(1)
	{
		// Start generating the signal
		// Note that the function does not block; the program continues execution!
		// You can generate signal or multiple channels (pins).
		// 500 us high and 500 us low > 1 kHz square wave signal.
		waveio_out_start(WAVEIO_C0, 500, 500);

		// Generate the signal for 10 seconds, then pause for 5 seconds
		//and generate again for 10 s, and so on...
		msf_pin_write(RED_LED, true);		// red off
		msf_pin_write(GREEN_LED, false);	// green on

		msf_delay_ms(10000);

		msf_pin_write(GREEN_LED, true);		// green off
		msf_pin_write(RED_LED, false);		// red on

		// Stop the signal
		waveio_out_stop(WAVEIO_C0);

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

	// Initialize waveio driver telling it which channel(s) we will use.
	// In this case we initialize for all channels ( 0 thru 9).
	waveio_init(WAVEIO_RANGE_0_5 | WAVEIO_RANGE_6_7 | WAVEIO_RANGE_8_9);

	// Start generating the signal
	waveio_out_start(WAVEIO_C0, 500, 500);	// 1 kHz, 50% duty; D0  (Arduino 10)
	waveio_out_start(WAVEIO_C1, 100, 900);	// 1 kHz, 10% duty; A4	(Arduino 4)
	waveio_out_start(WAVEIO_C2, 900, 100);	// 1 kHz, 90% duty; A5	(Arduino 5 )
	waveio_out_start(WAVEIO_C3, 250, 250);	// 2 kHz, 50% duty; D3	(Arduino 12)
	waveio_out_start(WAVEIO_C4, 5000, 5000);	// 100 Hz, 50% duty; D4	(Arduino 2)
	waveio_out_start(WAVEIO_C5, 1500, 18500);	// servo, 50 Hz, 1.5 ms pulse; C9	(Arduino 7)
	waveio_out_start(WAVEIO_C6, 250, 750);	// A12	(Arduino 3)
	waveio_out_start(WAVEIO_C7, 500, 500);	// A13	(Arduino 8)
	waveio_out_start(WAVEIO_C8, 750, 250);	// B2  (Arduino 16 (A2))
	waveio_out_start(WAVEIO_C9, 600, 400);	// B3	(Arduino 17 (A3))

	// Do not leave this function.
	// Note that the waveout_channel_start does not block the caller.
	for (;;) {
	     i++;
	}
}

/* test_waveio_driver_servo
 * Demonstrate control of a RC servo motor.
 * Sweep the servo from one end position to the other.
 *
 * Channels used:
 * WAVEIO_C0 = pin PTD0 (Arduino pin 10).
 * WAVEIO_C1 = pin PTA4 (Arduino pin 4).
 * WAVEIO_C2 = pin PTA5 (Arduino pin 5).
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
			waveio_out_servo(WAVEIO_C0, angle);
			waveio_out_servo(WAVEIO_C1, angle);
			waveio_out_servo(WAVEIO_C2, angle);
			msf_delay_ms(20);
		}

		for ( angle = 180; angle > 0; angle-- )
		{
			// generate servo signal on channel 0 = pin D0 (arduino pin 10)
			waveio_out_servo(WAVEIO_C0, angle);
			waveio_out_servo(WAVEIO_C1, angle);
			waveio_out_servo(WAVEIO_C2, angle);
			msf_delay_ms(20);
		}
	}

}




/* test_waveio_driver_wave
 * Measure the half-waves of the signal.
 * Connect the signal to one of the supported pins (see table at the top of this file)
 * and set the INPUT_CHANNEL constant above.
 * Start the program and connect to the OpenSDA port with terminal. (baud 19200).
 * You should see the lengths of the half-waves of the signal.
 * */
// Define the channel for test_wavein_driver_wave() and test_wavein_driver_pulse().
#define		INPUT_CHANNEL	(WAVEIO_C0)

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
 * Connect the input signal to one or more of the supported pins
 * (see table at the top of this file).
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

	// Connect the channels - all available channels.
	waveio_in_attach(WAVEIO_C0);
	waveio_in_attach(WAVEIO_C1);
	waveio_in_attach(WAVEIO_C2);
	waveio_in_attach(WAVEIO_C3);
	waveio_in_attach(WAVEIO_C4);
	waveio_in_attach(WAVEIO_C5);
	waveio_in_attach(WAVEIO_C6);
	waveio_in_attach(WAVEIO_C7);
	waveio_in_attach(WAVEIO_C8);
	waveio_in_attach(WAVEIO_C9);

	// Start measuring on these channels
	waveio_in_start(WAVEIO_C0);
	waveio_in_start(WAVEIO_C1);
	waveio_in_start(WAVEIO_C2);
	waveio_in_start(WAVEIO_C3);
	waveio_in_start(WAVEIO_C4);
	waveio_in_start(WAVEIO_C5);
	waveio_in_start(WAVEIO_C6);
	waveio_in_start(WAVEIO_C7);
	waveio_in_start(WAVEIO_C8);
	waveio_in_start(WAVEIO_C9);

	while(1)
	{
		// Read values from all channels
		for ( i = 0; i<10; i++ )
		{
			display_channel(i);
			msf_delay_ms(50);	// delay needed so that the transmit buffer
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
		msf_print(" us, 2: ");
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


/* test_waveio_driver_rc
 * Show how to use the wavein driver for processing input from
 * RC receiver (radio controlled models).
 * The signal is a pulse between 1 and 2 ms repeating every 20 ms.
 * */

/* array with the used channels. Thanks to this array we can use loops to
 * work with the channels and still use the channel names rather than simple
 * numbers, for example we use WAVEIO_C2 instead of 2. */
WAVEIO_channel channels[] = {
		WAVEIO_C0, WAVEIO_C1, WAVEIO_C2, WAVEIO_C3,
		/* WAVEIO_C4, WAVEIO_C5, WAVEIO_C6, WAVEIO_C7,*/
		/* WAVEIO_C8, WAVEIO_C9,*/
		WAVEIO_C_INVALID /* keep this as the last value in the array! */};

void test_waveio_driver_rc(void)
{
	uint16_t inputs[10];	// values for the pulse on each channel
	uint16_t pulse;
	uint_fast8_t i;

	WAVEIO_channel channel;
	uint16_t a, b;

	// Initialize the wavein driver - for using 6 channels: 0 thru 5
	waveio_init(WAVEIO_RANGE_0_5 | WAVEIO_RANGE_6_7 | WAVEIO_RANGE_8_9);

	// Connect the channels we will need now
	for ( i=0; channels[i] != WAVEIO_C_INVALID; i++ )
	{
		waveio_in_attach(channels[i]);
	}


	// Start measuring on these channels
	for ( i=0; channels[i] != WAVEIO_C_INVALID; i++ )
	{
		waveio_in_start(channels[i]);
	}

	while(1)
	{
		//
		// 1) read the inputs
		//
		// We do not need to check for return value of error waveio_in_read,
		// measured results are guaranteed to be 0 in case of error.
		for ( i = 0; channels[i] != WAVEIO_C_INVALID; i++ )
		{
			inputs[i] = 0;	// preset invalid pulse value

			// Version A:
			// Using convenience function waveio_in_servo_read_us
			// which is also available in the waveio driver.
			a = waveio_in_servo_read_us(channels[i]);
			if ( a > 0 )
				inputs[i] = a;

			// Version B:
			// Processing the input from receiver using general
			// purpose function waveio_in_read:
			/*
			waveio_in_read(channels[i], &a, &b);
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
					inputs[i] = pulse;
				}

			}*/
		}


		//
		// 2) Display the inputs
		//
		for ( i = 0; channels[i] != WAVEIO_C_INVALID; i++ )
		{
			msf_print("CH ");
			msf_printnum(i);
			if ( inputs[i] != 0 )
			{
				msf_print(": ");
				msf_printnum(inputs[i]);
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

/* test_waveio_driver_sonar
 * Show how to use the Parallax Ping ultrasonic sensor.
 * The sensor uses one pin both for starting the measurement and for returning
 * the result.
 * To start measurement, send 5 us pulse on the pin.
 * After 750 us the sensor will respond with pulse on the same pin.
 * The pulse has length proportional to distance the sound travelled
 * to and from the obstacle.
 * The pulse is 115 us to 18.5 ms long.
 *
 * This example uses MCU pin PTD0 (Arduino pin 10), which is
 * channel WAVEIO_C0 for waveio driver.
 *
 * Connect the PING sensor SIG pin to PTD0 (Arduino pin 10) on the FRDM board.
 * Connect the PING GND to FRDM GND and the PING 5V to the +5V pin of FRDM.
 * Start the program and connect to the OpenSDA port with terminal. (baud 19200).
 * You should see the distance returned by the Ping sensor.
 *
 * */
void test_waveio_driver_sonar(void)
{
	uint16_t a;

	// Initialize the wavein driver
	waveio_init(WAVEIO_RANGE_0_5);

	while(1)
	{
		// Generate start pulse - 5 us high level on the pin
		msf_pin_direction(GPIO_D0, output);
		msf_pin_write(GPIO_D0, true);
		msf_delay_us(5);
		msf_pin_write(GPIO_D0, false);

		// Measure the pulse
		waveio_in_attach(WAVEIO_C0);
		a = waveio_in_pulse_wait(WAVEIO_C0, 200);	// 200 ms timeout
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

		// detach the channel from waveio driver so that it can be used in
		// GPIO mode to generate new start pulse.
		waveio_in_detach(WAVEIO_C0);

		msf_delay_ms(1000);
	}

}

/* test_waveio_rc_follow
 * Demonstrate reading input from RC receiver and controlling servo.
 * Connect output from RC receiver to pin PTD0 (channel WAVEIO_C0).
 * Connect servo to pin PTA4 (WAVEIO_C1).
 * The servo should respond to the signal from the receiver in the same
 * was as if connected directly.
 *
 * Channels used:
 * WAVEIO_C0 = pin PTD0 (Arduino pin 10) - input from receiver (PPM)
 * WAVEIO_C1 = pin PTA4 (Arduino pin 4). - output for servo
 */
void test_waveio_rc_follow(void)
{
	uint16_t input;

	// Initialize the wavein driver - for using 6 channels: 0 thru 5
	waveio_init(WAVEIO_RANGE_0_5);	// | WAVEIO_RANGE_6_7 | WAVEIO_RANGE_8_9);

	// Connect the channel we will need now - only for inputs.
	// Outputs do not need to be connected.
	waveio_in_attach(WAVEIO_C0);

	// Start measuring on the channel
	waveio_in_start(WAVEIO_C0);

	// Set the servo to the middle for now (1500 us pulses)
	waveio_out_servo_us(WAVEIO_C1, 1500);

	while (1)
	{
		input = waveio_in_servo_read_us(WAVEIO_C0);
		if ( input > 0 )
			waveio_out_servo_us(WAVEIO_C1, input);

		msf_delay_ms(100);
	}



}


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
