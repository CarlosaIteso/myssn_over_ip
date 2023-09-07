/*
 * myssn_over_ip.c
 *
 *  Created on: 6 sep. 2023
 *      Author: guion
 */

#include "myssn_over_ip.h"

/*
 * Funcion main de myssn_over_ip
 * dataptr --- data cifrada
 */
void myssn_over_ip(void **dataptr,u16_t *len){
	bool myssn_over_ip_error = 1;
	//dataptr direccion
	//*dataptr data

	//comprobar si el mensaje es multpiplo de 16 + 4
	if ((*len % (16 + 4)) == 0) {

		//Config
		CRC_Type *base = CRC0;
		uint32_t checksum32; //resultado de CRC
		uint32_t checkCrc32; //resultado de CRC para comparar
		crc_config_t config;
		config.polynomial         = 0x04C11DB7U;
		config.seed               = param_seed;
		config.reflectIn          = true;
		config.reflectOut         = true;
		config.complementChecksum = true;
		config.crcBits            = kCrcBits32;
		config.crcResult          = kCrcFinalChecksum;

		CRC_Init(base, &config);

		//CRC32. como data = data + CRC (4 bytes) entonces datazise es el tamaño de dataptr - 1 byte - 4 bytes
		CRC_WriteData(base, (uint8_t *)dataptr[0], sizeof(dataptr) - 1 - 4);

		//checksum32 = CRC_Get32bitResult(base);
		myssn_over_ip_error = 1;
	}
	else {
		//Paquete no es multiplo de 16 + 4
		myssn_over_ip_error = 1;
		}
}
