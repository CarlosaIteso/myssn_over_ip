/*
 * myssn_over_ip.h
 *
 *  Created on: 6 sep. 2023
 *      Author: Carlos Zepeda
 */

#ifndef MYSSN_OVER_IP_H_
#define MYSSN_OVER_IP_H_

#define param_seed 0xFFFFFFFFU

#include "stdint.h"
#include "stdbool.h"

//aes
#include "aes.h"

//crc
#include "fsl_crc.h"

//memcpy
#include "string.h"

//PRINTF
#include "fsl_debug_console.h"

uint8_t myssn_over_ip(void **dataptr,uint16_t *len);

#endif /* MYSSN_OVER_IP_H_ */
