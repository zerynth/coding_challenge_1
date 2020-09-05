/**
 * @file ring_buffer.h
 * @author mv (mvidojevic82@gmail.com)
 * @brief Ring Buffer
 * @version 0.1
 * @date 2020-09-05
 *
 * @copyright Copyright (c) 2020 - GPLv3
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

#ifndef RING_BUFFER_USED_FROM_ISR
#define RING_BUFFER_USED_FROM_ISR           (1)
#endif

#ifndef RING_BUFFER_USED_FROM_THREAD
#define RING_BUFFER_USED_FROM_THREAD        (1)
#endif

// ------------------------------------------------------- MODULE CONFIGURATION

//  Use stdint types here only

#define rb_node_t           uint8_t
#define rb_size_t           uint32_t

// --------------------------------------------------------------- PUBLIC TYPES

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
 * @brief
 *
 */
typedef struct
{
    rb_node_t * buf;        ///< Buffer
    rb_size_t   size;       ///< Buffer size
    rb_size_t   h;          ///< Head
    rb_size_t   t;          ///< Tail
    rb_size_t   used;       ///< Used

} rb_ctx_t;

// --------------------------------------------------------- EXPORTED FUNCTIONS

/**
 * @brief
 *
 * @param ctx
 * @param buffer
 * @param size
 * @return rb_result_t
 */
rb_result_t
rb_initialize( rb_ctx_t * ctx, rb_node_t buffer, rb_size_t size );

rb_size_t
rb_available( rb_ctx_t * ctx );

rb_result_t
rb_put( rb_ctx_t * ctx, rb_node_t data );

rb_result_t
rb_get( rb_ctx_t * ctx, rb_node_t * data );

rb_size_t
rb_read( rb_ctx_t * ctx, rb_node_t * data, rb_size_t size );

rb_size_t
rb_write( rb_ctx_t * ctx, rb_node_t * data, rb_size_t size );

#ifdef __cplusplus
}
#endif
#endif

// ---------------------------------------------------------------- END OF FILE