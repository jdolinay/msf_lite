/*
 * Sample program for MSF lite
 *
 * This sample demonstrates the UART driver.
 * On your computer you will need Terminal emulator, for example Tera Term or Hyperterminal
 * to communicate with the program in the MCU.
 * Use the Open SDA virtual serial port for communication. You can find the COM number
 * for this port on your computer in the Windows Device manager, but ONLY WHEN
 * the FRDM board is connected to your computer.
 *
 * This file contains the code for following modes:
 * - UART0 driver in polled and interrupt-driven mode
 * - coniob MSF driver (using UART0) - this is buffered 'console' I/O driver
 * - UART1 and UART2 drivers
 *
 */

/* MSF configuration for this project */
#include "msf_config.h"
/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"
#include "msf.h"
#include <string.h>	// for strlen()

#include "msf.h"
#include "drv_uart.h"	/* UART driver */
#include "coniob.h"		/* Buffered console I/O */


#define	RED_LED		GPIO_B18
#define	GREEN_LED	GPIO_B19
#define	BLUE_LED	GPIO_D1

// prototypes
void polled_uart_test(void);
void interrupt_uart_simple_test(void);
void interrupt_uart_full_test(void);
void coniob_test(void);
void uart12_test(void);



static int i = 0;

int main(void)
{

	/* Always initialise the MSF library*/
	msf_init(0);

	msf_pin_direction(RED_LED, output);
	msf_pin_write(RED_LED, true);
	msf_pin_direction(GREEN_LED, output);
	msf_pin_write(GREEN_LED, true);


	/* Test the UART driver
	 * Please un-comment one of the functions.
	 */
	polled_uart_test();
	//interrupt_uart_simple_test();
	//interrupt_uart_full_test();
	//coniob_test();
	//uart12_test();

	/* This for loop should be replaced. By default this loop allows a single stepping. */
    for (;;) {
        i++;
    }
    /* Never leave main */
    return 0;
}

/* Strings used in polled_uart_test to display current baud rate */
const char* str_115200 = "115200\n\r";
const char* str_9600 = "9600\n\r";

/* Test the UART driver in simple polled mode.
 * Prints text to serial line.
 * Send any char from terminal to display prompt for
 * changing baud rate. Then send 9 to use 9600 bd or 1 to use 115200 bd,
 * anything else to keep current baudrate.
 * IMPORTANT: After you change the baudrate, you need to change it also
 * in your Terminal program to continue the communication with the board!
 */
void polled_uart_test(void)
{
	int i;
	uint32_t baud = BD9600;
	char buff[4];
	const char* str;

	/* Initialize the driver for UART0 */
	Driver_UART0.Initialize(baud, null);

	/* Set the communication options */
	/* Note for mode with parity and 2 stop bits
	 * IMPORTANT: the terminal must be set for 7 data bits if the UART driver is
	 * set to 8-bit mode and parity as the 8th bit will be parity.
	 * Or set the driver to 9-bit mode and use 8-data bits in terminal. */
	Driver_UART0.Control(MSF_UART_PARITY_NONE | MSF_UART_STOP_BITS_2, 0);
	//Driver_UART0.Control(MSF_UART_PARITY_EVEN | MSF_UART_DATA_BITS_9, 0);
	//Driver_UART0.Control(MSF_UART_PARITY_ODD, 0);
	str = str_9600;

	while (1)
	{
		/* send with current baud */
		for (i = 0; i< 5; i++)
		{
			Driver_UART0.Send(str, strlen(str));
			msf_delay_ms(1000);

			/* Check for data received from the PC */
			if ( Driver_UART0.DataAvailable() )
			{
				Driver_UART0.Receive(buff, 1);		// drop the byte which was available

				/* Change baudrate? */
				Driver_UART0.Send("Change baud? (15=115200;96=9600;0=no\n\r",  38);
				Driver_UART0.Receive(buff, 2);
				buff[2] = 0;
				Driver_UART0.Send("Entered: ",  9);
				Driver_UART0.Send(buff,  2);

				if ( buff[0] == '1' )
				{
					str = str_115200;
					baud = BD115200;
				}
				else if ( buff[0] == '9')
				{
					str = str_9600;
					baud = BD9600;
				}
				Driver_UART0.Send("\n\rBaud will be:",  15);
				Driver_UART0.Send(str, strlen(str));
				/* Now really change the baud */
				msf_delay_ms(8000);
				Driver_UART0.Control(MSF_UART_BAUD_SET, baud);
			}
		}	// for
	}	// while(1)

}


/*------------------ Code for interrupt mode --------------------*/
char g_buffer[32] = "String to send";

/* Test the UART driver in interrupt mode. */
/* Simple test:
 * Test sending in background.
	Place breakpoint on the if(Driver_UART0.GetTxCount() >= n) line below.
	When it is hit, check the value of g_cnt.
	It should count to, for example, 6000 during sending a string
	with baudrate 9600. If you use baudrate 115200, it should count to lower number,
	for example, 500.
	In any case it counts, which means the program is not blocked in Driver_UART0.Send()
*/
void interrupt_uart_simple_test(void)
{
	uint32_t n, sending, cnt;

	/* Initialize the UART driver.
	 * Note that we do not need to provide callback if we do not care
	 * about the UART events */
	Driver_UART0.Initialize(BD9600, null);
	/* go to interrupt driven mode */
	Driver_UART0.Control(MSF_UART_INT_MODE, 0);

	sending = 0;
	cnt = 0;
	while(1)
	{
		if ( !sending )
		{
			sending = 1;
			n = strlen(g_buffer);
			Driver_UART0.Send(g_buffer, n);
		}
		cnt++;
		// check how many bytes were already sent...
		if ( Driver_UART0.GetTxCount() >= n)
		{
			// NOTE: we cannot use the MSF convenience functions in interrupt mode
			// so we do not print here...
			//msf_print("\n");
			sending = 0;
			cnt = 0;
			msf_delay_ms(2000);
		}
	}
}

/* -----------------Code for the complex interrupt mode test -------------------- */

/* The states of the program for full interrupt mode test*/
typedef enum _IntModeStates {
	Idle,
	Overflow,
	TxComplete,
	Receiving,
	Sending,
	Sent,
	Report,
} IntModeStates;

volatile IntModeStates g_state = Idle;

/* UART_SignalEvent
 * This function will be called by the UART driver in interrupt mode to report events,
 * such as sending completed, etc.
 * NOTE: this is called from ISR, so
 * - do not spend much time here!
 * - beware of creating a loop, for example, by sending text to UART in response
 *   to send complete event!
 *
 * */
void UART_SignalEvent(uint32_t event, uint32_t arg)
{
	switch( event) {
	case MSF_UART_EVENT_SEND_COMPLETE:
		// Note that if we send something in response to this event, we get into a loop
		// - sending results in send-complete event, which sends message again...
		g_state = Sent;
		msf_pin_write(BLUE_LED, true);	// turn LED off
		// NOTE that the MSF_UART_EVENT_TRANSFER_COMPLETE will come before the main() can see
		// the state == Sent, so we need to turn off the LED here.
		break;

	case MSF_UART_EVENT_RECEIVE_COMPLETE:
		g_state = Report;
		msf_pin_write(GREEN_LED, true);	// turn LED off
		break;

	case MSF_UART_EVENT_TRANSFER_COMPLETE:
		g_state = TxComplete;
		// Note that if we sent something here, we get into a loop - sending results in
		// transfer complete which sends message again...
		break;

	case MSF_UART_EVENT_RX_OVERFLOW:
		g_state = Overflow;	// main() will do the work
		break;
	}

}

/* Full test of interrupt-driven mode of the UART driver.
 * Sends back texts received through serial line. Receives 5 chars in a row.
 * As it is hard to synchronize the sends and receives to provide command line interface,
 * LEDs are used to show some states.
 *
 * GREEN - now receiving text
 * RED - overflow (on for 2 sec); short on-off means Tx complete
 * BLUE - on while sending
 *
 * */
void interrupt_uart_full_test(void)
{
	// Init the LEDs
	msf_pin_direction(RED_LED, output);
	msf_pin_direction(GREEN_LED, output);
	msf_pin_direction(BLUE_LED, output);
	// When the pin is 0, the LED is on; so turn it off
	msf_pin_write(RED_LED, true);
	msf_pin_write(GREEN_LED, true);
	msf_pin_write(BLUE_LED, true);

	/* Initialise UART driver; provide callback function to receive events */
	Driver_UART0.Initialize(BD9600, UART_SignalEvent);
	/* go to interrupt driven mode */
	Driver_UART0.Control(MSF_UART_INT_MODE, 0);

	while(1)
	{
		switch ( g_state )
		{
		case Idle:
			/* receive 5 bytes in the background */
			/* Do not call Receive again until complete event occurs!*/
			Driver_UART0.Receive(g_buffer, 5);
			g_state = Receiving;
			msf_pin_write(GREEN_LED, false);	// turn LED on
			break;

		case Report:
			/* Send what was received */
			g_buffer[5] = 0;	//
			Driver_UART0.Send(g_buffer, strlen(g_buffer));
			g_state = Sending;
			msf_pin_write(BLUE_LED, false);	// turn LED on
			break;

		case Sent:
			// This would work only if we do not use the MSF_UART_EVENT_TRANSFER_COMPLETE
			// try to comment out the g_state = TxComplete; in the UART_SignalEvent
			msf_delay_ms(100);	// little delay so user can see the LED was on
			msf_pin_write(BLUE_LED, true);	// turn LED off
			g_state = Idle;
			break;


		case Overflow:
			msf_pin_write(RED_LED, false);	// turn LED on
			msf_delay_ms(2000);
			msf_pin_write(RED_LED, true);	// turn LED off
			msf_delay_ms(2000);
			//g_state = Idle;
			g_state = Report;	// display the data received before overflow
			break;

		case TxComplete:
			msf_pin_write(RED_LED, false);	// turn LED on
			msf_delay_ms(200);
			msf_pin_write(RED_LED, true);	// turn LED off
			msf_delay_ms(2000);
			g_state = Idle;
			break;

		case Receiving:
			break;
		case Sending:
			break;
		}	// end switch

		msf_delay_ms(100);
	}

}

/* --------------------------- Test buffered console I/O   ----------------------*/

/* coniob_test
 * Waits for some commands with at least 3 chars. When received, prints which command
 * it received. */
void coniob_test(void)
{
	uint32_t cnt, menu = 1;
	char buff[8];


	while( 1 )
	{
		if ( menu )
		{
			coniob_puts("Enter command:\n");
			menu = 0;
		}
		/* we expect always 3 chars as a command */
		cnt = coniob_kbhit();
		if (  cnt >= 3  )
		{
			// do not read more than our buffer can handle!
			coniob_gets(buff, 3, 0);
			if (strcmp(buff, "dir") == 0 )
				coniob_puts("dir command received\n");
			else if (strcmp(buff, "end") == 0 )
			{
				coniob_puts("end command received\n");
			}
			else
			{
				coniob_puts("unknown command received: ");
				coniob_puts(buff);
				coniob_putch('\n');
			}
			menu = 1;

		}

		msf_delay_ms(500);

	}
}

/* ---------------------- Test UART1 and UART2 ----------------------*/

/* uart12_test
 * UART1 is available on pins Arduino #14 (PTE0) Tx; #15 (PTE1) Rx
 * UART2 is available on pins Arduino #12 (PTD3) Tx; #11 Rx (PTD2)
 * Tested by connecting with another board (Arduino) which sends some
 * chars to UART and also reads the reply.
 * Another test was using the coniob_test() with the coniob using UART2
 * driver instead of UART0.
 */
void uart12_test(void)
{
	char buff[8];
	char ack[4] = "ACK";

	coniob_puts("starting receive...\n");
	msf_delay_ms(2000);
	//Driver_UART1.Initialize(BD9600, null);
	Driver_UART2.Initialize(BD9600, null);
	while(1)
	{
		// wait for 3 bytes; blocking
		//Driver_UART1.Receive(buff, 3);
		Driver_UART2.Receive(buff, 3);
		buff[3] = 0;

		// print received string to UART0
		coniob_puts(buff);

		// also send something to the sender
		Driver_UART2.Send(ack, 3);

		msf_delay_ms(500);
	}

}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
