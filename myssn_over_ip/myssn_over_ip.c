/*
 * myssn_over_ip.c
 *
 *  Created on: 6 sep. 2023
 *      Author: guion
 */

#include "myssn_over_ip.h"

//funcion: myssn_over_ip
//dataptr -- mensaje completo
uint8_t myssn_over_ip(void **dataptr,uint16_t *len){
	const uint8_t *message;
	uint32_t *crc;
	uint32_t crcResult;

	// Comprobar si el mensaje es múltiplo de 16 + 4
	if ((*len % (16 + 4)) == 0) {

		// Divide el buffer en "message" (16 bytes) y "crc" (4 bytes)
		message = (uint8_t *)*dataptr;
		crc = (uint32_t *)((uint8_t *)*dataptr + 16);

		//Configuracion CRC
		CRC_Type *base = CRC0;
		crc_config_t config;
		config.polynomial         = 0x04C11DB7U;
		config.seed               = param_seed;
		config.reflectIn          = true;
		config.reflectOut         = true;
		config.complementChecksum = true;
		config.crcBits            = kCrcBits32;
		config.crcResult          = kCrcFinalChecksum;

		//Iniciar CRC
		CRC_Init(base, &config);

		//Realizar CRC
		CRC_WriteData(base, message, 16);

		//obtener el resultado del CRC
		crcResult = CRC_Get32bitResult(base);

		//comprobar resultado CRC
		if (crcResult == *crc){
			PRINTF("\n crc coincide");


			//decodificar

		}
		else{
			//crc no valido
			PRINTF("\n crc no coincide");
			return 0;
		}
	}
	else {
		//paquete no es multiplo de 16 + 4
		return 0;
		}
}
