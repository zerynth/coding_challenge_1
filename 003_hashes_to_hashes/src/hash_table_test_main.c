/**
 * @file hast_table_test_main.c
 * @author mv (mvidojevic82@gmail.com)
 * @brief Test for Hash Table
 * @version 0.1
 * @date 2020-09-05
 *
 * @copyright Copyright (c) 2020 - MV - GPLv3
 *
 * ---
 *
 * Just simple demonstration of the Hash Table implementation.
 *
 */

#include <stdio.h>
#include "hash_table.h"

ht_ctx_t table_1;
ht_ctx_t table_2;

hash_t
custom_hash_calculator( char * key )
{
    uint16_t i;
    hash_t hash = 0x55;

    for ( i = 0; key[i] != 0; ++i )
    {
        //  Some custom algorithm.

        hash <<= ( key[i] % 4 );
        hash *= key[i];
    }

    return hash;
}

int main( int argc, char ** argv )
{
    printf( "Hash Table Test\r\n\r\n" );

    //  Testing table 1 with default hasher.

    ht_initialize( &table_1, NULL );
    ht_insert( &table_1, "word1", "ALO" );
    ht_insert( &table_1, "word2", "AOL" );
    ht_insert( &table_1, "word3", "OLA" );
    ht_insert( &table_1, "word4", "Popocatepetl" "Mexico" "Sombrero" "Tequila" );
    ht_insert( &table_1, "word5", "hi" );
    ht_insert( &table_1, "word6", "ih" );
    ht_print( &table_1 );

    ht_insert( &table_1, "word2", "aol" );
    ht_insert( &table_1, "word3", "ola" );
    ht_delete( &table_1, "word4" );
    ht_delete( &table_1, "word5" );
    ht_print( &table_1 );

    printf( "\r\n\r\n" );
    printf( "Word 1 is : %s\r\n", ht_retreive( &table_1, "word1" ) );
    printf( "Word 4 is : %s\r\n", ht_retreive( &table_1, "word4" ) );
    printf( "Word 6 is : %s\r\n", ht_retreive( &table_1, "word6" ) );

    //  Testing table 2 with custom hasher.

    ht_initialize( &table_2, custom_hash_calculator );
    ht_insert( &table_2, "word1", "ALO" );
    ht_insert( &table_2, "word2", "AOL" );
    ht_insert( &table_2, "word3", "OLA" );
    ht_insert( &table_2, "word4", "Popocatepetl" "Mexico" "Sombrero" "Tequila" );
    ht_insert( &table_2, "word5", "hi" );
    ht_insert( &table_2, "word6", "ih" );
    ht_print( &table_2 );

    ht_insert( &table_2, "word2", "aol" );
    ht_insert( &table_2, "word3", "ola" );
    ht_delete( &table_2, "word4" );
    ht_delete( &table_2, "word5" );
    ht_print( &table_2 );

    printf( "\r\n\r\n" );
    printf( "Word 1 is : %s\r\n", ht_retreive( &table_2, "word1" ) );
    printf( "Word 4 is : %s\r\n", ht_retreive( &table_2, "word4" ) );
    printf( "Word 6 is : %s\r\n", ht_retreive( &table_2, "word6" ) );


    return 0;
}



