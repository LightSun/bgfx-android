/*
blowfish.h:  Header file for blowfish.c

Copyright (C) 1997 by Paul Kocher

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


See blowfish.c for more information about this file.
*/

/*************************** HEADER FILES ***************************/
#include <stddef.h>

/****************************** MACROS ******************************/
#define BLOWFISH_BLOCK_SIZE 8           // Blowfish operates on 8 bytes at a time

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;             // 8-bit byte
typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

typedef struct{
    WORD p[18];
    WORD s[4][256];
}BLOWFISH_KEY;

/*********************** FUNCTION DECLARATIONS **********************/
void blowfish_key_setup(const BYTE user_key[], BLOWFISH_KEY *keystruct, size_t len);
void blowfish_encrypt(const BYTE in[], BYTE out[], const BLOWFISH_KEY *keystruct);
void blowfish_decrypt(const BYTE in[], BYTE out[], const BLOWFISH_KEY *keystruct);

void BF_de(BLOWFISH_KEY *ctx, const char* key, int keyLen, const char* in, char* out, int len);
void BF_en(BLOWFISH_KEY *ctx, const char* key, int keyLen, const char* in, char* out, int inLen);