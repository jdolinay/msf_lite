/****************************************************************************
 * @file     waveout.h
 * @brief    Driver for generating square wave output signal
 * @version  1
 * @date     10. Feb. 2015
 *
 * @note        
 *
 ******************************************************************************/
#ifndef MSF_WAVEOUT_DRV_H
    #define MSF_WAVEOUT_DRV_H

/** @defgroup waveout_driver waveout Driver
 * @{
 * @brief High-level driver for generating output signals
 * @details
 * The driver has several output channels, depending on the timer driver
 * you configure in WAVEOUT_TIMER_DRIVER.
 * The MCU pins corresponding to each channel depends on configuration of
 * the underlying timer driver, see msf_config.h.
 * Map of this driver's channels to TPM timer channels:
 * waveout channel  timer channel   default pin (msf_config.h)
 * <b>TPM0:</b><br>
 * 0				TPM0 channel 0	D0  (Arduino 10)<br>
 * 1				TPM0 channel 1	A4	(Arduino 4 )<br>
 * 2				TPM0 channel 2	A5	(Arduino 5)<br>
 * 3				TPM0 channel 3	D3	(Arduino 12)<br>
 * 4				TPM0 channel 4	D4	(Arduino 2)<br>
 * 5				TPM0 channel 5	C9	(Arduino 7)<br>
 *
 * <b>TPM1:</b><br>
 * 6				TPM1 channel 0  A12	(Arduino 3)<br>
 * 7				TPM1 channel 1  A13	(Arduino 8)<br>
 *
 * <b>TPM2:</b><br>
 * 8				TPM2 channel 0  B2	(Arduino 16 (A2))<br>
 * 9				TPM2 channel 1  B3	(Arduino 17 (A3))<br>
 *
 * <b>Howto use the driver</b><br>
 * 1) Initialize: waveout_init();<br>
 * 2) Start generating on given channel(s): waveout_channel_start(channel, half-wave1, halfwave2);<br>
 * You specify the lengths of the half-waves in microseconds.<br>
 *
 * <b>Useful equations</b>
 * frequency_in_kHz = 1 000 / period_in_us<br>
 * period_in_us = 1 000 / frequency_in_kHz<br><br>
 *
 * <b>Important limits</b>
 * Maximum frequency of the signal if only 1 channel is used for 48 MHz CPU clock:<br>
 * Fmax = 35 kHz for channels 0 thru 5 (TPM0). <br>
 * Fmax = 45 kHz for channels 6 thru 9 (TPM1 and TPM2)<br>
 * If more channels are used, the maximum frequency is proportionally lower.<br>
 * For example, if 2 channels are used, the Fmax is half of the Fmax for single channel.
 * If 3 channels are used, Fmax is 1/3rd, etc.<br>
 * If lower CPU clock is used, the values are proportionally lower.<br><br>
 * Notes<br>
 * If multiple channels are used, it seems that the real Fmax can be little higher than
 * Fmax for single channel / number of channels.<br>
 * For example when using 5 channels, 0 thru 4, at the same frequency, the maximum frequency
 * obtained was about 10 kHz while theoretically it should be only 7 kHz (35/5).
 *
 * The lower Fmax for TPM0 is due to more channels available in TPM0 (6) which requires
 * more processing time in the ISR.<br>
 *
 * The waveout driver uses the timer in output compare mode with pin toggle on compare match.
 * It would be possible to achieve higher maximum frequencies using PWM mode of the timer,
 * but the PWM mode requires that the period of the signal is tied to the value at
 * which the timer counter overflows. So all the channels of one physical
 * timer (TPM driver) have to have the same frequency.
 *
 * Discussion about the performance<br>
 *
 * Limit by the resolution of the timer:
 * We enter the lengths of the half-waves in timer counter units, which are microseconds.
 * It probably makes little sense to use half-wave lengths smaller than say, 2. Lets assume
 * the shortest period of the signal is 5 us which means the frequency is 200 kHz, BUT this
 * is not reachable due to CPU performance. So the timer resolution is not limiting.
 *
 * Limit by CPU computing power:
 * Generating the signal requires 2 interrupts per period for each channel.
 * Assuming the ISR takes about 500 CPU cycles to execute (just an estimate), if the
 * CPU runs at 1 MHz, it can handle 2000 ISRs per second - leaving no time for the main program to execute.
 * We need 2 ISR executions per period of the signal, so we can obtain 1 KHz output for each MHz
 * of CPU speed.
 * For the max. CPU clock in KL25Z, 48 MHz, the max frequency we can generate is about 48 kHz.<br>
 * BUT THERE IS ALSO ANOTHER LIMIT:<br>
 * The ISR must execute in shorter time than it takes for the timer counter to reach
 * the new value (new output pin toggle). For example, if the requested half-wave lengths are 50 and 50,
 * the ISR needs to execute in less than 50 us.
 * Assuming ISR needs 500 CPU clocks, CPU running at 1 MHz will execute the ISR in 500 us.
 * For 48 MHz clock the ISR will execute in about 11 us, so the lowest half-wave value is 11.
 *
 * In general, to obtain shortest possible half-wave for 1 channel:
 * half_wave_MIN = ISR_clocks/CPU_clock_MHz  [result in us]
 * Experimental results show that the ISR takes in about 500 clock cycles, so
 * half_wave_MIN = 500/CPU_clock_MHz
 * Thus the shortest pulse we can generate with the maximum CPU speed 48 MHz is about 11 us.
 *
 *
 *  Note: the driver cannot share the underlying TPM driver with wavein because the interrupt handler
 *  can be only one (only in one driver).
 *
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

/** @defgroup group_waveout_error_codes Error codes for the wavein driver
 @{*/
#define	WAVEOUT_NO_ERROR					(MSF_ERROR_OK)		/**< no error, success. */
#define	WAVEOUT_ERROR_INVALID_CHANNEL	(MSF_ERROR_LAST+1)	/**< given channel number is not valid (too high) */
/** @}*/

/* Helper macro to create mask for channel number n */
#define		WAVEOUT_CH_MASK(n) ((uint32_t)1 << n)
/** Masks for waveout_init() to define which channels the user want to use.
 *  Based on this information, the waveout driver initializes the underlying
 *  TPM timer driver(s).
 * driver should use. For the KL25Z there are 3 timers available:
 *  TPM0 thru TPM2. The waveout driver can use any or all of
 *  them depending on which channels (pins) are needed. */
#define		WAVEOUT_RANGE_0_5	(WAVEOUT_CH_MASK(0) | WAVEOUT_CH_MASK(1) | WAVEOUT_CH_MASK(2) | WAVEOUT_CH_MASK(3) | WAVEOUT_CH_MASK(4) | WAVEOUT_CH_MASK(5) )
#define		WAVEOUT_RANGE_6_7	(WAVEOUT_CH_MASK(6) | WAVEOUT_CH_MASK(7))
#define		WAVEOUT_RANGE_8_9	(WAVEOUT_CH_MASK(8) | WAVEOUT_CH_MASK(9))


/** Low-level timer driver to be used by the waveout driver.
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
#ifndef WAVEOUT_TIMER_DRIVER
	#define	WAVEOUT_TIMER_DRIVER	Driver_TPM0
#endif
//#define	WAVEOUT_TIMER_DRIVER	Driver_TPM1
//#define	WAVEOUT_TIMER_DRIVER	Driver_TPM2

/* Number of channels available in the driver*/
#if	WAVEOUT_TIMER_DRIVER == Driver_TPM0
	#define	WAVEIN_NUM_CHANNELS		(6)
#elif WAVEOUT_TIMER_DRIVER == Driver_TPM1
	#define	WAVEIN_NUM_CHANNELS		(2)
#elif WAVEOUT_TIMER_DRIVER == Driver_TPM2
	#define	WAVEIN_NUM_CHANNELS		(2)
#else
	#error Selected timer driver is not supported by waveout driver
#endif


/* WMSF_WAVEOUT_PRESCALER
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
 * WAVEOUT_OVERFLOW_UNITMS
 * Define how many milliseconds it takes before the counter overflows.
 * The counter is 16-bit.
 * For timer clock 1 MHz it overflows about 15 times per second.
 * The time between overflows is about 65.5 ms
 * For timer clock 1.311 MHz overflows 20times per second; time
 * between overflows is 50 ms.
 *
 * WAVEOUT_MULT_FACTOR
 * WAVEOUT_DIV_FACTOR
 * Also define the factor to account for different clock speed than 1 MHz.
 * The resulting time is: time = counter * WAVEIN_MULT_FACTOR/WAVEIN_DIV_FACTOR;
 * */
#if F_CPU == 48000000
    #define WMSF_WAVEOUT_PRESCALER      MSF_TPM_PRESCALER_8
	#define	WAVEOUT_OVERFLOW_UNITMS	(66)
	#define	WAVEOUT_MULT_FACTOR		(1)
	#define	WAVEOUT_DIV_FACTOR		(1)

#elif ((F_CPU == 20900000) || (F_CPU == 20970000))
    #define WMSF_WAVEOUT_PRESCALER      MSF_TPM_PRESCALER_16		/* timer clock 1310625 Hz! */
	#define	WAVEOUT_OVERFLOW_UNITMS	(50)
	#define	WAVEOUT_MULT_FACTOR		(76)
	#define	WAVEOUT_DIV_FACTOR		(100)

#elif  F_CPU == 8000000
    #define WMSF_WAVEOUT_PRESCALER      MSF_TPM_PRESCALER_8
	#define	WAVEOUT_OVERFLOW_UNITMS	(66)
	#define	WAVEOUT_MULT_FACTOR		(1)
	#define	WAVEOUT_DIV_FACTOR		(1)

#elif  F_CPU == 4000000
	#if F_BUS == 800000
    	#define WMSF_WAVEOUT_PRESCALER      MSF_TPM_PRESCALER_4
	#else
		#define WMSF_WAVEOUT_PRESCALER      MSF_TPM_PRESCALER_8
	#endif

	#define	WAVEOUT_OVERFLOW_UNITMS	(66)
	#define	WAVEOUT_MULT_FACTOR		(1)
	#define	WAVEOUT_DIV_FACTOR		(1)

#elif F_CPU == 41943040
    #define WMSF_WAVEOUT_PRESCALER     MSF_TPM_PRESCALER_32	/* timer clock 1310720 */
	#define	WAVEOUT_OVERFLOW_UNITMS	(50)
	#define	WAVEOUT_MULT_FACTOR		(76)
	#define	WAVEOUT_DIV_FACTOR		(100)
#else
	#error The CPU clock defined by F_CPU is not supported by waveout driver.
#endif



/* Function Prototype Section
* Add prototypes for all public functions.
* Write doxygen comments here!
*
***************************************************/ 
/**
 * @brief initialize the driver.
 * @param channel_mask mask indicating which channels will be used.
 * For convenience, use the macros WAVEOUT_RANGE_0_5 etc.
 * You can also create the mask yourself: 1 in bit number N in the mask indicates that
 * the channel N will be used. For example, mask 0x01 means channel 0 will be used, 0x03 means
 * channels 0 and 1 will be used.
 * @note
 *
 **/
void waveout_init(uint32_t channel_mask);

/**
 * @brief un-initialize the driver.
 * @note
 *
 **/
void waveout_uninit(void);

/** 
 * @brief Connect given channel to the driver.
 * @param channel the channel to attach.
 * @return WAVEIN_NO_ERROR (0) if OK; error code otherwise, see wavein.h
 * @note  This configures the pin for underlying timer channel into timer mode.
 *  
 */
uint8_t waveout_channel_attach(uint8_t channel);

/**
 * @brief Disconnect given channel from the driver.
 * @param channel the channel to detach.
 * @note This disconnects the pin from the timer.
 *
 */
void waveout_channel_detach(uint8_t channel);

/**
 * @brief Start generating signal on given channel (pin).
 * @param channel the channel to generate the signal on.
 * @param half1 length of the 1st part of the wave in microseconds (us)
 * @param half2 length of the 2nd part of the wave in microseconds (us)
 * @return WAVEOUT_NO_ERROR (0) if OK, or WAVEOUT_ERROR_INVALID_CHANNEL if the channel
 * number is not valid.
 * @note  The values given to waveout_channel_start() are length of the half-waves
 *	of the signal we want to generate in microseconds (us).
 *	For example: 100 and 100 means wave length 200 us (signal period = 200 us)
 *	frequency_in_Hz = 1/period_in_second = 1 000 000 / period_in_us
 *	frequency_in_kHz = 1 000 / period_in_us
 *	period_in_us = 1 000 / frequency_in_kHz
 *	Example:
 *	We want to obtain 1 kHz signal: period = 1000/1 = 1000
 *	For square wave signal the half-wave values are 500 and 500.
 *	NOTE:
 *	Besides the maximum frequency described above, there is also limit to the shortest length
 *	of the half-wave in the signal. For example, it is OK to generate signal with f = 1 KHz
 *	with 50% duty (half-wave values 500 and 500). BUT it may not be OK to ask for
 *	1% duty in this signal (half-wave values 10 and 990)! The value 10 is to low and cannot be
 *	achieved.
 *	The shortest half-wave length is approximately:
 *	half_wave_MIN = 500/CPU_clock_MHz
 *	Examples:
 *	For 48 MHz CPU clock it is 11 (us)
 *	For 4 MHz CPU clock it is 125 (us)
 *
 *
 */
uint8_t waveout_channel_start(uint8_t channel, uint16_t half1, uint16_t half2);

/**
 * @brief Start generating signal on given channel for RC servo.
 * @param channel the channel to generate the signal on.
 * @param angle the angle between 0 and 180
 * @note this is convenience function for controlling the commonly used servo motor
 * used in Radio controlled models.
 *
 */
uint8_t waveout_servo(uint8_t channel, uint8_t angle);

/**
 * @param us the pulse width which should be between 1000 and 2000.
 * The function does not check for validity except for us < 19 ms; the
 * period of the signal will always be 20 ms.
 * @note this is convenience function for controlling the commonly used servo motor
 * used in Radio controlled models.
 */
uint8_t waveout_servo_us(uint8_t channel, uint16_t us);

/**
 * @brief Stop generating the output for given channel.
 * @param channel the channel to stop generating output to.
 * @return WAVEOUT_NO_ERROR (0) if OK; error code otherwise, see waveout.h
 * @note The function will wait for the pin to become low before it returns so
 * it may block the caller up to the half2 microseconds of the current signal.
 * At most this could be about 65 ms.
 *
 */
void waveout_channel_stop(uint8_t channel);




#ifdef __cplusplus
}
#endif

/**@}*/
/* ----------- end of file -------------- */
#endif /* MSF_WAVEOUT_DRV_H */
