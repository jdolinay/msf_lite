/****************************************************************************
 * @file     tpm_kl25.c
 * @brief    CMSIS-like driver for TPM timer in kinetis KL25
 * @version  1
 * @date     17. June 2014
 *
 * @note     Drivers: Driver_TPM0, Driver_TPM1, Driver_TPM2 
 *  see TPM_SetChannelMode comment for calculating the freq, duty etc. for various
 *  timer modes.
 *
 ******************************************************************************/
/* MSF includes */
#include "msf_config.h"

/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"
#include "msf.h"

#include "tpm_kl25.h"


/* Define the resource for each TPM available on the MCU */
#if (MSF_DRIVER_TPM0)    
/* runtime info for TPM0 */
static TPM_INFO TPM0_Info;    

/* The pins for TPM0 
 * The pins are user-configurable through msf_config.h file.
 * Each is defined by pin-code (see msf_<device>.h) and the number of the alternate
 * function (ALTn) which is the timer channel function for this pin. 
 * IMPORTANT: there must always be 6 pins defined for each TMP module, even if the pin/channel is not used!
 * use {GPIO_INVALID_PIN, 0} for such pins/channels. 
 *  */
static TPM_PINS TPM0_Pins = {
		{	/* the array within the structure */
		{MSF_TPM0_CH0_PIN, MSF_TPM0_CH0_ALT},	/* pin for channel 0 */
		{MSF_TPM0_CH1_PIN, MSF_TPM0_CH1_ALT},	/* pin for channel 1 */
		{MSF_TPM0_CH2_PIN, MSF_TPM0_CH2_ALT},	/* pin for channel 2 */
		{MSF_TPM0_CH3_PIN, MSF_TPM0_CH3_ALT},	/* pin for channel 3 */
		{MSF_TPM0_CH4_PIN, MSF_TPM0_CH4_ALT},	/* pin for channel 4 */
		{MSF_TPM0_CH5_PIN, MSF_TPM0_CH5_ALT},	/* pin for channel 5 */
		}
};
/* TPM0 Resources */
static TPM_RESOURCES TPM0_Resources = {
  TPM0,    /* ADC type object defined in CMSIS <device.h>*/
  &TPM0_Info,
  &TPM0_Pins,
  6,	/* number of channels */
};
#endif /* MSF_DRIVER_TPM0 */


#if (MSF_DRIVER_TPM1)    
/* runtime info for TPM1 */
static TPM_INFO TPM1_Info;    

/* The pins for TPM1 
 * IMPORTANT: there must always be 6 pins defined for each TMP module, even if the pin/channel is not used!
 * use {GPIO_INVALID_PIN, 0} for such pins/channels. 
 *  */
static TPM_PINS TPM1_Pins = {
		{	/* the array within the structure */
		{MSF_TPM1_CH0_PIN, MSF_TPM1_CH0_ALT},	/* pin for channel 0 */
		{MSF_TPM1_CH1_PIN, MSF_TPM1_CH1_ALT},	/* pin for channel 1 */
		{GPIO_INVALID_PIN, 0},		/* pin for channel 2 */
		{GPIO_INVALID_PIN, 0},		/* pin for channel 3 */
		{GPIO_INVALID_PIN, 0},		/* pin for channel 4 */
		{GPIO_INVALID_PIN, 0},		/* pin for channel 5 */
		}
};
/* TPM1 Resources */
static TPM_RESOURCES TPM1_Resources = {
  TPM1,    /* ADC type object defined in CMSIS <device.h>*/
  &TPM1_Info,
  &TPM1_Pins,
  2,
};
#endif /* MSF_DRIVER_TPM1 */

#if (MSF_DRIVER_TPM2)    
/* runtime info for TPM1 */
static TPM_INFO TPM2_Info;    

/* The pins for TPM1 
 * IMPORTANT: there must always be 6 pins defined for each TMP module, even if the pin/channel is not used!
 * use {GPIO_INVALID_PIN, 0} for such pins/channels. 
 *  */
static TPM_PINS TPM2_Pins = {
		{	/* the array within the structure */
		{MSF_TPM2_CH0_PIN, MSF_TPM2_CH0_ALT},	/* pin for channel 0 */
		{MSF_TPM2_CH1_PIN, MSF_TPM2_CH1_ALT},	/* pin for channel 1 */
		{GPIO_INVALID_PIN, 0},		/* pin for channel 2 */
		{GPIO_INVALID_PIN, 0},		/* pin for channel 3 */
		{GPIO_INVALID_PIN, 0},		/* pin for channel 4 */
		{GPIO_INVALID_PIN, 0},		/* pin for channel 5 */
		}
};
/* TPM2 Resources */
static TPM_RESOURCES TPM2_Resources = {
  TPM2,    /* ADC type object defined in CMSIS <device.h>*/
  &TPM2_Info,
  &TPM2_Pins,
  2,
};
#endif /* MSF_DRIVER_TPM1 */



/* prototypes */
static uint32_t TPM_Control(uint32_t control, uint32_t arg, TPM_RESOURCES* tpm);

/* internal functions */
static void wtpm_enable_pin(uint32_t channel, TPM_RESOURCES* tpm);
static void wtpm_enable_int(uint32_t irqno, uint32_t enable);    

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
	/* channel configuration registers - there are 6 in the <device>.h declaration of TPM_Type but we cannot
	 * access those not available, e.g. if TPM1 only has 2 channels, we can only write to CONSTROLS[0] and [1]
	 * Note: TPM1 and TPM2 have only 2 channels */
	for ( i=0; i<tpm->nchannels; i++ )
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

#if (MSF_DRIVER_TPM2)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM2_Initialize (MSF_TPM_Event_t pEvent) 
{
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;	/* Enable clock for TPM1 BEFORE calling TPM_Initialize! */
	return TPM_Initialize(pEvent, &TPM2_Resources); 	
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
	uint32_t irq;
    /* Reset internal state for this instance of the TPM driver */
    tpm->info->cb_event = null;    
    /* if interrupt is enabled in NVIC, disable it */
    if ( (tpm->info->status & (MSF_TPM_STATUS_ANY_CHANNEL | MSF_TPM_STATUS_SIGNAL_TOF)) != 0 )
    {
    	irq = MSF_TPM_GETNVIC_IRQn(tpm->reg);
    	wtpm_enable_int(irq, 0);
    }
    
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

#if (MSF_DRIVER_TPM1)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM1_Uninitialize (void) 
{
  return TPM_Uninitialize(&TPM1_Resources);
}
#endif

#if (MSF_DRIVER_TPM2)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM2_Uninitialize (void) 
{
  return TPM_Uninitialize(&TPM2_Resources);
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

#if (MSF_DRIVER_TPM1)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM1_PowerControl(MSF_power_state state) 
{
  return TPM_PowerControl(state, &TPM1_Resources);
}
#endif    /* MSF_DRIVER_TPM1 */

#if (MSF_DRIVER_TPM2)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM2_PowerControl(MSF_power_state state) 
{
  return TPM_PowerControl(state, &TPM2_Resources);
}
#endif    /* MSF_DRIVER_TPM2 */

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
				wtpm_enable_int(i, 1 );
				
				tpm->reg->SC |= TPM_SC_TOIE_MASK;
				tpm->info->status |=  MSF_TPM_STATUS_SIGNAL_TOF;	/* remember we report TOF now */
			}
			else
			{
				tpm->info->status &=  ~MSF_TPM_STATUS_SIGNAL_TOF;	/* remember we do not report TOF now */
				tpm->reg->SC &= ~TPM_SC_TOIE_MASK; 				
				/* Disable the interrupt in NVIC */
				wtpm_enable_int(i, 0 );				
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

#if (MSF_DRIVER_TPM1)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM1_Control(uint32_t control, uint32_t arg) 
{
  return TPM_Control(control, arg, &TPM1_Resources);
}
#endif   /* MSF_DRIVER_TPM1 */

#if (MSF_DRIVER_TPM2)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM2_Control(uint32_t control, uint32_t arg) 
{
  return TPM_Control(control, arg, &TPM2_Resources);
}
#endif   /* MSF_DRIVER_TPM2 */

/**
  \brief       Set the channel to PWM, input capture or output compare mode
  \param[in]   channel The number of the channel to set (0-5) but depends on TPMn module!
  \param[in]   mode The mode to set. One of the enum values defined in drv_tpm.h!
  \param[in]   args Optional other options for the mode;
  \return      error code (0 = OK)
  \note   
  Information for the modes:
  Fsrc - frequency of the clock source for the timer (before prescaler) 
  Ft - frequency of the timer clock (after prescaling)
  Fo - frwquency of the output (PWM) signal
  MOD is the TOP value at which the counter resets. Set by Control with MSF_TPM_TOP_VALUE flag.  
  
  PWM edge aligned: 
   PWM frequency: Fo = Ft / (MOD+1)
   Duty for channel n: CnV 
   To calculate the MOD for required Fo: 
   MOD = Ft/Fo - 1 = Fsrc / (Fo . PRESCALER) - 1     
  
  PWM center-aligned:
   PWM frequency: Fo = Ft / 2*MOD
   Duty for channel n: 2xCnV
   MOD must be within 1 and 0x7fff (32767)!
   MOD = Ft/(2.Fo) = Fsrc / (2 . Fo . PRESCALER)   
  
  Possible masked values for the "args" parameter:
   MSF_TPM_PARAM_CHANNEL_EVENT - signal the channel event (interrupt)   
           
*/
static uint32_t TPM_SetChannelMode(uint32_t channel, TMP_channel_mode_t mode, uint32_t args, TPM_RESOURCES* tpm)
{
	uint32_t val, i;
	 	
	/* Check if the channel is valid for given instance of TPM (TPM1 and 2 only have 2 channels) */	 
	if ( channel >= tpm->nchannels )
		return MSF_ERROR_ARGUMENT;
	
	/* disable the counter before changing values */
	val = (tpm->reg->SC & TPM_SC_CMOD_MASK);	/* save current clock mode */
	tpm->reg->SC &= ~TPM_SC_CMOD_MASK;
	/* wait until the counter is really disabled (change is "acknowledged in the timer clock domain" :) )*/
	while (tpm->reg->SC & TPM_SC_CMOD_MASK ) ;
	
	tpm->reg->CNT = 0;	/* reset counter */
	
	/* disable the channel which will be changed*/
	tpm->reg->CONTROLS[channel].CnSC = 0;
	while ( tpm->reg->CONTROLS[channel].CnSC != 0 )
		;	/* wait for the change to be "acknowledged in the timer clock domain" */
	tpm->reg->CONTROLS[channel].CnV = 0;
	
	/* Set mode to up counting - this is valid for all modes except center-aligned PWM,
	 * which sets the mode below. */	
	tpm->reg->SC &=  ~TPM_SC_CPWMS_MASK;	
				
	switch(mode)
	{
	case Disabled:	/* The channel is disabled */
		/* the channel is disabled above... */
		break;
		
	case SWcompare:	/* TODO: what is this mode? */
		tpm->reg->CONTROLS[channel].CnSC |= TPM_CnSC_MSB_MASK;	
		break;

		
	case PWM_edge_hightrue:	/* PWM; set output at the start of the period */
		wtpm_enable_pin(channel, tpm);	
		tpm->reg->CONTROLS[channel].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK);	/* edge aligned PWM... */
		break;
		
	case PWM_edge_lowtrue:	/* PWM; clear output at the start of the period */		
		wtpm_enable_pin(channel, tpm);	
		tpm->reg->CONTROLS[channel].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK);	/* edge aligned PWM... */
		break;
		
	case PWM_center_hightrue:
		tpm->reg->SC |=  TPM_SC_CPWMS_MASK;	// Set mode to up-down counting
		wtpm_enable_pin(channel, tpm);	
		tpm->reg->CONTROLS[channel].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK);		
		break;
		
	case PWM_center_lowtrue:
		tpm->reg->SC |=  TPM_SC_CPWMS_MASK;	// Set mode to up-down counting
		wtpm_enable_pin(channel, tpm);
		tpm->reg->CONTROLS[channel].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK);	
		break;
			
	/* TODO: add other modes */
			
	}	/* end switch */
	
	i = MSF_TPM_GETNVIC_IRQn(tpm->reg);	/* IRQ number in NVIC */
	if ( i > 0 )
	{
		/* Enable/disable the event for this channel */
		tpm->reg->CONTROLS[channel].CnSC |= TPM_CnSC_CHF_MASK;	/* clear possible pending interrupt */
		if ( args & MSF_TPM_PARAM_CHANNEL_EVENT)
		{
			/* If the TPM interrupt is not enabled yet in NVIC, we need to enable it */
			if ( (tpm->info->status & (MSF_TPM_STATUS_ANY_CHANNEL | MSF_TPM_STATUS_SIGNAL_TOF)) == 0 )
			{
				wtpm_enable_int(i, 1);
			}
	
			/* enable the event */
			tpm->reg->CONTROLS[channel].CnSC |= TPM_CnSC_CHIE_MASK;	/* enable channel interrupt */
			tpm->info->status |=  MSF_TPM_STATUS_SIGNAL_CHN(channel);	/* remember we report TOF now */	
		}
		else
		{
			/* disable the event */
			tpm->reg->CONTROLS[channel].CnSC &= ~TPM_CnSC_CHIE_MASK;	/* disable channel interrupt */
			tpm->info->status &=  ~MSF_TPM_STATUS_SIGNAL_CHN(channel);	/* remember we do not report TOF now */
			
			/* Disable the interrupt in NVIC if not needed anymore */
			if ( (tpm->info->status & (MSF_TPM_STATUS_ANY_CHANNEL | MSF_TPM_STATUS_SIGNAL_TOF)) == 0 )
			{
				wtpm_enable_int(i, 0);
			}
		}
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

#if (MSF_DRIVER_TPM1)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM1_SetChannelMode(uint32_t channel, TMP_channel_mode_t mode, uint32_t args) 
{
  return TPM_SetChannelMode(channel, mode, args, &TPM1_Resources);
}
#endif   /* MSF_DRIVER_TPM1 */

#if (MSF_DRIVER_TPM2)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t TPM2_SetChannelMode(uint32_t channel, TMP_channel_mode_t mode, uint32_t args) 
{
  return TPM_SetChannelMode(channel, mode, args, &TPM2_Resources);
}
#endif   /* MSF_DRIVER_TPM2 */

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

#if (MSF_DRIVER_TPM1)   
static uint32_t TPM1_WriteChannel(uint32_t channel, uint16_t value) 
{
	return TPM_WriteChannel(channel, value,  &TPM1_Resources); 
}
#endif   /* MSF_DRIVER_TPM0 */

#if (MSF_DRIVER_TPM2)   
static uint32_t TPM2_WriteChannel(uint32_t channel, uint16_t value) 
{
	return TPM_WriteChannel(channel, value,  &TPM2_Resources); 
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

#if (MSF_DRIVER_TPM1)   
static uint32_t TPM1_ReadChannel(uint32_t channel) 
{
	return TPM_ReadChannel(channel, &TPM1_Resources); 
}
#endif   /* MSF_DRIVER_TPM1 */

#if (MSF_DRIVER_TPM2)   
static uint32_t TPM2_ReadChannel(uint32_t channel) 
{
	return TPM_ReadChannel(channel, &TPM2_Resources); 
}
#endif   /* MSF_DRIVER_TPM2 */


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
		for ( i=0; i<tpm->nchannels; i++)
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
#endif /* MSF_DRIVER_TPM1 */

#if (MSF_DRIVER_TPM1) 
void FTM1_IRQHandler(void)
{
	TPM_IRQHandler(&TPM1_Resources);
}
#endif /* MSF_DRIVER_TPM1 */

#if (MSF_DRIVER_TPM2) 
void FTM2_IRQHandler(void)
{
	TPM_IRQHandler(&TPM2_Resources);
}
#endif /* MSF_DRIVER_TPM2 */

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

/* Access structure for TPM1 */
#if (MSF_DRIVER_TPM1)
/* Only if the user-configuration in msf_config_<device>.h specifies that
 * it needs this driver we will create the instance */
MSF_DRIVER_TPM Driver_TPM1 = {
  TPM1_Initialize,
  TPM1_Uninitialize,
  TPM1_PowerControl,
  TPM1_Control,  
  TPM1_SetChannelMode,
  TPM1_WriteChannel,
  TPM1_ReadChannel,
  
};
#endif	/* MSF_DRIVER_TPM1 */

/* Access structure for TPM2 */
#if (MSF_DRIVER_TPM2)
/* Only if the user-configuration in msf_config_<device>.h specifies that
 * it needs this driver we will create the instance */
MSF_DRIVER_TPM Driver_TPM2 = {
  TPM2_Initialize,
  TPM2_Uninitialize,
  TPM2_PowerControl,
  TPM2_Control,  
  TPM2_SetChannelMode,
  TPM2_WriteChannel,
  TPM2_ReadChannel,  
};
#endif	/* MSF_DRIVER_TPM2 */


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

/* Enable/disable interrupt for the timer in NVIC
 * @param  irqno number of the interrupt (e.g. TPM0_IRQn, see <device>.h CMSIS file)
 * @param enable If enable = 0 > disable interrupt; any other value > enable interrupt */
static void wtpm_enable_int(uint32_t irqno, uint32_t enable)
{
	if ( enable )
	{
		NVIC_ClearPendingIRQ(irqno);	/* Clear possibly pending interrupt */
		NVIC_EnableIRQ(irqno);			/* and enable it */	
		/* Set priority for the interrupt; 0 is highest, 3 is lowest */
		NVIC_SetPriority(irqno, MSF_TPM_INT_PRIORITY);					
	}
	else
	{
		NVIC_DisableIRQ(irqno);	
	}
}

/* ----------- end of file -------------- */
