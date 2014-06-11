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
static void uart0_setbaudrate(uint32_t baudrate, UART_RESOURCES* uart);
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
	uart->info->cb_event = event;	/* store pointer to user callback*/
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
	if ( (control & MSF_UART_BAUD_Mask) && (arg != 0) )
	{	/* arg must be is valid baudrate value from the enum UART_speed_t */
		
		/* Disable UART0 before changing registers */	
		uart->reg->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);
		
		uart0_setbaudrate((uint32_t)(UART_speed_t)arg, uart);	  
		
		/* Enable receiver and transmitter */
		uart->reg->C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK );		
	}
	
	/* Changing mode: interrupt vs polled */
	if ( (control & MSF_UART_INTMODE_Mask) != 0 )
	{
		/* stop any transfer in progress */
		uart->info->status &= ~(MSF_UART_STATUS_TXNOW | MSF_UART_STATUS_RXNOW);	
		/* Disable Tx and Rx interrupts */
		uart->reg->C2 &= ~(UART0_C2_TIE_MASK | UART0_C2_RIE_MASK); 	
		
		if ( (control & MSF_UART_INTMODE_Mask) == MSF_UART_INT_MODE )
		{
			/* interrupt mode */
			/*if ( uart->info->cb_event == null )
				return MSF_ERROR_ARGUMENT;*/ /* for interrupt mode the event must be specified in Initialize() */ 
			
			/* internal status to interrupt mode */
			uart->info->status &= ~MSF_UART_STATUS_POLLED_MODE;
			uart->info->status |= MSF_UART_STATUS_INT_MODE;
			
			uart0_intconfig(1, uart);	/* enable interrupt */
		}
		else
		{
			uart0_intconfig(0, uart);	/* disable interrupt */
			
			/* internal status to polled mode */
			uart->info->status &= ~MSF_UART_STATUS_INT_MODE;
			uart->info->status |= MSF_UART_STATUS_POLLED_MODE;			
		}
	}
	
	/* Abort current Send or Receive command */
	if ( control & MSF_UART_ABORTRXTX_Mask)
	{
		/* stop any transfer in progress */
		uart->info->status &= ~(MSF_UART_STATUS_TXNOW | MSF_UART_STATUS_RXNOW);	
		/* Disable Tx and Rx interrupts */
		uart->reg->C2 &= ~(UART0_C2_TIE_MASK | UART0_C2_RIE_MASK); 	
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
        until the data are sent in polled mode.
        If interrupt mode is enabled, the function sends 1st byte and then returns to caller.        
        The next bytes are sent "in the background" using interrupt handler. 
        NOTE that the memory pointed to by "data" must be still available (do not use local variable in caller)!
        The caller is notified by MSF_UART_EVENT_SEND_COMPLETE event when send is complete.
            
        Common function called by instance-specific function.
*/
uint32_t UART_Send(const void* data, uint32_t cnt, UART_RESOURCES* uart)
{
	uint32_t i;
	if ( uart->info->status & MSF_UART_STATUS_POLLED_MODE )
	{	/* blocking (polled) mode */
		for ( i =0; i< cnt; i++ )	
		{
			/* Wait until space is available in the FIFO */
			while(!(uart->reg->S1 & UART0_S1_TDRE_MASK))
				 ;
			 /* Send the character */
			uart->reg->D = ((const uint8_t*)data)[i];		
		}
	}
	else
	{
		/* non-blocking (interrupt) mode */
		//if ( cnt > 1 )	/* If there is only 1 byte, we just send it below... */
		{
			/* Disable any pending transmit - this would be error to call us while in progress. */			
			uart->info->status &= ~MSF_UART_STATUS_TXNOW;
			/* Disable Tx  interrupt. If user calls (by error) Send or Receive before previous 
			 * operation is completed, the interrupt would never be disabled. */
			uart->reg->C2 &= ~UART0_C2_TIE_MASK; 	
			
			/* Setup the internal data to start sending */
			uart->info->txbuff = (void*)data;
			uart->info->tx_total = cnt;
			uart->info->tx_cnt = 0;		/* 1 because we send the 1st char here */
			uart->info->status |= MSF_UART_STATUS_TXNOW;	/* now sending... */
			uart->reg->C2 |= UART0_C2_TIE_MASK;
		}
#if 0
		/* Send first character now; the next one will be sent by ISR */ 
		/* Wait until space is available in the FIFO */
		while(!(uart->reg->S1 & UART0_S1_TDRE_MASK))
			 ;
		 /* Send the character */
		uart->reg->D = ((const uint8_t*)data)[0];
		/* Enable interrupt for Tx buffer empty */
		if ( cnt > 1 )
			uart->reg->C2 |= UART0_C2_TIE_MASK;
#endif
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
        Receive one or mode bytes from serial line. 
        In polled mode (MSF_UART_POLLED_MODE) it blocks the caller until specified number 
        of characters is received.
        In interrupt mode it returns to caller immediately and receives in the background (in ISR).
        NOTE that the memory pointed to by "data" must be still available (do not use local variable in caller)!
        When the "cnt" number of bytes it received the caller is notified by MSF_UART_EVENT_RECEIVE_COMPLETE event.

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
	 			
		/* Disable any pending receive - this would be error to call us while in progress. */
		uart->info->status &= ~MSF_UART_STATUS_RXNOW;	
		/* Disable Rx interrupt. If user calls (by error) Send or Receive before previous 
		* operation is complete, the interrupt would never be disabled. */
		uart->reg->C2 &= ~UART0_C2_RIE_MASK; 	
		
		/* Setup the internal data to start receiving */
		uart->info->rxbuff = data;
		uart->info->rx_total = cnt;
		uart->info->rx_cnt = 0;
		uart->info->status |= MSF_UART_STATUS_RXNOW;	/* now receiving... */
		uart->reg->C2 |= UART0_C2_RIE_MASK; /* Enable interrupt for Rx buffer full */
	}
	
    return MSF_ERROR_OK;
} 
/* Instance specific function pointed-to from the driver access struct */
static uint32_t UART0_Receive(void* data, uint32_t cnt) 
{
  return UART_Receive(data, cnt, &UART0_Resources);
}

/**
  \brief       Get number of bytes received so far during Receive operation in interrupt mode
  \param[in]   uart    Pointer to UART resources
  \return      number of bytes received so far
  \note        
          Common function called by instance-specific function.
*/
static uint32_t UART_GetRxCount(UART_RESOURCES* uart)
{
	if ( uart->info->status & MSF_UART_STATUS_POLLED_MODE )
		return 0;
	return uart->info->rx_cnt;	
}

static uint32_t UART0_GetRxCount(void)
{
	return UART_GetRxCount(&UART0_Resources);
}

/**
  \brief       Get number of bytes sent so far during Send operation in interrupt mode
  \param[in]   uart    Pointer to UART resources
  \return      number of bytes sent so far
  \note        
          Common function called by instance-specific function.
*/
static uint32_t UART_GetTxCount(UART_RESOURCES* uart)
{
	if ( uart->info->status & MSF_UART_STATUS_POLLED_MODE )
		return 0;
	return uart->info->tx_cnt;
}

static uint32_t UART0_GetTxCount(void)
{
	return UART_GetTxCount(&UART0_Resources);
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
	  UART0_GetRxCount,
	  UART0_GetTxCount,
	  UART0_DataAvailable,
	};
#endif /* MSF_DRIVER_UART0 */
	
	
/* Common interrupt handler for all UARTs */
void UART_handleIRQ( UART_RESOURCES* uart)
{	
	
	/* sanity check - are we in interrupt mode? we should not be called if not. */
	if ( (uart->info->status & MSF_UART_STATUS_INT_MODE) == 0 )
		return;
	
	/* nothing to do if callback was not provided in Initialize() */
	/*if ( uart->info->cb_event == null )
		return;*/
	
	
	/* Tx buffer empty int. */
	/* If sending now and the Tx buffer is empty
	 * Note that it is empty all the time except when sending! */
	if ( (uart->info->status & MSF_UART_STATUS_TXNOW) && (uart->reg->S1 & UART0_S1_TDRE_MASK) )
	{
		/* Send next char */			
		uart->reg->D = ((const uint8_t*)uart->info->txbuff)[uart->info->tx_cnt++];
		/* Check if sent all we wanted */
		if ( uart->info->tx_cnt >= uart->info->tx_total )
		{
			/* stop sending */
			uart->info->status &= ~MSF_UART_STATUS_TXNOW;	
			/* Disable this interrupt; the Send() will re-enable it */
			uart->reg->C2 &= ~UART0_C2_TIE_MASK;
			/* generate user event */
			if ( uart->info->cb_event )
				uart->info->cb_event(MSF_UART_EVENT_SEND_COMPLETE, 0);
			/* Reset the Tx count */
			uart->info->tx_cnt = 0;
			/* Enable Transmit complete interrupt  - to generate event when line is idle */
			uart->reg->C2 |= UART0_C2_TCIE_MASK;	
		}	
		
	}
	

	/* Transmit complete int.  */
	/* If Tx complete int is enabled and the flag is set.
	 * Note that the flag is set all the time if th eline is idle, so we use the int enable bit
	 * to chech if we are interested in this event now */
	if ( (uart->reg->C2 & UART0_C2_TCIE_MASK) && (uart->reg->S1 & UART0_S1_TC_MASK) )
	{
		/* disable the TC interrupt */
		uart->reg->C2 &= ~UART0_C2_TCIE_MASK;	
		if ( uart->info->cb_event )
			uart->info->cb_event(MSF_UART_EVENT_TRANSFER_COMPLETE, 0);
	}

	
	/* Rx buffer full flag is set AND we are receiving now  */
	if ( (uart->reg->S1 & UART0_S1_RDRF_MASK) && (uart->info->status & MSF_UART_STATUS_RXNOW) )
	{		
		/* Save next byte */
		((uint8_t*)uart->info->rxbuff)[uart->info->rx_cnt++] = uart->reg->D;
		/* Check if received all we wanted */
		if ( uart->info->rx_cnt >= uart->info->rx_total )
		{
			/* stop receiving */
			uart->info->status &= ~MSF_UART_STATUS_RXNOW;
			/* Disable this interrupt; the Receive() will re-enable it when needed */
			uart->reg->C2 &= ~UART0_C2_RIE_MASK;
			/* generate user event */
			if ( uart->info->cb_event )
				uart->info->cb_event(MSF_UART_EVENT_RECEIVE_COMPLETE, 0);
			/* reset the Rx count */
			uart->info->rx_cnt = 0;
		}				
		
	}
	

	/* Rx overflow occurred? */
	if ( uart->reg->S1 & UART0_S1_OR_MASK )
	{	
		/* Clear the overflow flag */
		uart->reg->S1 |= UART0_S1_OR_MASK;		
		if ( uart->info->cb_event )
			uart->info->cb_event(MSF_UART_EVENT_RX_OVERFLOW, 0);		
	}

	
}

/* Interrupt handler for the UART0 */
void UART0_IRQHandler()
{
	UART_handleIRQ( &UART0_Resources);
	
}



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
		/* uart->reg->C2 |= UART0_C2_TIE_MASK | UART0_C2_RIE_MASK; */
		/* We cannot enable the interrupts for all time, only when really Tx-ing or Rx-ing, otherwise the
		 * ISR is called all the time */
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
