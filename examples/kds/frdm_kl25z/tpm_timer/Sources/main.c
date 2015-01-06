/*
 * Sample program for MSF lite.
 *
 * This sample demonstrates the TPM Timer driver.
 * There are three test functions which demonstrate using the timer.
 * You can call one of these functions from main():
 *  tpm_tof_test - Example of using TOF event for blinking RED LED.
 *  tpm0_pwm_test - Example of driving the BLUE LED with PWM.
 *  tpm2_pwm_test - Example of driving the GREEN or RED LED with PWM.
 *
 * IMPORTANT: the pins for timer channels must be configured correctly in msf_config.h
 * 	for the PWM examples to works.
 * 	BLUE LED: pin D1, see MSF_TPM0_CH1_PIN in msf_config.h
 * 	RED LED: pin B18, see MSF_TPM2_CH0_PIN in msf_config.h
 * 	GREEN LED: pin B19, see MSF_TPM2_CH1_PIN in msf_config.h
 *
 * IMPORTANT: This example assumes that the clock for the timer is 8 MHz, which
 * is true for the 48 MHz CPU speed clock option.
 * If you use the 4 MHz CPU, the clock for the timer is 4 MHz and the example
 * needs to be modified - You can change the MSF_TPM_MOD_VALUE or the
 * MSF_TPM_PRESCALER_SET (prescaler value).
 *
 */

/* MSF configuration for this project */
#include "msf_config.h"
/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"
#include "msf.h"

#include "drv_tpm.h"	/* TPM driver */

#define	RED_LED		GPIO_B18
#define	GREEN_LED	GPIO_B19
#define	BLUE_LED	GPIO_D1


// example of using TOF event for blinking RED LED
void tpm_tof_test(void);
// example of using PWM to drive the LED
void tpm0_pwm_test(void);
void tpm2_pwm_test(void);


// event handler for timer TPM0
void TPM0_SignalEvent(uint32_t event, uint32_t arg);
void TPM2_SignalEvent(uint32_t event, uint32_t arg);
uint32_t g_duty;
uint32_t g_duty2;
static int i = 0;

int main(void)
{
	/* Always initialize the MSF library*/
	msf_init(0);

	// Initialize LEDs
	msf_pin_direction(RED_LED, output);
	msf_pin_write(RED_LED, true);
	msf_pin_direction(GREEN_LED, output);
	msf_pin_write(GREEN_LED, true);


	//
    // Un-comment one of the test functions below to test the timer driver
	//
	tpm_tof_test();	// blink RED LED
	//tpm2_pwm_test();	// change brightness of GREEN and/or RED LED
	//tpm0_pwm_test();	// blink with BLUE LED using PWM

    /* This for loop should be replaced. By default this loop allows a single stepping. */
    for (;;) {
        i++;
    }
    /* Never leave main */
    return 0;
}

/* TPM0_SignalEvent
 * Event handler for TPM0 timer.
 * If enabled, it will be called by the timer driver when an event occurs,
 * for example, when the timer counter overflows.
 * Note that this is actually interrupt service routine, so do not
 * perform any lengthy operations here.
 *
 */
void TPM0_SignalEvent(uint32_t event, uint32_t arg)
{
	//static uint32_t cnt = 0;
	switch ( event )
	{
	case MSF_TPM_EVENT_TOF:
		// This event is signaled when timer TPM0 counter overflows
		msf_pin_toggle(RED_LED);
		break;

	case MSF_TPM_EVENT_CH0:
		break;

	case MSF_TPM_EVENT_CH1:
		Driver_TPM0.ChannelWrite(1, g_duty );
		break;
	}

}

/* TPM2_SignalEvent
 * Event handler for TPM2 timer
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
		//Driver_TPM2.ChannelWrite(0, g_duty2 );
		break;

	case MSF_TPM_EVENT_CH1:
		//Driver_TPM2.WriteChannel(1, g_duty );
		break;
	}

}

/* tpm_tof_test
 * Example of using TOF event for blinking RED LED.
 * The LED should blink once per second.
 */
void tpm_tof_test(void)
{
	// Initialize TPM0. The clock speed depends on F_CPU,
	// see MSF_TPM_CLKSEL in msf_mkl25z.h
	Driver_TPM0.Initialize(TPM0_SignalEvent);	// init timer, internal clock
	// Set the prescaler and modulo to obtain desired frequency of overflows
	// MOD = Fsrc / (Fo . PRESCALER) - 1
	// Assuming Fsrc = 8 MHz and desired frequency of overflow Fo = 2 Hz
	// (which means the LED will be toggled twice per second, blinking once
	// per second):
	// MOD = 8000000 / (2*128) - 1 = 31249
	Driver_TPM0.Control(MSF_TPM_PRESCALER_SET, MSF_TPM_PRESCALER_128);
	Driver_TPM0.Control(MSF_TPM_MOD_VALUE | MSF_TPM_TOF_SIGNAL, 31249);

	// Now just wait. The TPM0_SignalEvent function will handle the LED blinking
	while(1)
		i++;
}

/* tpm0_pwm_test
 * Example of driving the BLUE LED with PWM.
 * Blue LED is TPM0 channel 1
 * IMPORTANT: the LEDS are connected so that they are ON when the pin is LOW,
 * keep it in mind when testing PWM.
 * For example, with high-true pulses and duty 10%, the log.1 pulse is 10% of the period
 * long but the LED will be off 10% of the period and on 90%!
 *
 * This example will blink the blue LED with PWM signal with period 1 second.
 * The duty cycle is 10, 90 or 50 percent. The change in duty occurs every 5 seconds.
 *
 */
void tpm0_pwm_test(void)
{
	g_duty = 15000;		// set in event handler
	Driver_TPM0.Initialize(TPM0_SignalEvent);
	// With 8 MHz clock source for timer, we get lowest counter frequency Ft = 62500 Hz
	// If we want 1 Hz PWM, the MODULUS register value will be:
	// MOD = Ft/Fo - 1 = 62500/1 - 1 = 62499
	// 100% duty is MOD+1 = 62500
	Driver_TPM0.Control(MSF_TPM_PRESCALER_SET, MSF_TPM_PRESCALER_128);

	// For center aligned PWM, max value of MOD is about 32000!
	// If we want 1 Hz PWM:
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

}


/* tpm2_pwm_test
 * Example of driving RED or GREEN LED with PWM
 * RED LED is TPM2 channel 0 (pin B18),
 * GREEN LED is TPM2 channel 1 (pin B19)
 *
 * This example will drive the GREEN (or RED) LED with 50 Hz PWM and slowly
 * change the duty from about 10 percent to 100%, then jump back to 10% and
 * so on. You should see the LED brightness change slowly.
 * NOTE: you can enable both the channels (both green and RED LED) at the same time.
 *
 */
void tpm2_pwm_test(void)
{
	Driver_TPM2.Initialize(TPM2_SignalEvent);	// TPM2 timer - for RED and green led
	// Test in mode for driving LED with HW PWM
	// If we want 50 Hz PWM signal, the MODULUS register value will be:
	// Note: Ft is the frequency at which the counter timer runs (after prescaling)
	// MOD = Ft/Fo - 1 = Fsrc / (Fo . PRESCALER) - 1
	// = 8000000 / (50 * PRSC) - 1; for PRESCALER = 64: MOD = 2499
	Driver_TPM2.Control(MSF_TPM_PRESCALER_SET, MSF_TPM_PRESCALER_64);
	Driver_TPM2.Control(MSF_TPM_MOD_VALUE, 2499);

	// Preset the duty to 10 percent
	g_duty2 = 249;

	// Set channel 0 (RED LED), low true pulses so that the
	// "pulse" is actually log. 0 state, signal channel event (to change the duty safely)
	// Note that handling event 50-times per second is not so good, so we do not use
	// the MSF_TPM_PARAM_CHANNEL_EVENT option.
	//Driver_TPM2.ChannelSetMode(0, PWM_edge_lowtrue, 0); 	// without channel event
	//Driver_TPM2.SetChannelMode(0, PWM_edge_lowtrue, MSF_TPM_PARAM_CHANNEL_EVENT);	// with channel event

	// Channel 1 for GREEN LED
	Driver_TPM2.ChannelSetMode(1, PWM_edge_lowtrue, 0); 	// without channel event

	while(1)
	{
		// change the duty slowly...
		if ( g_duty2 <= 2250 )
			g_duty2 += 249;		// add 10%
		else
			g_duty2 = 249;	// start over from about 10%
		//Driver_TPM2.ChannelWrite(0, g_duty2 );
		Driver_TPM2.ChannelWrite(1, g_duty2 );
		msf_delay_ms(1000);
	}

}




////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
