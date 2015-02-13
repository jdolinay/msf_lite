/****************************************************************************
 * @file     waveout.c
 * @brief    Driver for generating output signal
 * @version  1
 * @date     10. Feb. 2012
 *
 * @note
 *      
 ******************************************************************************/

/** @addtogroup waveout_driver
 * @{
 */


/* Include section
* Add all #includes here
*
***************************************************/
/* Include user configuration */
#include "msf_config.h"
/* Include hardware definitions */
#include "coredef.h"
/* Include any standard headers, such as string.h */

/* Include our main header*/           
#include "msf.h"
/* Include the header for this module */
#include "waveout.h"


/* Defines section
* Add all internally used #defines here
*
***************************************************/

/* Variables section
* Add all global variables here
*
***************************************************/
/*---------- Internal variables ---------------- */

/* The states for the output channel */
typedef enum wwaveout_output_state
{
	waveout_idle,
	waveout_1st_half,	/* generating 1st half now */
	waveout_2nd_half,

} waveout_output_state;

/* the tpm drivers which can be used by waveout driver
 * This is initialized in wavein_init to allow the user to have
 * only as many TPM drivers allocated in his program
 * as he really needs. */
#define		WAVEOUT_MAX_DRIVERS		(3)
static MSF_DRIVER_TPM* waveout_drivers[WAVEOUT_MAX_DRIVERS];
/* Status information for each of the TPM drivers */
static uint8_t waveout_drv_status[WAVEOUT_MAX_DRIVERS];

#define		WAVEOUT_NUM_CHANNELS	(10)
/* Structure with information about 1 channel */
typedef struct _waveout_channel_data
{
	//MSF_DRIVER_TPM*	pdriver;	/* the underlying timer driver for this channel */
	uint16_t half_wave[2];			/* half-wave 1 and 2 parts */
	waveout_output_state status;	/* status of this channel/driver */

} waveout_channel_data;

static volatile waveout_channel_data gwaveout_data[WAVEOUT_NUM_CHANNELS];

/* Mask of active (started) channels.
 * bit 0 is channel 1; value 1 means measuring now.*/
static volatile uint32_t gwaveout_channel_mask;


/* Function Prototype Section
* Add prototypes for all functions called by this
* module, with the exception of runtime routines.
*
***************************************************/   
/* -------- Prototypes of internal functions   -------- */
static void waveout_timer0_event(uint32_t event, uint32_t arg);
static void waveout_timer1_event(uint32_t event, uint32_t arg);
static void waveout_timer2_event(uint32_t event, uint32_t arg);
static void waveout_on_event(MSF_DRIVER_TPM* pdriver, uint8_t tpm_channel, uint8_t channel, uint16_t cntval);
static MSF_DRIVER_TPM* waveout_get_tpm_driver(uint8_t channel, uint8_t* out_channel);

/* Code section 
* Add the code for this module.
*
***************************************************/  
/* -------- Implementation of public functions   -------- */

/*
 * initialize the waveout driver.
 *
 **/
void waveout_init(uint32_t channel_mask )
{
	// need to initialize TPM0?
	if ( channel_mask & WAVEOUT_RANGE_0_5 )
	{
		Driver_TPM0.Initialize(waveout_timer0_event);
		// The timer clock speed depends on F_CPU, see MSF_TPM_CLKSEL in msf_mkl25z.h
		// and in waveout.h we set the WMSF_WAVEOUT_PRESCALER to obtain 1 MHz.
		Driver_TPM0.Control(MSF_TPM_PRESCALER_SET, WMSF_WAVEOUT_PRESCALER);
	}

	if ( channel_mask & WAVEOUT_RANGE_6_7 )
	{
		Driver_TPM1.Initialize(waveout_timer1_event);
		Driver_TPM1.Control(MSF_TPM_PRESCALER_SET, WMSF_WAVEOUT_PRESCALER);
	}

	if ( channel_mask & WAVEOUT_RANGE_8_9 )
	{
		Driver_TPM2.Initialize(waveout_timer2_event);
		Driver_TPM2.Control(MSF_TPM_PRESCALER_SET, WMSF_WAVEOUT_PRESCALER);
	}



}

/*
 * un-initialize the driver.
 *
 **/
void waveout_uninit(void)
{

}

/* Attach and detach are not needed.
 * User should call stop() before using the same pin for other purposes.
 */
#if 0
/**
 * @note  channels 0 thru 5 are TPM0, 6 and 7 to TPM1; 8 and 9 to TPM2.
 *
 **/
uint8_t waveout_channel_attach(uint8_t channel)
{
	uint8_t tpm_channel;
	MSF_DRIVER_TPM* drv;

	drv = waveout_get_tpm_driver(channel, &tpm_channel);
	if ( !drv )
		return WAVEOUT_ERROR_INVALID_CHANNEL;

	gwaveout_data[channel].half_wave[0] = 0;
	gwaveout_data[channel].half_wave[1] = 0;
	//drv->ChannelSetMode(tpm_channel, OutCompare_toggle, MSF_TPM_PARAM_CHANNEL_EVENT);
	//drv->ChannelWrite(tpm_channel, 0);	// do not generate anything until start is called

}

/**
 *
 * @note This function may do nothing. The pin can be configured
 * to e.g. GPIO mode without the need to disconnect it from the timer first.
 *
 **/
void waveout_channel_detach(uint8_t channel)
{
	// do nothing
}
#endif	/* #if 0*/

/**
 * @note
 *
 **/
uint8_t waveout_channel_start(uint8_t channel, uint16_t half1, uint16_t half2)
{
	uint8_t tpm_channel;
	MSF_DRIVER_TPM* drv;

	drv = waveout_get_tpm_driver(channel, &tpm_channel);
	if ( !drv )
		return WAVEOUT_ERROR_INVALID_CHANNEL;

	// stop generating the signal before updating values
	gwaveout_channel_mask &= ~(1 << channel);

	// save the requested values to our buffer for updates in ISR
	gwaveout_data[channel].half_wave[0] = half1;
	gwaveout_data[channel].half_wave[1] = half2;
	gwaveout_data[channel].status = waveout_idle;
	// The 1st interrupt will occur when the timer counter value == channel value (which is half1).
	// Then in the ISR we start generating the 1st part of the wave

	// We always configure the channel because this will force the pin low on 1st match
	// and also enable the channel event, which may be disabled after stop().
	// Note that setting the channel mode itself will not change the pin state!
	drv->ChannelSetMode(tpm_channel, OutCompare_toggle, MSF_TPM_PARAM_CHANNEL_EVENT);
	// write the first half to timer register - we can write anything actually, just need the
	// 1st interrupt to occur to start generating the signal
	drv->ChannelWrite(tpm_channel, half1);

	// enable updating the signal in timer interrupt
	gwaveout_channel_mask |= (1 << channel);
	return WAVEOUT_NO_ERROR;

}

/**
 * @note
 **/
void waveout_channel_stop(uint8_t channel)
{
	uint8_t tpm_channel;
	MSF_DRIVER_TPM* drv;

	drv = waveout_get_tpm_driver(channel, &tpm_channel);
	if ( !drv )
		return;

	// if the channel is not active, do nothing
	if ( gwaveout_channel_mask & (1 << channel) )
		return;

	// Howto be sure the pin is low when we stop?
	// Version a) wait for the output pin to go low
	// Version b) disconnect and again connect the channel?
	// Version b) does not work - the disconnecting itself will not change the pin state.
	// Ver. a): waiting
	while (gwaveout_data[channel].status != waveout_2nd_half ) ;


	gwaveout_channel_mask &= ~(1 << channel);
	// We disconnect the pin so that it is forced low after the 1st match when connected again.
	// The datasheet says "When a channel is initially configured to output compare mode,
	// the channel output updates with its negated value (logic 0 for set/toggle/pulse high..."
	// In fact the pin updates on the 1st compare match after connecting the pin and in our case,
	// toggle, the pin goes low.
	drv->ChannelSetMode(tpm_channel, Disabled, 0);

	// Ver. b) - re-connect the channel to force the output pin low
	// Does not work - the disconnecting itself will not change the pin state.
	//drv->ChannelSetMode(tpm_channel, OutCompare_toggle, 0);

	// Reset the data for this channel - not needed, but to be sure...
	gwaveout_data[channel].status = waveout_idle;
	gwaveout_data[channel].half_wave[0] = 0;
	gwaveout_data[channel].half_wave[1] = 0;

}

/*
 * Start generating signal on given channel for RC servo
 * with desired value in degrees (0 - 180)
 *
 */
uint8_t waveout_servo(uint8_t channel, uint8_t angle)
{
	uint16_t us;

	if (angle > 180)
		return MSF_ERROR_ARGUMENT;
	// convert the angle to microseconds:
	// 0 = 1000; 180 = 2000
	// us = 5.55 * angle + 1000
	// For integers:
	// us = (555 * angle)/100 + 1000
	us = (uint16_t)(((uint32_t)angle * 555) / 100 + 1000);
	return waveout_servo_us(channel, us);

}

/*
 * Start generating signal on given channel for RC servo
 * with desired value in microseconds (typically 1000 - 2000)
*/
uint8_t waveout_servo_us(uint8_t channel, uint16_t us)
{
	uint16_t space;

	if ( us > 19000 )
		return MSF_ERROR_ARGUMENT;

	space = 20000 - us;
	return waveout_channel_start(channel, us, space);
}


/*
 * --------------------------------------------
 * Internal functions
 * --------------------------------------------
 */

/* Return pointer to the TPM driver for given channel
 * @return pointer to the TPM driver or null
 * */
static MSF_DRIVER_TPM* waveout_get_tpm_driver(uint8_t channel, uint8_t* out_channel)
{
	*out_channel = 0;

	if (channel >= 0 && channel <= 5 )
	{
#if (MSF_DRIVER_TPM0)
		*out_channel = channel;
		return &Driver_TPM0;
#endif
	}
	else if ( channel >= 6 && channel <= 7 )
	{
#if (MSF_DRIVER_TPM1)
		*out_channel = channel - 6;
		return  &Driver_TPM1;
#endif
	}
	else if ( channel >= 8 && channel <= 9 )
	{
#if (MSF_DRIVER_TPM2)
		*out_channel = channel - 8;
		return &Driver_TPM2;
#endif
	}

	return null;
}



/* The code is in #if because if the driver TPMx does not exists (user does not
 * need to use it, this would generate compile errors. */
#if (MSF_DRIVER_TPM0)
/* Handler for the timer low-level driver events */
static void waveout_timer0_event(uint32_t event, uint32_t arg)
{
	switch ( event )
	{
	// This event is signaled when timer TPM0 counter overflows
	case MSF_TPM_EVENT_TOF:
		break;

	case MSF_TPM_EVENT_CH0:
		if ( gwaveout_channel_mask & (1 << 0) )
		{
			waveout_on_event(&Driver_TPM0, 0, 0, (uint16_t)arg);
		}
		break;

	case MSF_TPM_EVENT_CH1:
		if ( gwaveout_channel_mask & (1 << 1) )
		{
			waveout_on_event(&Driver_TPM0, 1, 1, (uint16_t)arg);
		}
		break;

	case MSF_TPM_EVENT_CH2:
		if ( gwaveout_channel_mask & (1 << 2) )
		{
			waveout_on_event(&Driver_TPM0, 2, 2, (uint16_t)arg);
		}
		break;

	case MSF_TPM_EVENT_CH3:
		if ( gwaveout_channel_mask & (1 << 3) )
		{
			waveout_on_event(&Driver_TPM0, 3, 3, (uint16_t)arg);
		}
		break;

	case MSF_TPM_EVENT_CH4:
		if ( gwaveout_channel_mask & (1 << 4) )
		{
			waveout_on_event(&Driver_TPM0, 4, 4, (uint16_t)arg);
		}

		break;

	case MSF_TPM_EVENT_CH5:
		if ( gwaveout_channel_mask & (1 << 5) )
		{
			waveout_on_event(&Driver_TPM0, 5, 5, (uint16_t)arg);
		}

		break;

	}

}
#endif	/* MSF_DRIVER_TPM0 */

#if (MSF_DRIVER_TPM1)
static void waveout_timer1_event(uint32_t event, uint32_t arg)
{
	switch ( event )
	{
	case MSF_TPM_EVENT_TOF:
		break;

	case MSF_TPM_EVENT_CH0:
		if ( gwaveout_channel_mask & (1 << 6) )
		{
			waveout_on_event(&Driver_TPM1, 0, 6, (uint16_t)arg);
		}
		break;

	case MSF_TPM_EVENT_CH1:
		if ( gwaveout_channel_mask & (1 << 7) )
		{
			waveout_on_event(&Driver_TPM1, 1, 7, (uint16_t)arg);
		}
		break;
	}
}
#endif	/* MSF_DRIVER_TPM1 */

#if (MSF_DRIVER_TPM2)
static void waveout_timer2_event(uint32_t event, uint32_t arg)
{
	switch ( event )
	{
	case MSF_TPM_EVENT_TOF:
		break;

	case MSF_TPM_EVENT_CH0:
		if ( gwaveout_channel_mask & (1 << 8) )
		{
			waveout_on_event(&Driver_TPM2, 0, 8, (uint16_t)arg);
		}
		break;

	case MSF_TPM_EVENT_CH1:
		if ( gwaveout_channel_mask & (1 << 9) )
		{
			waveout_on_event(&Driver_TPM2, 1, 9, (uint16_t)arg);
		}
	}
}
#endif /* MSF_DRIVER_TPM2 */


/* Called from timer event handler to process the event.
 * @param tpm_channel the channel in the TPM driver
 * @param channel the channel in waveout driver
 * @param cntval current value of the timer counter.

 * */
static void waveout_on_event(MSF_DRIVER_TPM* pdriver, uint8_t tpm_channel, uint8_t channel, uint16_t cntval)
{
	uint16_t next_match;

	switch( gwaveout_data[channel].status )
	{
	case waveout_idle:
		// On the 1st compare match the pin is forced low, so we set the
		// status as if we were just in the 2nd pulse now, and the next match
		// will really start to generate the signal.
		next_match = cntval + gwaveout_data[channel].half_wave[1];
		pdriver->ChannelWrite(tpm_channel, next_match);
		gwaveout_data[channel].status = waveout_2nd_half;	// now waiting for next match
		break;

	case waveout_1st_half:	// match after 1st match, now half 2nd part of the wave
		// calculate the value when next match should occur
		next_match = cntval + gwaveout_data[channel].half_wave[1];
		pdriver->ChannelWrite(tpm_channel, next_match);
		gwaveout_data[channel].status = waveout_2nd_half;	// now generating 2st half
		break;

	case waveout_2nd_half:	// match after 2nd half; now start the 1st half again
		// calculate the value when next match should occur
		next_match = cntval + gwaveout_data[channel].half_wave[0];
		pdriver->ChannelWrite(tpm_channel, next_match);
		gwaveout_data[channel].status = waveout_1st_half;	// now generating 1st half
		break;

	}


}

/** @}*/ 
 
