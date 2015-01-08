/****************************************************************************
 * @file     module_platform.c
 * @brief    CMSIS-like driver for [module] in [platform]
 * @version  1
 * @date     7. July 2014
 *
 * @note     Drivers: Driver_[module]0, Driver_[module]1, ...
 *
 ******************************************************************************/
/* MSF includes */
#include "msf_config.h"

/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"
#include "msf.h"

#include "[module]_[platform].h"

/** @addtogroup group_DRIVER_template 
 * @{
 */
 
/* Define the resource for each TPM available on the MCU */
#if (MSF_DRIVER_[module]0)    
/* runtime info for [module]0 */
static [module]_INFO [module]0_Info;    

/* The pins for [module] 
 */
static [module]_PINS [module]0_Pins = {
		{	/* the array within the structure */
        /* EXAMPLE */
        #if  0
		{MSF_TPM0_CH0_PIN, MSF_TPM0_CH0_ALT},	/* pin for channel 0 */
		{MSF_TPM0_CH1_PIN, MSF_TPM0_CH1_ALT},	/* pin for channel 1 */
		#endif
		}
};
/* [module]0 Resources */
static [module]_RESOURCES [module]0_Resources = {
  [module]0,    /* ADC type object defined in CMSIS <device.h>*/
  &[module]0_Info,
  &[module]0_Pins,
  
};
#endif /* MSF_DRIVER_[module]0 */


/* Prototypes  for internal use in this file (if needed)*/
static uint32_t [module]_Control(uint32_t control, uint32_t arg, [module]_RESOURCES* tpm);

/* internal functions */
static void w[module]_[function](uint32_t any, [module]_RESOURCES* mod);
    

/* The driver API functions */

/**  
  @brief       Initialize the module  
  @param[in]   cb_event  Pointer to [module]_Event function or null
  @param[in]   adc       Pointer to [module] resources
  @return      error code (0 = OK)
  @note           	  
*/
static uint32_t  [module]_Initialize( MSF_[module]_Event_t event,  [module]_RESOURCES* tpm)
{	

    return MSF_ERROR_OK;
}

#if (MSF_DRIVER_[module]0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t [module]0_Initialize (MSF_[module]_Event_t pEvent) 
{	
	return [module]_Initialize(pEvent, &[module]0_Resources); 		
}
#endif



/**  
  @brief       Uninitialize the module 
  @param[in]   adc    Pointer to module resources 
  @return      error code (0 = OK)
  @note        Common function called by instance-specific function.
*/
static uint32_t  [module]_Uninitialize( [module]_RESOURCES* mod)
{
	uint32_t irq;
    /* Reset internal state for this instance of the TPM driver */
    mod->info->cb_event = null;    
    mod->info->status = 0;
    return MSF_ERROR_OK;
}

#if (MSF_DRIVER_[module]0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t [module]0_Uninitialize (void) 
{
  return [module]_Uninitialize(&[module]0_Resources);
}
#endif


/**
  @brief       Change power mode for the module
  @param[in]   state  The requested power state
  @param[in]   tpm    Pointer to TPM resources
  @return      error code (0 = OK)
  @note        Common function called by instance-specific function.
                Currently does nothing!                
*/
static uint32_t [module]_PowerControl(MSF_power_state state, [module]_RESOURCES* tpm)
{
    return MSF_ERROR_NOTSUPPORTED;
} 

#if (MSF_DRIVER_[module]0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t [module]0_PowerControl(MSF_power_state state) 
{
  return [module]_PowerControl(state, &[module]0_Resources);
}
#endif    /* MSF_DRIVER_[module]0 */

/**
  @brief       Control various options of the module
  @param[in]   control  Flags indicating which parameter(s) to set
  @param[in]   arg Optional argument for the command
  @param[in]   tpm    Pointer to module resources
  @return      error code (0 = OK)
  @note        Common function called by instance-specific function.
            
*/
static uint32_t [module]_Control(uint32_t control, uint32_t arg, [module]_RESOURCES* mod)
{	
/* EXAMPLE */
#if 0
	/* Changing clock option: disabled/internal/external */
	if ( control & MSF_TPM_CLOCK_MODE_Mask )
	{
		val = ((control & MSF_TPM_CLOCK_MODE_Mask) >> MSF_TPM_CLOCK_MODE_Pos);
		tpm->reg->SC &= ~TPM_SC_CMOD_MASK;
		/* wait until the counter is really disabled (change is "acknowledged in the timer clock domain" :) )*/
		while (tpm->reg->SC & TPM_SC_CMOD_MASK ) ;
		tpm->reg->SC |= TPM_SC_CMOD(val-1);				
	}
#endif
		
	
    return MSF_ERROR_OK;
}

#if (MSF_DRIVER_[module]0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t [module]0_Control(uint32_t control, uint32_t arg) 
{
  return [module]_Control(control, arg, &[module]0_Resources);
}
#endif   /* MSF_DRIVER_[module]0 */


/

/** Interrupt handler for all [module] instances */
void [module]_IRQHandler([module]_RESOURCES* mod)
{

}


/* Interrupt handler for [module]0 */
#if (MSF_DRIVER_[module]0) 

/* TODO: Specify the name of the IRQ as defined by the vendor-provided code
void [module]0_IRQHandler(void)
{
	[module]_IRQHandler(&[module]0_Resources);
}
*/
#endif /* MSF_DRIVER_[module]0 */


/* Access structure for [module]0 */
#if (MSF_DRIVER_[module]0)
/* Only if the user-configuration in msf_config_<device>.h specifies that
 * it needs this driver we will create the instance */
MSF_DRIVER_[module] Driver_[module]0 = {
  [module]0_Initialize,
  [module]0_Uninitialize,
  [module]0_PowerControl,
  [module]0_Control,  
  /* TODO: add other functions */
};
#endif	/* MSF_DRIVER_[module]0 */

/**@}*/

/***************************** Internal functions ***************************** */

/* ----------- end of file -------------- */
