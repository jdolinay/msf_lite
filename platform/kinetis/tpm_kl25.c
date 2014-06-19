/****************************************************************************
 * @file     tpm_kl25.c
 * @brief    CMSIS-like driver for TPM timer in kinetis KL25
 * @version  1
 * @date     17. June 2014
 *
 * @note     Drivers: Driver_TPM0, Driver_TPM1, Driver_TPM2 
 *
 ******************************************************************************/
/* MSF includes */
#include "msf_config.h"

/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"
#include "msf.h"

#include "tpm_kl25.h"


#if (MSF_DRIVER_TPM0)    
/* Define the resource for each TPM available on the MCU */
/* runtime info for TPM0 */
static TPM_INFO TPM0_Info;    

/* The pins for TPM0 
 * Each is defined by pin-code (see msf_<device>.h) and the number of the alternate
 * function (ALTn) which is the timer channel function for this pin. 
 * IMPORTANT: there must always be 6 pins defined for each TMP module, even if the pin/channel is not used!
 * use {GPIO_INVALID_PIN, 0} for such pins/channels. */
static TPM_PINS TPM0_Pins = {
		{	/* the array within the structure */
		{GPIO_A3, 3},		/* pin for channel 0 */
		{GPIO_A4, 3},		/* pin for channel 1 */
		{GPIO_A5, 3},		/* pin for channel 2 */
		{GPIO_E30, 3},	/* pin for channel 3 */
		{GPIO_E31, 3},	/* pin for channel 4 */
		{GPIO_A0, 3},		/* pin for channel 5 */
		}
};
/* TPM0 Resources */
static TPM_RESOURCES TPM0_Resources = {
  TPM0,    /* ADC type object defined in CMSIS <device.h>*/
  &TPM0_Info,
  &TPM0_Pins,
};
#endif /* MSF_DRIVER_TPM0 */

/* prototypes */
static uint32_t TPM_Control(uint32_t control, uint32_t arg, TPM_RESOURCES* tpm);

/* internal functions */
static void wtpm_enable_pin(uint32_t channel, TPM_RESOURCES* tpm);
    

/* The driver API functions */

/**
  \fn          uint32_t  TPM_Initialize( MSF_TPM_Event_t event,  TPM_RESOURCES* tpm)
  \brief       Initialize timer  
  \param[in]   cb_event  Pointer to TPM_Event function or null
  \param[in]   adc       Pointer to TPM resources
  \return      error code (0 = OK)
  \note         
  	  Initialises TPM timer: timer clocked from internal clock source (see MSF_TPM_CLKSEL
  	  in msf_<device>.h for the values for given F_CPU); prescaler = 1, tof not signalled.
*/
static uint32_t  TPM_Initialize( MSF_TPM_Event_t event,  TPM_RESOURCES* tpm)
{
	uint32_t i;
	
	/* init given timer */	
	tpm->info->cb_event = event;	/* store pointer to user callback*/
	tpm->info->status = 0;	
	
	/* Set the clock for timers TPMx (shared by all instances) */
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(MSF_TPM_CLKSEL);
	/* note that the caller needs to enable the clock for the specific TPMn module before calling this!
	 * if the clock for TPMn is disabled, attempt to write its register will cause an exception!
	 * in SIM->SCGC6 */	
	
	tpm->reg->SC = 0;	/* default values, counter disabled */
	tpm->reg->MOD = 0x0000ffff;
	/* channel configuration registers - there are always 6 even if TPM1 and TPM2 have only 2 channels */
	for ( i=0; i<6; i++ )
		tpm->reg->CONTROLS[i].CnSC = 0;
	tpm->reg->CONF = 0;
	
	/* Now set internal clock (prescaler = 0) */
	TPM_Control(MSF_TPM_CLOCK_INTERNAL, 0, tpm);

    return MSF_ERROR_OK;
}

#if (MSF_DRIVER_TPM0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM0_Initialize (MSF_TPM_Event_t pEvent) 
{
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;	/* Enable clock for TPM0 BEFORE calling TPM_Initialize! */
	return TPM_Initialize(pEvent, &TPM0_Resources); 		
}
#endif

#if (MSF_DRIVER_TPM1)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM1_Initialize (MSF_TPM_Event_t pEvent) 
{
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;	/* Enable clock for TPM1 BEFORE calling TPM_Initialize! */
	return TPM_Initialize(pEvent, &TPM1_Resources); 	
}
#endif


/**
  \fn          uint32_t  TPM_Uninitialize( void)
  \brief       Uninitialize timer 
  \param[in]   adc    Pointer to timer resources 
  \return      error code (0 = OK)
  \note        Common function called by instance-specific function.
*/
static uint32_t  TPM_Uninitialize( TPM_RESOURCES* tpm)
{
    /* TODO: reset internal state for this instance of the TPM driver */
    tpm->info->cb_event = null;    
    tpm->info->status = 0;
    return MSF_ERROR_OK;
}

#if (MSF_DRIVER_TPM0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM0_Uninitialize (void) 
{
  return TPM_Uninitialize(&TPM0_Resources);
}
#endif


/**
  \brief       Change power mode for the timer
  \param[in]   state  The requested power state
  \param[in]   tpm    Pointer to TPM resources
  \return      error code (0 = OK)
  \note        Common function called by instance-specific function.
                Currently does nothing!
                Note that the ADC automatically enters low power mode after conversion, see datasheet.
*/
static uint32_t TPM_PowerControl(MSF_power_state state, TPM_RESOURCES* tpm)
{
    return MSF_ERROR_NOTSUPPORTED;
} 

#if (MSF_DRIVER_TPM0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM0_PowerControl(MSF_power_state state) 
{
  return TPM_PowerControl(state, &TPM0_Resources);
}
#endif    /* MSF_DRIVER_TPM0 */

/**
  \brief       Control various options of the TPM
  \param[in]   control  Flags indicating which parameter(s) to set
  \param[in]   arg Optional argument for the command
  \param[in]   tpm    Pointer to TPM resources
  \return      error code (0 = OK)
  \note        Common function called by instance-specific function.
            
*/
static uint32_t TPM_Control(uint32_t control, uint32_t arg, TPM_RESOURCES* tpm)
{	
	uint32_t val, i;
	
	tpm->reg->CNT = 0;	/* reset counter */
	
	/* Changing clock option: disabled/internal/external */
	if ( control & MSF_TPM_CLOCK_MODE_Mask )
	{
		val = ((control & MSF_TPM_CLOCK_MODE_Mask) >> MSF_TPM_CLOCK_MODE_Pos);
		tpm->reg->SC &= ~TPM_SC_CMOD_MASK;
		/* wait until the counter is really disabled (change is "acknowledged in the timer clock domain" :) )*/
		while (tpm->reg->SC & TPM_SC_CMOD_MASK ) ;
		tpm->reg->SC |= TPM_SC_CMOD(val-1);				
	}
	
	/* changing prescaler settings; arg is the value */
	if ( control & MSF_TPM_PRESCALER_SET && arg != 0)
	{		
		/* disable the counter before changing values */
		val = (tpm->reg->SC & TPM_SC_CMOD_MASK);	/* save current clock mode */
		tpm->reg->SC &= ~TPM_SC_CMOD_MASK;
		/* wait until the counter is really disabled (change is "acknowledged in the timer clock domain" :) )*/
		while (tpm->reg->SC & TPM_SC_CMOD_MASK ) ;
					
		tpm->reg->SC &= ~TPM_SC_PS_MASK;
		tpm->reg->SC |= TPM_SC_PS(arg);
		/*tpm->reg->CNT = 0;*/	/* reset counter */
		
		/* restore counter clock mode */
		tpm->reg->SC |= val;	
	}
	
	/* changing TOF interrupt (event) settings */
	if ( control & MSF_TPM_TOF_Mask )
	{
		i = MSF_TPM_GETNVIC_IRQn(tpm->reg);
		if ( i > 0 )
		{
			/* disable the counter before changing values */
			val = (tpm->reg->SC & TPM_SC_CMOD_MASK);	/* save current clock mode */
			tpm->reg->SC &= ~TPM_SC_CMOD_MASK;
			/* wait until the counter is really disabled (change is "acknowledged in the timer clock domain" :) )*/
			while (tpm->reg->SC & TPM_SC_CMOD_MASK ) ;
			
			
			/* clear possibly pending TOF interrupt */
			tpm->reg->SC |= TPM_SC_TOF_MASK;
			
			if ( control & MSF_TPM_TOF_SIGNAL )
			{				
				/* Configure NVIC */
				NVIC_ClearPendingIRQ(i);	/* Clear possibly pending interrupt */
				NVIC_EnableIRQ(i);			/* and enable it */	
				/* Set priority for the interrupt; 0 is highest, 3 is lowest */
				NVIC_SetPriority(i, MSF_TPM_INT_PRIORITY);
				
				tpm->reg->SC |= TPM_SC_TOIE_MASK;
				tpm->info->status |=  MSF_TPM_STATUS_SIGNAL_TOF;	/* remember we report TOF now */
			}
			else
			{
				tpm->info->status &=  ~MSF_TPM_STATUS_SIGNAL_TOF;	/* remember we do not report TOF now */
				tpm->reg->SC &= ~TPM_SC_TOIE_MASK; 				
				/* Disable the interrupt in NVIC */
				NVIC_DisableIRQ(i);	
			}
			
			/* restore counter clock mode */
			tpm->reg->SC |= val;
		}
	}
	
	/* Changing the modulo value? */
	if ( control & MSF_TPM_MOD_Mask )
	{
		/* disable the counter before changing values */
		val = (tpm->reg->SC & TPM_SC_CMOD_MASK);	/* save current clock mode */
		tpm->reg->SC &= ~TPM_SC_CMOD_MASK;
		/* wait until the counter is really disabled (change is "acknowledged in the timer clock domain" :) )*/
		while (tpm->reg->SC & TPM_SC_CMOD_MASK ) ;
				
		tpm->reg->CNT = 0;	/* recommended in datasheet */
		tpm->reg->MOD = (arg & 0x0000ffff);		
				
		/* restore counter clock mode */
		tpm->reg->SC |= val;	
	}
	
	
    return MSF_ERROR_OK;
}

#if (MSF_DRIVER_TPM0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM0_Control(uint32_t control, uint32_t arg) 
{
  return TPM_Control(control, arg, &TPM0_Resources);
}
#endif   /* MSF_DRIVER_TPM0 */

/**
  \brief       Set the channel to PWM, input capture or output compare mode
  \param[in]   channel The number of the channel to set (0-5) but depends on TPMn module!
  \param[in]   mode The mode to set. One of the enum values defined in drv_tpm.h!
  \param[in]   args Optional other options for the mode;
  \return      error code (0 = OK)
  \note        
  Possible masked values for the "args" parameter:
   MSF_TPM_PARAM_CHANNEL_EVENT - signal the channel event (interrupt)   
           
*/
static uint32_t TPM_SetChannelMode(uint32_t channel, TMP_channel_mode_t mode, uint32_t args, TPM_RESOURCES* tpm)
{
	uint32_t val;
	/* We do not check if the channel is valid for given instance of TPM (TPM1 and 2 only have 2 channels),
	 * but just check if it is valid in general for TPM, which has max 6 channels. */
	if ( channel > 5 )
		return MSF_ERROR_ARGUMENT;
	
	/* disable the counter before changing values */
	val = (tpm->reg->SC & TPM_SC_CMOD_MASK);	/* save current clock mode */
	tpm->reg->SC &= ~TPM_SC_CMOD_MASK;
	/* wait until the counter is really disabled (change is "acknowledged in the timer clock domain" :) )*/
	while (tpm->reg->SC & TPM_SC_CMOD_MASK ) ;
	
	tpm->reg->CNT = 0;	/* reset counter */
	
	/* Set mode to up counting - this is valid for all modes except center-aligned PWM,
	 * which sets the mode below. */	
	tpm->reg->SC &=  ~TPM_SC_CPWMS_MASK;	
				
	switch(mode)
	{
	case Disabled:	/* The channel is disabled */
		tpm->reg->CONTROLS[channel].CnSC = 0;
		break;
		
	case SWcompare:	/* TODO: what is this mode? */
		break;

		
	case PWM_edge_hightrue:	/* PWM; set output at the start of the period */
		wtpm_enable_pin(channel, tpm);			
		tpm->reg->CONTROLS[channel].CnSC = 0;
		tpm->reg->CONTROLS[channel].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK);	/* edge aligned PWM... */
		break;
		
	case PWM_edge_lowtrue:	/* PWM; clear output at the start of the period */		
		wtpm_enable_pin(channel, tpm);				
		tpm->reg->CONTROLS[channel].CnSC = 0;
		tpm->reg->CONTROLS[channel].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK);	/* edge aligned PWM... */
		break;
		
	case PWM_center_hightrue:
		tpm->reg->SC |=  TPM_SC_CPWMS_MASK;	// Set mode to up-down counting
		break;
		
	case PWM_center_lowtrue:
		tpm->reg->SC |=  TPM_SC_CPWMS_MASK;	// Set mode to up-down counting
		break;
			
	/* TODO: add other modes */
			
	}	/* end switch */
	
	
	/* Enable/disable the event for this channel */
	tpm->reg->CONTROLS[channel].CnSC |= TPM_CnSC_CHF_MASK;	/* clear possible pending interrupt */
	if ( args & MSF_TPM_PARAM_CHANNEL_EVENT)
	{
		/* enable the event */
		tpm->info->status |=  MSF_TPM_STATUS_SIGNAL_CHN(channel);	/* remember we report TOF now */		
		tpm->reg->CONTROLS[channel].CnSC |= TPM_CnSC_CHIE_MASK;	/* enable channel interrupt */
	}
	else
	{
		/* disable the event */
		tpm->reg->CONTROLS[channel].CnSC &= ~TPM_CnSC_CHIE_MASK;	/* disable channel interrupt */
		tpm->info->status &=  ~MSF_TPM_STATUS_SIGNAL_CHN(channel);	/* remember we do not report TOF now */
	}
		
	/* restore counter clock mode */
	tpm->reg->SC |= val;
	
	return MSF_ERROR_OK;
}

#if (MSF_DRIVER_TPM0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM0_SetChannelMode(uint32_t channel, TMP_channel_mode_t mode, uint32_t args) 
{
  return TPM_SetChannelMode(channel, mode, args, &TPM0_Resources);
}
#endif   /* MSF_DRIVER_TPM0 */

/**
  \brief       Write value to the channel register (e.g. duty in pwm mode)
  \param[in]   channel The number of the channel to set (0-5) but depends on TPMn module!
  \param[in]   value The value to write. Only 16-bit value is supported by TPM timers.
  \return      error code (0 = OK)
  \note                      
*/
static uint32_t TPM_WriteChannel(uint32_t channel, uint16_t value, TPM_RESOURCES* tpm) 
{
	if ( channel > 5 )
		return MSF_ERROR_ARGUMENT;
	tpm->reg->CONTROLS[channel].CnV = value;
	return MSF_ERROR_OK;
}

#if (MSF_DRIVER_TPM0)   
static uint32_t TPM0_WriteChannel(uint32_t channel, uint16_t value) 
{
	return TPM_WriteChannel(channel, value,  &TPM0_Resources); 
}
#endif   /* MSF_DRIVER_TPM0 */

/**
  \brief       Read value from the channel register (e.g. in input capture mode the timestamp)
  \param[in]   channel The number of the channel to set (0-5) but depends on TPMn module!
  \param[in]   value The value to write
  \return      The 16-bit value from the CnV register or error code 0xffffffff.
  \note                      
*/
static uint32_t TPM_ReadChannel(uint32_t channel, TPM_RESOURCES* tpm) 
{
	if ( channel > 5 )
		return MSF_ERROR_MAXDWORD;
	return tpm->reg->CONTROLS[channel].CnV;	
}

#if (MSF_DRIVER_TPM0)   
static uint32_t TPM0_ReadChannel(uint32_t channel) 
{
	return TPM_ReadChannel(channel, &TPM0_Resources); 
}
#endif   /* MSF_DRIVER_TPM0 */



/** Interrupt handler for all TMPn instances */
void TPM_IRQHandler(TPM_RESOURCES* tpm)
{
	uint32_t i;
	
	if ( tpm->reg->SC & TPM_SC_TOF_MASK)
	{
		tpm->reg->SC |= TPM_SC_TOF_MASK;	/* clear TOF flag */
		if ( tpm->info->cb_event && (tpm->info->status & MSF_TPM_STATUS_SIGNAL_TOF) )
			tpm->info->cb_event(MSF_TPM_EVENT_TOF, 0);
	}
	
	/* should we report any channel interrupt? */
	if ( tpm->info->status & MSF_TPM_STATUS_ANY_CHANNEL )
	{
		for ( i=0; i<6; i++)
		{
			if (tpm->reg->CONTROLS[i].CnSC & TPM_CnSC_CHF_MASK)
			{
				tpm->reg->CONTROLS[i].CnSC |= TPM_CnSC_CHF_MASK;	/* clear the interrupt flag */
				tpm->info->cb_event(MSF_TPM_EVENT_CHN(i), 0);				
			}
		}
	}
}


/* Interrupt handler for TPM0 */
#if (MSF_DRIVER_TPM0) 
void FTM0_IRQHandler(void)
{
	TPM_IRQHandler(&TPM0_Resources);
}
#endif /* MSF_DRIVER_TPM0 */


/* Access structure for TPM0 */
#if (MSF_DRIVER_TPM0)
/* Only if the user-configuration in msf_config_<device>.h specifies that
 * it needs this driver we will create the instance */
MSF_DRIVER_TPM Driver_TPM0 = {
  TPM0_Initialize,
  TPM0_Uninitialize,
  TPM0_PowerControl,
  TPM0_Control,  
  TPM0_SetChannelMode,
  TPM0_WriteChannel,
  TPM0_ReadChannel,
  
};
#endif	/* MSF_DRIVER_TPM0 */

/*========================  Internal functions ========================== */
/* Configure the pin for given channel of TPM to be used by the TPM (for PWM, input/output capture)*/
static void wtpm_enable_pin(uint32_t channel, TPM_RESOURCES* tpm)
{
	/* Enable the TPMn function on given pin */
	if ( tpm->pins->pin[channel].pin_code != GPIO_INVALID_PIN)
	{
		GPIO_PORT_OBJECT(tpm->pins->pin[channel].pin_code)->PCR[GPIO_PIN_NUM(tpm->pins->pin[channel].pin_code)] = PORT_PCR_MUX(tpm->pins->pin[channel].alt_num);
	}
}

/* ----------- end of file -------------- */
