/****************************************************************************
 * @file     msf_print.c
 * @brief    Print to serial line (console) - MSF global functions 
 * @version  1
 * @date     23. Apr 2014
 *
 * @note    Prototypes of these functions are in msf.h
 *      Uses msf conio driver to output the strings. 
 *
 ******************************************************************************/

#include "msf_config.h"                    

/* Include hardware definitions */
#include "coredef.h"   

#include "msf.h"
#include "conio.h"  /* generic console driver */
#include <stdio.h> /* for sprintf */


/** print line of text.
 * @note Supports printing new line is "\n" is in the string.
 *   msf_init should be called before using any of the msf_print 
 *   functions. 
 **/
void msf_print(const char* str)    // print string
{
    conio_puts(str);
} 

/** Print simple integer (as with sprintf %d) 
**/
void msf_printnum(uint32_t number) 
{
    char buffer[12];
    sprintf(buffer, "%ld", number);
    conio_puts(buffer);    
}

/** Print simple integer as hexadecimal number (as with sprintf %x)
**/
void msf_printhex(uint32_t number)
{
    char buffer[9];
    sprintf(buffer, "%lx", number);
    conio_puts(buffer);         
}



/** print string with one formatted 16-bit number 
* @note he number converted to string must not exceed 8 characters!
*  For example "1234" is OK; "123456789" is NOT OK.
**/
void msf_printf_16(const char* str, const char* format, uint16_t data)  
{
    char buffer[9];
    sprintf(buffer, format, data);
    conio_puts(str);
    conio_putch(' ');
    conio_puts(buffer);    
}
/** print string with one formatted 32-bit number 
* @note The number converted to string must not exceed 11 characters!
**/
void msf_printf32(const char* str, const char* format, uint32_t data)  
{
    char buffer[12];
    sprintf(buffer, format, data);
    conio_puts(str);
    conio_putch(' ');
    conio_puts(buffer);    
} 

/** print string with one real number (float) 
* @note The number converted to string must not exceed 11 characters!
**/
void msf_printf_real(const char* str, const char* format, double data)  
{
    char buffer[12];
    sprintf(buffer, format, data);
    conio_puts(str);
    conio_putch(' ');
    conio_puts(buffer);    
} 


/** Read character from serial interface. Block the caller until char is received */
char msf_read_char(void)
{
    return conio_getch();    
}

/** Check if character was received through serial line */
bool msf_char_available(void)
{
    return (conio_kbhit() > 0);
}


