/****************************************************************************
 * @file     drv_[module].h
 * @brief    CMSIS-like driver for [module] for [platform]
 * @version  1
 * @date     7. July 2014
 *
 * @note     This is driver in CMSIS style, but not exactly according to CMSIS 
 * definition even if such definition exists in CMSIS     
 *   
 *
 ******************************************************************************/
#ifndef MSF_DRIVER_TEMPLATE_H
    #define MSF_DRIVER_TEMPLATE_H
/** @defgroup group_DRIVER_template driver name
 * @brief template file for device driver
 * @{
 */
 
/* Make sure the base definitions are available */
#ifndef MSF_MSF_H
	#error Plese include msf.h before including this file. 
#endif


#ifdef __cplusplus
extern "C" {
#endif

/** Version of this drivers API */
#define     MSF_[MODULE]_VERSION    (1)

/* Pointer to call back function for reporting events from the driver to
 * client application. Set in Initialize function */
typedef void (*MSF_[MODULE]_Event_t) (uint32_t event, uint32_t arg);

/* The priority of the module interrupts; lower number means higher priority.
 * For KL25Z valid value is 0 thru 3 */
#define		MSF_[MODULE]_INT_PRIORITY	(2)

/* Flags (operations and parameters) for the Control function */
/* Positions and meaning of the bit-fields:
 EXAMPLE:
 Bit(s)  Meaning
 0:1	clock source: 0=do not change settings; 1=disable counter, 2=internal clock, 3=external clock
 2		prescaler: 0=no change; 1=set according to arg value (arg=0-7 meaning prescaler 1-128 in powers of 2)
 */
 
/* Defines for the positions and masks of the flags */
/*  EXAMPLE
#define		MSF_TPM_CLOCK_MODE_Pos	(0)
#define		MSF_TPM_CLOCK_MODE_Mask	(0x03)
#define		MSF_TPM_PRESCALER_Pos	(2)
#define		MSF_TPM_PRESCALER_Mask	(0x04)
*/

/* Definitions of the flags for Control function  */
/* EXAMPLE */
#if 0
#define     MSF_TPM_CLOCK_NONE  	(1UL << MSF_TPM_CLOCK_MODE_Pos)  /**< disable counter */
#define     MSF_TPM_CLOCK_INTERNAL  (2UL << MSF_TPM_CLOCK_MODE_Pos)  /**< use internal clock (see SIM->SOPT2) */
#define     MSF_TPM_CLOCK_EXTERNAL  (3UL << MSF_TPM_CLOCK_MODE_Pos)  /**< use external clock LPTPM_EXTCLK */
#define     MSF_TPM_PRESCALER_SET  	(1UL << MSF_TPM_PRESCALER_Pos)  /**< set prescaler; arg=one of the constants MSF_TPM_PRESCALER_1,... */
#endif


/** [Module] events (masks) 
 * The driver will generate these events (if enabled)
 * The user defined function MSF_[module]_Event_t will get the mask in event parameter.
 */
/* EXAMPLE */
#if 0
#define		MSF_TPM_EVENT_TOF		(1UL << 0)	/**< Timer overflow (TOF) interrupt.  */
#endif

/** The driver status flags (configuration), stored in [module]_INFO for each instance */
												
/**
\brief Access structure of the [module] Driver.
*/
typedef struct _MSF_DRIVER_TPM {  
  uint32_t      (*Initialize)   (MSF_TPM_Event_t cb_event);  
  uint32_t      (*Uninitialize) (void);                       
  uint32_t      (*PowerControl) (MSF_power_state state); 
  uint32_t      (*Control)      (uint32_t control, uint32_t arg);
  uint32_t		(*TODO)(uint32_t args);  
              
} const MSF_DRIVER_TPM;

/* Declare the instance(s) of the driver(s). 
 * Placed here rather than in the device-specific implementation
 * file ([module]_k25.h) because then the client program would need to include 
 * different files for different derivatives... 
 * ... which is not good for generic drivers such as Conio.
 * 
 * The #if constant is defined in application configuration file so that the
 * user can decide which drivers will be used (and allocated in memory).    
 */        
#if (MSF_DRIVER_[module]0)
	extern MSF_DRIVER_[module] Driver_[module]0;
#endif	/* MSF_DRIVER_[module]0 */


#ifdef __cplusplus
}
#endif
/**@}*/
/* ----------- end of file -------------- */
#endif /* MSF_DRIVER_TPM_H */
