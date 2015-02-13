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
 * 0				TPM0 channel 0	D0  (Arduino 10)
 * 1				TPM0 channel 1	A4	(Arduino 4 )
 * 2				TPM0 channel 2	A5	(Arduino 5)
 * 3				TPM0 channel 3	D3	(Arduino 12)
 * 4				TPM0 channel 4	D4	(Arduino 2)
 * 5				TPM0 channel 5	C9	(Arduino 7)
 * 6				TPM1 channel 0  A12	(Arduino 3)
 * 7				TPM1 channel 1  A13	(Arduino 8)
 * 8				TPM2 channel 0  B2	(Arduino 16 (A2))
 * 9				TPM2 channel 1  B3	(Arduino 17 (A3))
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

#include "waveout.h"	// waveout driver

/* Defines for handling LEDs easier
LED connections:
 B18 	- Red LED
 B19 	- Green LED
 D1	- Blue LED (Arduino pin D13)
 All LEDs are on, if the pin is low and off if it is high.
 */
#define	RED_LED			PIN_B18		// or GPIO_B18
#define	GREEN_LED		PIN_B19		// or GPIO_B19

void test_waveout_driver_single(void);
void test_waveout_driver_all(void);
void test_waveout_driver_servo(void);

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
	//test_waveout_driver_single();
	//test_waveout_driver_all();
	test_waveout_driver_servo();


    /* This for loop should be replaced. By default this loop allows a single stepping. */
    for (;;) {
        gi++;
    }
    /* Never leave main */
    return 0;
}

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
