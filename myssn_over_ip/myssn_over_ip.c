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

	//vector incial
	uint8_t iv[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	//clave
	uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };

	const uint8_t *data;
	uint32_t *crc;
	uint32_t crc_result;

	// comprobar si el mensaje es múltiplo de 16 + 4
	if ((*len % (16 + 4)) == 0) {

		// dividir mensaje en data y crc
		data = (uint8_t *)*dataptr;
		crc = (uint32_t *)((uint8_t *)*dataptr + 16);

		//configurar CRC
		CRC_Type *base = CRC0;
		crc_config_t config;
		config.polynomial         = 0x04C11DB7U;
		config.seed               = param_seed;
		config.reflectIn          = true;
		config.reflectOut         = true;
		config.complementChecksum = true;
		config.crcBits            = kCrcBits32;
		config.crcResult          = kCrcFinalChecksum;

		//iniciar crc
		CRC_Init(base, &config);

		//calcular crc
		CRC_WriteData(base, data, 16);

		//imprimir crc recibido
		PRINTF("crc recibido: \n");
		for (int i = 0; i < 4; i++) {
			uint8_t crc_result_byte = (*crc >> (i * 8)) & 0xFF;
			PRINTF("%02X ", crc_result_byte);
		}
		PRINTF("\n");

		//obtener el resultado de crc
		crc_result = CRC_Get32bitResult(base);

		//imprimir resultado del crc calculado
		PRINTF("crc calculado: \n");
	    for (int i = 0; i < 4; i++) {
	        uint8_t crc_result_byte = (crc_result >> (i * 8)) & 0xFF;
	        PRINTF("%02X ", crc_result_byte);
	    }
	    PRINTF("\n");

		//comprobar resultado crc
		if (crc_result == *crc){

			struct AES_ctx ctx;

			//inicializar ctx
			AES_init_ctx_iv(&ctx, key, iv);

			// imprimir data byte por byte
			PRINTF("Mensaje cifrado:\n");
			for (uint8_t i = 0; i < 16; i++) {
				PRINTF("%02X ", data[i]);
			}
			PRINTF("\n");

			//desencriptar data
			AES_CBC_decrypt_buffer(&ctx, (uint8_t *) data, 16);

			//imprimir data descifrfada byte por byte
			PRINTF("Mensaje descifrado:\n");
			for (size_t i = 0; i < 16; i++) {
			    PRINTF("%02X ", data[i]);
			}
			PRINTF("\n");

			//inicializar ctx
			AES_init_ctx_iv(&ctx, key, iv);

			//encriptar
			AES_CBC_encrypt_buffer(&ctx, (uint8_t *) data, 16);

			//imprimir data encryptada
			PRINTF("data cifrado:\n");
			for (uint8_t i = 0; i < 16; i++) {
				PRINTF("%02X ", data[i]);
			}
			PRINTF("\n");

			//iniciar crc
			CRC_Init(base, &config);

			//calcular crc
			CRC_WriteData(base, data, 16);

			//obtener el resultado del crc
			crc_result = CRC_Get32bitResult(base);

			//imprimir resultado del crc calculado
			PRINTF("crc calculado: \n");
			for (int i = 0; i < 4; i++) {
				uint8_t crc_result_byte = (crc_result >> (i * 8)) & 0xFF;
				PRINTF("%02X ", crc_result_byte);
			}
			PRINTF("\n");

            // Actualizar data y crc en dataptr
            *dataptr = (void *)data;
            ((uint32_t *)*dataptr+4) = crc;

            //imprimir mensaje
            const uint8_t *byteptr = (const uint8_t *)*dataptr;

            for (size_t i = 0; i < 20; i++) {
                PRINTF("%02X ", byteptr[i]);
            }
            PRINTF("\n");

			return 1;

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
