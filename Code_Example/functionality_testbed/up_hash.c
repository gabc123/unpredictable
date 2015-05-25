//
//  up_hash.c
//  up_game
//
//  Created by Waleed Hassan on 2015-05-08.
//  Copyright (c) 2015 Waleed Hassan. All rights reserved.
//
#include "sha256.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


// takes in password or usrName and puts it in hashed_text

int up_hashText(char hashed_text[],char text[],int length){
    //printf("hash : %s\n ", text);
    //some good number value
    BYTE hash[SHA256_BLOCK_SIZE] = {0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,
                        0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad};
    
    SHA256_CTX ctx;
    BYTE buffer[SHA256_BLOCK_SIZE];

    sha256_init(&ctx);
    // takes in password or usrName and puts it in ctx structure with the hash hexNumbers values
    sha256_update(&ctx, (unsigned char *)text,length);
    for(int i = 0; i<10000; i++){
    
        sha256_update(&ctx, hash,length);

    }
    //result of the hashTranformation
    sha256_final(&ctx, buffer);
    

    
    memcpy(hashed_text, buffer,SHA256_BLOCK_SIZE);
    
    // for debugger use
//    for (int i=0; i<SHA256_BLOCK_SIZE; i++) {
//        printf(" %d", hashed_text[i]);
//    }
//    printf("hash : %s\n ", hashed_text);
//    printf("hash : %s\n ", hashed_text);
//    printf("hash : %s\n ", hashed_text);
//    //strncpy(text,hashed_text);
    
   return 1;
}