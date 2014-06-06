/****************************************************************************
 * @file     msf_analog.c
 * @brief    Analog read - MSF global functions 
 * @version  1
 * @date     28. May 2014
 *
 * @note Prototypes of these functions are in msf.h
 *      Uses msf ADC driver. The driver is initialized in msf_init() if
 *      proper option is specified in msf_config.h  
 *
 ******************************************************************************/

#include "msf_config.h"                    

/* Include hardware definitions */
#include "coredef.h"
              
#include "msf.h"
#include "drv_adc.h"  /* ADC driver */

/* Define which driver (instance of the ADC) is used by us */
#define	 MSF_ANALOG_DRIVER	Driver_ADC0

/** Read analog value from given analog pin
* @param apin analog pin
 * @note The analog pins are defined in msf_<device>.h. In future this definition
 * may allow mapping the pin to ADC module number but currently we support only
 * one ADC module for analog inputs.
 * Note: Not all analog input pins are available in every configuration of the ADC.
 * The function will use the ADC driver Control function to switch between ADC "a" and "b" channels
 * to get the reading from the requested pin. This will slow down the reading a little if
 * you use pins which require this switching (SExa or SExb only). All SEx pins (without a or b at the end)
 * do not require any switching. 
 * See the Analog_pin_t enum in msf_<device>.h for the analog pin definitions   
 **/
#if (MSF_USE_ANALOG)
 uint16_t msf_analog_read(Analog_pin_t apin)
 {
    /* future: use MSF_PIN2ADC() macro to obtain ADC module number (for MCUs 
    with more than 1 ADC module) and use the apropriate adc driver. */

	 /* Simple version which does not check if the channel is actually available */
#if 0
    /* Obtain ADC channel number from the Analog pin code*/
	 MSF_ANALOG_DRIVER.SetChannel(MSF_PIN2CHANNEL(apin)); 
    return MSF_ANALOG_DRIVER.Read();
#endif    
    
#if (MSF_DRIVER_ADC0)    
    /* Version which sets the proper channel mux in the ADC driver as needed */
    if ( ((uint32_t)apin & MSF_ADC_BOTH) != MSF_ADC_BOTH )
    {
    	/* If the requested channel is not available in both A and B channels,
    	 * we have to select the A or B channels first. As the driver does not 
    	 * support querying its current configuration, we always have to set A or B */
    	if ( MSF_ADC_ISA_CHANNEL(apin) )
    		MSF_ANALOG_DRIVER.Control(MSF_ADC_ABSEL_A, 0);
    	else
    		MSF_ANALOG_DRIVER.Control(MSF_ADC_ABSEL_B, 0);    	    
    }
    
    MSF_ANALOG_DRIVER.SetChannel(MSF_PIN2CHANNEL(apin)); 	/* Select the channel */
    return MSF_ANALOG_DRIVER.Read();		/* Read the value from this channel */
#else
	#warning	Analog input driver Driver_ADC0 is not enabled in msf_config.h; msf_analog_read will not work.
    return 0;
#endif
 }
 
#endif
