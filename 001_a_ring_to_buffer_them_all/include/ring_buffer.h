/**
 * @file ring_buffer.h
 * @author mv (mvidojevic82@gmail.com)
 * @brief Ring Buffer
 * @version 0.1
 * @date 2020-09-05
 *
 * @copyright Copyright (c) 2020 - GPLv3
 *
 * ---
 *
 * Modified version of https://github.com/RIOT-OS/RIOT/tree/master/sys/tsrb.
 * Author: Kaspar Schleiser
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * ---
 *
 *
 *
 */

#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------------------------------------------- INCLUDES

#include <stdint.h>

// -------------------------------------------------------- USAGE CONFIGURATION

/**
 * @macro RING_BUFFER_SINGLE_PRODUCER_CONSUMER
 * @brief Set how many consumers or prodeucers use single buffer context.
 *
 * If there is more then one consumer and one producer per single buffer context
 * set value 1 here, otherwise set 0.
 *
 * In general single consumer and producer is faster.
 */
#ifndef RING_BUFFER_SINGLE_PRODUCER_CONSUMER
#define RING_BUFFER_SINGLE_PRODUCER_CONSUMER            (0)
#endif

#if ( RING_BUFFER_SINGLE_PRODUCER_CONSUMER == 0 )

/**
 * @brief Set if the multile ISRs are consumer/producer of same buffer.
 *
 * If for example multiple ISR putting or getting data to/from the single buffer
 * context the set value 1 here - otherwise set 0.
 *
 * @note This value is in relation to the @ref RING_BUFFER_MULTIPLE_THREADS
 * only one of them must be non 0.
 *
 * In case when non 0 is set here you must implement macros.
 * @ref DEFINE_ATOMIC
 * @ref TURN_ATOMIC_ON
 * @ref TURN_ATOMIC_OFF
 * which will enable atomic execution of the critical parts of the library.
 *
 * For more details navigate to the @ref ring_buffer.c.
 */
#ifndef RING_BUFFER_MULTIPLE_ISRS
#define RING_BUFFER_MULTIPLE_ISRS                       (1)
#endif

/**
 * @brief Set if the multile threads are consumer/producer of same buffer.
 *
 * If for example multiple threads putting or getting data to/from the single
 * buffer context the set value 1 here - otherwise set 0.
 *
 * @note This value is in relation to the @ref RING_BUFFER_MULTIPLE_ISRS
 * only one of them must be non 0.
 *
 * In case when non 0 is set here you must provide appropriate function pointers
 * using @ref rb_attach_mutex_calls funcions. The mutexes should allow access
 * to the critical parts of the library only to the single thread.
 */
#ifndef RING_BUFFER_MULTIPLE_THREADS
#define RING_BUFFER_MULTIPLE_THREADS                    (0)
#endif

#endif

// ------------------------------------------------------- MODULE CONFIGURATION

/*
    Set note type. Only scalar types are allowed.
*/
#define rb_node_t           uint8_t

/*
    Set size type. Only unsigned types are allowed here. IN general use as more
    narrow type as possible to store the size of the buffer.
*/
#define rb_size_t           uint32_t

// --------------------------------------------------------------- PUBLIC TYPES

#if ( RING_BUFFER_MULTIPLE_THREADS != 0 )

typedef void ( * rb_mutex_fp_t ) ( void * );

#endif

/**
 * @brief
 *
 */
typedef enum
{
    RB_OK,
    RB_ERROR

} rb_result_t;

/**
 * @struct rb_ctx_t
 * @brief Ring Buffer Context
 */
typedef struct
{
    rb_node_t *     buf;        ///< Buffer
    rb_size_t       size;       ///< Buffer size
    rb_size_t       reads;      ///< Overall number of nodes read
    rb_size_t       writes;     ///< Overall number of nodes written

#if ( RING_BUFFER_MULTIPLE_THREADS != 0 )

    rb_mutex_fp_t   lock;       ///< Mutex Lock
    rb_mutex_fp_t   unlock;     ///< MUtex Unlock

#endif

} rb_ctx_t;

// --------------------------------------------------------- EXPORTED FUNCTIONS

/**
 * @brief Is the Ring Buffer is empty.
 *
 * @param ctx Pointer to Ring Buffer Context
 * @return rb_size_t ( 0 not empty / Other values empty )
 * @note Function is not thread safe in case of multiple accessers.
 */
static inline rb_size_t
rb_empty( rb_ctx_t * ctx )
{
    return ( ctx->reads == ctx->writes );
}

/**
 * @brief Get number of nodes available for reading.
 *
 * @param ctx Pointer to Ring Buffer Context
 * @return rb_size_t Number of available nodes.
 * @note Function is not thread safe in case of multiple accessers.
 */
static inline rb_size_t
rb_available( rb_ctx_t * ctx )
{
    return ( ctx->writes - ctx->reads );
}

/**
 * @brief Is the Ring Buffer full.
 *
 * @param ctx Pointer to Ring Buffer Context
 * @return rb_size_t  ( 0 not full / Other values full )
 * @note Function is not thread safe in case of multiple accessers.
 */
static inline rb_size_t
rb_full( rb_ctx_t * ctx )
{
    return ( ctx->writes - ctx->reads ) == ctx->size;
}

/**
 * @brief Get free space in ringbuffer.
 *
 * @param ctx Pointer to Ring Buffer Context
 * @return rb_size_t Number of free nodes.
 * @note Function is not thread safe in case of multiple accessers.
 */
static inline rb_size_t
rb_free( rb_ctx_t * ctx )
{
    return ( ctx->size - ctx->writes + ctx->reads );
}

/**
 * @brief Initialize Ring Buffer
 *
 * @param ctx Pointer to Ring Buffer Context
 * @param buffer Pointer to buffer.
 * @param size Size of the buffer.
 * @return rb_result_t @ref rb_result_t
 * @warning Buffer size must be power of 2.
 */
rb_result_t
rb_initialize( rb_ctx_t * ctx, rb_node_t * buffer, rb_size_t size );

#if ( RING_BUFFER_SINGLE_PRODUCER_CONSUMER == 0 )
#if ( RING_BUFFER_MULTIPLE_THREADS != 0 )

/**
 * @brief Attach Mutex Lock Funcions
 *
 * @param ctx Pointer to Ring Buffer Context
 * @param locker Mutex Lock funcion call
 * @param unlocker Mutex Unlock funcion call
 * @return rb_result_t @ref rb_result_t
 */
rb_result_t
rb_attach_mutex_calls( rb_ctx_t * ctx, rb_mutex_fp_t locker,
                       rb_mutex_fp_t unlocker );

#endif
#endif

/**
 * @brief Put single node.
 *
 * @param ctx Pointer to Ring Buffer Context
 * @param data Node to put.
 * @return rb_result_t @ref rb_result_t
 */
rb_result_t
rb_put( rb_ctx_t * ctx, rb_node_t data );

/**
 * @brief Get single node.
 *
 * @param ctx Pointer to Ring Buffer Context
 * @param data Node got.
 * @return rb_result_t @ref rb_result_t
 */
rb_result_t
rb_get( rb_ctx_t * ctx, rb_node_t * data );

/**
 * @brief Read array of nodes.
 *
 * @param ctx Pointer to Ring Buffer Context
 * @param data Pointer to array of nodes.
 * @param size Number of nodes to write.
 * @return rb_size_t Number of nodes read.
 */
rb_size_t
rb_read( rb_ctx_t * ctx, rb_node_t * data, rb_size_t size );

/**
 * @brief Write Array of nodes.
 *
 * @param ctx Pointer to Ring Buffer Context
 * @param data Pointer to buffer where to store nodes.
 * @param size Number of nodes to read.
 * @return rb_size_t Number of nodes written.
 */
rb_size_t
rb_write( rb_ctx_t * ctx, rb_node_t * data, rb_size_t size );

#ifdef __cplusplus
}
#endif
#endif

// ---------------------------------------------------------------- END OF FILE