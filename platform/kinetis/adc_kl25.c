/****************************************************************************
 * @file     adc_kl25.c
 * @brief    CMSIS-like ADC driver for kinetis KL25
 * @version  1
 * @date     23. May 2014
 *
 * @note     Drivers: Driver_ADC0 
 *
 ******************************************************************************/
/* MSF includes */
#include "msf_config.h"

/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"
#include "msf.h"

#include "adc_kl25.h"

/** @addtogroup group_adc  
 * @{ 
*/

#if (MSF_DRIVER_ADC0)    
/* Define the resource for each ADC available on the MCU */
/* runtime info for ADC0 */
static ADC_INFO ADC0_Info;    

/* UART0 Resources */
static ADC_RESOURCES ADC0_Resources = {
  ADC0,    /* ADC type object defined in CMSIS <device.h>*/
  &ADC0_Info
};

#endif /* MSF_DRIVER_ADC0 */


/* Prototypes */
static void adcx_init(ADC_RESOURCES* adc);	  
static uint32_t adcx_calibrate(ADC_RESOURCES* adc);

        
/* The driver API functions */

/**
  \brief       Initialize ADC  
  \param[in]   event  Pointer to ADC_Event function or null
  \param[in]   adc       Pointer to ADC resources
  \return      error code (0 = OK)
  \note         Common function called by instance-specific function.
  	  Initializes ADC for single-conversion mode, busy-wait for conversion 
      completion, channel 0 selected.
*/
static uint32_t  ADC_Initialize( MSF_ADC_Event_t event,  ADC_RESOURCES* adc)
{
    adc->info->cb_event = event;	/* store pointer to user callback; not used for now */
    adc->info->channel = 0;
    
    /* Enable clock for ADC0 */
    /* Note: if modifying for MCUs with ADC1, need to update this code! */
    SIM->SCGC6 |= (SIM_SCGC6_ADC0_MASK );	
      
    /* init the given ADC */
    adcx_init(adc);
    
    /* calibrate the ADC */
    adcx_calibrate(adc);
 
    /* re-init the ADC. 
     Needed because the calibration changes some values */
    adcx_init(adc);
  

    return MSF_ERROR_OK;
}

#if (MSF_DRIVER_ADC0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t ADC0_Initialize (MSF_ADC_Event_t pEvent) 
{
  return ADC_Initialize(pEvent, &ADC0_Resources);
}
#endif

/* example: function for another instance of the UART
static int32_t UART1_Initialize (UART_Event_t pEvent) {
  return UART_Initialize(pEvent, &UART1_Resources);
}
}
*/

/**  
  \brief       Uninitialize ADC 
  \param[in]   adc    Pointer to adc resources 
  \return      error code (0 = OK)
  \note        Common function called by instance-specific function.
*/
static uint32_t  ADC_Uninitialize( ADC_RESOURCES* adc)
{
    /* TODO: reset internal state for this instance of the ADC */
    adc->info->cb_event = null;
    adc->info->channel = 0;
    return MSF_ERROR_OK;
}

#if (MSF_DRIVER_ADC0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t ADC0_Uninitialize (void) 
{
  return ADC_Uninitialize(&ADC0_Resources);
}
#endif


/**
  \brief       Initialize ADC Interface.
  \param[in]   state  The requested power state
  \param[in]   adc    Pointer to adc resources
  \return      error code (0 = OK)
  \note        Currently does nothing!
               Note that the ADC automatically enters low power mode after conversion, see datasheet.
*/
static uint32_t ADC_PowerControl(MSF_power_state state, ADC_RESOURCES* adc)
{
    return MSF_ERROR_NOTSUPPORTED;
} 

#if (MSF_DRIVER_ADC0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t ADC0_PowerControl(MSF_power_state state) 
{
  return ADC_PowerControl(state, &ADC0_Resources);
}
#endif

/**
  \brief       Control various options of the ADC
  \param[in]   control  Flags indicating which parameter(s) to set, see \ref group_adc_control_flags
  \param[in]   arg Optional argument for the command
  \param[in]   adc    Pointer to ADC resources
  \return      error code (0 = OK)
  \note        Common function called by instance-specific function.
            
*/
static uint32_t ADC_Control(uint32_t control, uint32_t arg, ADC_RESOURCES* adc)
{
	uint32_t tmp;
	
	/* Disable the ADC before changing parameters by writing all 1s to channel
	 * It will be re-enabled when Read is called.
	 */
	adc->reg->SC1[0] =  ADC_SC1_ADCH(AIN_ADC_DISALED);
	
	
	/* Single or continuous conversion  */
	if ( (control & MSF_ADC_CONV_Mask) != 0 )
	{
		if ( (control & MSF_ADC_CONV_Mask) == MSF_ADC_CONV_SINGLE )
		{
			/* single-shot mode */
			adc->reg->SC3 &= ~ADC_SC3_ADCO_MASK;		
		}
		else
		{	
			/* continuous mode */
			adc->reg->SC3 |= ADC_SC3_ADCO_MASK;
		}
	}
	
	/* Polled or interrupt mode */
	/* TODO: implement */
	
	
	/* Change resolution */	
	if ( (control & MSF_ADC_RES_Mask) != 0 )
	{
		tmp = ((control & MSF_ADC_RES_Mask) >> MSF_ADC_RES_Pos) - 1;	/* the value which means resolution */
		adc->reg->CFG1 &= ~ADC_CFG1_MODE_MASK;	/* clear the bits in register */
		adc->reg->CFG1 = ADC_CFG1_MODE(tmp);	/* now set the new resolution */
	}
	
	
	/* Set averaging option? */
	if ( (control & MSF_ADC_AVERAGE_Mask) != 0)
	{
		if ( (control & MSF_ADC_AVERAGE_Mask) != MSF_ADC_AVERAGE_NONE )
		{
			/* the bit field in control contains the value for register + 2 */
			tmp = ((control & MSF_ADC_AVERAGE_Mask) >> MSF_ADC_AVERAGE_Pos) - 2;
			/* Clear the current AVG value */
			adc->reg->SC3 &= ~ADC_SC3_AVGS_MASK;
			/* write new value and enable averaging */
			adc->reg->SC3 |= ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(tmp);
		}
		else
		{
			/* disable averaging */ 
			adc->reg->SC3 &= ~ADC_SC3_AVGE_MASK;
		}
	}
	
	
	/* Select ADC A or ADC B channels . 
	 * Some AIN pins are available only for A and some only for B. */
	if ( (control & MSF_ADC_ABSEL_Mask) != 0)
	{
		if ( (control & MSF_ADC_ABSEL_Mask) == MSF_ADC_ABSEL_A )
		{
			/* select ADC A */
			adc->reg->CFG2 &= ~ADC_CFG2_MUXSEL_MASK;
		}
		else
		{
			/* select ADC B */
			adc->reg->CFG2 |= ADC_CFG2_MUXSEL_MASK;
		}
	}	
	
	/* Select ADC reference 
	 * KL25Z uses VREFH (external pin) as default reference
	 * Alternate reference Valt is VDDA (analog power supply)*/
	if ( (control & MSF_ADC_REFSEL_Mask) != 0)
	{
		if ( (control & MSF_ADC_REFSEL_Mask) == MSF_ADC_REFSEL_DEFAULT )
		{
			/* select VREFH + VREFL pins */
			adc->reg->SC2 &= ~ADC_SC2_REFSEL_MASK;
		}
		else
		{
			/* select VALTH + VALTL pins */
			adc->reg->SC2 |= ADC_SC2_REFSEL(1);
		}
	}	
	
    return MSF_ERROR_OK;
}

#if (MSF_DRIVER_ADC0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t ADC0_Control(uint32_t control, uint32_t arg) 
{
  return ADC_Control(control, arg, &ADC0_Resources);
}
#endif


/**
  \brief       Read data from ADC; from current channel.  
  \param[in]   adc    Pointer to ADC resources
  \return      The value from ADC converter or 0xFFFFFFFF on error.
  \note        Common function called by instance-specific function.
            Only 16-bits in the return value or less are used depending on ADC configuration
*/
static uint32_t ADC_Read(ADC_RESOURCES* adc)
{	
	uint32_t tmp;
	/* Write the channel to SC1A (SC1[0] register to start a conversion 
	 * Note that SC1B cannot be used for SW triggered operation and write to 
	 * SC1B does not start new conversion! */
	if ( adc->info->channel < 31 )
	{
		tmp = adc->reg->SC1[0];
		tmp &= ~(ADC_SC1_ADCH_MASK << ADC_SC1_ADCH_SHIFT);	// set channel to 0, keep other values
		tmp |= ADC_SC1_ADCH(adc->info->channel);	// select the channel
		adc->reg->SC1[0] =  tmp;
		
		/* wait for conversion to complete */
		while ( !WMSF_ADCA_COMPLETE(adc->reg) ) ; 
		/* return the result */
		return adc->reg->R[0];	
	}
	
	/* Invalid channel (should never happen) */
	return MSF_ERROR_MAXDWORD;	
} 

#if (MSF_DRIVER_ADC0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t ADC0_Read(void) 
{
  return ADC_Read(&ADC0_Resources);
}
#endif

/**  
  \brief       Select the channel used for conversions by ADC_Read
  \param[in]   channel Number of the chanel to select. 
  \param[in]   adc    Pointer to ADC resources 
  \return      Error code; 0 = OK
  \note        Common function called by instance-specific function.
  	  	  	  The channel number must be the direct value which will be used in the ADC register.
  	  	  	  The caller must make sure the channel is valid, see enum in msf_<device>.h for 
  	  	  	  valid channel definitions.
*/
static uint32_t  ADC_SetChannel( uint32_t channel, ADC_RESOURCES* adc)
{     
	/* We check if the channel is valid but we do not check if the channel really exist..
	 * Not all of the 0 thru 30 channels are really available on the KL25Z. */
	/* The driver does not set the pins for analog input. This it default
  		function for the pin so this is in general not needed. If it is needed,
  		the user should set the appropriate pin.   */
	if ( channel < 31 )
	{
		/* just store the channel into our resources; */
		adc->info->channel = (uint8_t)channel;
		return MSF_ERROR_OK; 
	}
	
    return MSF_ERROR_ARGUMENT;
}

#if (MSF_DRIVER_ADC0)    
/* Instance specific function pointed-to from the driver access struct */
static uint32_t ADC0_SetChannel(uint32_t channel) 
{
  return ADC_SetChannel(channel, &ADC0_Resources);
}	
#endif


/* Access structure for ADC0 */
#if (MSF_DRIVER_ADC0)
/* Only if the user-configuration in msf_config_<device>.h specifies that
 * it needs this driver we will create the instance */
MSF_DRIVER_ADC Driver_ADC0 = {
  ADC0_Initialize,
  ADC0_Uninitialize,
  ADC0_PowerControl,
  ADC0_Control,  
  ADC0_Read,
  ADC0_SetChannel,    
};

#endif	/* MSF_DRIVER_ADC0 */

/*========================  Internal functions ========================== */
static void adcx_init(ADC_RESOURCES* adc)
{
	/* Note1: The SC1 and result registers may have multiple instances (A and B in case of KL25).
	 in the CMSIS definitions there is an array for these registers. We use A, i.e. SC1[0] */
	/* Note 2: Write to SC1A starts a conversion if the channel bit field is not all 1s.
	 * Write to SC1B does not start conversion (it must be hw triggered, not sw triggered)*/
	
	/* Disable interrupt and select channel 31 = ADC disabled, select single-ended mode */
	adc->reg->SC1[0] =  ADC_SC1_ADCH(AIN_ADC_DISALED);
	/*adc->reg->SC1 |= (ADC_SC1_AIEN_MASK << ADC_SC1_AIEN_SHIFT); */ /* enable interrupt */
	
	/* Select ADC clock, prescaler and resolution  */
	adc->reg->CFG1 = ADC_CFG1_ADICLK(WMSF_ADC_CLOCK) | ADC_CFG1_ADIV(WMSF_ADC_PRESCALER) 
			| ADC_CFG1_MODE(WMSF_ADC_RESOLUTION);
	
	/* ADC A or B, long sample time selection */
	adc->reg->CFG2 = 0;	/* default values: ADxxa selected, longest sample time 24 ADCK total */
	
	/* Select reference, triggering, compare mode */
	adc->reg->SC2 = 0;	/* default values: sw trigger, default reference=VREFH,VREFL pins, ... */
	
	/* Set averaging, continuous conversion,.... */
	if ( WMSF_ADC_AVERAGE != 0x0F )
		/* enable averaging and set desired samples to average */
		adc->reg->SC3 = ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(WMSF_ADC_AVERAGE);
	else
		adc->reg->SC3 = 0;	/* default values, no averaging */
}
  
/* Calibrate the ADC 
 * From FRDM-KL25Z sample code 
 * return 0 on success; 1 on error */
static uint32_t adcx_calibrate(ADC_RESOURCES* adc)
{
	 unsigned short cal_var;
	  
	  adc->reg->SC2 &= ~ADC_SC2_ADTRG_MASK;	/* Enable Software Conversion Trigger for Calibration Process */
	  adc->reg->SC3 &= ( ~ADC_SC3_ADCO_MASK & ~ADC_SC3_AVGS_MASK );    /* set single conversion, clear avgs bitfield for next writing */ 
	  //ADC_SC3_REG(adcmap) &= ( ~ADC_SC3_ADCO_MASK & ~ADC_SC3_AVGS_MASK ); 
	  adc->reg->SC3 |= ( ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(WMSF_ADC_AVERAGE) ); /* turn averaging ON and set desired value */
	  //ADC_SC3_REG(adcmap) |= ( ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(AVGS_32) );  
	  
	  adc->reg->SC3 |= ADC_SC3_CAL_MASK;      /* Start CAL */
	  //ADC_SC3_REG(adcmap) |= ADC_SC3_CAL_MASK ;      // Start CAL
	  
	  while ( !WMSF_ADCA_COMPLETE(adc->reg) ) ; /* Wait calibration end */
	  //while ( (ADC_SC1_REG(adcmap,A) & ADC_SC1_COCO_MASK ) == COCO_NOT ); // Wait calibration end
	  	
	  if ( (adc->reg->SC3 & ADC_SC3_CALF_MASK) != 0 )
		  return 1;
	  /*if ((ADC_SC3_REG(adcmap)& ADC_SC3_CALF_MASK) == CALF_FAIL )
	  {  
	   return(1);    // Check for Calibration fail error and return 
	  }*/
		  
	  // Calculate plus-side calibration
	  cal_var = 0;	  
	  cal_var =  adc->reg->CLP0;	//ADC_CLP0_REG(adcmap); 
	  cal_var += adc->reg->CLP1;	// ADC_CLP1_REG(adcmap);
	  cal_var += adc->reg->CLP2;	//ADC_CLP2_REG(adcmap);
	  cal_var += adc->reg->CLP3;	//ADC_CLP3_REG(adcmap);
	  cal_var += adc->reg->CLP4;	//ADC_CLP4_REG(adcmap);
	  cal_var += adc->reg->CLPS;	// ADC_CLPS_REG(adcmap);

	  cal_var = cal_var/2;
	  cal_var |= 0x8000; // Set MSB
	  adc->reg->PG = ADC_PG_PG(cal_var);
	  //ADC_PG_REG(adcmap) = ADC_PG_PG(cal_var);
	 

	  // Calculate minus-side calibration
	  cal_var = 0;
	  cal_var =  adc->reg->CLM0;	//ADC_CLM0_REG(adcmap); 
	  cal_var += adc->reg->CLM1;	//ADC_CLM1_REG(adcmap);
	  cal_var += adc->reg->CLM2;	//ADC_CLM2_REG(adcmap);
	  cal_var += adc->reg->CLM3;	//ADC_CLM3_REG(adcmap);
	  cal_var += adc->reg->CLM4;	//ADC_CLM4_REG(adcmap);
	  cal_var += adc->reg->CLMS;	//ADC_CLMS_REG(adcmap);

	  cal_var = cal_var/2;
	  cal_var |= 0x8000; // Set MSB
	  adc->reg->MG = ADC_MG_MG(cal_var); 
	  //ADC_MG_REG(adcmap) = ADC_MG_MG(cal_var); 
	  
	  adc->reg->SC3 &= ~ADC_SC3_CAL_MASK ;  
	  //ADC_SC3_REG(adcmap) &= ~ADC_SC3_CAL_MASK ; /* Clear CAL bit */

	  return 0;
}
    	
/**@}*/


/* ----------- end of file -------------- */
