/*
 * Sample program for Arduino-compatibility layer in MSF lite
 *
 * This sample demonstrates the Serial functions.
 * Note that the implementation is limited compared to the Arduino serial class,
 * there is no "general" function for printing any variable type; just several
 * functions, each to print specific type - see the code.
 *
 * How the program works:
 * When started, it will print "hello".
 * When you send any character to the program, it will echo it back and moreover if
 * the character is:
 * f - will print sample float number
 * d - will print sample integer in decimal
 * x - will print sample integer in hex format
 * o - will print sample integer in octal format
 *
 */
#include "arduino.h"

void setup(void)
{

	// Serial line
	Serial.begin(115200);
}


void loop(void)
{
	char c;
	int decplaces = 2;

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
