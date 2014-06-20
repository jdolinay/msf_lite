/*
 * Example for MSF Lite UART driver 
 * 6/2014 Jan Dolinay
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

#include <string.h>	// for strlen()

#include "msf.h"
#include "drv_uart.h"	/* UART driver */
#include "coniob.h"		/* Buffered console I/O */

#include "drv_tpm.h"	/* TPM driver */

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

// event handler for timer TPM0
void TPM0_SignalEvent(uint32_t event, uint32_t arg);
uint32_t g_duty;

int main(void)
{
	int counter = 0;
	
	/* Always initialise the MSF library*/
	msf_init(0);
	
	msf_pin_direction(RED_LED, output);
	msf_pin_write(RED_LED, true);
	msf_pin_direction(GREEN_LED, output);
	msf_pin_write(GREEN_LED, true);
		
	
	// ======= timer test
	// TODO: move to new project
	Driver_TPM0.Initialize(TPM0_SignalEvent);	// init timer, internal clock
	
	// Enable TOF signal and set prescaler
	// With clock option 1 (48 MHz) the timer clock is 8 MHz.
	// with prescaler = 128 we get counter freq = 62500 Hz and about 1 overflow in 1 second.
//	Driver_TPM0.Control(MSF_TPM_TOF_SIGNAL | MSF_TPM_PRESCALER_SET, MSF_TPM_PRESCALER_128);
	// wait
//	msf_delay_ms(10000);
	// change modulo = change freq of blinking.
	// F_of_interrupt = F_of_counter / (MOD+1)
	// F = 62500 / (12500) = 5  (5times toggle per second)
	//Driver_TPM0.Control(MSF_TPM_TOP_VALUE, 12499);
	
	// -------- PWM test
	// blue LED is TPM0 channel 1
	// IMPORTANT: the LEDS are connected so that they are ON when the pin is LOW, keep it in mind when testing PWM.
	// For example with hightrue pulses and duty 10%, the log.1 pulse is 10% of the period long but the LED will be
	// 10% of the period off and 90% on!
	
	g_duty = 15000;		// set in event handler
	// with 8 MHz src for timer, we get lowest counter freq. of Ft = 62500 Hz
	// IF we want 1 Hz PWM:
	// MOD = Ft/Fo - 1 = 62500/1 - 1 = 62499
	// 100% duty is MOD+1 = 62500
	Driver_TPM0.Control(MSF_TPM_PRESCALER_SET, MSF_TPM_PRESCALER_128);

	// center aligned PWM, max value of MOD is about 32000!
	// if we want 1 Hz PWM:
	// MOD = Ft/(2.Fo) = 62500/(2.1)= 31250
	// 100% duty is 2xMOD = 62500; we set CnV between 0 and 31250
	// For 10% duty set channel to: 10% od MOD = 3125   
	Driver_TPM0.Control(MSF_TPM_MOD_VALUE, 31250);	
	Driver_TPM0.SetChannelMode(1, PWM_center_hightrue, MSF_TPM_PARAM_CHANNEL_EVENT); 	// with event
	while(1)
	{
		// assuming the write to uint32_t is atomic
		g_duty = 3125;		// 10%
		msf_delay_ms(5000);
		g_duty = 28125;		// 90%
		msf_delay_ms(5000);
		g_duty = 15625;	// 50%
		msf_delay_ms(5000);		
	}
	
	
#if 0	/* edge aligned PWM */
	Driver_TPM0.Control(MSF_TPM_MOD_VALUE, 62499);	
	//Driver_TPM0.SetChannelMode(1, PWM_edge_lowtrue, 0);  // without event	
	//Driver_TPM0.SetChannelMode(1, PWM_edge_lowtrue, MSF_TPM_PARAM_CHANNEL_EVENT); 	// with event
	Driver_TPM0.SetChannelMode(1, PWM_edge_hightrue, MSF_TPM_PARAM_CHANNEL_EVENT); 	// with event
	while(1)
	{
		// Note: better is to write new duty in the event of the channel (at the end of period)
		//Driver_TPM0.WriteChannel(1, 6249 );	// 10%
		g_duty = 6249;	// assuming th ewrite is atomic
		msf_delay_ms(5000);
		//Driver_TPM0.WriteChannel(1, 31250 );	// 50%
		g_duty = 31250;
		msf_delay_ms(5000);
		//Driver_TPM0.WriteChannel(1, 56250 );	// 90%
		g_duty = 56250;
		msf_delay_ms(5000);		
	}
#endif	
	
	while(1) 
	{
		//msf_pin_toggle(GREEN_LED);
		//msf_delay_ms(500);
	}
	// =========
	
	/* Now test the uart driver 
	 * Please un-comment one of the functions.
	 * */
	polled_uart_test();
	//interrupt_uart_simple_test();
	//interrupt_uart_full_test();
	//coniob_test();
		
	for(;;) {	   
	   	counter++;
	}
	
	return 0;
}

/* --------------------- timer code ---------------- */
void TPM0_SignalEvent(uint32_t event, uint32_t arg)
{
	//static uint32_t cnt = 0;
	switch ( event )
	{
	case MSF_TPM_EVENT_TOF:
		//if ( cnt++ > 100 )
		//{
		//	cnt = 0;
		//	msf_pin_toggle(RED_LED);
		//}
		break;
	
	case MSF_TPM_EVENT_CH0:
		break;
	
	case MSF_TPM_EVENT_CH1:
		Driver_TPM0.WriteChannel(1, g_duty );	// 90%
		break;
	}
	
}
/* --------------------- end timer code ---------------- */

/* Test the UART driver in simple polled mode.
 * Prints text to serial line. Send any char from terminal to display prompt for
 * changing baud rate. */
void polled_uart_test(void)	
{
	int i;
	uint32_t baud = BD9600;
	char buff[4];
	const char* str;
	Driver_UART0.Initialize(baud, null);
	/* Test mode with parity and 2 stop bits 
	 * IMPORTANT: the terminal must be set for 7 data bits if the UART driver is 
	 * set to 8-bit mode and parity as the 8th bit will be parity. 
	 * Or set the driver to 9-bit mode and use 8-data bits in terminal. */
	//Driver_UART0.Control(MSF_UART_PARITY_NONE | MSF_UART_STOP_BITS_2, 0);
	Driver_UART0.Control(MSF_UART_PARITY_EVEN | MSF_UART_DATA_BITS_9, 0);
	//Driver_UART0.Control(MSF_UART_PARITY_ODD, 0);
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
	  When it is hit, check the value of g_cnt. 
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
 * - beware of loops, e.g. by sending text to UART in response to send complete event!
 * - you can call UART.Send() from here also.
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
 * Sends back texts received through serial line. Receives 5 chars in a row.
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

/* Test buffered console I/O 
 * Waits for some commands with at least 3 chars. When received, prints which command
 * it received. */
void coniob_test(void)
{
	uint32_t cnt, menu = 1;
	char buff[8];
	
	//coniob_init(BD19200);
	
	while( 1 ) 
	{
		if ( menu )
		{
			coniob_puts("Enter commnad:\n");
			menu = 0;
		}
		/* we expect always 3 chars as a command */
		cnt = coniob_kbhit();
		if (  cnt >= 3 )
		{
			coniob_gets(buff, cnt, 0);
			if (strcmp(buff, "dir") == 0 )
				coniob_puts("dir command received\n");
			else if (strcmp(buff, "end") == 0 )
			{
				coniob_puts("end command received\n");
				/// change baudrate 
				//coniob_init(BD115200);
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
		
		/*
		coniob_puts("ahoj jak ");
		//msf_delay_ms(500);
		coniob_puts("se mas. ");
		//msf_delay_ms(500);
		coniob_puts("Dlouhy string string 123456789\n");
		msf_delay_ms(2000);
		*/
	}
}

