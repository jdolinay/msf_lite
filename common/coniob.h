/****************************************************************************
 * @file     coniob.h
 * @brief    Console I/O driver with internal BUFFER; generic definitions for all platforms 
 * @version  1
 * @date     10. June 2014
 *
 * @note        
 *
 ******************************************************************************/
#ifndef MSF_CONIOB_H
#define MSF_CONIOB_H

/* Platform-specific UART driver */
#include "drv_uart.h"	

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup group_coniob  
 * @{
*/
/* Define which driver (instance of the UART) is used by us */
#define	 CONIOB_UART_DRIVER	Driver_UART0

/** Define the size of the buffer in bytes.
 * NOTE:  The size must be a power of two
*   and it needs to fit in the get/put indicies. i.e. if you use an
*   8 bit index, then the maximum supported size would be 128. (see cbuf.h) 
*  NOTE: you cannot send longer string than CONIOB_TXBUFFER_SIZE in one call to coniob_puts! 
*/
#define	 CONIOB_TXBUFFER_SIZE		(64)
#define	 CONIOB_RXBUFFER_SIZE		(64)

#define LF 0x0A						// Line Feed ASCII code
#define CR 0x0D						// Carriage Return ASCII code

/* initialize console I/O */
void coniob_init(UART_speed_t baudrate);  
/* send one character to console */
void coniob_putch(char c);        
/* send null-terminated string */
void coniob_puts(const char* str);     
/* read one character. return 0 if no character is available */ 
char coniob_getch(void);               
/* read string from console. */
uint32_t coniob_gets(char* str, uint32_t max_chars, char terminator);		 
/* Return number of characters available in input buffer */ 
uint32_t coniob_kbhit(void);
/* get the char from input buffer without removing it from the buffer.
 * return 0 if no char is available */
char coniob_peek(void);
/* empty the receive buffer */
/*void coniob_flush(void);*/


/** @} */
#ifdef __cplusplus
}
#endif
/* ----------- end of file -------------- */
#endif /* MSF_CONIOB_H */
