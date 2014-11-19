/*
 * Blink sample for FRDM-KL25Z board.
 * MSF Lite library
 * LED connections:
 * B18 	- Red LED
 * B19 	- Green LED
 * D1	- Blue LED (Arduino pin D13)
 * All LEDs are on, if the pin is low and off if it is high.
 *
 */

/* MSF configuration for this project */
#include "msf_config.h"
/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"
#include "msf.h"

int main(void)
{
	int counter = 0;

	/* Initialise the MSF library*/
	msf_init(0);

	/* Set pin direction to output */
	msf_pin_direction(GPIO_B18, output);
	/* Writing the pin high turns the LED off*/
	msf_pin_write(GPIO_B18, true);

	for(;;) {
		counter++;
		/* Writing the pin high turns the LED off*/
		msf_pin_write(GPIO_B18, false);
		msf_delay_ms(500);
		msf_pin_write(GPIO_B18, true);
		msf_delay_ms(500);
	}

	return 0;

}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
