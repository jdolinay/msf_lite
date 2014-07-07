/****************************************************************************
 * @file     [module]_[platform].h
 * @brief    CMSIS-like driver for TPM timer in kinetis K25
 * @version  1
 * @date     7. July 2014
 *
 * @note     Template file for CMSIS-like driver; the file specific to
 *          given platform, e.g. Kinetis KL25. 
 *
 ******************************************************************************/
#ifndef MSF_[module]_[platform]_H
    #define MSF_[module]_[platform]_H

/* Make sure basic definitions are available */
#ifndef MSF_MSF_H
	#error Plese include msf.h before including this file. 
#endif

/* Common definitions for this platform and this driver class */
#include "drv_[module].h"


#ifdef __cplusplus
extern "C" {
#endif


/* Driver Run-time information*/
typedef struct _[module]_INFO {
  MSF_[module]_Event_t cb_event;          // Event Callback
  uint32_t      status;               // Status flags 
       
} [module]_INFO;


/** Driver information about 1 pin  
*/
typedef struct _[module]_PIN {
	MCU_pin_t	pin_code;	/* the pin-code defined in msf_<device>.h*/
	uint8_t		alt_num; /* the number of alt function for the given pin, e.g. 0 for ALT0, etc. */
} [module]_PIN;

/* Driver information about the pins 
 * 
 */
typedef struct _[module]_PINS {
	[module]_PIN pin[2];
} [module]_PINS;



/* The data for one instance of the driver - the "resource" */
typedef struct {
        [module]_Type  	*reg;  		/* TPM peripheral register interface, CMSIS */        
        [module]_INFO	*info;   	/* Run-Time information */
        [module]_PINS	*pins;		/* Pins used for our channels*/        
} const [module]_RESOURCES;



#ifdef __cplusplus
}
#endif

/* ----------- end of file -------------- */
#endif /* MSF_[module]_[platform]_H */
