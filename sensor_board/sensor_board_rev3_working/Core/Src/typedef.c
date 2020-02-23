/*
 * typedef.c
 *
 *  Created on: Jan 2, 2020
 *      Author: stoja
 */

#include "typedef.h"

uint8_t UsbPrint(const char *format, ...) {
	uint8_t ret = 1;
#ifdef DEBUG
	if (osMutexAcquire(print_mutex, 0U) == osOK) {
		va_list argptr;
		va_start(argptr, format);
		vsnprintf(print_buffer, PRINT_BUFFER_LEN, format, argptr);
		va_end(argptr);
		ret = CDC_Transmit_FS((uint8_t*) print_buffer, strlen(print_buffer));
		osMutexRelease(print_mutex);
	}
#endif
	return ret;
}
