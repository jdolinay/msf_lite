/****************************************************************************
 * @file     coniob.c
 * @brief    Console Input/output functions; with buffer
 * @version  1
 * @date     10 June 2014
 *
 * @note	After initialisation, the coniob driver receives characters from
 * 			serial line into internal buffer. The user can read them using coniob_getch or gets.
 * 			This is very simple version which handles each received/sent char in UART driver event
 * 			(not efficient :( ), but sending more data at once proved to be very complicated.
 * 			For F_CPU = 48 MHz:
 * 			9600 BD:  1 byte is sent in about 5000 CPU clock cycles (instructions)
 * 			115200 BD: 1 byte is sent in about 420 CPU clock cycles   
 * 			Even with high overhead of this version it will save lot of CPU time which would
 * 			be vasted in waiting for Tx/Rx complete in polled mode. 
 * 			
 * These functions rely on UART driver.
 * 
 *      
 *
 ******************************************************************************/

/** @defgroup group_conio Console I/O driver 
 * @details MSF console I/O generic driver
 * @{
 */

/* Include user configuration */
#include "msf_config.h"

/* Include hardware definitions */
#include "coredef.h"
       
#include <stdio.h>   /* for sprintf */
#include <string.h>	/* for strlen */

#include "cbuf.h"	/* circular buffer */

           
#include "msf.h"

#include "coniob.h"


/*---------- Internal variables ---------------- */
/* The name of the size for the buffer must match this patters */
#define coniob_txQ_SIZE    CONIOB_TXBUFFER_SIZE
#define coniob_rxQ_SIZE    CONIOB_RXBUFFER_SIZE
   
volatile struct
{
	uint8_t     m_getIdx;
    uint8_t     m_putIdx;
    uint8_t     m_entry[ coniob_txQ_SIZE ];
} coniob_txQ;

volatile struct
{
	uint8_t     m_getIdx;
    uint8_t     m_putIdx;
    uint8_t     m_entry[ coniob_rxQ_SIZE ];
} coniob_rxQ;

/* the number of read bytes last time we updated the Rx FIFO */
volatile uint32_t coniob_nowSending;


/* -------- Prototypes of internal functions   -------- */
void coniob_UART_SignalEvent(uint32_t event, uint32_t arg);
void wconiob_update_rxfifo(void);
void wconiob_update_txfifo(void);

/* -------- Implementation of public functions   -------- */

/** Initialize the console I/O driver
 **/
void coniob_init(UART_speed_t baudrate)               
{
    CONIOB_UART_DRIVER.Initialize(baudrate, coniob_UART_SignalEvent);
	/* go to interrupt driven mode */
	CONIOB_UART_DRIVER.Control(MSF_UART_INT_MODE, 0);
    /* Note: if you get compiler error that the speed constant is not defined, check if
    for given F_CPU this speed is available; in msf_<device>.h file included into <platform>.h,
    e.g. s08.h */ 
		
	/* Init the FIFOs */
	CBUF_Init(coniob_txQ);
	CBUF_Init(coniob_rxQ);	
	coniob_nowSending = 0;
	
	/* We automatically start to receive data from serial line */
	CONIOB_UART_DRIVER.Receive(CBUF_GetPushEntryPtr(coniob_rxQ), 1);
}

/** Read one character from SCI.
 * @return the character read or 0 if there is no character available.
 * @note  
 **/
char coniob_getch(void)               
{	
	if ( CBUF_IsEmpty( coniob_rxQ ) )
		return 0;
	
	return CBUF_Pop( coniob_rxQ );
	
}

/** Check if data are available for reading
 * @return 0 if there are no data (call to getch would block the caller;
 *  or the number of characters available in input buffer. 
 **/
uint32_t coniob_kbhit(void)
{
	return CBUF_Len(coniob_rxQ);
}

/** Send one character to SCI. If the char is '\n', send CR + LF
 * @param char to send
 **/
void coniob_putch(char c)        
{	
	
	uint8_t* pStart = CBUF_GetLastEntryPtr(coniob_txQ);
	
	/* push to FIFO; if full, overwrites the oldest char 
	 * TODO: would it be better to block the caller and wait for ISR to transmit some bytes? */
    if(c == '\n')
    {	 	
    	CBUF_Push( coniob_txQ, CR );
    	CBUF_Push( coniob_txQ, LF ); 
	} 
    else 
    {
    	CBUF_Push( coniob_txQ, c ); 
	}
        
    if ( !coniob_nowSending )
    {    	
    	coniob_nowSending = 1;    	
    	/* Note: we always send only 1 char and leave it to the callback to send the rest of the FIFO;
    	 * otherwise we would have to handle the situation when the 1st char is at the end of the 
    	 * FIFO buffer and the next char is at the beginning, which the UART driver does not handle-
    	 * - it needs simple flat buffer.  */
    	CONIOB_UART_DRIVER.Send(pStart, 1);
    }
}

/** Send null-terminated string to SCI. 
 * @param pointer to string to send
 * @note If the string contains '\n', CR + LF are sent. 
 **/
void coniob_puts(const char* str)     
{		
	uint8_t* pStart = CBUF_GetLastEntryPtr(coniob_txQ);
    
	while(*str) 
    {	    	
    	if( *str == '\n')
    	{	 	
    	 	CBUF_Push( coniob_txQ, CR );
    	  	CBUF_Push( coniob_txQ, LF );    
    	} 
    	else 
    	{
    	  	CBUF_Push( coniob_txQ, *str );    	  	
    	}	  
    	
    	str++;
    }
        
    if ( !coniob_nowSending )
    {
      	/* Only if we are not sending, start sending */
    	coniob_nowSending = 1;
    	/* Note: we always send only 1 char and leave it to the callback to send the rest of the FIFO;
    	* otherwise we would have to handle the situation when the 1st char is at the end of the 
    	* FIFO buffer and the next char is at the beginning, which the UART driver does not handle-
    	* - it needs simple flat buffer.  */
    	CONIOB_UART_DRIVER.Send(pStart, 1);
    }
}


/** Read string from console (from buffer). Does not block the caller. 
 * @note The possible results are:
 * 1) string with max_chars valid characters .
 * 2) string up to terminator character
 * 3) string with all the chars from buffer (which may be empty string if buffer is empty) 
 *  
 * The resulting string is null-terminated (valid C-language string) in all cases.   
 * The terminator character in not included in the resulting string.
 *
 * @param str [out] buffer provided by the user to receive the string
 * @param max_chars [in] maximum character (not including terminating 0) to 
 *  receive.
 * @param terminator [in] character which means the end of the string. Can be 0 if not needed.
 * @return number of characters actually written to the string str; not including 
 * the ending 0.      
 **/
uint32_t coniob_gets(char* str, uint32_t max_chars, char terminator)
{
    char c;
    uint8_t i;
    
    for ( i = 0; i < max_chars && !CBUF_IsEmpty(coniob_rxQ); i++ )
    { 
    	c = CBUF_Pop( coniob_rxQ );
    	if ( c != terminator )
            str[i] = c;        
    }
    str[i] = 0; 
    return i;
}

char coniob_peek(void)
{
	if ( CBUF_IsEmpty(coniob_rxQ) )
		return 0;
	return CBUF_Get(coniob_rxQ, 0);
}

#if 0	/* not working properly */
/** Empty the receive buffer.
 *  Note that if data keep on coming from the serial line, some data may appear in the
 *  buffer even before this function returns. 
 *  It is useful in processing commands in 
 *  command-line-interface if we start processing e.g. when 3 chars are received, but 
 *  the user might have typed some extra chars in the time before we process the 
 *  command and wait for another command - so there would some chars left in the
 *  Rx buffer.   
 * */
void coniob_flush(void)
{
	CBUF_Init(coniob_rxQ);	
}
#endif

/** @}*/

/* ---------------------- Internal functions ------------------------------------------- */
/* This function will be called by the UART driver in interrupt mode to report events,
 * such as sending completed, etc.
 * NOTE: this is called from ISR, so 
 * - do not spend much time here! 
 * - preferably do not send to UART from here!
 * - beware of loops, e.g. by sending text to UART in response to send complete event!
 * */
void coniob_UART_SignalEvent(uint32_t event, uint32_t arg)
{
	volatile uint8_t* pData;
	
	switch( event) 
	{
	case MSF_UART_EVENT_SEND_COMPLETE:
		
		/* sending just completed; if there is something more to send, start sending again... */		
		if ( !CBUF_IsEmpty(coniob_txQ))
		{
			coniob_nowSending = 1;
			pData = &CBUF_Pop(coniob_txQ);
			CONIOB_UART_DRIVER.Send( pData, 1);					
		}
		else
			coniob_nowSending = 0;
		break;
		
	case MSF_UART_EVENT_RECEIVE_COMPLETE:
		/* just put the new char to FIFO and start new receive */
		CBUF_AdvancePushIdx(coniob_rxQ);	/* the new char is now available in FIFO (UART driver 
			copied it there already, we just now update the FIFO index) */
		/* Get next char */
		CONIOB_UART_DRIVER.Receive(CBUF_GetPushEntryPtr(coniob_rxQ), 1);
		break;
		
		
		
	/*case MSF_UART_EVENT_TRANSFER_COMPLETE:
		break;
		
	case MSF_UART_EVENT_RX_OVERFLOW:
		break;*/
	}

}

