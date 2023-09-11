/*
 * myssn_over_ip.h
 *
 *  Created on: 6 sep. 2023
 *      Author: Carlos Zepeda
 */

#include "myssn_over_ip.h"

/**
 * @brief Función myssn_over_ip
 *
 * La función `myssn_over_ip` se utiliza para procesar paquetes de datos que contienen información cifrada y un valor CRC (Cyclic Redundancy Check) para verificar la integridad de los datos. La función realiza la validación del CRC, desencripta y luego vuelve a encriptar los datos utilizando AES (Advanced Encryption Standard) en modo CBC (Cipher Block Chaining).

 * @param dataptr Puntero a un puntero de tipo void que apunta a los datos de entrada y al valor CRC. La función modificará los datos de entrada en este puntero si se procesan correctamente.
 * @param len Puntero a un valor uint16_t que indica la longitud de los datos de entrada.
 *
 * @return uint8_t Un valor entero sin signo (1 o 0) que indica el resultado del procesamiento:
 * - 1: Si el procesamiento fue exitoso y el CRC se validó correctamente.
 * - 0: Si se produjo un error durante el procesamiento, como un CRC no válido o una longitud de datos incorrecta.
 *
 * La función realiza las siguientes operaciones:
 * 1. Valida si la longitud de los datos es un múltiplo de 16 bytes más 4 bytes.
 * 2. Calcula el CRC de los datos y lo compara con el CRC recibido.
 * 3. Si el CRC coincide, desencripta los datos utilizando AES en modo CBC y luego los vuelve a encriptar.
 * 4. Actualiza el CRC en los datos procesados.
 * 5. Retorna 1 si el procesamiento fue exitoso, 0 en caso contrario.
 *
 * Es importante asegurarse de que las bibliotecas y funciones adecuadas de CRC y AES estén disponibles y sean utilizadas en la implementación real de esta función.
 *      Author: Carlos Zepeda
 */

uint8_t myssn_over_ip(void **dataptr,uint16_t *len){

	//vector incial
	const uint8_t iv[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	//clave
	const uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };

	const uint8_t *data;
	uint32_t *crc;
	uint32_t crc_result;
	uint8_t byte;
	//To-do implementar un handleo para fragmentacion de paquetes

	//bloque de 16 bytes en dataptr
	uint8_t data_blocks = (*len-*len % 16)/16;
	PRINTF("\nBloques de data (16-bytes):\n%d\n",data_blocks);

	// comprobar si el mensaje es múltiplo de 16
	if (*len % 16 == 4) {

		// dividir mensaje en data y crc
		data = (uint8_t *)*dataptr;
		crc = (uint32_t *)((uint8_t *)*dataptr + data_blocks*16);

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

		//imprimir crc recibido
		PRINTF("crc recibido: \n");
		for (int i = 0; i < 4; i++) {
			uint8_t byte = (*crc >> (i * 8)) & 0xFF;
			PRINTF("%02X ", byte);
		}
		PRINTF("\n");

		//iniciar crc
		CRC_Init(base, &config);

		//calcular crc
		CRC_WriteData(base, data, data_blocks*16);

		//obtener el resultado de crc
		crc_result = CRC_Get32bitResult(base);

		//imprimir resultado del crc calculado
		PRINTF("crc calculado: \n");
	    for (int i = 0; i < 4; i++) {
	        uint8_t byte = (crc_result >> (i * 8)) & 0xFF;
	        PRINTF("%02X ", byte);
	    }
	    PRINTF("\n");

		//comprobar resultado crc
		if (crc_result == *crc){

			// imprimir data byte por byte
			PRINTF("data cifrada:\n");
			for (uint8_t i = 0; i < (data_blocks*16); i++) {
				PRINTF("%02X ", data[i]);
			}
			PRINTF("\n");

			//inicializar ctx
			struct AES_ctx ctx;
			AES_init_ctx_iv(&ctx, key, iv);

			//desencryptar por bloques de 16-bytes
			for (uint8_t i = 0; i < (data_blocks); i++){

				//desencriptar data
				AES_CBC_decrypt_buffer(&ctx, (uint8_t *) (data + i*16) , 16);
			}

			//imprimir data descifrfada byte por byte
			PRINTF("data descifrada:\n");
			for (uint8_t i = 0; i < (data_blocks*16); i++) {
			    PRINTF("%02X ", data[i]);
			}
			PRINTF("\n");

			//inicializar ctx
			AES_init_ctx_iv(&ctx, key, iv);

			//encryptar por bloques de 16-bytes
			for (uint8_t i = 0; i < (data_blocks); i++){

				//desencriptar data
				AES_CBC_encrypt_buffer(&ctx, (uint8_t *) (data + i*16) , 16);
			}

			//imprimir data encryptada
			PRINTF("data cifrada:\n");
			for (uint8_t i = 0; i < (data_blocks*16); i++) {
				PRINTF("%02X ", data[i]);
			}
			PRINTF("\n");

			//iniciar crc
			CRC_Init(base, &config);

			//calcular crc
			CRC_WriteData(base, data, data_blocks*16);

			//obtener el resultado del crc
			crc_result = CRC_Get32bitResult(base);

			//imprimir resultado del crc calculado
			PRINTF("crc calculado: \n");
			for (uint8_t i = 0; i < 4; i++) {
				uint8_t crc_result_byte = (crc_result >> (i * 8)) & 0xFF;
				PRINTF("%02X ", crc_result_byte);
			}
			PRINTF("\n");

            // Actualizar data y crc en dataptr
            //*dataptr = (void *)data;
            memcpy((uint8_t *)*dataptr + data_blocks * 16, &crc_result, sizeof(uint32_t));

            //imprimir mensaje
            PRINTF("Mensaje (data + crc) para enviar: \n");
            const uint8_t *byteptr = (const uint8_t *)*dataptr;

            for (uint16_t i = 0; i < *len; i++){
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
