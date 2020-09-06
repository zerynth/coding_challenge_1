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

#include <string.h>
#include <stddef.h>
#include "hash_table.h"

// ------------------------------------------------------------ PRIVATES MACROS

#define ASSERT(x)       //  Add assertion if needed here.

#if ( HT_DEBUG == 1 )
#ifdef __linux__
#include <stdio.h>
#define PRINTF(t,...)   printf((t),##__VA_ARGS__)
#else
#define PRINTF(t,...)   //  Add print through serial for your device here.
#endif
#else
#define PRINTF(t,...)
#endif
// ------------------------------------------------------ PRIVATES DECLARATIONS

/**
 * @brief Check key size and calculate hash if not 0
 *
 * @param ctx hash table context pointer.
 * @param key key to hash
 * @return hash_t hash value or 0 if key length is 0.
 */
static hash_t
check_key( ht_ctx_t * ctx, char * key );

/**
 * @brief Get the node idx.
 *
 * @param ctx hash table context pointer.
 * @param hash hash to find.
 * @return ht_size_t index of the node.
 *
 * @note If node not found it will return.
 */
static ht_size_t
get_node_idx( ht_ctx_t * ctx, hash_t hash );

/**
 * @brief Default calculate hash algorithm.
 *
 * @param key key which should be hashed.
 * @return hash_t calculated hash.
 */
static hash_t
default_hash_calculator( char * key );

// --------------------------------------------------------- EXPORTED FUNCTIONS

void
ht_initialize( ht_ctx_t * ctx, ht_calculate_t hasher )
{
    ht_size_t idx;

    ASSERT( NULL != ctx );

    //  If function for hash algorithm not provided then set default one.

    if ( NULL == hasher )
    {
        ctx->calc = default_hash_calculator;
    }
    else
    {
        ctx->calc = hasher;
    }

    for ( idx = 0; idx < HT_NODES_MAX; ++idx )
    {
        ctx->node[idx].hash = 0;
        memset( ctx->node[idx].value, 0, HT_VALUE_SIZE_MAX + 1 );
    }

    ctx->used = 0;
}

ht_result_t
ht_insert( ht_ctx_t * ctx,  char * key, char * val )
{
    hash_t hash;

    ASSERT( NULL != ctx );
    ASSERT( NULL != key );
    ASSERT( NULL != val );

    if ( 0 == ( hash = check_key( ctx, key ) ) )
    {
        return HT_ERROR_KEY_LENGTH;
    }

    //  Does table already have same hash stored ?

    if ( HT_NODES_MAX != get_node_idx( ctx, hash ) )
    {
        PRINTF( "[HT] Hash exists\r\n" );
        return HT_ERROR_HASH_EXIST;
    }

    //  Is there enough space in the table ?

    if ( ctx->used >= HT_NODES_MAX )
    {
        PRINTF( "[HT] Table full\r\n" );
        return HT_ERROR_FULL;
    }
    else
    {
        //  Is value string length too big ?

        if ( strlen( val ) > HT_VALUE_SIZE_MAX )
        {
#if ( HT_TRIM_VALUE_LEN == 1 )

            memcpy( ctx->node[ctx->used].value, val, HT_VALUE_SIZE_MAX );
#else
            PRINTF( "[HT] Value string too long\r\n" );
            return HT_ERROR_VALUE_LENGTH;
#endif
        }
        else
        {
            memset( ctx->node[ctx->used].value, 0, HT_VALUE_SIZE_MAX + 1 );
            strcpy( ctx->node[ctx->used].value, val );
        }

        ctx->node[ctx->used].hash = hash;
    }

    ctx->used++;
    return HT_OK;
}

void
ht_delete( ht_ctx_t * ctx, char * key )
{
    hash_t hash;
    ht_size_t idx;

    ASSERT( NULL != ctx );
    ASSERT( NULL != key );

    if ( 0 == ( hash = check_key( ctx, key ) ) )
    {
        //  Invalid key provided - nothing to delete.

        return;
    }

    /*
        Find the index of the hash calculated previously, but if not exists then
        do nothing.
    */

    if ( HT_NODES_MAX != ( idx = get_node_idx( ctx, hash ) ) )
    {
        /*
            Overwrite the node on the particular index with node on the last
            index, and decrement used number of nodes. This helps us not to
            search the whole table each time but also to have enough wide and
            unique.
        */

        ctx->node[idx].hash = ctx->node[ctx->used - 1].hash;
        memcpy( ctx->node[idx].value, ctx->node[ctx->used - 1].value,
                HT_VALUE_SIZE_MAX );
        ctx->used--;
    }
}

char *
ht_retreive( ht_ctx_t * ctx, char * key )
{
    hash_t hash;
    ht_size_t idx;

    ASSERT( NULL != ctx );
    ASSERT( NULL != key );

    if ( 0 == ( hash = check_key( ctx, key ) ) )
    {
        return NULL;
    }

    /*
        Find the index of the hash calculated previously, but if not exists then
        return null.
    */

    if ( HT_NODES_MAX != ( idx = get_node_idx( ctx, hash ) ) )
    {
        return ctx->node[idx].value;
    }

    return NULL;
}

// ----------------------------------------------------- DEBUG HELPER FUNCTIONS

#if ( HT_DEBUG == 1 )

void
ht_print( ht_ctx_t * ctx )
{
    ht_size_t idx;

    PRINTF( "------------+----------------------------------------\r\n" );
    PRINTF( "    HASH    |    VALUE\r\n" );

    for ( idx = 0; idx < ctx->used; ++idx )
    {
        PRINTF( "------------+----------------------------------------\r\n" );
        PRINTF( " 0x%08x |    %s\r\n", ctx->node[idx].hash,
                ctx->node[idx].value );
    }

    PRINTF( "------------+----------------------------------------\r\n" );
}

#endif

// ---------------------------------------------------- PRIVATES IMPLEMENTATION

static hash_t
check_key( ht_ctx_t * ctx, char * key )
{
    if ( 0 != strlen( key ) )
    {
        return ctx->calc( key );
    }

    return 0;
}

static hash_t
default_hash_calculator( char * key )
{
    uint16_t i;
    hash_t hash = 0x05;        //  Some "whatever" start value.

    for ( i = 0; key[i] != 0; ++i )
    {
        //  Some custom algorithm.

        hash <<= ( key[i] % 2 );
        hash += ( key[i] % 23 );
        hash *= key[i];
    }

    return hash;
}

static ht_size_t
get_node_idx( ht_ctx_t * ctx, hash_t hash )
{
    ht_size_t idx;

    //  Loop until last used index in the table.

    for ( idx = 0; idx < ctx->used; ++idx )
    {
        if ( ctx->node[idx].hash == hash )
        {
            return idx;
        }
    }

    return HT_NODES_MAX;
}

// ---------------------------------------------------------------- END OF FILE