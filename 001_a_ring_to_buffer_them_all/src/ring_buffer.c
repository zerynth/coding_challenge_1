/**
 * @file ring_buffer.c
 * @author mv (mvidojevic82@gmail.com)
 * @brief Ring Buffer
 * @version 0.1
 * @date 2020-09-05
 *
 * @copyright Copyright (c) 2020 - GPLv3
 *
 * Modified version of https://github.com/RIOT-OS/RIOT/tree/master/sys/tsrb.
 * Author: Kaspar Schleiser
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "ring_buffer.h"
#include <stddef.h>

#define ASSERT(x)

#if ( RING_BUFFER_SINGLE_PRODUCER_CONSUMER == 0 )
#if ( RING_BUFFER_MULTIPLE_ISRS != 0 ) && ( RING_BUFFER_MULTIPLE_THREADS != 0 )
#error "Chose only one of available options to ensure thread safety."
#endif
#endif

#if ( RING_BUFFER_SINGLE_PRODUCER_CONSUMER == 0 )
#if ( RING_BUFFER_MULTIPLE_ISRS != 0 )

/*
    Declare all necessary variables which will later allow atomic execution of
    the code which will be placed between @ref TURN_ATOMIC_ON and
    @ref TURN_ATOMIC_OFF.
*/
#define DEFINE_ATOMIC( )

/*
    This macro should enable atomic execution of the code which is later
    disabled by @ref TURN_ATOMIC_OFF macro. In general interrups should be
    disable in this macro - as fastest as possible.
*/
#define TURN_ATOMIC_ON( )

/*
    This macro should disable atomic execution of the code which is previously
    enable by @ref TURN_ATOMIC_ON macro. In general interrups should be
    enabled in this macro - as fastest as possible.
*/
#define TURN_ATOMIC_OFF( )

/*
    Comment out nex line of the code if you implemented macros which allows
    atomic execution of the code.
*/
#warning "Atomic execution macros not defined"
#endif
#if ( RING_BUFFER_MULTIPLE_THREADS != 0 )

#define DEFINE_ATOMIC( )

#define TURN_ATOMIC_ON( )                                                       \
    do                                                                          \
    {                                                                           \
        if ( NULL != ctx->lock ) ctx->lock( ctx );                              \
    }                                                                           \
    while( 0 )

#define TURN_ATOMIC_OFF( )                                                      \
    do                                                                          \
    {                                                                           \
        if ( NULL != ctx->unlock ) ctx->unlock( ctx );                          \
    }                                                                           \
    while( 0 )
#endif
#else
#define DEFINE_ATOMIC( )
#define TURN_ATOMIC_ON( )
#define TURN_ATOMIC_OFF( )
#endif


// ---------------------------------------------- PRIVATE FUNCTION DECLARATIONS

static inline void
_push( rb_ctx_t * ctx, rb_node_t n )
{
    ctx->buf[ctx->writes++ & (ctx->size - 1)] = n;
}

static inline void
_pop( rb_ctx_t * ctx, rb_node_t * n )
{
    * n = ctx->buf[ctx->reads++ & (ctx->size - 1)];
}

// --------------------------------------------------------- EXPORTED FUNCTIONS

rb_result_t
rb_initialize( rb_ctx_t * ctx, rb_node_t * buffer, rb_size_t size )
{
    ASSERT( ctx != NULL );
    ASSERT( buffer != NULL );

    /*
        Make sure bufsize is a power of two.
        http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
    */
    if ((size != 0) && ((size & (~size + 1)) != size))
    {
        return RB_ERROR;
    }

    ctx->buf = buffer;
    ctx->size = size;
    ctx->reads = 0;
    ctx->writes = 0;

#if ( RING_BUFFER_SINGLE_PRODUCER_CONSUMER == 0 )
#if ( RING_BUFFER_MULTIPLE_THREADS != 0 )

    ctx->lock = NULL;
    ctx->unlock = NULL;

#endif
#endif

    return RB_OK;
}

#if ( RING_BUFFER_SINGLE_PRODUCER_CONSUMER == 0 )
#if ( RING_BUFFER_MULTIPLE_THREADS != 0 )

rb_result_t
rb_attach_mutex_calls( rb_ctx_t * ctx, rb_mutex_fp_t locker,
                       rb_mutex_fp_t unlocker )
{
    ASSERT( ctx != NULL );
    ASSERT( locker != NULL );
    ASSERT( unlocker != NULL );

    ctx->lock = locker;
    ctx->unlock = unlocker;
}

#endif
#endif

rb_result_t
rb_get( rb_ctx_t * ctx, rb_node_t * data )
{
    DEFINE_ATOMIC( );
    TURN_ATOMIC_ON( );

    if ( !rb_empty( ctx ) )
    {
        _pop( ctx, data );
    }
    else
    {
        TURN_ATOMIC_OFF( );
        return RB_ERROR;
    }

    TURN_ATOMIC_OFF( );

    return RB_OK;
}

rb_result_t
rb_put( rb_ctx_t * ctx, rb_node_t data )
{
    DEFINE_ATOMIC( );
    TURN_ATOMIC_ON( );

    if ( !rb_full( ctx ) )
    {
        _push( ctx, data );
    }
    else
    {
        TURN_ATOMIC_OFF( );
        return RB_ERROR;
    }

    TURN_ATOMIC_OFF( );

    return RB_OK;
}

rb_size_t
rb_read( rb_ctx_t * ctx, rb_node_t * data, rb_size_t size )
{
    rb_size_t idx = 0;

    DEFINE_ATOMIC( );
    TURN_ATOMIC_ON( );

    while ( !rb_empty( ctx ) && ( idx < size ) )
    {
        _pop( ctx, &data[idx++] );
    }

    TURN_ATOMIC_OFF( );

    return idx;
}

rb_size_t
rb_write( rb_ctx_t * ctx, rb_node_t * data, rb_size_t size )
{
    rb_size_t idx = 0;

    DEFINE_ATOMIC( );
    TURN_ATOMIC_ON( );

    while ( !rb_full( ctx ) && ( idx < size ) )
    {
        _push( ctx, data[idx++] );
    }

    TURN_ATOMIC_OFF( );

    return idx;
}

// ---------------------------------------------------------------- END OF FILE