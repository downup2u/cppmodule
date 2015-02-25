/*
 * Authors:
 * - Filippo Sironi (filippo.sironi@gmail.com)
 * - Matteo Villa (villa.matteo@gmail.com)
 */

#include <string.h>

#include "aes.h"
#include "aes-core.h"	
	
void ecb_aes_encrypt(const uint8_t plain_text[], const unsigned int plain_text_size, uint8_t cipher_text[], const uint8_t key[])
{
	size_t deal_size = 0;
	uint8_t key_sched[176] = { 0 };
	unsigned int i;
	deal_size = ROUND_TO_SIZE_AGLIN(plain_text_size, 16);
	aes_keyexp(key, key_sched);
	for (i = 0; i < deal_size; i += 16){
		aes_encrypt(plain_text + i, cipher_text + i, key_sched);
	}
}

void ecb_aes_decrypt(const uint8_t cipher_text[], const unsigned int cipher_text_size, uint8_t plain_text[], const uint8_t key[])
{
	size_t deal_size = 0;
	uint8_t key_sched[176] = { 0 };
	unsigned int i;
	deal_size = ROUND_TO_SIZE_AGLIN(cipher_text_size, 16);
	aes_keyexp(key, key_sched);
	for (i = 0; i < deal_size; i += 16){
		aes_decrypt(cipher_text + i, plain_text + i, key_sched);
	}
}
