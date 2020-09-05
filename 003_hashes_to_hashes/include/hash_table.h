/**
 * @file hash_table.h
 * @author mv (mvidojevic82@gmail.com)
 * @brief Hash Table
 * @version 0.1
 * @date 2020-09-05
 *
 * @copyright Copyright (c) 2020 - GPLv3
 *
 */

#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------------------------------------------- INCLUDES

#include <stdint.h>

// ------------------------------------------------------- MODULE CONFIGURATION

#ifndef HT_NODES_MAX
#define HT_NODES_MAX ( 256 )
#endif

#ifndef HT_VALUE_SIZE_MAX
#define HT_VALUE_SIZE_MAX ( 32 )
#endif

// --------------------------------------------------------- TYPE CONFIGURATION

#define hash_t              uint32_t

// --------------------------------------------------------------- PUBLIC TYPES

typedef hash_t ( * ht_calculate_t ) ( char * );

typedef enum
{
    HT_OK,
    HT_ERROR

} ht_result_t;

typedef struct
{
    hash_t      hash;
    char        value[HT_VALUE_SIZE_MAX];

} ht_node_t;

typedef struct
{
    ht_calculate_t  calc;
    ht_node_t       node[HT_NODES_MAX];

} ht_ctx_t;

// --------------------------------------------------------- EXPORTED FUNCTIONS

ht_result_t
ht_initialize( ht_ctx_t * ctx, ht_calculate_t hasher );

ht_result_t
ht_insert( char * key, char * val );

void
ht_delete( char * key );

char *
ht_retreive( char * key );

#ifdef __cplusplus
}
#endif
#endif

// ---------------------------------------------------------------- END OF FILE