/*
 * myssn_over_ip.h
 *
 *  Created on: 6 sep. 2023
 *      Author: guion
 */

#ifndef MYSSN_OVER_IP_H_
#define MYSSN_OVER_IP_H_

#define param_seed 0xFFFFFFFFU

#include "stdint.h"
#include "stdbool.h"
//CRC
#include "fsl_crc.h"

//memcpy
#include "string.h"

//PRINTF
#include "fsl_debug_console.h"

void myssn_over_ip(void **dataptr,u16_t *len);

#endif /* MYSSN_OVER_IP_H_ */
