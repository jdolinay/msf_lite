/****************************************************************************
 * @file     uart_k25.h
 * @brief    CMSIS-like UART driver for kinetis
 * @version  1
 * @date     20. May 2014
 *
 * @note     This is simplified imitation od CMSIS driver which does not 
 *          exist for Kinetis(?). Should be used as a standard for MSF drivers.  
 *
 *	TODO: configuration for Tx Rx pins, now are hard-coded  
 *
 ******************************************************************************/
#ifndef MSF_UART_K25_H
#define MSF_UART_K25_H

/* Make sure the definitions of baudrate constants etc. are available */
#ifndef MSF_MSF_H
	#error Plese include msf.h before including this file. 
#endif

/* Common definitions for this platform and this driver class */
#include "drv_uart.h"


#ifdef __cplusplus
extern "C" {
#endif


/* TODO: define the I/O pins structure and runtime info structure if needed */
/* UART Run-time information*/
typedef struct _UART_INFO {
  MSF_UART_Event_t cb_event;          // Event Callback
  uint32_t  status;              // Status flags 
  void*		txbuff;				// buffer for current Tx operation
  void*		rxbuff;				// buffer for current Tx operation
  uint32_t  tx_cnt;				// number of bytes already transmitted
  uint32_t  rx_cnt;				// number of bytes already transmitted
  uint32_t  tx_total;			// total number of bytes to receive or transmit
  uint32_t  rx_total;			// total number of bytes to receive or transmit
} UART_INFO;



/* The data for one instance of the driver - the "resource" */
/* Note: for K25 the UART0 is different than UART1 and UART2 which have different 
overlay struct. There will always be only 1 instance of the UART0;
there could be 1 or 2 instances of UART driver for UART1 and UART2. */
typedef struct {
        UART0_Type  *reg;  // UART peripheral register interface, CMSIS
        // TODO: UART_IO        io; // UART I/O pins
        UART_INFO   *info;   // Run-Time information
} const UART_RESOURCES;



#ifdef __cplusplus
}
#endif
/* ----------- end of file -------------- */
#endif /* MSF_UART_K25_H */
