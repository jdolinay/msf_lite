/****************************************************************************
 * @file     wavein.h
 * @brief    Driver for measuring input signal
 * @version  1
 * @date     5. Feb. 2015
 *
 * @note        
 *
 ******************************************************************************/
#ifndef MSF_WAVEIN_DRV_H
    #define MSF_WAVEIN_DRV_H

/** @defgroup wavein_driver wavein Driver
 * @{
 * @brief High-level driver for measuring iunput signals
 * @details
 * The driver has several input channels, depending on the timer driver
 * you configure in WAVEIN_TIMER_DRIVER.
 * The MCU pins corresponding to each channel depends on configuration of
 * the underlying timer driver, see msf_config.h.
 * Map of this driver's channels to TPM timer channels:
 * waveint channel  timer channel   default pin (msf_config.h)
 * <b>TPM0:</b><br>
 * 0				TPM0 channel 0	D0  (Arduino 10)<br>
 * 1				TPM0 channel 1	A4	(Arduino 4 )<br>
 * 2				TPM0 channel 2	A5	(Arduino 5)<br>
 * 3				TPM0 channel 3	D3	(Arduino 12)<br>
 * 4				TPM0 channel 4	D4	(Arduino 2)<br>
 * 5				TPM0 channel 5	C9	(Arduino 7)<br>
 *
 * <b>TPM1:</b><br>
 * 0				TPM1 channel 0  A12	(Arduino 3)<br>
 * 1				TPM1 channel 1  A13	(Arduino 8)<br>
 *
 * <b>TPM2:</b><br>
 * 0				TPM2 channel 0  B2	(Arduino 16 (A2))<br>
 * 1				TPM2 channel 1  B3	(Arduino 17 (A3))<br>
 *
 * <b>Howto use the driver</b><br>
 * 1) Initialize: wavein_init();<br>
 * 2) Attach the channel(s) you want to use: wavein_channel_attach(channel);<br>
 * 3) Start measuring on given channel(s): wavein_channel_start(channel);<br>
 *  OR<br>
 *    Wait for a pulse on single channel: wavein_channel_pulse_wait(channel, timeout_ms);<br>
 * 4) In the continuous measurement mode started by wavein_channel_start() you can<br>
 *    read the measured values using: wavein_channel_read(half-wave_A, half-wave_B);<br>
 *
 *  The driver will configure the timer to run at 1 MHz clock so that the time values
 *  are in microseconds.
 *
 */
 
/* Platform-specific timer driver */
#include "drv_tpm.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Defines section
* Add all public #defines here
*
***************************************************/

/** @defgroup group_wavein_error_codes Error codes for the wavein driver
 @{*/
#define	WAVEIN_NO_ERROR					(MSF_ERROR_OK)		/**< no error, success. */
#define	WAVEIN_ERROR_INVALID_CHANNEL	(MSF_ERROR_LAST+1)	/**< given channel number is not valid (too high) */
#define	WAVEIN_ERROR_NOINPUT			(MSF_ERROR_LAST+2)	/**< no input on this channel detected so far */
#define	WAVEIN_DISABLED_CHANNEL			(MSF_ERROR_LAST+3)	/**< channel is disabled */
#define	WAVEIN_TIMEOUT					(MSF_ERROR_LAST+4)	/**< timeout occurred while waiting for edge */
/** @}*/


/** Low-level timer driver to be used by the wavein driver.
 * This is defined in msf_config.h. Just in case it is not
 * defined, define it here defaulting to TPM0.
 * Note that the code assumes direct mapping of channels: channel 0 in this
 * driver is always channel 0 in underlying TPM driver.
 * Also note that only on timer driver at a time can be used. It would be
 * pretty easy to support all the drivers at the same time, but not done so far...
 * In general, the best choice may be timer TPM0, which has 6 channels. The other
 * TPM timers only have 2 channels. Use them if you need to use pin(s) which are not
 * connected to TPM0 and/or if you do not need more than 2 inputs and can use the TPM0
 * for other purposes.
 *
 * */
#ifndef WAVEIN_TIMER_DRIVER
	#define	WAVEIN_TIMER_DRIVER	Driver_TPM0
#endif
//#define	WAVEIN_TIMER_DRIVER	Driver_TPM1
//#define	WAVEIN_TIMER_DRIVER	Driver_TPM2

/* Number of channels available in the driver*/
#if	WAVEIN_TIMER_DRIVER == Driver_TPM0
	#define	WAVEIN_NUM_CHANNELS		(6)
#elif WAVEIN_TIMER_DRIVER == Driver_TPM1
	#define	WAVEIN_NUM_CHANNELS		(2)
#elif WAVEIN_TIMER_DRIVER == Driver_TPM2
	#define	WAVEIN_NUM_CHANNELS		(2)
#else
	#error Selected timer driver is not supported by wavein driver
#endif


/* WMSF_WAVEIN_PRESCALER
 * Auto-select the prescaler for wavein_init to achieve 1 MHz timer clock
 * The clock setup for TPM drivers depends on MSF_TPM_CLKSEL in msf_mkl25z.h
 * Timer clock for given F_CPU is:
 * F_CPU				TPM clock
 * 48 MHz 		 		8 MHz
 * 20.9 MHz				20.9 MHz
 * 8 MHz 				8 MHz
 * 4 MHz,F_BUS=0.8 MHz	4 MHz
 * 4 MHZ,F_BUS=1 MHz	8 MHz
 * 41.9 MHz				41943040 Hz
 *
 * WAVEIN_OVERFLOW_UNITMS
 * Define how many milliseconds it takes before the counter overflows.
 * The counter is 16-bit.
 * For timer clock 1 MHz it overflows about 15 times per second.
 * The time between overflows is about 65.5 ms
 * For timer clock 1.311 MHz overflows 20times per second; time
 * between overflows is 50 ms.
 *
 * WAVEIN_MULT_FACTOR
 * WAVEIN_DIV_FACTOR
 * Also define the factor to account for different clock speed than 1 MHz.
 * The resulting time is: time = counter * WAVEIN_MULT_FACTOR/WAVEIN_DIV_FACTOR;
 * */
#if F_CPU == 48000000
    #define WMSF_WAVEIN_PRESCALER      MSF_TPM_PRESCALER_8
	#define	WAVEIN_OVERFLOW_UNITMS	(66)
	#define	WAVEIN_MULT_FACTOR		(1)
	#define	WAVEIN_DIV_FACTOR		(1)

#elif ((F_CPU == 20900000) || (F_CPU == 20970000))
    #define WMSF_WAVEIN_PRESCALER      MSF_TPM_PRESCALER_16		/* timer clock 1310625 Hz! */
	#define	WAVEIN_OVERFLOW_UNITMS	(50)
	#define	WAVEIN_MULT_FACTOR		(76)
	#define	WAVEIN_DIV_FACTOR		(100)

#elif  F_CPU == 8000000
    #define WMSF_WAVEIN_PRESCALER      MSF_TPM_PRESCALER_8
	#define	WAVEIN_OVERFLOW_UNITMS	(66)
	#define	WAVEIN_MULT_FACTOR		(1)
	#define	WAVEIN_DIV_FACTOR		(1)

#elif  F_CPU == 4000000
	#if F_BUS == 800000
    	#define WMSF_WAVEIN_PRESCALER      MSF_TPM_PRESCALER_4
	#else
		#define WMSF_WAVEIN_PRESCALER      MSF_TPM_PRESCALER_8
	#endif

	#define	WAVEIN_OVERFLOW_UNITMS	(66)
	#define	WAVEIN_MULT_FACTOR		(1)
	#define	WAVEIN_DIV_FACTOR		(1)

#elif F_CPU == 41943040
    #define WMSF_WAVEIN_PRESCALER     MSF_TPM_PRESCALER_32	/* timer clock 1310720 */
	#define	WAVEIN_OVERFLOW_UNITMS	(50)
	#define	WAVEIN_MULT_FACTOR		(76)
	#define	WAVEIN_DIV_FACTOR		(100)
#else
	#error The CPU clock defined by F_CPU is not supported by wavein driver.
#endif



/* Function Prototype Section
* Add prototypes for all public functions.
* Write doxygen comments here!
*
***************************************************/ 
/**
 * @brief initialize the driver.
 * @note
 *
 **/
void wavein_init(void);

/**
 * @brief un-initialize the driver.
 * @note
 *
 **/
void wavein_uninit(void);

/** 
 * @brief Connect given channel to the driver.
 * @param channel the channel to attach.
 * @return WAVEIN_NO_ERROR (0) if OK; error code otherwise, see wavein.h
 * @note  This configures the pin for underlying timer channel into timer mode.
 *  
 */
uint8_t wavein_channel_attach(uint8_t channel);

/**
 * @brief Disconnect given channel from the driver.
 * @param channel the channel to detach.
 * @note This disconnects the pin from the timer.
 *
 */
void wavein_channel_detach(uint8_t channel);

/**
 * @brief Start capturing the input for given channel.
 * @param channel the channel to capture.
 * @note  This starts updating the values for the given channel.
 *
 */
uint8_t wavein_channel_start(uint8_t channel);

/**
 * @brief Stop capturing the input for given channel.
 * @param channel the channel to stop capturing.
 * @return WAVEIN_NO_ERROR (0) if OK; error code otherwise, see wavein.h
 * @note  This stops updating the values for the given channel. Use this when the data
 *  from the channel are needed for some time (but will be needed later, so it is not
 *  suitable to detach the channel.
 *  It will reset the internal data for this channel.
 *
 */
void wavein_channel_stop(uint8_t channel);

/**
 * @brief Read the values captured for given channel
 * @param channel the channel for which we want to read
 * @param pulseA pointer to user-provided variable which receives the length
 * of the 1st part of the wave (the "pulse"). In microseconds (us).
 * @param pulseB pointer to user-provided variable which receives the length
 * of the 2nd part of the wave (space after the pulse)
 * @return WAVEIN_NO_ERROR (0) if OK; error code otherwise, see wavein.h
 * @note Do not assume that both parts A and B must be valid. Invalid (unavailable) parts
 * will be set to 0.
 * Also do not assume any particular order of the parts. For example, if measuring RC PPM signal,
 * the pulse may be in part A or part B depending on when the measurement started (or
 * when the input signal was connected).
 *
 */
uint8_t wavein_channel_read(uint8_t channel, uint16_t* pulseA, uint16_t* pulseB  );

/**
 * @brief Wait for a pulse (2 edges) on given channel.
 * @param channel the channel for which we want to wait.
 * @param timeout maximum time in milliseconds to wait. If 0 is specified, wait infinitely.
 *  Note that the timeout measurement is "rough", about 66 ms for 1 MHZ timer clock,
 *  see WAVEIN_OVERFLOW_UNITMS.
 * @return the length of the pulse or 0 if the timeout occurs.
 * @note This function can be used to measure single pulse on input, for example, in case of the
 * Ping ultrasonic sensor we sent a pulse and then wait for a pulse on the same pin.
 * It will reset the internal data buffers and then wait for 2 edges.
 * Must be called before the 1st edge occurs on the input pin!
 *
 **/
uint16_t wavein_channel_pulse_wait(uint8_t channel, uint32_t timeout );



#ifdef __cplusplus
}
#endif

/**@}*/
/* ----------- end of file -------------- */
#endif /* MSF_WAVEIN_DRV_H */
