/*
 * Example for MSF Lite UART driver 
 * 6/2014 Jan Dolinay
 *
 */

/* MSF configuration for this project */
#include "msf_config.h"

/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"

#include "msf.h"
#include "drv_uart.h"	/* UART driver */

void polled_uart_test(void);	
void interrupt_uart_test(void);

const char* str_115200 = "115200\n\r";
const char* str_9600 = "9600\n\r";



int main(void)
{
	int counter = 0;
	
	/* Always initialise the MSF library*/
	msf_init(0);
	
	/* Now test the uart driver */
	//polled_uart_test();
	interrupt_uart_test();
	
		
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
char g_buffer[32];
uint32_t g_cnt = 0;
const char* g_msg = null;

/* The callback for interrupt mode 
 * NOTE: this is called from ISR. SHould not use the driver to print message!
 * */
void UART_SignalEvent(uint32_t event, uint32_t arg)
{
	switch( event) {
	case MSF_UART_EVENT_SEND_COMPLETE:
		//Driver_UART0.Send("\n\rSend complete",  15);
		//g_msg = "\n\rSend complete";
		break;
		
	case MSF_UART_EVENT_RECEIVE_COMPLETE:
		//Driver_UART0.Send("\n\rReceive complete",  18);
		g_msg = "\n\rReceive complete";
		break;
		
	case MSF_UART_EVENT_TRANSFER_COMPLETE:
		//Driver_UART0.Send("\n\rTransfer complete",  19);
		//g_msg = "\n\rTransfer complete";
		break;
		
	case MSF_UART_EVENT_RX_OVERFLOW:
		//Driver_UART0.Send("\n\rOverflow",  19);
		g_msg = "\n\rOverflow";
		break;
	}

}

/* Test the UART driver in interrupt mode */
void interrupt_uart_test(void)
{
	g_cnt = 0;
	
	
	Driver_UART0.Initialize(BD9600, UART_SignalEvent);
	/* go to interrupt driven mode */
	Driver_UART0.Control(MSF_UART_INT_MODE, 0);
	
	while(1)
	{
		/* receive 5 bytes in the background */
		/* Do not call Receive again until complete event occurs!*/
		if ( g_cnt == 0 )
			Driver_UART0.Receive(g_buffer, 5);	
		
		g_cnt++;
		msf_delay_ms(200);
		/* check if there is message from callback */
		if ( g_msg )
		{
			g_buffer[5] = 0;
			Driver_UART0.Send(g_msg, strlen(g_msg));
			Driver_UART0.Send(g_buffer, strlen(g_msg));
			g_msg = null;
			g_cnt = 0;
		}
	}
	
	
	
}

