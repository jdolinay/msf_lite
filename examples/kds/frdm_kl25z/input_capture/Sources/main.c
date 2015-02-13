/*
  input_capture example for MSF

  This is example project demonstrating usage of the TPM timer in input capture mode to
  measure pulse length etc.
  The example works with square wave input signal (50% duty).
  It can also handle signal from RC transceivers (pulses between 1 and 2 ms with about 50 Hz period)
  - for this call the process_servo() function from main() below instead of the process_square().

 Note: You can also use the high-level wavein driver for measuring inputs and processing
   RC signal - see the wavein example project.

  Testing the program:
  - open terminal emulator on your PC (e.g. Tera Term or Putty)
   and open the OpenSDA COM port and set baudrate to 19200
  - run this example project
  - connect input signal to the selected pin of your FRDM board (see below)
  You should see the measured length of the pulses in your terminal window.

  Without changes in code this example works with 48 MHZ F_CPU.
  It sets timer clock to 1 MHz, which means "1" in the counter
  amounts to 1 microsecond (1 us). The longest pulse it can measure
  is about 65 ms (input signals from about 8 Hz up).

  INPUT PINS
  You can select one of the pins (timer channels) below by changing the
  TIMER_CHANNEL define below;
  Note that in principle you can measure several pins at the same time, but this
  example program is not equipped to handle more than one channel at a time.

  If you modify the example to use other timer than TPM0 (TPM1 or TPM2),
  you can use other pins. For the mapping of the pins to timer channels
  see the msf_config.h in this project (Project Explorer > input_capture > Sources folder).

 */

/* MSF configuration for this project */
#include "msf_config.h"
/* Include the core definitions - CMSIS <device>.h file etc.*/
#include "coredef.h"
#include "msf.h"

#include "drv_tpm.h"	/* TPM timer driver */


/*
 INPUT PINS = Timer channels:
  Pin: D0 (Arduino pin 10) - TPM0, channel 0.
  Pin: A4 (Arduino pin 4) - TPM0, channel 1.
  Pin: A5 (Arduino pin 5) - TPM0, channel 2.
  Pin: D3 (Arduino pin 12) - TPM0, channel 3.
  Pin: D4 (Arduino pin 2) - TPM0, channel 4.
  Pin: C9 (Arduino pin 7) - TPM0, channel 5.

  Select one by changing the TIMER_CHANNEL value below:
 */
#define TIMER_CHANNEL	(5)


#define	RED_LED		GPIO_B18
#define	GREEN_LED	GPIO_B19
#define	BLUE_LED	GPIO_D1

// Prototypes
void TPM0_SignalEvent(uint32_t event, uint32_t arg);
void on_edge(uint16_t arg);
void process_square(uint16_t pulse);
void process_servo(uint16_t pulse);



// Globals
volatile uint16_t start;
volatile uint8_t overflow;
volatile uint8_t data_ready;
volatile uint16_t pulse_time;



int main(void)
{
	uint32_t i;
	uint16_t pulse;

	// Always initialize the MSF library
	msf_init(0);

	// Initialize LEDs
	msf_pin_direction(RED_LED, output);
	msf_pin_write(RED_LED, true);

	msf_pin_write(RED_LED, false);
	msf_delay_ms(1500);
	msf_pin_write(RED_LED, true);


	start = 0;
	overflow = 0;
	data_ready = 0;

	// Initialize TPM0 timer.
	Driver_TPM0.Initialize(TPM0_SignalEvent);

	// Set the speed of the timer counter
	// The timer clock speed depends on F_CPU, see MSF_TPM_CLKSEL in msf_mkl25z.h
	// It is 8 MHz in default clock setup (48 MHz F_CPU)
	// Our desired speed will depend on the purpose.
	// For RC receiver signal: the pulse is about 1 ms long; if we want to measure it with
	// resolution of 1000 units we need the timer tick to be 1 us > the frequency 1 MHz
	// Set prescaler to 8 to obtain 1 MHz clock for timer.
	Driver_TPM0.Control(MSF_TPM_PRESCALER_SET | MSF_TPM_TOF_SIGNAL, MSF_TPM_PRESCALER_8);

	// Set one or more of the timer channels to input capture mode.
	// The channel depends on the input pin we want to use.
	// When the input changes from low to high, the value from counter will be
	// stored in channel register and we can read it.
	Driver_TPM0.ChannelSetMode(TIMER_CHANNEL, InCapture_both_edges, MSF_TPM_PARAM_CHANNEL_EVENT);

	msf_print("Waiting for signal...\n");

    while(1)
    {
    	if ( data_ready )
    	{
    		// Get the data from interrupt handler
    		pulse = pulse_time;
			data_ready = 0;		// signal the event handler it can prepare new data

			// Process the data
			// In case it is square wave signal, use process_square(),
			// for signal from RC receiver, use process_servo().

			process_square(pulse);
    		//process_servo(pulse);
    	}

    	i++;
    }

    /* Never leave main */
    return 0;
}

// Process square wave signal
void process_square(uint16_t pulse)
{
	static int cnt = 0;
	static long sum = 0;

	sum += pulse;
	cnt++;
	if (cnt == 10 )
	{
		// Average N pulses
		sum = sum / 10;
		msf_print("Pulse: ");
		msf_printnum(sum);
		msf_print(" us \n");
		sum = 0;
		cnt = 0;
	}
}

// Servo signal test
void process_servo(uint16_t pulse)
{
	static int cnt = 0;
	static long sum = 0;

	// ignore pulses outside the valid range for RC signal
	if ( pulse > 800 && pulse < 2200 )
	{
		sum += pulse;
		cnt++;
		if (cnt == 5 )
		{
			// Average N pulses
			sum = sum / 5;
			msf_print("Pulse: ");
			msf_printnum(sum);
			msf_print(" us \n");
			sum = 0;
			cnt = 0;
		}
	}
}

/* TPM0_SignalEvent
 * Event handler for TPM0 timer.
 * If enabled, it will be called by the timer driver when an event occurs,
 * for example, when the timer counter overflows.
 * Note that this is actually interrupt service routine, so do not
 * perform any lengthy operations here.
 *
 * The arg param will contain the channel register value for channel events
 * so it it not needed to call ReadChannel in input capture mode.
 *
 */
void TPM0_SignalEvent(uint32_t event, uint32_t arg)
{
	switch ( event )
	{
	// This event is signaled when timer TPM0 counter overflows
	case MSF_TPM_EVENT_TOF:
		overflow = 1;
		break;

	// This event is signaled when event (edge) on input pin for channel 0 is detected
	case MSF_TPM_EVENT_CH0:
		on_edge((uint16_t)arg);
		break;

	case MSF_TPM_EVENT_CH1:
		on_edge((uint16_t)arg);
		break;

	case MSF_TPM_EVENT_CH2:
		on_edge((uint16_t)arg);
		break;

	case MSF_TPM_EVENT_CH3:
		on_edge((uint16_t)arg);
		break;

	case MSF_TPM_EVENT_CH4:
		on_edge((uint16_t)arg);
		break;

	case MSF_TPM_EVENT_CH5:
		on_edge((uint16_t)arg);
		break;

	}

}

/* on_edge
 * Called when edge is detected on input pin.
*/
void on_edge(uint16_t arg)
{
	uint16_t stop;

	if ( start == 0 )
	{
		// 1st edge
		overflow = 0;
		start = (uint16_t)arg;
	}
	else
	{
		// when the main loop finishes processing the data, it will clear data_ready
		if ( data_ready == 0 )
		{
			stop = (uint16_t)arg;
			// 2nd edge
			if ( overflow )
				pulse_time = 0xffff - start + stop;
			else
				pulse_time = stop - start;

			data_ready = 1;
		}

		start = 0;
	}
}


/* ======================================================================== */

/* handler for the non-maskable (NMI) interrupt.
 * The PTA4 pin (Arduino pin 4) is by default used as NMI input and
 * connecting low signal to this pin causes NMI interrupt - the default handler
 * is just endless loop. This is no problem if we use the PTA4 pin, because we
 * configure it to a different mode than NMI and all works fine.
 * However, if we by mistake connect the signal to this pin while it is not used
 * in the program, the program will hang (go into the default handler).
*/
void NMI_Handler(void)
{
	// do nothing, just override the default handler which contains endless loop.
}


////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
