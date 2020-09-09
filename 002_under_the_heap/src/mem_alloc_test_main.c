/**
 * @file mem_alloc_test_main.c
 * @author mv (mvidojevic82@gmail.com)
 * @brief Test for Memory Allocator
 * @version 0.1
 * @date 2020-09-05
 *
 * @copyright Copyright (c) 2020 - MV - GPLv3
 *
 */

#include <stdio.h>
#include "mem_alloc.h"

int main( int argc, char ** argv )
{
    mem_initialize( );

    uint8_t * ptr_a = mem_alloc( 44 );
    uint8_t * ptr_b = mem_alloc( 254 );
    uint8_t * ptr_c = mem_alloc( 700 );
    uint8_t * ptr_d = mem_alloc( 200 );

    printf( "\r\nA : %08x", ptr_a );
    printf( "\r\nB : %08x", ptr_b );
    printf( "\r\nC : %08x", ptr_c );
    printf( "\r\nD : %08x", ptr_d );

    mem_free( ptr_b );
    mem_free( ptr_c );

    uint16_t ptr_e = mem_alloc( 254 );

    printf( "\r\nA : %08x", ptr_a );
    printf( "\r\nB : %08x", ptr_b );
    printf( "\r\nC : %08x", ptr_c );
    printf( "\r\nD : %08x", ptr_d );
    printf( "\r\nE : %08x", ptr_e );


    return 0;
}
