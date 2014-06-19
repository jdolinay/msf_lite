/****************************************************************************
 * @file     drv_tpm.h
 * @brief    CMSIS-like driver for TPM timer for kinetis
 * @version  1
 * @date     17. June 2014
 *
 * @note     This is driver in CMSIS style, but CMSIS does not define timer driver.
 *   Should be used as a standard for MSF drivers.    
 *   
 *   The TPM timer has up to 6 channels which can be configured independently to 
 *   input capture/output compare/pwm mode. Of course they are not too independent
 *   as they share the same counter and other timer settings.
 *
 ******************************************************************************/
#ifndef MSF_DRIVER_TPM_H
#define MSF_DRIVER_TPM_H

/* Make sure the base definitions are available */
#ifndef MSF_MSF_H
	#error Plese include msf.h before including this file. 
#endif


#ifdef __cplusplus
extern "C" {
#endif

/** Version of this drivers API */
#define     MSF_TPM_VERSION    (1)

/** Pointer to call back function for reporting events from the driver to
 * client application. Set in Initialize function */
typedef void (*MSF_TPM_Event_t) (uint32_t event, uint32_t arg);

/* The priority of the TPM interrupts; lower number means higher priority.
 * For KL25Z valid value is 0 thru 3 */
#define		MSF_TPM_INT_PRIORITY	(2)

/** Flags (operations and parameters) for the Control function */
/* Positions and meaning of the bit-fields:
 Bit(s)  Meaning
 0:1	clock source: 0=do not change settings; 1=disable counter, 2=internal clock, 3=external clock
 2		prescaler: 0=no change; 1=set according to arg value (arg=0-7 meaning prescaler 1-128 in powers of 2)
 3:4	signal TOF event: 0=no change; 1 =do not signal TOF; 2=signal TOF (timer overflow)
 5		set modulo (TOP) value: 0=no change; 1=set acording to arg
   
 
  	
 */
/* Defines for these positions*/
#define		MSF_TPM_CLOCK_MODE_Pos	(0)
#define		MSF_TPM_CLOCK_MODE_Mask	(0x03)
#define		MSF_TPM_PRESCALER_Pos	(2)
#define		MSF_TPM_PRESCALER_Mask	(0x04)
#define		MSF_TPM_TOF_Pos			(3)
#define		MSF_TPM_TOF_Mask		(0x18)
#define		MSF_TPM_MOD_Pos			(5)
#define		MSF_TPM_MOD_Mask		(0x20)

/* Definitions of the flags for Control */
#define     MSF_TPM_CLOCK_NONE  	(1UL << MSF_TPM_CLOCK_MODE_Pos)  /**< disable counter */
#define     MSF_TPM_CLOCK_INTERNAL  (2UL << MSF_TPM_CLOCK_MODE_Pos)  /**< use internal clock (see SIM->SOPT2) */
#define     MSF_TPM_CLOCK_EXTERNAL  (3UL << MSF_TPM_CLOCK_MODE_Pos)  /**< use external clock LPTPM_EXTCLK */
#define     MSF_TPM_PRESCALER_SET  	(1UL << MSF_TPM_PRESCALER_Pos)  /**< set prescaler; arg=one of the constants MSF_TPM_PRESCALER_1,... */
#define		MSF_TPM_TOF_NO_SIGNAL	(1UL << MSF_TPM_TOF_Pos)  		/**< do not signal timer overflow event to user */
#define		MSF_TPM_TOF_SIGNAL		(2UL << MSF_TPM_TOF_Pos)  		/**< signal timer overflow event to user */
#define		MSF_TPM_TOP_VALUE		(1UL << MSF_TPM_MOD_Pos)  		/**< set top value for counter (MOD register); arg = the value (16-bit) */

/** Helper values for Arg parameter in Control() */
#define		MSF_TPM_PRESCALER_1		(0)
#define		MSF_TPM_PRESCALER_2		(1)
#define		MSF_TPM_PRESCALER_4		(2)
#define		MSF_TPM_PRESCALER_8		(3)
#define		MSF_TPM_PRESCALER_16	(4)
#define		MSF_TPM_PRESCALER_32	(5)
#define		MSF_TPM_PRESCALER_64	(6)
#define		MSF_TPM_PRESCALER_128	(7)


/** TPM events (masks) 
 * The driver will generate these events (if enabled)
 * The user defined function MSF_TPM_Event_t will get the mask in event parameter */
#define		MSF_TPM_EVENT_TOF		(1UL << 0)	/**< Timer overflow (TOF) interrupt.  */
#define		MSF_TPM_EVENT_CHN(n)	(1UL << (n+1))	/**< Timer channel N event.  */
/* For user convenience...*/
#define		MSF_TPM_EVENT_CH0		MSF_TPM_EVENT_CHN(0) /**< Timer channel 0 event.  */
#define		MSF_TPM_EVENT_CH1		MSF_TPM_EVENT_CHN(1) /**< Timer channel 1 event.  */
#define		MSF_TPM_EVENT_CH2		MSF_TPM_EVENT_CHN(2) /**< Timer channel 2 event.  */
#define		MSF_TPM_EVENT_CH3		MSF_TPM_EVENT_CHN(3) /**< Timer channel 3 event.  */
#define		MSF_TPM_EVENT_CH4		MSF_TPM_EVENT_CHN(4) /**< Timer channel 4 event.  */
#define		MSF_TPM_EVENT_CH5		MSF_TPM_EVENT_CHN(5) /**< Timer channel 5 event.  */

/** Options for the SetChannelMode parameter "args". */
#define		MSF_TPM_PARAM_CHANNEL_EVENT		(1UL << 0)	/**< enable the signalling the event for this channel (user callback) */


/** TPM driver status flags (configuration), stored in TPM_INFO for each instance */
#define		MSF_TPM_STATUS_SIGNAL_TOF		MSF_TPM_EVENT_TOF	/**< signal TOF event to user) */
#define		MSF_TPM_STATUS_SIGNAL_CHN(n)	(1UL << (n+1))		/**< signal event for channel n  */
/** Mask with 1s in all positions of possible channel events */
#define		MSF_TPM_STATUS_ANY_CHANNEL		(MSF_TPM_STATUS_SIGNAL_CHN(0) | \
											MSF_TPM_STATUS_SIGNAL_CHN(1) | \
											MSF_TPM_STATUS_SIGNAL_CHN(2) | \
											MSF_TPM_STATUS_SIGNAL_CHN(3) | \
											MSF_TPM_STATUS_SIGNAL_CHN(4) | \
											MSF_TPM_STATUS_SIGNAL_CHN(5) )
													



/** Modes for timer channel 
 * Note that the channels are not completely independent. They share the same counter
 * and settings. In PWM mode all channels must be either edge-aligned or center aligned.
 * If at least one channel is in Center aligned mode, all other channels must be in the same
 * mode or disabled, because the counter is counting up+down which is not compatible with any
 * other mode than center-aligned PWM.  */
typedef enum _MSF_DRIVER_CHANNEL_MODES {
	Disabled,
	SWcompare,
	InCapture_rising_edge,
	InCapture_falling_edge,
	InCapture_both_edges,
	OutCompate_toggle,
	OutCompare_clear,
	OutCompare_set,
	OutCompare_pulselow,
	OutCompare_pulsehigh,
	PWM_edge_hightrue,	/**< set output at the start of the period */
	PWM_edge_lowtrue,	/**< clear output at the start of the period */
	PWM_center_hightrue,
	PWM_center_lowtrue,
	
} TMP_channel_mode_t;

/**
\brief Access structure of the TPM Driver.
*/
typedef struct _MSF_DRIVER_TPM {  
  uint32_t      (*Initialize)   (MSF_TPM_Event_t cb_event);  
  uint32_t      (*Uninitialize) (void);                       
  uint32_t      (*PowerControl) (MSF_power_state state); 
  uint32_t      (*Control)      (uint32_t control, uint32_t arg);
  uint32_t		(*SetChannelMode)(uint32_t channel, TMP_channel_mode_t mode, uint32_t args);
  uint32_t		(*WriteChannel)(uint32_t channel, uint16_t value);
  uint32_t		(*ReadChannel)(uint32_t channel);
              
} const MSF_DRIVER_TPM;

/* Declare the instance(s) of the driver(s). 
 * There are 3 TPM timers on KL25Z
 * This is placed here rather than in the device-specific implementation
 * file (tpm_k25.h) because then the client program would need to include 
 * different files for different derivatives... 
 * ... which is not good for generic drivers such as Conio. 
 */        
#if (MSF_DRIVER_TPM0)
	extern MSF_DRIVER_TPM Driver_TPM0;
#endif	/* MSF_DRIVER_TPM0 */

#ifdef __cplusplus
}
#endif
/* ----------- end of file -------------- */
#endif /* MSF_DRIVER_TPM_H */
