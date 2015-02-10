/* wavein example.
 *
 * Demonstrate usage of the wavein high-level driver.
 *
 * The driver can be used with either TPM0, TPM1 or TPM2 low-level timer driver,
 * see msf_config.h > WAVEIN_TIMER_DRIVER.
 *
 * This project contains several example use-cases in separate functions.
 * Uncomment one of the functions at a time to test it.
 *
 * Testing the program:
 * - open terminal emulator on your PC (for example, Tera Term or Putty)
 *  and open the OpenSDA COM port and set baudrate to 19200
 * - run this example project
 * - connect input signal to the selected pin of your FRDM board (see below)
 * You should see the output of the program in your terminal window.
 *
 *
 * The wavein driver supports all the clock options available as of KDS 1.1.1
 * and automatically sets the timer to 1 MHz clock and returns values in
 * microseconds (us).
 *
 * Table of the pins which can be used as input:
 * wavein channel    timer channel   default pin (see msf_config.h)
 * if TPM0 used for wavein
 * 0				TPM0 channel 0	D0  (Arduino 10)
 * 1				TPM0 channel 1	A4	(Arduino 4 )
 * 2				TPM0 channel 2	A5	(Arduino 5)
 * 3				TPM0 channel 3	D3	(Arduino 12)
 * 4				TPM0 channel 4	D4	(Arduino 2)
 * 5				TPM0 channel 5	C9	(Arduino 7)
 *
 * if TPM1 used for wavein
  * 0				TPM1 channel 0  A12	(Arduino 3)
 * 1				TPM1 channel 1  A13	(Arduino 8)
 *
 * if TPM2 used for wavein
 * 0				TPM2 channel 0  B2	(Arduino 16 (A2))
 * 1				TPM2 channel 1  B3	(Arduino 17 (A3))
 *
*/

/* MSF configuration for this project */
#include "msf_config.h"
/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"
#include "msf.h"

//#include "drv_tpm.h"	/* TPM timer driver */

#include "wavein.h"	// wavein high-level driver for measuring input signal


void test_wavein_driver_wave(void);
void test_wavein_driver_pulse(void);
void test_wavein_driver_multichannel(void);
void display_channel(uint8_t channel);
void test_wavein_driver_rc(void);
void test_wavein_driver_sonar(void);
static int i = 0;

int main(void)
{


	// Always initialize the MSF library
	msf_init(0);

	// Various examples of use for the wavein driver.
	// un-comment one at a time to try it.
	//test_wavein_driver_wave();
	//test_wavein_driver_pulse();
	//test_wavein_driver_multichannel();
	test_wavein_driver_rc();
	//test_wavein_driver_sonar();

    /* This for loop should be replaced. By default this loop allows a single stepping. */
    for (;;) {
        i++;
    }
    /* Never leave main */
    return 0;
}


// Define the channel for test_wavein_driver_wave() and test_wavein_driver_pulse().
#define		INPUT_CHANNEL	(0)

/* test_wavein_driver_wave
 * Measure the half-waves of the signal.
 * Connect the signal to one of the supported pins (see table above) and set
 * the INPUT_CHANNEL constant above.
 * Start the program and connect to the OpenSDA port with terminal. (baud 19200).
 * You should see the lengths of the half-waves of the signal.
 * */
void test_wavein_driver_wave(void)
{
	uint16_t a, b;
	uint8_t err;

	// Use case 1 - measure wave signal
	wavein_init();
	wavein_channel_attach(INPUT_CHANNEL);
	wavein_channel_start(INPUT_CHANNEL);
	while(1)
	{
		err = wavein_channel_read(INPUT_CHANNEL, &a, &b);
		if ( err == WAVEIN_NO_ERROR )
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
			// see wavein.h for error codes; most common will be 2 = WAVEIN_ERROR_NOINPUT
			msf_print("\n");
		}

		msf_delay_ms(1500);
	}

}

/* test_wavein_driver_pulse
 * Measure pulse on input.
 * Connect the signal to one of the supported pins (see table above) and set
 * the INPUT_CHANNEL constant above.
 * Start the program and connect to the OpenSDA port with terminal. (baud 19200).
 * You should see the length of the pulse detected on the input. If no pulse
 * is detected for 1 second, the program will display "timeout" message.
 * */
void test_wavein_driver_pulse(void)
{
	uint16_t a;

	wavein_init();
	wavein_channel_attach(INPUT_CHANNEL);
	// Note: no need to start measurement by calling wavein_channel_start(n);
	while(1)
	{
		a = wavein_channel_pulse_wait(INPUT_CHANNEL, 1000);
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

/* test_wavein_driver_multichannel
 * Measure the half-waves of the signal on multiple pins at once.
 * Connect the input signal to one or more of the supported pins (see table above).
 * Start the program and connect to the OpenSDA port with terminal. (baud 19200).
 * You should see the lengths of half-waves detected on each of the active pins.
 * For unconnected pins the output will say Error 2, which means no signal.
 * See wavein.h for error codes.
 *
 * */
void test_wavein_driver_multichannel(void)
{
	uint8_t i;

	// Use case 1 - measure wave signal
	wavein_init();

	// Connect the channels
	wavein_channel_attach(0);
	wavein_channel_attach(1);
	wavein_channel_attach(2);
	wavein_channel_attach(3);
	wavein_channel_attach(4);
	wavein_channel_attach(5);

	// Start measuring on these channels
	wavein_channel_start(0);
	wavein_channel_start(1);
	wavein_channel_start(2);
	wavein_channel_start(3);
	wavein_channel_start(4);
	wavein_channel_start(5);

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

	err = wavein_channel_read(channel, &a, &b);
	if ( err == WAVEIN_NO_ERROR )
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

void test_wavein_driver_rc(void)
{
	uint16_t inputs[RC_NUM_CHANNELS];	// values for the pulse on each channel
	uint16_t pulse;

	uint8_t channel;
	uint16_t a, b;

	// Initialize the wavein driver
	wavein_init();

	// Connect the channels
	for ( channel = 0; channel < RC_NUM_CHANNELS; channel++ )
	{
		wavein_channel_attach(channel);
	}

	// Start measuring on these channels
	for ( channel = 0; channel < RC_NUM_CHANNELS; channel++ )
	{
		wavein_channel_start(channel);
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

			wavein_channel_read(channel, &a, &b);

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
void test_wavein_driver_sonar(void)
{
	uint16_t a;

	// Initialize the wavein driver
	wavein_init();

	while(1)
	{
		// Generate start pin
		msf_pin_direction(GPIO_D0, output);
		msf_pin_write(GPIO_D0, true);
		msf_delay_us(5);
		msf_pin_write(GPIO_D0, false);

		// measure the pulse
		wavein_channel_attach(0);
		a = wavein_channel_pulse_wait(0, 200);
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
		wavein_channel_detach(0);

		msf_delay_ms(1000);
	}

}


/* ======================================================================== */

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
