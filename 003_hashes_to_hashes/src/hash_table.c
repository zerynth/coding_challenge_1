/**
 * @file hash_table.c
 * @author mv (mvidojevic82@gmail.com)
 * @brief Hash Table
 * @version 0.1
 * @date 2020-09-05
 *
 * @copyright Copyright (c) 2020 - GPLv3
 *
 */

#include "hash_table.h"

// ------------------------------------------------------ PRIVATES DECLARATIONS

static hash_t
default_hash_calculator( char * key );

// --------------------------------------------------------- EXPORTED FUNCTIONS

ht_result_t
ht_initialize( ht_ctx_t * ctx, ht_calculate_t hasher )
{

}

ht_result_t
ht_insert( char * key, char * val )
{

}

void
ht_delete( char * key )
{

}

char *
ht_retreive( char * key )
{

}

// ---------------------------------------------------- PRIVATES IMPLEMENTATION

static hash_t
default_hash_calculator( char * key )
{

}

// ---------------------------------------------------------------- END OF FILE