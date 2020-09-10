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
    uint8_t * ptr_c = mem_alloc( 698 );
    uint8_t * ptr_d = mem_alloc( 200 );

    printf( "\r\nA : %x", ptr_a );
    printf( "\r\nB : %x", ptr_b );
    printf( "\r\nC : %x", ptr_c );
    printf( "\r\nD : %x", ptr_d );
    mem_print( );

    mem_free( ptr_b );
    mem_print( );
    //mem_free( ptr_c );

    uint16_t * ptr_e = mem_alloc( 254 );

    printf( "\r\nA : %x", ptr_a );
    printf( "\r\nB : %x", ptr_b );
    printf( "\r\nC : %x", ptr_c );
    printf( "\r\nD : %x", ptr_d );
    printf( "\r\nE : %x", ptr_e );
    mem_print( );

    mem_free( ptr_c );
    mem_print( );

    uint16_t * ptr_f = mem_alloc( 254 );
    uint16_t * ptr_g = mem_alloc( 254 );

    printf( "\r\nA : %x", ptr_a );
    printf( "\r\nB : %x", ptr_b );
    printf( "\r\nC : %x", ptr_c );
    printf( "\r\nD : %x", ptr_d );
    printf( "\r\nE : %x", ptr_e );
    printf( "\r\nF : %x", ptr_f );
    printf( "\r\nG : %x", ptr_g );
    mem_print( );

    return 0;
}
