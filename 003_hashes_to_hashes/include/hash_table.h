/**
 * @file hash_table.h
 * @author mv (mvidojevic82@gmail.com)
 * @brief Hash Table
 * @version 0.1
 * @date 2020-09-05
 *
 * @copyright Copyright (c) 2020 - GPLv3
 *
 * ---
 *
 * Light weight implementation of the Hash table suitable for embedded devices
 * with limited resources. Main features :
 *  - Not using dynamic memory allocation
 *  - RAM usage can be simple adjusted using @ref HT_NODES_MAX and
 * @ref HT_VALUE_SIZE_MAX macros. Approx HT_NODES_MAX * (HT_VALUE_SIZE_MAX + 5).
 *  - Hash accuracy can be adjusted with @ref hash_t.
 *  - Multiple table context are allowed.
 *
 */

#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------------------------------------------- INCLUDES

#include <stdint.h>

#ifndef HT_DEBUG
#define HT_DEBUG ( 1 )      //  Enable debug mode here
#endif

// ------------------------------------------------------- MODULE CONFIGURATION

/**
 * @macro HT_NODES_MAX
 * @brief Maximum Number of the nodes in the table.
 */
#ifndef HT_NODES_MAX
#define HT_NODES_MAX ( 254 )
#endif

/**
 * @macro HT_VALUE_SIZE_MAX
 * @brief Maximum Value string length.
 */
#ifndef HT_VALUE_SIZE_MAX
#define HT_VALUE_SIZE_MAX ( 32 )
#endif

/**
 * @macro HT_TRIM_VALUE_LEN
 * @brief This macro is used to decide what to do when value string length
 * provided during @ref ht_insert is longer then maximum allowed
 * @ref HT_VALUE_SIZE_MAX .
 *
 * - 0 for this macro means function will return @ref HT_ERROR_VALUE_LENGTH .
 * - Non 0 value for this macro means function will trim the value string to the
 * maximum allowed size.
 */
#ifndef HT_TRIM_VALUE_LEN
#define HT_TRIM_VALUE_LEN ( 1 )
#endif

// --------------------------------------------------------- TYPE CONFIGURATION

/**
 * @typedef hash_t
 * @brief Type which will be used to store calculated hash.
 * @warning Use only unsigned integer types.
 *
 * The wider type means higher hash accuracy - lower chance for duplicate.
 */
typedef uint32_t    hash_t;

/**
 * @typedef ht_size_t
 * @brief Type which will be used for indexing. This type should be big enough
 * to store @ref HT_NODES_MAX + 1.
 * @warning Use only unsigned integer types.
 *
 */
typedef uint16_t    ht_size_t;

// --------------------------------------------------------------- PUBLIC TYPES

/**
 * @typedef ht_calculate_t
 * @brief Hash calculation function prototype.
 */
typedef hash_t ( * ht_calculate_t ) ( char * );

/**
 * @enum ht_result_t
 * @brief Operation result.
 */
typedef enum
{
    HT_OK,
    HT_ERROR_FULL,          ///< Table is full - no space for new nodes.
    HT_ERROR_HASH_EXIST,    ///< Node with particular hash already exists.
    HT_ERROR_VALUE_LENGTH   ///< Length of the Value string above limit.

} ht_result_t;

/**
 * @struct ht_node_t
 * @brief Node type.
 */
typedef struct
{
    hash_t          hash;
    char            value[HT_VALUE_SIZE_MAX + 1];

} ht_node_t;

/**
 * @struct ht_ctx_t
 * @brief Hash table context.
 */
typedef struct
{
    ht_calculate_t  calc;
    ht_node_t       node[HT_NODES_MAX];
    ht_size_t       used;

} ht_ctx_t;

// --------------------------------------------------------- EXPORTED FUNCTIONS

/**
 * @brief Initialize hash table
 *
 * @param ctx Pointer to hash table context.
 * @param hasher Custom function used for hashing.
 * @note Function will set all nodes inside the table context to the 0.
 * @warning Function must be called prior to any other.
 */
void
ht_initialize( ht_ctx_t * ctx, ht_calculate_t hasher );

/**
 * @brief Insert key-value pair to the Hash table.
 *
 * @param ctx Pointer to hash table context.
 * @param key Key string.
 * @param val Value string.
 * @return ht_result_t @ref ht_result_t
 * @warning Parameters key and val must be valid 0 terminated strings.
 */
ht_result_t
ht_insert( ht_ctx_t * ctx, char * key, char * val );

/**
 * @brief Delete key-value pair from the Hash table.
 *
 * @param ctx Pointer to hash table context.
 * @param key Key string.
 * @warning Parameters key must be valid 0 terminated strings.
 * @note If there is no particular key in the table nothing will happen.
 */
void
ht_delete( ht_ctx_t * ctx, char * key );

/**
 * @brief Retreive value from the table based on key.
 *
 * @param ctx Pointer to hash table context.
 * @param key Key string.
 * @return char* Pointer to Value string.
 * @warning Parameters key must be valid 0 terminated strings.
 */
char *
ht_retreive( ht_ctx_t * ctx, char * key );

// ----------------------------------------------------- DEBUG HELPER FUNCTIONS

#if ( HT_DEBUG == 1 )

/**
 * @brief Retreive value from the table based on key.
 *
 * @param ctx Pointer to hash table context.
 * @param key Key string.
 * @return char* Pointer to Value string.
 * @warning Parameters key must be valid 0 terminated strings.
 */
void
ht_print( ht_ctx_t * ctx );

#endif

#ifdef __cplusplus
}
#endif
#endif

// ---------------------------------------------------------------- END OF FILE