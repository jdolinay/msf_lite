/*
 * Example for MSF Lite UART driver 
 * 6/2014 Jan Dolinay
 *
 */

/* MSF configuration for this project */
#include "msf_config.h"

/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"

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

const char* str_115200 = "115200\n\r";
const char* str_9600 = "9600\n\r";



int main(void)
{
	int counter = 0;
	
	/* Always initialise the MSF library*/
	msf_init(0);
	
	/* Now test the uart driver 
	 * Please un-comment one of the functions.
	 * */
	//polled_uart_test();
	//interrupt_uart_simple_test();
	//interrupt_uart_full_test();
	coniob_test();
		
	for(;;) {	   
	   	counter++;
	}
	
	return 0;
}

/* Test the UART driver in simple polled mode */
void polled_uart_test(void)	
{
	int i;
	uint32_t baud;
	char buff[4];
	const char* str;
	Driver_UART0.Initialize(BD9600, null);
	str = str_9600;
	
	while (1) 
	{
		/* send with current baud*/		
		for (i = 0; i< 5; i++)
		{
			Driver_UART0.Send(str, strlen(str));
			msf_delay_ms(1000);
		
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
				/* Now really change */
				msf_delay_ms(8000);
				Driver_UART0.Control(MSF_UART_BAUD_SET, baud);
			}
		}	// for
	}	// while(1)

}

/*------------------ Code for interrupt mode --------------------*/
char g_buffer[32] = "String to send";

/* Test the UART driver in interrupt mode. */
/* Simple test: Test sending in background.
	  Place breakpoint into the if(Driver_UART0.GetTxCount() >= n) below.  
	  When it is hi, check the value of g_cnt. 
	  It should count to e.g. 6000 during sending a string
	  with baudrate 9600. If you use baudrate 115200, it should count to e.g. 500.
	  In any case it counts, which means the program is not blocked in Driver_UART0.Send() 
*/
void interrupt_uart_simple_test(void)
{
	uint32_t n, sending, cnt;	
	
	/* initialise the UART driver.
	 * Note that we do not need to provide callback if we do not care about the UART events */
	Driver_UART0.Initialize(BD9600, null);
	/* go to interrupt driven mode */
	Driver_UART0.Control(MSF_UART_INT_MODE, 0);
	
	sending = 0;
	cnt = 0;
	while(1)
	{
		if ( !sending )
		{
			n = strlen(g_buffer);
			Driver_UART0.Send(g_buffer, n);
			sending = 1;
		}
		cnt++;
		// check how many bytes were already sent...
		if ( Driver_UART0.GetTxCount() >= n)
		{
			// NOTE: we cannot use the MSF convenience functions yet in interrupt mode
			// so we do not print here...
			//msf_print("\n");
			//msf_delay_ms(500);	// since in int mode we have to wait until sent!
			//msf_printnum(g_cnt);
			sending = 0;
			cnt = 0;
			msf_delay_ms(2000);
		}
	}
}

/* ------------------------------------- */



//uint32_t g_cnt = 0;
//const char* g_msg = null;
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

/* This function will be called by the UART driver in interrupt mode to report events,
 * such as sending completed, etc.
 * NOTE: this is called from ISR, so 
 * - do not spend much time here! 
 * - preferably do not send to UART from here!
 * - beware of loops, e.g. by sending text to UART in response to send complete event!
 * */
void UART_SignalEvent(uint32_t event, uint32_t arg)
{
	switch( event) {
	case MSF_UART_EVENT_SEND_COMPLETE:
		// Note that if we send something in response to this event, we get into a loop - sending results in
		// send-complete event, which sends message again...		
		g_state = Sent;
		msf_pin_write(BLUE_LED, true);	// turn LED off
		// NOTE that the MSF_UART_EVENT_TRANSFER_COMPLETE will come before the main() can see the state == Sent
		// so we need to turn off the LED here.
		break;
		
	case MSF_UART_EVENT_RECEIVE_COMPLETE:		
		g_state = Report;
		msf_pin_write(GREEN_LED, true);	// turn LED off
		break;
		
	case MSF_UART_EVENT_TRANSFER_COMPLETE:
		g_state = TxComplete;
		// Note that if we send something, we get into a loop - sending results in
		// transfer complete which sends message again...
		break;
		
	case MSF_UART_EVENT_RX_OVERFLOW:
		g_state = Overflow;	// main() will do the work		
		break;
	}

}

/* Full test of interrupt-driven mode of the UART driver.
 * As it is hard to synchronise the sends and receives to provide command line interface, 
 * LEDs are used to show some states. 
 * 
 * GREEN - now receiving text 
 * RED - overflow (on for 2 sec); short on-off - Tx complete
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
			/* Echo what was received */
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

/* Test buffered console I/O */
void coniob_test(void)
{
	coniob_init();
	
	while( 1 ) 
	{
		coniob_puts("ahoj jak ");
		msf_delay_ms(500);
		coniob_puts("se mas. ");
		msf_delay_ms(500);
		coniob_puts("Dlouhy string string 123456789");
		msf_delay_ms(2000);
	}
}

