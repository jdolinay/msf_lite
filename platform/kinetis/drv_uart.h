/****************************************************************************
 * @file     drv_uart.h
 * @brief    CMSIS-like UART driver for kinetis
 * @version  1
 * @date     20. May 2014
 *
 * @note     This is simplified imitation od CMSIS driver which does not 
 *          exist for Kinetis(?). Should be used as a standard for MSF drivers.    
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
typedef void (*MSF_UART_Event_t) (uint32_t event);   

/** Flags (operations and params) for the Control function */
#if 0
/* for now the only parameter is baudrate which is passed into Initialize */
#define     MSF_UART_MODE_ASYNCHRONOUS      (1UL << 0)  /* arg = baudrate */
/* TODO: other modes/runtime options */
#endif

/* UART_speed_t 
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
