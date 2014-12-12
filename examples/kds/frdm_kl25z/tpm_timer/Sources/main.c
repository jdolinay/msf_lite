/*
 * Sample program for MSF lite
 *
 * This sample demonstrates the TPM Timer driver.
 */

/* MSF configuration for this project */
#include "msf_config.h"
/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"
#include "msf.h"

#include "drv_tpm.h"	/* TPM driver */

void tpm_timer_test(void);


// event handler for timer TPM0
void TPM0_SignalEvent(uint32_t event, uint32_t arg);
void TPM2_SignalEvent(uint32_t event, uint32_t arg);
uint32_t g_duty;
uint32_t g_duty2;
static int i = 0;

int main(void)
{

    /* Write your code here */
	tpm_timer_test();

    /* This for loop should be replaced. By default this loop allows a single stepping. */
    for (;;) {
        i++;
    }
    /* Never leave main */
    return 0;
}

/* TPM0_SignalEvent
 * Event handler for TPM0
 *
 */
void TPM0_SignalEvent(uint32_t event, uint32_t arg)
{
	//static uint32_t cnt = 0;
	switch ( event )
	{
	case MSF_TPM_EVENT_TOF:
		//if ( cnt++ > 100 )
		//{
		//	cnt = 0;
		//	msf_pin_toggle(RED_LED);
		//}
		break;

	case MSF_TPM_EVENT_CH0:
		break;

	case MSF_TPM_EVENT_CH1:
		Driver_TPM0.ChannelWrite(1, g_duty );
		break;
	}

}

/* TPM2_SignalEvent
 * Event handler for TPM2
 *
 */
void TPM2_SignalEvent(uint32_t event, uint32_t arg)
{
	switch ( event )
	{
	case MSF_TPM_EVENT_TOF:
		msf_pin_toggle(RED_LED);
		break;

	case MSF_TPM_EVENT_CH0:
		Driver_TPM2.ChannelWrite(0, g_duty2 );
		break;

	case MSF_TPM_EVENT_CH1:
//		Driver_TPM2.WriteChannel(1, g_duty );
		break;
	}

}

void tpm_timer_test(void)
{
	Driver_TPM0.Initialize(TPM0_SignalEvent);	// init timer, internal clock
	Driver_TPM2.Initialize(TPM2_SignalEvent);	// TPM2 timer - for RED and green led

	// test TPM2 in TOF mode
	//Driver_TPM2.Control(MSF_TPM_TOF_SIGNAL | MSF_TPM_PRESCALER_SET, MSF_TPM_PRESCALER_64);

	// Test in mode for driving LED with HW PWM
	// If we want 50 Hz PWM signal:
	// MOD = Ft/Fo - 1 = Fsrc / (Fo . PRESCALER) - 1
	// = 8000000 / (50 * PRSC) - 1; for prescaler = 64: MOD = 2499
	Driver_TPM2.Control(MSF_TPM_PRESCALER_SET, MSF_TPM_PRESCALER_64);
	Driver_TPM2.Control(MSF_TPM_MOD_VALUE, 2499);
	g_duty2 = 249;	/* 10% */

	//Driver_TPM2.SetChannelMode(0, PWM_edge_lowtrue, MSF_TPM_PARAM_CHANNEL_EVENT); 	// Set channel 0 (RED LED), lowtrue pulses so that the
	// "pulse" is actually log. 0 state, signal channel event (to change the duty safely)
	// Note that handling event 50-times per second is not good..
	Driver_TPM2.ChannelSetMode(1, PWM_edge_lowtrue, 0); 	// without event
	while(1)
	{
		if ( g_duty2 <= 2250 )
			g_duty2 += 249;		// add 10%
		else
			g_duty2 = 0;
		Driver_TPM2.ChannelWrite(1, g_duty2 );
		msf_delay_ms(1000);
	}


	// Enable TOF signal and set prescaler
	// With clock option 1 (48 MHz) the timer clock is 8 MHz.
	// with prescaler = 128 we get counter freq = 62500 Hz and about 1 overflow in 1 second.
//	Driver_TPM0.Control(MSF_TPM_TOF_SIGNAL | MSF_TPM_PRESCALER_SET, MSF_TPM_PRESCALER_128);
	// wait
//	msf_delay_ms(10000);
	// change modulo = change freq of blinking.
	// F_of_interrupt = F_of_counter / (MOD+1)
	// F = 62500 / (12500) = 5  (5times toggle per second)
	//Driver_TPM0.Control(MSF_TPM_TOP_VALUE, 12499);

	// -------- PWM test
	// blue LED is TPM0 channel 1
	// IMPORTANT: the LEDS are connected so that they are ON when the pin is LOW, keep it in mind when testing PWM.
	// For example with hightrue pulses and duty 10%, the log.1 pulse is 10% of the period long but the LED will be
	// off 10% of the period and on 90%!

	g_duty = 15000;		// set in event handler
	// with 8 MHz src for timer, we get lowest counter freq. of Ft = 62500 Hz
	// IF we want 1 Hz PWM:
	// MOD = Ft/Fo - 1 = 62500/1 - 1 = 62499
	// 100% duty is MOD+1 = 62500
	Driver_TPM0.Control(MSF_TPM_PRESCALER_SET, MSF_TPM_PRESCALER_128);

	// center aligned PWM, max value of MOD is about 32000!
	// if we want 1 Hz PWM:
	// MOD = Ft/(2.Fo) = 62500/(2.1)= 31250
	// 100% duty is 2xMOD = 62500; we set CnV between 0 and 31250
	// For 10% duty set channel to: 10% od MOD = 3125
	Driver_TPM0.Control(MSF_TPM_MOD_VALUE, 31250);
	Driver_TPM0.ChannelSetMode(1, PWM_center_hightrue, MSF_TPM_PARAM_CHANNEL_EVENT); 	// with event
	while(1)
	{
		// assuming the write to uint32_t is atomic
		g_duty = 3125;		// 10%
		msf_delay_ms(5000);
		g_duty = 28125;		// 90%
		msf_delay_ms(5000);
		g_duty = 15625;	// 50%
		msf_delay_ms(5000);
	}


#if 0	/* edge aligned PWM */
	Driver_TPM0.Control(MSF_TPM_MOD_VALUE, 62499);
	//Driver_TPM0.SetChannelMode(1, PWM_edge_lowtrue, 0);  // without event
	//Driver_TPM0.SetChannelMode(1, PWM_edge_lowtrue, MSF_TPM_PARAM_CHANNEL_EVENT); 	// with event
	Driver_TPM0.SetChannelMode(1, PWM_edge_hightrue, MSF_TPM_PARAM_CHANNEL_EVENT); 	// with event
	while(1)
	{
		// Note: better is to write new duty in the event of the channel (at the end of period)
		//Driver_TPM0.WriteChannel(1, 6249 );	// 10%
		g_duty = 6249;	// assuming th ewrite is atomic
		msf_delay_ms(5000);
		//Driver_TPM0.WriteChannel(1, 31250 );	// 50%
		g_duty = 31250;
		msf_delay_ms(5000);
		//Driver_TPM0.WriteChannel(1, 56250 );	// 90%
		g_duty = 56250;
		msf_delay_ms(5000);
	}
#endif
}


////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
