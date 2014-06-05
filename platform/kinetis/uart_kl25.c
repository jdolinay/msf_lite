/****************************************************************************
 * @file     uart_kl25.c
 * @brief    CMSIS-like UART driver for kinetis KL25
 * @version  1
 * @date     20. May 2014
 *
 * @note    This is simplified imitation od CMSIS driver (which does not 
 *          exist for Kinetis(?)). 
 *          Should be used as a standard for MSF drivers.    
 *
 ******************************************************************************/
/* MSF includes */
#include "msf_config.h"

/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"
#include "msf.h"

#include "uart_kl25.h"


#if (MSF_DRIVER_UART0)
/* Define the resource for each UART available on the MCU */
/* runtime info for UART0 */
static UART_INFO UART0_Info;    

/* UART0 Resources */
static UART_RESOURCES UART0_Resources = {
  UART0,    /* UART0 type object defined in CMSIS <device.h>*/
  /*  This is how the I/O pin structure is created for one instance
  {
  #if (RTE_SPI1_NSS_PIN)
    RTE_SPI1_NSS_PORT,
  #else
    NULL,
  #endif
    RTE_SPI1_SCL_PORT,
    RTE_SPI1_MISO_PORT,
    RTE_SPI1_MOSI_PORT,
    RTE_SPI1_NSS_BIT,
    RTE_SPI1_SCL_BIT,
    RTE_SPI1_MISO_BIT,
    RTE_SPI1_MOSI_BIT,
    GPIO_AF_SPI1,
  },*/
 
  &UART0_Info
};

#endif /* MSF_DRIVER_UART0 */

/* Internal function */
static void uart0_setbaudrate(uint32_t baudrate);
static void uart0_intconfig(uint32_t enable, UART_RESOURCES* uart);

/* The driver API functions */

/**
  \fn          uint32_t  UART_Initialize( UART_Event_t event,  UART_RESOURCES* uart)
  \brief       Initialize UART Interface.
  \param[in]   baudrate  baudrate constant as defined in msf_<device>.h
  \param[in]   cb_event  Pointer to UART_Event function or null
  \param[in]   uart       Pointer to UART resources
  \return      error code (0 = OK)
  \note         Common function called by instance-specific function.
  	  Initializes UART for 8N1 operation, interrupts disabled, and
  	  no hardware flow-control.
*/
static uint32_t  UART_Initialize( UART_speed_t baudrate, MSF_UART_Event_t event,  UART_RESOURCES* uart)
{
    /* init given UART */	
	uart->info->cb_event = event;	/* store pointer to user callback; not used for now */
	uart->info->status = MSF_UART_STATUS_POLLED_MODE;
		
	/* Enable clock for PORTA needed for Tx, Rx pins */
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
			
	/* Enable the UART_TXD function on PTA1 */	
	GPIO_PORT_OBJECT(GPIO_A1)->PCR[GPIO_PIN_NUM(GPIO_A1)] = PORT_PCR_MUX(2);
	/* Enable the UART_TXD function on PTA2 */	
	GPIO_PORT_OBJECT(GPIO_A2)->PCR[GPIO_PIN_NUM(GPIO_A2)] = PORT_PCR_MUX(2);
	
	/* set clock for UART0 */
	//SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
	//SIM->SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(MSF_UART0_CLKSEL);
	
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;	/* Enable clock for UART */	
		
	// Disable UART0 before changing registers	
	uart->reg->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);
	  
	uart0_setbaudrate((uint32_t)baudrate, uart);
		
	/* Enable receiver and transmitter */
	uart->reg->C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK );
    return MSF_ERROR_OK;
}

/* Instance specific function pointed-to from the driver access struct */
static uint32_t UART0_Initialize (UART_speed_t baudrate, MSF_UART_Event_t pEvent) 
{
  return UART_Initialize(baudrate, pEvent, &UART0_Resources);
}

/* example: function for another instance of the UART
static int32_t UART1_Initialize (UART_Event_t pEvent) {
  return UART_Initialize(pEvent, &UART1_Resources);
}
}
*/

/**
  \fn          uint32_t  UART_Uninitialize( void)
  \brief       Uninitialize UART Interface. 
  \param[in]   uart    Pointer to UART resources 
  \return      error code (0 = OK)
  \note        Common function called by instance-specific function.
  	  	  Disables the UART.
*/
static uint32_t  UART_Uninitialize( UART_RESOURCES* uart)
{
    /* Reset internal state for this instance of the UART driver */
    uart->info->cb_event = null;
    uart->info->status = 0;
    
    /* Disable the UART */
    uart->reg->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);
    
    return MSF_ERROR_OK;
}
/* Instance specific function pointed-to from the driver access struct */
static uint32_t UART0_Uninitialize (void) 
{
  return UART_Uninitialize(&UART0_Resources);
}


/**
  \brief       Initialize UART Interface.
  \param[in]   state  The requested power state
  \param[in]   uart    Pointer to UART resources
  \return      error code (0 = OK)
  \note        Common function called by instance-specific function.
                Currently does nothing!
*/
static uint32_t UART_PowerControl(MSF_power_state state, UART_RESOURCES* uart)
{
    return MSF_ERROR_NOTSUPPORTED;
} 

/* Instance specific function pointed-to from the driver access struct */
static uint32_t UART0_PowerControl(MSF_power_state state) 
{
  return UART_PowerControl(state, &UART0_Resources);
}

/**
  \brief       Initialize UART Interface.
  \param[in]   control  Flags indicating what parameter(s) to set
  \param[in]   arg Optional argument for the command
  \param[in]   uart    Pointer to UART resources
  \return      error code (0 = OK)
  \note        Common function called by instance-specific function.
            Currently does nothing.
*/
static uint32_t UART_Control(uint32_t control, uint32_t arg, UART_RESOURCES* uart)
{
	/* Changing baudrate? */
	if ( (control & MSF_UART_BAUD_Mask) != 0 && arg != 0 )
	{	/* arg must be is valid baudrate value from the enum UART_speed_t */
		
		/* Disable UART0 before changing registers */	
		uart->reg->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);
		
		uart0_setbaudrate((uint32_t)(UART_speed_t)baudrate, uart);	  
		
		/* Enable receiver and transmitter */
		uart->reg->C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK );		
	}
	
	/* Changing mode: interrupt vs polled */
	if ( (control & MSF_UART_INTMODE_Mask) != 0 )
	{
		if ( (control & MSF_UART_INTMODE_Mask) == MSF_UART_INT_MODE )
		{
			/* interrupt mode */
			if ( uart->info->cb_event == null )
				return MSF_ERROR_ARGUMENT;	/* for interrupt mode the event must be specified in Initialize() */ 
					
			/* internal status to interrupt mode */
			uart->info->status &= ~MSF_UART_STATUS_POLLED_MODE;
			uart->info->status |= MSF_UART_STATUS_INT_MODE;
			
			uart0_intconfig(1);	/* enable interrupt */
		}
		else
		{
			uart0_intconfig(0);	/* disable interrupt */
			
			/* internal status to polled mode */
			uart->info->status &= ~MSF_UART_STATUS_INT_MODE;
			uart->info->status |= MSF_UART_STATUS_POLLED_MODE;
			
		}
	}
	
	
    return MSF_ERROR_OK;
}
/* Instance specific function pointed-to from the driver access struct */
static uint32_t UART0_Control(uint32_t control, uint32_t arg) 
{
  return UART_Control(control, arg, &UART0_Resources);
}

/**
  \brief       Send data over UART.
  \param[in]   data pointer to buffer to send
  \param[in]   cnt number of bytes to send
  \param[in]   uart    Pointer to UART resources
  \return      error code (0 = OK)
  \note        
            Send array of bytes to serial line. Note that it blocks the caller
            until the data are sent. If interrupt mode is enabled, the user callback
            will be called for each character when Tx buffer becomes empty.
            Common function called by instance-specific function.
*/
uint32_t UART_Send(const void* data, uint32_t cnt, UART_RESOURCES* uart)
{
	uint32_t i;
	for ( i =0; i< cnt; i++ )	
	{
		/* Wait until space is available in the FIFO */
		while(!(uart->reg->S1 & UART0_S1_TDRE_MASK))
			 ;
	 	 /* Send the character */
		uart->reg->D = ((const uint8_t*)data)[i];		
	}
	
    return MSF_ERROR_OK;
} 
/* Instance specific function pointed-to from the driver access struct */
static uint32_t UART0_Send(const void* data, uint32_t cnt) 
{
  return UART_Send(data, cnt, &UART0_Resources);
}

/**
  \brief       Receive data from UART.
  \param[in]   data pointer to buffer which receives the data
  \param[in]   cnt number of bytes to receive
  \param[in]   uart    Pointer to UART resources
  \return      error code (0 = OK)
  \note        
            Blocks the caller until specified number of characters is received.
            You should NOT call this function in interrupt mode; in this mode the user callback
            provided in Initialize() will be called for each received character with the
            character as an input argument.
             
            Common function called by instance-specific function.
*/
uint32_t UART_Receive(void* data, uint32_t cnt, UART_RESOURCES* uart)
{
	uint32_t i;
	if ( uart->info->status & MSF_UART_STATUS_POLLED_MODE )
	{	/* blocking (polled) mode */
		
		/* Clear overrun flag 
		 It is possible Receive was not called for some time and we missed some 
		 characters. When Overrun is set, we do not get the receiver data full flag? */
		uart->reg->S1 |= UART0_S1_OR_MASK;
		
		for ( i=0; i<cnt; i++ )
		{		
			/* Wait until character has been received */
			while (!(uart->reg->S1 & UART0_S1_RDRF_MASK))
				;
					 
			/* store the 8-bit data from the receiver */
			((uint8_t*)data)[i] = uart->reg->D;
		}
	}
	else
	{	/* non-blocking (interrupt) mode */
	 			
		/* Disable any pending receive or transmit - this would be error to call us while in progress. */
		uart->info->status &= (~MSF_UART_STATUS_RXNOW & ~MSF_UART_STATUS_TXNOW);	
		/* Setup the internal data to start receiving */
		uart->info->buff = data;
		uart->info->txrx_total = cnt;
		uart->info->txrx_cnt = 0;
		uart->info->status |= MSF_UART_STATUS_RXNOW;	/* now receiving... */
	}
	
    return MSF_ERROR_OK;
} 
/* Instance specific function pointed-to from the driver access struct */
static uint32_t UART0_Receive(void* data, uint32_t cnt) 
{
  return UART_Receive(data, cnt, &UART0_Resources);
}

/**
  \fn          uint32_t  UART_DataAvailable( void)
  \brief       Check is there is some byte received
  \param[in]   uart    Pointer to UART resources 
  \return      number of bytes available (0 or 1 as we do not have any buffer)
  \note        Common function called by instance-specific function.
*/
static uint32_t  UART_DataAvailable( UART_RESOURCES* uart)
{
    if ( (uart->reg->S1 & UART0_S1_RDRF_MASK) != 0 )
        return 1;
    else        
        return 0;
}
/* Instance specific function pointed-to from the driver access struct */
static uint32_t UART0_DataAvailable(void) 
{
  return UART_DataAvailable(&UART0_Resources);
}	


/* Access structure for UART0 */
#if (MSF_DRIVER_UART0)
	MSF_DRIVER_USART Driver_UART0 = {
	  UART0_Initialize,
	  UART0_Uninitialize,
	  UART0_PowerControl,
	  UART0_Control,  
	  UART0_Send,
	  UART0_Receive,  
	  UART0_DataAvailable,
	};
#endif /* MSF_DRIVER_UART0 */

/* Internal workers */
static void uart0_setbaudrate(uint32_t baudrate, UART_RESOURCES* uart)
{
	uint32_t osr_val;
	uint32_t sbr_val;
	uint32_t reg_temp = 0;

	osr_val = UART_GET_OSR(baudrate);
	sbr_val = UART_GET_BR(baudrate);
	// If the OSR is between 4x and 8x then both
	// edge sampling MUST be turned on.  
	if ((osr_val >3) && (osr_val < 9))
		uart->reg->C5|= UART0_C5_BOTHEDGE_MASK;
			
	// Setup OSR value 
	reg_temp = uart->reg->C4;
	reg_temp &= ~UART0_C4_OSR_MASK;
	reg_temp |= UART0_C4_OSR(osr_val-1);
		
	// Write reg_temp to C4 register
	uart->reg->C4 = reg_temp;
				
	/* Save current value of uartx_BDH except for the SBR field */
	reg_temp = uart->reg->BDH & ~(UART0_BDH_SBR(0x1F));
	/* write new value */  
	uart->reg->BDH = reg_temp |  UART0_BDH_SBR(((sbr_val & 0x1F00) >> 8));
	uart->reg->BDL = (uint8_t)(sbr_val & UART0_BDL_SBR_MASK);
}

/* Configure interrupt for UART0 
 * enable = 0 > disable interrupt; anything else >  enablke int*/
static void uart0_intconfig(uint32_t enable, UART_RESOURCES* uart)
{	
	if ( enable )
	{
		/* Configure UART0 */
		/* Enable int when transmit data buffer empty; TDRE flag and Receiver buffer full (RDRF) */ 				
		uart->reg->C2 |= UART0_C2_TIE_MASK | UART0_C2_RIE_MASK;	
		uart->reg->C3 |= UART0_C3_ORIE_MASK;	/* Rx overflow interrupt enabled */
		
		/* Configure NVIC */
		NVIC_ClearPendingIRQ(UART0_IRQn);	/* Clear possibly pending interrupt */
		NVIC_EnableIRQ(UART0_IRQn);			/* and enable it */	
		/* Set priority for the interrupt; 0 is highest, 3 is lowest */
		NVIC_SetPriority(UART0_IRQn, MSF_UART_INT_PRIORITY);
	}
	else
	{
		/* Disable the interrupt in NVIC */
		NVIC_DisableIRQ(UART0_IRQn);	
	}
}

/* Common interrupt handler for all UARTs */
void UART_handleIRQ( UART_RESOURCES* uart)
{
	uint32_t mask = 0;
	uint32_t arg;
	
	/* sanity check - are we in interrupt mode? we should not be called if not. */
	if ( (uart->info->status & MSF_UART_STATUS_INT_MODE) == 0 )
		return;
	
	/* nothing to do if callback was not provided in Initialize() */
	if ( uart->info->cb_event == null )
		return;
	
	if ( uart->reg->S1 & UART0_S1_TDRE_MASK )
	{
		/* Tx buffer empty int. */
		//mask |= MSF_UART_EVENT_TX_EMPTY;
	}
	
	if ( uart->reg->S1 & UART0_S1_TC_MASK )
	{
		/* Transmit complete int. 
		 * just inform the user. */
		uart->info->cb_event(MSF_UART_EVENT_TRANSFER_COMPLETE, 0);
	}
	
	if ( uart->reg->S1 & UART0_S1_RDRF_MASK )
	{
		/* Rx buffer full int. */
		if ( uart->info->status & MSF_UART_STATUS_RXNOW )
		{
			/* Save next byte */
			uart->info->buff[uart->info->txrx_cnt++] = uart->reg->D;
			/* Check if received all we wanted */
			if ( uart->info->txrx_cnt >= uart->info->txrx_total )
			{
				/* stop receiving */
				uart->info->status &= ~MSF_UART_STATUS_RXNOW;	
				/* generate user event */
				uart->info->cb_event(MSF_UART_EVENT_RECEIVE_COMPLETE, 0);
			}
		}
		
	}
	
	/* Rx overflow occurred? */
	if ( uart->reg->S1 & UART0_S1_OR_MASK )
	{		
		uart->info->cb_event(ARM_UART_EVENT_RX_OVERFLOW, 0);
		/* Clear the overflow flag */
		uart->reg->S1 |= UART0_S1_OR_MASK;
	}
	
	/* Call the user handler if needed */
	/*if ( mask != 0 && uart->info->cb_event != null )
		uart->info->cb_event(mask, arg);
	*/
}

/* Interrupt handler for the UART0 */
void UART0_IRQHandler()
{
	UART_handleIRQ( &UART0_Resources);
	
}






/* ------- Simple functions (a C-like driver for the UART) --------------- */
#if 0
/* not used for performance reasons */

/** Write single byte to SCI
  * @param byte of data to write
  **/
 static void uart_write(uint8_t data)
 {
	 /* Wait until space is available in the FIFO */
	 while(!(UART0->S1 & UART0_S1_TDRE_MASK))
		 ;
	    
	 /* Send the character */
	 UART0->D = data;
 }
 
 /** Read byte form SCI; wait for data to arrive!
  * @return byte received from SCI.
  **/
 static uint8_t uart_read(void)
 {
	 /* Wait until character has been received */
	 while (!(UART0->S1 & UART0_S1_RDRF_MASK))
		 ;
	     
	 /* Return the 8-bit data from the receiver */
	 return UART0->D;
 }
 
 /** Check if data are available for reading
  * @return true if there are data 
 **/
 static bool uart_data_available(void)
 {
	 return ((UART0->S1 & UART0_S1_RDRF_MASK) != 0);
 }
#endif
 
 
 
 
/* ----------- end of file -------------- */
