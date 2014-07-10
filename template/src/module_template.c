/****************************************************************************
 * @file     module_template.c
 * @brief    Template for modules
 * @version  1
 * @date     7 July 2014
 *
 * @note	This is template which should be used for any module in this library.
 *      
 ******************************************************************************/

/** @defgroup group_template Template module 
 * @details Source code template file
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
#include "module_template.h"


/* Defines section
* Add all internally used #defines here
*
***************************************************/
/* The name of the size for the buffer must match this pattern */
#define coniob_txQ_SIZE    CONIOB_TXBUFFER_SIZE
#define coniob_rxQ_SIZE    CONIOB_RXBUFFER_SIZE


/* Variables section
* Add all global variables here
*
***************************************************/
/*---------- Internal variables ---------------- */
volatile struct
{
	uint8_t     m_getIdx;
    uint8_t     m_putIdx;
    uint8_t     m_entry[ coniob_txQ_SIZE ];
} coniob_txQ;

volatile struct
{
	uint8_t     m_getIdx;
    uint8_t     m_putIdx;
    uint8_t     m_entry[ coniob_rxQ_SIZE ];
} coniob_rxQ;

/* the number of read bytes last time we updated the Rx FIFO */
volatile uint32_t coniob_nowSending;


/* Function Prototype Section
* Add prototypes for all functions called by this
* module, with the exception of runtime routines.
*
***************************************************/   
/* -------- Prototypes of internal functions   -------- */
void coniob_UART_SignalEvent(uint32_t event, uint32_t arg);
void wconiob_update_rxfifo(void);
void wconiob_update_txfifo(void);


/* Code section 
* Add the code for this module.
*
***************************************************/  
/* -------- Implementation of public functions   -------- */

/** @fn   void function(type param1) 
 * @brief Short description of the function
 * @param[in] param1 description of param1
 * @return none
 * @note  
 *  
 * Created by: JD   
 **/
 
 