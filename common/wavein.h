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

/** @defgroup wavein Driver wavein_driver
 * @details Driver for measuring input signal.
 * The driver has several input channels, depending on the timer driver
 * you configure in WAVEIN_TIMER_DRIVER.
 * The MCU pins corresponding to each channel depends on configuration of
 * the underlying timer driver, see msf_config.h.
 * Map of this driver's channels to TPM timer channels:
 * waveint channel  timer channel   default pin (msf_config.h)
 * TPM0:
 * 0				TPM0 channel 0	D0  (Arduino 10)
 * 1				TPM0 channel 1	A4	(Arduino 4 )
 * 2				TPM0 channel 2	A5	(Arduino 5)
 * 3				TPM0 channel 3	D3	(Arduino 12)
 * 4				TPM0 channel 4	D4	(Arduino 2)
 * 5				TPM0 channel 5	C9	(Arduino 7)
 *
 * TPM1:
 * 0				TPM1 channel 0  A12	(Arduino 3)
 * 1				TPM1 channel 1  A13	(Arduino 8)
 *
 * TPM2:
 * 0				TPM2 channel 0  B2	(Arduino 16 (A2))
 * 1				TPM2 channel 1  B3	(Arduino 17 (A3))

 * @{
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
/* Error codes for the wavein driver */
#define	WAVEIN_NO_ERROR					(0)	/*> no error, success. */
#define	WAVEIN_ERROR_INVALID_CHANNEL	(1)	/*> given channel number is not valid (too high) */
#define	WAVEIN_ERROR_NOINPUT			(2)	/*> no input on this channel detected so far */
#define	WAVEIN_DISABLED_CHANNEL			(3)	/*> channel is disabled */
#define	WAVEIN_TIMEOUT					(4)	/*> timeout occured while waiting for edge */

/* TODO: move this to project-configuration file  */
/** Low-level timer driver(s) to use by this driver.
 * Note that the code assumes direct mapping of channels: channel 0 in this
 * driver is always channel 0 in underlying TPM driver.
 * Also note that only on timer driver at a time can be used. It would be
 * pretty easy to support all the drivers at the same time, but not done so far...
 * In general, the best choice may be timer TPM0, which has 6 channels. The other
 * TPM timers only have 2 channels.
 *
 * */
#define	WAVEIN_TIMER_DRIVER	Driver_TPM0
//#define	WAVEIN_TIMER_DRIVER	Driver_TPM1
//#define	WAVEIN_TIMER_DRIVER	Driver_TPM2


/* end TODO */


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
 **/
uint8_t wavein_channel_attach(uint8_t channel);

/**
 * @brief Short description of the function
 * @param channel the channel to attach.
 * @note This disconnects the pin from the timer.
 *
 **/
void wavein_channel_detach(uint8_t channel);

/**
 * @brief Start capturing the input for given channel.
 * @param channel the channel to capture.
 * @note  This starts updating the values for the given channel
 *
 **/
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
 **/
void wavein_channel_stop(uint8_t channel);

/**
 * @brief Read the values captured for given channel
 * @param channel the channel for which we want to read
 * @param pulseA the 1st part of the wave (the "pulse")
 * @param pulseB 2nd part of the wave (space after the pulse)
 * @return WAVEIN_NO_ERROR (0) if OK; error code otherwise, see wavein.h
 * @note Do not assume that both parts A and B must be valid - invalid (unavailable) parts
 * will be set to 0.
 * Also do not assume any particular order of the parts. For example, if measuring RC PPM signal,
 * the pulse may be in part A or part B depending on when the measurement started (or
 * when the input signal was connected).
 *
 **/
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
 * Must be called before the 1st pulse occurs on the input pin!
 *
 **/
uint16_t wavein_channel_pulse_wait(uint8_t channel, uint32_t timeout );



#ifdef __cplusplus
}
#endif

/**@}*/
/* ----------- end of file -------------- */
#endif /* MSF_WAVEIN_DRV_H */
