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
	
	// Serial line
	Serial.begin(115200);
}


void loop(void)
{
	char c;
	int decplaces = 2;
	
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
	
	Serial.println("hello");
	if ( Serial.available() > 0)
	{
		c = Serial.read();
		Serial.print("received: ");	
		msf_print_char(c);
		Serial.print("\n");		
		if ( c == 'f')
			Serial.printFloat(15.12345678912, decplaces);
		else if ( c == 'd' )
			Serial.printInt(254, DEC);
		else if ( c == 'x' )
			Serial.printInt(254, HEX);
		else if ( c == 'o' )
			Serial.printInt(17, OCT);
	}
	
	// testing analog write
	// default pins: 2,3,4,5,7,8,10,12,18,19. 
	analogWrite(2, 3); 	// 1% - D4	
	analogWrite(3, 178); // 70% - A12
	analogWrite(4, 52);	// 20% - A4  
	analogWrite(5, 52);	// 20% - A5
	analogWrite(7, 52);	// 20% - C9
	analogWrite(8, 128);	// 50% - A13
	analogWrite(10, 52);	// 20% - D0
	analogWrite(12, 255);	// 100% - D3
	analogWrite(18, 52);	// 20% - B2
	analogWrite(19, 178);	// 70% - B3
}




/* ------------------------------------------------
 * End of Arduino code. Please do not modify the code below. 
*/
/* -------------------------- Hic sunt leones  ---------------------------- */
int main(void)
{
	/* Internal initialisation */
	msf_init(0); 
	arduino_init();
	
	
	setup();
	
	
	for(;;) {	   
	   	loop();
	}
	
	return 0;
}
