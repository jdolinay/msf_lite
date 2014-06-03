/*
 * Sample program for Arduino-compatibility layer in MSF lite
 * The FRDM-KL25Z board has RGB LED:
 * Arduino pin: 13 (PTD1) - blue LED (cathode)
 *  PTB18 - red LED
 *  PTB19 - green LED
 *  The red and green LEDs are not available using the Arduino pin numbers because they are
 *  not connected to the Arduino-compatible pins on the board. Use can use msf_pin_write with 
 *  the board pin code, e.g. PIN_B18 to access these pins.
 * 
 *
 */
#include "arduino.h"

void setup(void)
{
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);	// Setting the pin high turns the blue LED off
	
	// Access to non-arduino pins
	// B18 where red LED is connected
	// B19 where green LED is connected
	msf_pin_direction(PIN_B18, output);
	msf_pin_write(PIN_B18, true);		// set pin high to turn LED off	
	msf_pin_direction(PIN_B19, output);
	msf_pin_write(PIN_B19, true);	
}


void loop(void)
{
	// Arduino program
	digitalWrite(13, LOW);
	delay(500);
	digitalWrite(13, HIGH);
	delay(500);
	
	// Non-Arduino pin accessed using the native MSF functions
	msf_pin_write(PIN_B18, false);	// Red LED on	
	delay(500);
	msf_pin_write(PIN_B18, true);	// off
	delay(500);
	
	msf_pin_write(PIN_B19, false);	// Green LED on	
	delay(300);
	msf_pin_write(PIN_B19, true);	// LED off
	delay(300);
}




/* ------------------------------------------------
 * End of Arduino code. Please do not modify the code below. 
*/
/* -------------------------- Hic sunt leones  ---------------------------- */
int main(void)
{
	/* Internal initialization */
	msf_init(0);
	
	setup();
	
	
	for(;;) {	   
	   	loop();
	}
	
	return 0;
}
