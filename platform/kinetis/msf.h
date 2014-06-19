/****************************************************************************
 * @file     msf.h
 * @brief    root definitions for the framework 
 * @version  1
 * @date     31. Jan 2014
 *
 * @note     Main header file for MSF.  
 *  The implementation should be mostly in platform\...\msf.c but can be also 
 *  in other files.
 *
 ******************************************************************************/
#ifndef MSF_MSF_H
#define MSF_MSF_H

#ifndef MSF_COREDEF_H
	#error Please include the coredef.h file before including msf.h
#endif

/* include the core definitions for this platform */
#include "msf_kinetis.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup group_errors Error Codes 
* @details MSF error codes
* @{ 
*/
#define     MSF_ERROR_OK            (0)     /**< success; no error! */
#define     MSF_ERROR_UNKNOWN       (1)     /**< error without further details */
#define     MSF_ERROR_NOTFOUND      (2)     /**< the object does not exit (is not valid) */
#define     MSF_ERROR_NOTSUPPORTED  (3)     /**< required option is not supported  */
#define		MSF_ERROR_SYSTIMER		(4)		/**< failed to initialise system timer  */
#define     MSF_ERROR_ARGUMENT      (6)     /**< invalid input argument for a function */
#define     MSF_ERROR_NOINIT        (7)     /**< device was not properly initialized */

#define     MSF_ERROR_MAXDWORD      (0xffffffff)    /**< special error indication used by ADC_Read and TPM_ReadChannel */

/** @} */



/**********************************************
* Basic functions available in MSF API 1
**********************************************/
#include "gpio.h"

/** @addtogroup group_globals  
* @{ 
**/
/** Initialize the MSF. 
 It may do nothing on some platforms for API level 1 but, for example,
 kinetis needs enabling clock to GPIO ports. */
uint8_t     msf_init(uint32_t param);
/* @} */

/** @defgroup group_msf_digitalIO Digital I/O 
* @details MSF Digital I/O functions
* @{ 
*/
/* Note 1: 
The GPIO funcitons are defined static inline in gpio.h
We include gpio.h here for convenience; this way the user does not need to include it
to be able to use the I/O functions.  

Note 2:
We use macros rather than inline functions for the MSF version here, because 
INLINE definition is compiler-specific, so this code would need #if compiler is XY...
*/ 
#define msf_pin_direction(pin, dir)       gpio_direction(pin, dir) 
#define msf_pin_pullmode(pin, mode)  gpio_pullmode(pin, mode) 
#define msf_pin_write(pin, value)         gpio_write(pin, value)
#define msf_pin_read(pin)                   gpio_read(pin)
#define msf_pin_toggle(pin)                gpio_toggle(pin)
/* @} **/


/*****************************************************
* Functions for obtaining system time and variable delays 
*****************************************************/
#if MSF_TIME_API

/** @defgroup group_globals MSF global functions 
* @details MSF global functions
* @{ 
**/

uint32_t    msf_millis(void);
uint32_t    msf_micros(void);
void        msf_delay_us(uint32_t micros); 
void        msf_delay_ms(uint32_t millis);
/* @} **/

#endif /* MSF_TIME_API */


/*****************************************************
* Functions for very short delays (e.g. 1 us) 
*****************************************************/
#if MSF_SHORT_DELAY_API
/** Very short delay functions - implemented as inline functions in delay_util.h.  */
/* MSF_DELAY_xUS	- Wait x microseconds. X may be 1, 5, 10, etc.
The available values of X depend on F_CPU. See the included file for the 
actually available functions for your device (F_CU in fact) */
#include "delay_util.h"

#endif  /* MSF_SHORT_DELAY_API */



/*****************************************************
* Functions for reading and writing to SCI 
*****************************************************/
#if MSF_PRINT_API
/** @defgroup group_msf_print MSF text functions 
* @details MSF functions for printing text to console and reading text.
* @{ 
**/
/* Implemented in msf_print.c */
/* The Conio driver decides to which UART the output goes */
void msf_print(const char* str);    // print string  
void msf_print_char(char c);	// print single character

/* Easy to use functions */
/* Print simple integer in decimal and hex */
void msf_printnum(uint32_t number); 
void msf_printhex(uint32_t number);  

/* More flexible functions */
/** print string and one formatted number */
void msf_printf16(const char* str, const char* format, uint16_t data);
void msf_printf32(const char* str, const char* format, uint32_t data);
void msf_printf_real(const char* str, const char* format, double data);

/* Reading characters */
char msf_read_char(void); 
bool msf_char_available(void);

/* @} **/
#endif /* MSF_PRINT_API */



/*****************************************************
* Functions for reading analog values 
*****************************************************/
#if MSF_ANALOG_API
/** @defgroup group_msf_analog MSF analog I/O functions 
* @details MSF functions for reading analog values 
* @{ 
**/
    /** Read value from ADC **/ 
	uint16_t msf_analog_read(Analog_pin_t apin);
/* @} **/
#endif /* MSF_ANALOG_API */



/*****************************************************
* Functions for software timer
*****************************************************/
#if MSF_SWTIMER_API
/** @defgroup group_msf_swtimer MSF software timer  
* @details MSF software timer functions
* @{ **/
/* Software timer support 
 Implemented in sw_timer.c */
/** User-defined function which is called when timer fires.
The prototype is: void some_func(void);   
*/
typedef void (*MSF_TIMER_FUNC)(void);

/** @brief timer data */
typedef struct msf_sw_timer 
{
   uint16_t         period;    /**< time in ms for the timer to fire */
   uint16_t         cnt;        /**< helper variable */
   MSF_TIMER_FUNC   handler;    /**< user routine to call when timer fires */
   struct msf_sw_timer* next; /**< pointer to next element in list */ 
} MSF_TIMER;

uint8_t     msf_timer_set(MSF_TIMER* tdata, uint16_t period, MSF_TIMER_FUNC callback);
void        msf_timer_kill(MSF_TIMER* tdata);
void        msf_timer_stop(MSF_TIMER* tdata);
uint8_t     msf_timer_restart(MSF_TIMER* tdata);

/* @} **/
#endif    /* MSF_SWTIMER_API */

/** Power states used for drivers */
typedef enum 
{
    msf_power_off,  /**< power off */
    msf_power_low,  /**< minimal power, wake-up with external event */
    msf_power_full, /**< power on; normal operation */
    
} MSF_power_state;


#ifdef __cplusplus
}
#endif
/* ----------- end of file -------------- */
#endif /* MSF_MSF_H */
