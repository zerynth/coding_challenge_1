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

    //  Testing table 1

    ht_initialize( &table_1, NULL );
    ht_insert( &table_1, "user 1", "Miki Manojlovic" );
    ht_insert( &table_1, "user 2", "Paja Jovanovic" );
    ht_insert( &table_1, "user 3", "Petar Bozovic" );
    ht_insert( &table_1, "user 4", "Grof Drakula - Zamak u Transilvaniji "
                         "daleko u sumi gore" );
    ht_insert( &table_1, "user 3", "Petar Bozovic 2" );
    ht_insert( &table_1, "user 5", "Bata Zivotinja" );
    ht_print( &table_1 );
    ht_delete( &table_1, "user 1" );
    ht_delete( &table_1, "user 2" );
    printf( "User 3 : %s\r\n", ht_retreive( &table_1, "user 3" ) );
    printf( "User 5 : %s", ht_retreive( &table_1, "user 5" ) );
    ht_print( &table_1 );

    //  Testing table 2 with custom hasher.

    ht_initialize( &table_2, custom_hash_calculator );
    ht_insert( &table_2, "user 1", "Miki Manojlovic" );
    ht_insert( &table_2, "user 2", "Paja Jovanovic" );
    ht_insert( &table_2, "user 3", "Petar Bozovic" );
    ht_insert( &table_2, "user 4", "Grof Drakula - Zamak u Transilvaniji "
                         "daleko u sumi gore" );
    ht_insert( &table_2, "user 3", "Petar Bozovic 2" );
    ht_insert( &table_2, "user 5", "Bata Zivotinja" );
    ht_print( &table_2 );
    ht_delete( &table_2, "user 1" );
    ht_delete( &table_2, "user 2" );
    printf( "User 3 : %s\r\n", ht_retreive( &table_2, "user 3" ) );
    printf( "User 5 : %s\r\n", ht_retreive( &table_2, "user 5" ) );
    ht_print( &table_2 );

    return 0;
}



