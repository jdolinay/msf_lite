/****************************************************************************
 * @file     wavein.c
 * @brief    Driver for measuring input signal
 * @version  1
 * @date     5. Feb. 2012
 *
 * @note
 *      
 ******************************************************************************/

/** @addtogroup wavein_driver
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
#include "wavein.h"


/* Defines section
* Add all internally used #defines here
*
***************************************************/

/* Variables section
* Add all global variables here
*
***************************************************/
/*---------- Internal variables ---------------- */
/* Mask of active (started) channels.
 * bit 0 is channel 1; value 1 means measuring now.*/
static volatile uint8_t gwavein_channel_mask;
/* Counter incremented when timer counter overflows. */
static volatile uint32_t gwavein_overflow_cnt;
/* */



/* The states for the input channel */
typedef enum wwavein_input_state
{
	wavein_idle,
	wavein_1st_edge,	/* 1st edge WAS detected */
	wavein_2nd_edge,
	wavein_3rd_edge,

} wavein_input_state;

/* Data for one input channel */
typedef struct _wavein_channel_data
{
	uint16_t start;		// time stamp for 1st detected edge (start of the period)
	uint16_t vals[2];	// lengths of 1st and second parts of the wave
	wavein_input_state status;		// status of the measurement
} wavein_channel_data;

static volatile wavein_channel_data gwavein_data[WAVEIN_NUM_CHANNELS];

/* Function Prototype Section
* Add prototypes for all functions called by this
* module, with the exception of runtime routines.
*
***************************************************/   
/* -------- Prototypes of internal functions   -------- */
static void wavein_timer0_event(uint32_t event, uint32_t arg);
static void wavein_on_edge(uint32_t timestamp, uint8_t channel);

/* Code section 
* Add the code for this module.
*
***************************************************/  
/* -------- Implementation of public functions   -------- */

/*
 * initialize the driver.
 *
 **/
void wavein_init(void)
{
	uint32_t i;

	gwavein_channel_mask = 0;	// no active channel
	gwavein_overflow_cnt = 0;

	// Initialize channel data
	for (i=0; i<WAVEIN_NUM_CHANNELS; i++ )
	{
		gwavein_data[i].status = wavein_idle;
		gwavein_data[i].vals[0] = 0;
		gwavein_data[i].vals[1] = 0;
	}

	WAVEIN_TIMER_DRIVER.Initialize(wavein_timer0_event);
	// Set the speed of the timer counter to achieve 1 MHz
	// The timer clock speed depends on F_CPU, see MSF_TPM_CLKSEL in msf_mkl25z.h
	// and in wavein.h we set the WMSF_WAVEIN_PRESCALER to obtain 1 MHz.
	WAVEIN_TIMER_DRIVER.Control(MSF_TPM_PRESCALER_SET | MSF_TPM_TOF_SIGNAL, WMSF_WAVEIN_PRESCALER);
}

/*
 * un-initialize the driver.
 *
 **/
void wavein_uninit(void)
{
	WAVEIN_TIMER_DRIVER.Uninitialize();
}

/**
 *
 * @note  The wavein driver code now assumes that the
 * wavein channels are directly mapped to underlying timer
 * channels: wavein channel 0 is TPM channel 0, etc.
 *
 *
 **/
uint8_t wavein_channel_attach(uint8_t channel)
{
	if ( channel < WAVEIN_NUM_CHANNELS )
	{
		WAVEIN_TIMER_DRIVER.ChannelSetMode(channel, InCapture_both_edges, MSF_TPM_PARAM_CHANNEL_EVENT);
		return WAVEIN_NO_ERROR;
	}
	else
		return WAVEIN_ERROR_INVALID_CHANNEL;
}

/**
 *
 * @note This function may do nothing. The pin can be configured
 * to e.g. GPIO mode without the need to disconnect it from the timer first.
 *
 **/
void wavein_channel_detach(uint8_t channel)
{
	// do nothing
}

/**
 * @note  This starts updating the values for the given channel
 *
 **/
uint8_t wavein_channel_start(uint8_t channel)
{
	if ( channel < WAVEIN_NUM_CHANNELS )
	{
		gwavein_channel_mask |= (1 << channel);
		return WAVEIN_NO_ERROR;
	}
	else
		return WAVEIN_ERROR_INVALID_CHANNEL;
}

/**
 * @note
 **/
void wavein_channel_stop(uint8_t channel)
{
	if ( channel < WAVEIN_NUM_CHANNELS )
	{
		gwavein_channel_mask &= ~(1 << channel);
		// Reset the data for this channel
		gwavein_data[channel].status = wavein_idle;
		gwavein_data[channel].vals[0] = 0;
		gwavein_data[channel].vals[1] = 0;
	}
}

/*
 * Read the values captured for given channel
 *
 **/
uint8_t wavein_channel_read(uint8_t channel, uint16_t* pulseA, uint16_t* pulseB  )
{
	*pulseB = 0;
	*pulseA = 0;

	// valid channel number?
	if ( channel >= WAVEIN_NUM_CHANNELS )
		return WAVEIN_ERROR_INVALID_CHANNEL;	// invalid channel

	// is the channel enabled?
	if ( (gwavein_channel_mask & (1 << channel)) == 0 )
	{
		return WAVEIN_DISABLED_CHANNEL;
	}

	// have we detected any edges on this channel yet?
	if ( gwavein_data[channel].status == wavein_idle )
		return WAVEIN_ERROR_NOINPUT;



	// deactivate the channel to be sure the read of the value is
	// not interfered with by new channel event (interrupt)
	gwavein_channel_mask &= ~(1 << channel);

	// Note: cannot use the status is this way:
	// if gwavein_data[channel].status == wavein_2nd_edge
	// because with normal signal on input, we do not care that the 1st part
	// is for new period and the 2nd part from older period, this is better
	// than getting just one part of the wave many times because we did not
	// call this function in the right time to "catch" both the values for the
	// same period.
	if ( gwavein_data[channel].vals[0] != 0)
	{
		*pulseA = gwavein_data[channel].vals[0];
		// Reset the value we have just returned otherwise the user would get
		// still the same values even after the input is disconnected.
		gwavein_data[channel].vals[0] = 0;
	}

	if ( gwavein_data[channel].vals[1] != 0)
	{
		*pulseB = gwavein_data[channel].vals[1];
		gwavein_data[channel].vals[1] = 0;
	}

	gwavein_data[channel].status == wavein_idle;

	// re-enable the channel
	gwavein_channel_mask |= (1 << channel);

	// If both the values are 0, we have no input
	if ( (*pulseA == 0) && (*pulseB == 0) )
		return WAVEIN_ERROR_NOINPUT;

	// Account for different clock than 1 MHz
	*pulseA = (uint16_t)(((uint32_t)*pulseA * WAVEIN_MULT_FACTOR)/WAVEIN_DIV_FACTOR);
	*pulseB = (uint16_t)(((uint32_t)*pulseB * WAVEIN_MULT_FACTOR)/WAVEIN_DIV_FACTOR);

	return WAVEIN_NO_ERROR;

}

/*
 * Wait for pulse on given input channel (pin)
 */
uint16_t wavein_channel_pulse_wait(uint8_t channel, uint32_t timeout )
{
	uint32_t endtime;
	uint32_t curtime = gwavein_overflow_cnt;

	if ( timeout > 0 )
	{
		if ( timeout <= WAVEIN_OVERFLOW_UNITMS)
			endtime = curtime + 1;
		else
		{
			endtime = curtime + (timeout / WAVEIN_OVERFLOW_UNITMS) + 1;	// how many overflows to wait
			// add 1 so that, for example timeout 100 results in 2 overflows = 132 ms rather than
			// 1 overflow = 66 ms. (for WAVEIN_OVERFLOW_UNITMS = 66).
			if ( endtime < curtime )	// overflow of the gwavein_overflow_cnt
			{
				// just reset the counter
				gwavein_overflow_cnt = 0;
				endtime = (timeout / WAVEIN_OVERFLOW_UNITMS) + 1;
			}


		}
	}
	else
		endtime = 0;

	// disable the channel (synchronization with ISR)
	gwavein_channel_mask &= ~(1 << channel);
	// Reset the data for this channel
	gwavein_data[channel].status = wavein_idle;
	gwavein_data[channel].vals[0] = 0;
	gwavein_data[channel].vals[1] = 0;
	// enable the channel
	gwavein_channel_mask |= (1 << channel);
	// wait for 2 edges
	while ( gwavein_data[channel].status != wavein_2nd_edge )
	{
		if ( (endtime > 0) && (gwavein_overflow_cnt >= endtime) )
			return 0;	// timeout occurred
	}

	// Account for different clock than 1 MHz
	// using endtime for temporary storage!
	endtime = (((uint32_t)gwavein_data[channel].vals[0] * WAVEIN_MULT_FACTOR)/WAVEIN_DIV_FACTOR);

	return (uint16_t)endtime;
}


/*
 * --------------------------------------------
 * Internal functions
 * --------------------------------------------
 */

/* Called from timer event handler to process the event.
 * ? it could be better to just save the time stamps here and calculate the lenght only
 * when needed - in read function. But:
 * - the overflow of timer would be harder to take into account
 * - more data for each channel would be needed (3 timestamps instead of 2 length)
 * */
static void wavein_on_edge(uint32_t timestamp, uint8_t channel)
{
	switch( gwavein_data[channel].status )
	{
	case wavein_idle:	// 1st edge detected, just save the time stamp
		gwavein_data[channel].start = timestamp;
		gwavein_data[channel].status = wavein_1st_edge;
		break;

	case wavein_1st_edge:	// 2nd edge detected, calculate the length of 1st wave part
		if ( gwavein_data[channel].start > timestamp )
			gwavein_data[channel].vals[0] = 0xffff - gwavein_data[channel].start + timestamp;
		else
			gwavein_data[channel].vals[0] = timestamp - gwavein_data[channel].start;

		gwavein_data[channel].start = timestamp;
		gwavein_data[channel].status = wavein_2nd_edge;
		break;

	case wavein_2nd_edge:	// 3rd edge detected: it is the end of part 2 and start of next period part 1
		if ( gwavein_data[channel].start > timestamp )
			gwavein_data[channel].vals[1] = 0xffff - gwavein_data[channel].start + timestamp;
		else
			gwavein_data[channel].vals[1] = timestamp - gwavein_data[channel].start;

		gwavein_data[channel].start = timestamp;
		gwavein_data[channel].status = wavein_1st_edge;
		break;

	}

}

/* Handler for the timer low-level driver events */
static void wavein_timer0_event(uint32_t event, uint32_t arg)
{
	switch ( event )
	{
	// This event is signaled when timer TPM0 counter overflows
	case MSF_TPM_EVENT_TOF:
		gwavein_overflow_cnt++;
		break;

	// This event is signaled when event (edge) on input pin for channel 0 is detected
	case MSF_TPM_EVENT_CH0:
		if ( gwavein_channel_mask & (1 << 0) )
		{
			// if channel is active
			wavein_on_edge(arg, 0);
		}
		break;

	case MSF_TPM_EVENT_CH1:
		if ( gwavein_channel_mask & (1 << 1) )
		{
			wavein_on_edge(arg, 1);
		}
		break;

	case MSF_TPM_EVENT_CH2:
		if ( gwavein_channel_mask & (1 << 2) )
		{
			wavein_on_edge(arg, 2);
		}
		break;

	case MSF_TPM_EVENT_CH3:
		if ( gwavein_channel_mask & (1 << 3) )
		{
			wavein_on_edge(arg, 3);
		}
		break;

	case MSF_TPM_EVENT_CH4:
		if ( gwavein_channel_mask & (1 << 4) )
		{
			wavein_on_edge(arg, 4);
		}
		break;

	case MSF_TPM_EVENT_CH5:
		if ( gwavein_channel_mask & (1 << 5) )
		{
			wavein_on_edge(arg, 5);
		}
		break;

	}

}

/** @}*/ 
 
