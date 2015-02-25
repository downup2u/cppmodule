/*
 * Authors:
 * - Filippo Sironi (filippo.sironi@gmail.com)
 * - Matteo Villa (villa.matteo@gmail.com)
 */

#ifndef __AES_H
#define __AES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char uint8_t;
#define ROUND_TO_SIZE_AGLIN(_length, _alignment)                      \
	((((size_t)(_length)) + ((_alignment)-1)) & ~(size_t)((_alignment)-1))

void ecb_aes_encrypt(const uint8_t plain_text[], const unsigned int plain_text_size, uint8_t cipher_text[], const uint8_t key[]);
void ecb_aes_decrypt(const uint8_t cipher_text[], const unsigned int cipher_text_size, uint8_t plain_text[], const uint8_t key[]);

#ifdef __cplusplus
}
#endif

#endif
