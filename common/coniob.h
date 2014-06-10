/****************************************************************************
 * @file     coniob.h
 * @brief    Console I/O driver with internal BUFFER; generic definitions for all platforms 
 * @version  1
 * @date     10. June 2014
 *
 * @note        
 *
 ******************************************************************************/
#ifndef MSF_CONIOB_H
#define MSF_CONIOB_H

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup group_coniob  
 * @{
*/    

/* initialize console I/O */
void coniob_init(void);  
/* send one character to console */
void coniob_putch(char c);        
/* send null-terminated string */
void coniob_puts(const char* str);     
/* read one character (blocking) */ 
char coniob_getch(void);               
/* read string from console. */
void coniob_gets(char* str, uint8_t max_chars, char terminator);		 
/* check if data are available in input buffer */ 
uint8_t coniob_kbhit(void);             


/** @} */
#ifdef __cplusplus
}
#endif
/* ----------- end of file -------------- */
#endif /* MSF_CONIOB_H */
