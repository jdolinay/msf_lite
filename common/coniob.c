/****************************************************************************
 * @file     coniob.c
 * @brief    Console Input/output functions; with buffer
 * @version  1
 * @date     10 June 2014
 *
 * @note	After initialisation, the coniob driver receives characters from
 * 			serial line into internal buffer. The user can read them using coniob_getch or gets.
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


/* Platform-specific UART driver */
#include "drv_uart.h"	

/* Define which driver (instance of the UART) is used by us */
#define	 CONIOB_UART_DRIVER	Driver_UART0

/* Define the size of the buffer in bytes.
 * NOTE:  The size must be a power of two
*   and it needs to fit in the get/put indicies. i.e. if you use an
*   8 bit index, then the maximum supported size would be 128. (see cbuf.h) 
 * TODO: move to header */
#define	 CONIOB_TXBUFFER_SIZE		(64)
#define	 CONIOB_RXBUFFER_SIZE		(32)

#define LF 0x0A						// Line Feed ASCII code
#define CR 0x0D						// Carriage Return ASCII code

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
volatile uint32_t coniob_lastRxCount;
volatile uint32_t coniob_lastTxCount;
volatile uint32_t coniob_nowSending;


/* -------- Prototypes of internal functions   -------- */
void coniob_UART_SignalEvent(uint32_t event, uint32_t arg);
void wconiob_update_rxfifo(void);
void wconiob_update_txfifo(void);

/* -------- Implementation of public functions   -------- */

/** Initialize the console I/O driver
 **/
void coniob_init(void)               
{
    // TODO: howto specify speed?
    // probably in msf_config user can select or we provide default in msf_defcfg.h
    //uart_init(BD4800);
    //uart_init(BD9600);
	CONIOB_UART_DRIVER.Initialize(BD9600, coniob_UART_SignalEvent);
	/* go to interrupt driven mode */
	CONIOB_UART_DRIVER.Control(MSF_UART_INT_MODE, 0);
    /* Note: if you get compiler error that the speed constant is not defined, check if
    for given F_CPU this speed is available; in msf_<device>.h file included into <platform>.h,
    e.g. s08.h */ 
		
	/* Init the FIFOs */
	CBUF_Init(coniob_txQ);
	CBUF_Init(coniob_rxQ);
	coniob_lastRxCount = 0;
	coniob_lastTxCount = 0;
	coniob_nowSending = 0;
	
	/* We automatically start to receive data from serial line */
	CONIOB_UART_DRIVER.Receive(coniob_rxQ.m_entry, coniob_rxQ_SIZE);
}

/** Read one character from SCI.
 * @return the character read or 0 if there is no character available.
 * @note  
 **/
char coniob_getch(void)               
{
	wconiob_update_rxfifo();		
	
	if ( CBUF_IsEmpty( coniob_rxQ ) )
		return 0;
	
	return CBUF_Pop( coniob_rxQ );
	
}

/** Check if data are available for reading
 * @return 0 if there are no data (call to getch would block the caller;
 *  or the number of characters available in input buffer. 
 **/
uint8_t coniob_kbhit(void)
{
	return CBUF_Len(coniob_rxQ);
}

/** Send one character to SCI. If the char is '\n', send CR + LF
 * @param char to send
 **/
void coniob_putch(char c)        
{
	int cnt;
	/* push to FIFO; if full, overwrites the oldest char 
	 * TODO: would it be better to block the caller and wait for ISR to transmit some bytes? */
    if(c == '\n')
    {	 	
    	CBUF_Push( coniob_txQ, CR );
    	CBUF_Push( coniob_txQ, LF ); 
    	cnt = 2;
	} 
    else 
    {
    	CBUF_Push( coniob_txQ, c );  
    	cnt = 1;
	}
    
    /* TODO: send to UART! */
    /*problem: pokud uz posilame, nesmim volat Send ale jen "queue" pozadavek na poslani.... */
    wconiob_update_txfifo();
    if ( !coniob_nowSending )
    {
    	/* jen pokud nevysilame, musime vysilani spustit, dal se o to stara callback */
    	CONIOB_UART_DRIVER.Send(// todo odkud? CBUF_GetLastEntryPtr(coniob_txQ), cnt);
    }
}

/** Send null-terminated string to SCI. 
 * @param pointer to string to send
 * @note If the string contains '\n', CR + LF are sent. 
 **/
void coniob_puts(const char* str)     
{	
    while(*str) 
    {	    	
    	if( *str == '\n')
    	{	 	
    	 	CBUF_Push( coniob_txQ, CR );
    	  	CBUF_Push( coniob_txQ, LF );    	
    	} 
    	else 
    	{
    	  	CBUF_Push( coniob_txQ, c );    					
    	}	  
    	
    	str++;
    }
    
    /* TODO: send to UART! */
}


/** Read string from console (from buffer). Does not block the caller. 
 * @note The possible results are:
 * 1) string with max_chars valid characters and 0-terminated.
 * 2) string up to terminator character
 * 3) string with all the chars from buffer  
 * The resulting string is null-terminated (valid C-language string) in all cases.   
 * The terminator character in not included in the resulting string.
 *
 * @param str [out] buffer provided by the user to receive the string
 * @param max_chars [in] maximum character (not including terminating 0) to 
 *  receive.
 * @param terminator [in] character which means the end of the string. 
 * @return number of characters actually written to the string str; not including 
 * the ending 0.      
 **/
uint8_t coniob_gets(char* str, uint8_t max_chars, char terminator)
{
    char c;
    uint8_t i;
    
    wconiob_update_rxfifo();	
    
    for ( i = 0; i < max_chars && !CBUF_IsEmpty(coniob_rxQ); i++ )
    { 
    	c = CBUF_Pop( coniob_rxQ );
    	if ( c != terminator )
            str[i] = c;        
    }
    str[i] = 0; 
    return i;
}		 

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
	switch( event) {
	case MSF_UART_EVENT_SEND_COMPLETE:
		/* sending just completed; if there is something more to send, start sending again... */
		wconiob_update_txfifo();
		
		if ( !CBUF_IsEmpty(coniob_txQ))
		{
			//CONIOB_UART_DRIVER.Send(coniob_rxQ.m_entry, coniob_rxQ_SIZE);
		}
		break;
		
	case MSF_UART_EVENT_RECEIVE_COMPLETE:
		/* we are all the time receiving; when receive completes, we start over again */
		wconiob_update_rxfifo();	/* first update the fifo */
		CBUF_ResetPushIdx(coniob_rxQ);	/* pushing will start from the beginning of fifo again */
		coniob_lastRxCount = 0;	/* now we start again from the beginning*/
		CONIOB_UART_DRIVER.Receive(coniob_rxQ.m_entry, coniob_rxQ_SIZE);
		break;
		
	/*case MSF_UART_EVENT_TRANSFER_COMPLETE:
		break;
		
	case MSF_UART_EVENT_RX_OVERFLOW:
		break;*/
	}

}




/* The UART driver stores received data to the FIFO but without the FIFO
 * knowing about it. 
 * This function will "push" the data which are already in the FIFO but the 
 * FIFO does not know about them yet.
 * Then we can pop the data using the FIFO functions. 
*/
void wconiob_update_rxfifo(void)
{
	uint32_t cnt;
	/* Check how many chars are received */
	/* The UART driver stores received data to the FIFO but without the FIFO
	 * knowing about it. Now we "push" the data which are already there and then
	 * pop some out for the caller using the FIFO functions. 
	*/
	cnt = CONIOB_UART_DRIVER.GetRxCount();	
	if (cnt > 0 && cnt > coniob_lastRxCount )
	{		
		/* push the number of bytes received since we looked last time */
		CBUF_AdvancePushIdxN(coniob_rxQ, (cnt - coniob_lastRxCount) );
		coniob_lastRxCount = cnt;		
	}
	
}	

/* todo: */
/* The UART driver sends data from the FIFO without the FIFO knowing about it.
 * This function will "pop" the data which are already sent but the FIFO does not
 * know about it yet. */
void wconiob_update_txfifo(void)
{
	uint32_t cnt;
	cnt = CONIOB_UART_DRIVER.GetTxCount();	
	if (cnt > coniob_lastTxCount )
	{
		CBUF_AdvancePopIdxN(coniob_txQ, (cnt - coniob_lastTxCount) );
		coniob_lastTxCount = cnt;
	}
}
