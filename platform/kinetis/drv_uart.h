/****************************************************************************
 * @file     drv_uart.h
 * @brief    CMSIS-like UART driver for kinetis
 * @version  1
 * @date     20. May 2014
 *
 * @note     This is simplified imitation od CMSIS driver which does not 
 *          exist for Kinetis(?). Should be used as a standard for MSF drivers.  
 *          The driver does not implement any software buffer. It uses the Rx,Tx buffer
 *          of the MCU, if available. For KL25Z the buffer is just 1 B in size.  
 *
 ******************************************************************************/
#ifndef MSF_DRIVER_UART_H
#define MSF_DRIVER_UART_H

/* Make sure the definitions of baudrate constants etc. are available */
#ifndef MSF_MSF_H
	#error Plese include msf.h before including this file. 
#endif


#ifdef __cplusplus
extern "C" {
#endif

/** Version of this drivers API */
#define     MSF_UART_VERSION    (1)

/** Pointer to call back function for reporting events from the driver to
 * client application. Set in Initialize function */
typedef void (*MSF_UART_Event_t) (uint32_t event, uint32_t arg);   


/* The priority of the UART interrupt; lower number means higher priority.
 * For KL25Z valid value is 0 thru 3 */
#define		MSF_UART_INT_PRIORITY	(2)

/** Flags (operations and parameters) for the Control function */
/* To allow changing only desired values in call to Control(), most flags use
 * at least 2 bits (i.e. possible values 0,1,2 meaning do not set, set to 0, set to 1). 
 * Positions and meaning of the bit-fields:
 * 
 Bit(s)  Meaning
 0		Set baudrate; (0) = not set; (1) = set, arg = baudrate (one of the UART_speed_t values only!)	
 1:2		Set polled or interrupt mode; 0 = not set; 1 = polled; 2 = interrupt
  		Note: If you plan using interrupt mode the callback (MSF_UART_Event_t) function must be provided in call
  		to Initialize. This function will be called when character is received, etc.
*/

/* Defines for these positions*/
#define		MSF_UART_BAUD_Pos		(0)
#define		MSF_UART_BAUD_Mask		(0x01)
#define		MSF_UART_INTMODE_Pos	(1)
#define		MSF_UART_INTMODE_Mask	(0x06)

/* Definitions of the flags */
#define     MSF_UART_BAUD_SET      (1UL << MSF_UART_BAUD_Pos)  /**< set the baudrate */
#define     MSF_UART_POLLED_MODE   (1UL << MSF_UART_INTMODE_Pos)  /**< wait for each char to be sent/received in busy loop */
#define     MSF_UART_INT_MODE      (2UL << MSF_UART_INTMODE_Pos)  /**< use interrupts */


/** UART events (masks) 
 * The user defined function MSF_UART_Event_t will get the mask in event parameter */
#define		MSF_UART_EVENT_TX_EMPTY		(1UL<<0)	/**< Tx buffer is empty; arg not used; transmitter may still transmit data! */
#define		MSF_UART_EVENT_TX_COMPLETE	(1UL<<1)	/**< Tx transfer complete; line is idle, safe to turn off transmitter */
#define		MSF_UART_EVENT_RX_FULL		(1UL<<2)	/**< Rx buffer is full; arg = the data received */

/** UART driver status flags (configuration), stored in UART_INFO for each instance */
#define		MSF_UART_STATUS_INTMODE		MSF_UART_INT_MODE		/**< interrupt mode (not polled) */
#define		MSF_UART_STATUS_POLLEDMODE	MSF_UART_POLLED_MODE	/**< interrupt mode (not polled) */

/** UART_speed_t 
 @brief The data type for baud rate for UART (SCI). 
*/
typedef UART_baudrate_type  UART_speed_t;


/**
\brief Access structure of the UART Driver.
*/
typedef struct _MSF_DRIVER_USART {  
  uint32_t      (*Initialize)   (UART_speed_t baudrate, MSF_UART_Event_t cb_event);  
  uint32_t      (*Uninitialize) (void);                       
  uint32_t      (*PowerControl) (MSF_power_state state); 
  uint32_t      (*Control)      (uint32_t control, uint32_t arg);
  uint32_t      (*Send)         (const void* data, uint32_t cnt);
  uint32_t      (*Receive)      (void* data, uint32_t cnt);
  uint32_t      (*DataAvailable)    (void);            
  
} const MSF_DRIVER_USART;

/* Declare the instance(s) of the UART driver(s). 
 * This is placed here rather than in uart_k25.h because then
 * the client program would need to include the (possible) MCU-specific
 * file such us uart_k25.h, which is not good for generic drivers such as Conio. */
extern MSF_DRIVER_USART Driver_UART0;

#ifdef __cplusplus
}
#endif
/* ----------- end of file -------------- */
#endif /* MSF_DRIVER_UART_H */
