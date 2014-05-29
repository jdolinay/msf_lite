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
	uint32_t osr_val;
	uint32_t sbr_val;
	uint32_t reg_temp = 0;
	
	uart->info->cb_event = event;	/* store pointer to user callback; not used for now */
		
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
	
	osr_val = UART_GET_OSR(baudrate);
	sbr_val = UART_GET_BR(baudrate);
	
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	
	// Disable UART0 before changing registers	
	uart->reg->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);
	  
	
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
		
	//reg_temp = (reg_temp & UART0_C4_OSR_MASK) + 1;
		
	/* Save current value of uartx_BDH except for the SBR field */
	reg_temp = uart->reg->BDH & ~(UART0_BDH_SBR(0x1F));
   
	uart->reg->BDH = reg_temp |  UART0_BDH_SBR(((sbr_val & 0x1F00) >> 8));
	uart->reg->BDL = (uint8_t)(sbr_val & UART0_BDL_SBR_MASK);
		
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
*/
static uint32_t  UART_Uninitialize( UART_RESOURCES* uart)
{
    /* TODO: reset internal state for this instance of the uart */
    uart->info->cb_event = null;
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
     return MSF_ERROR_NOTSUPPORTED;
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
  \note        Common function called by instance-specific function.
            Send bytes to serial line
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
  \note        Common function called by instance-specific function.
            Blocks the caller until specified number of characters is received. 
*/
uint32_t UART_Receive(void* data, uint32_t cnt, UART_RESOURCES* uart)
{
	uint32_t i;
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
