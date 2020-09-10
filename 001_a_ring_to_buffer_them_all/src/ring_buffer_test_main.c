/**
 * @file ring_buffer_test_main.c
 * @author mv (mvidojevic82@gmail.com)
 * @brief Test for Ring Buffer
 * @version 0.1
 * @date 2020-09-05
 *
 * @copyright Copyright (c) 2020 - MV - GPLv3
 *
 */

#include <stdio.h>
#include <string.h>
#include "ring_buffer.h"

#define STORAGE_1_SIZE 22
#define STORAGE_2_SIZE 512
#define STORAGE_3_SIZE 256

rb_node_t storage_1[STORAGE_1_SIZE];
rb_node_t storage_2[STORAGE_2_SIZE];
rb_node_t storage_3[STORAGE_3_SIZE];

rb_ctx_t mybuf_1;
rb_ctx_t mybuf_2;
rb_ctx_t mybuf_3;

// -------------------

uint8_t temp_array[130] = "1234567890123456789012345678901234567890"
                          "assdasdadadasdadadaddadadasdasdadasdsada"
                          "assdasdadad\nsdadadaddadadasdasdadasdsada"
                          "123456789x";
uint32_t temp_res;

// ------------------

uint8_t tmp_buf[STORAGE_2_SIZE + 1];
uint8_t tmp_idx;

// --------------------

int main( int argc, char ** argv )
{
    if ( RB_OK != rb_initialize( &mybuf_1, storage_1, STORAGE_1_SIZE ) )
    {
        printf( "\r\nBuffer 1 init err" );
    }
    if ( RB_OK != rb_initialize( &mybuf_2, storage_2, STORAGE_2_SIZE ) )
    {
        printf( "\r\nBuffer 2 init err" );
    }
    if ( RB_OK != rb_initialize( &mybuf_3, storage_3, STORAGE_3_SIZE ) )
    {
        printf( "\r\nBuffer 3 init err" );
    }

    for ( ; ; )
    {
        if ( 0 == ( temp_res = rb_write( &mybuf_3, temp_array, 130 ) ) )
        {
            printf( "\r\nCan't write no more" );
            break;
        }
        else
        {
            printf( "\r\nBuffer 3 bytes written : %d", temp_res );
        }
    }

    // ----------- Exam from From readme.

    printf( "\r\nExam..." );
    memset( tmp_buf, 0, sizeof( tmp_buf ) );
    tmp_idx = 0;

    for ( ; ; )
    {
        //  Poll Ring buffer 2.

        if ( RB_OK == rb_get( &mybuf_2, &tmp_buf[ tmp_idx ] ) )
        {
            if ( '\n' == tmp_buf[ tmp_idx ] )
            {
                //  Print the content and reset temporary storage.

                printf( "%s", tmp_buf );
                memset( tmp_buf, 0, sizeof( tmp_buf ) );
                tmp_idx = 0;
            }
            else
            {
                tmp_idx++;
            }
        }
    }

    return 0;
}

void __SOME_UART_ISR ( void )
{
    uint8_t tmp;

    //  Store content of the UART RX register to the tmp variable.
    //  ...

    rb_put( &mybuf_2, tmp );

    //  Clean UART interrupt flag.
    //  ...
}
