/*
 * util.h
 *
 *  Created on: Feb 24, 2020
 *      Author: stoja
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_

#include "cmsis_os.h"

/* Debug flag */
#ifdef DEBUG
#undef DEBUG
#endif
/* Comment the next line in order to disable debug mode */
#define DEBUG

/* Functions */
uint8_t UsbPrint(const char *format, ...);


#ifdef DEBUG
//osMutexId_t print_mutex;
#define PRINT_BUFFER_LEN 200
char print_buffer[PRINT_BUFFER_LEN];
#endif

#endif /* INC_UTIL_H_ */
