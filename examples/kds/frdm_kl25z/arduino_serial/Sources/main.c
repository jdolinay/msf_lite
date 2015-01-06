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
 * IMPORTANT: For the floating point numbers to work, you need to add this flag:
 * -u _printf_float
 * to the Cross ARM C++ Linker settings > Miscellaneous > Other Linker Flags
 * Without adding this flag, you will see no output for the float number.
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

	delay(1000);

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


	/*
	// This is example code for analogWrite
	// You can customize the pins and frequency in msf_config.h
	// See the comment for analogWrite in arduino.c for detailed description.
	// The default pins: 2,3,4,5,7,8,10,12,18,19 can be used without any changes
	// in msf_config.h.
	// Default PWM frequency is 500 Hz, see MSF_AWRITE_500HZ in msf_config.h
	// For mapping of the Arduino pin numbers used in this example
	// to the MCU pins (port+pin) see arduino.h file.
	analogWrite(2, 3); 		// 1% - D4
	analogWrite(3, 178); 	// 70% - A12
	analogWrite(4, 52);		// 20% - A4
	analogWrite(5, 52);		// 20% - A5
	analogWrite(7, 52);		// 20% - C9
	analogWrite(8, 128);	// 50% - A13
	analogWrite(10, 52);	// 20% - D0
	analogWrite(12, 255);	// 100% - D3
	analogWrite(18, 52);	// 20% - B2
	analogWrite(19, 178);	// 70% - B3
	*/
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
