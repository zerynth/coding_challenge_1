/**
 * @file mem_alloc.c
 * @author mv (mvidojevic82@gmail.com)
 * @brief Memory Allocator
 * @version 0.1
 * @date 2020-09-05
 *
 * @copyright Copyright (c) 2020 - GPLv3
 *
 */

#include <stddef.h>
#include <string.h>
#include "mem_alloc.h"

// ------------------------------------------------------------- PRIVATE MACROS

#if ( MA_DEBUG == 1 )
#ifdef __linux__
#include <stdio.h>
#define PRINTF(t,...)   printf((t),##__VA_ARGS__)
#else
#define PRINTF(t,...)   //  Add print through serial for your device here.
#endif
#else
#define PRINTF(t,...)
#endif

#define CAST(x)     ( meta_t * )( &buf[x] )

// -------------------------------------------------------------- PRIVATE TYPES

/**
 * @struct meta_t
 * @brief Simple meta type used as starting segment of each mem block.
 */
typedef struct
{
    uint8_t     used :1;
    uint16_t    size :15;

} meta_t;

// ------------------------------------------------------------------ VARIABLES

static volatile uint8_t buf[MEM_ALLOC_BUFFER_SIZE];

// ------------------------------------------- PRIVATE FUNCTIONS IMPLEMENTATION

static inline uint16_t jump_to_next( uint16_t idx )
{
    meta_t * tmp = CAST( idx );
    return ( idx + sizeof( meta_t ) + tmp->size );
}

// --------------------------------------------------------- EXPORTED FUNCTIONS

void
mem_initialize( void )
{
    memset( buf, 0, MEM_ALLOC_BUFFER_SIZE );
    PRINTF( "Buffer address %x", buf );
}

void *
mem_alloc( uint16_t size )
{
    uint16_t idx;

    if ( size > 0x7FFF )
    {
        return NULL;
    }

    /*
        This loop search for first available space to allocate it for the new
        memory block.
    */

    for ( idx = 0; idx < MEM_ALLOC_BUFFER_SIZE;  )
    {
        meta_t * cur = CAST( idx );

        if ( !cur->used )
        {
            if ( !cur->size )
            {
                //  This is the last node in array.

                if ( ( idx + size + sizeof( meta_t ) ) <= MEM_ALLOC_BUFFER_SIZE )
                {
                    /*
                        Take current space for the new node and create next
                        node in space between current and the end of buffer.
                    */

                    meta_t * tmp;

                    //  Update fields for the current.

                    cur->used = 1;
                    cur->size = size;

                    /*
                        Jump to next when created and clear it up just for the
                        case it was previously used so it can be detected as
                        last.
                    */

                    tmp = CAST( jump_to_next( idx ) );
                    tmp->used = 0;
                    tmp->size = 0;

                    return &buf[idx + sizeof( meta_t )];
                }
                else
                if ( ( idx + size ) <= MEM_ALLOC_BUFFER_SIZE )
                {
                    /*
                        There is enough space for new node but no space for
                        creating node in between this and the end of buffer so
                        take the rest of the space.
                    */

                    cur->used = 1;
                    cur->size = MEM_ALLOC_BUFFER_SIZE - idx;
                    return &buf[idx + sizeof( meta_t )];
                }
                else
                {
                    //  There is no space for another block.

                    return NULL;
                }
            }
            else
            if ( ( cur->size + sizeof( meta_t ) ) <= size )
            {
                //  Create another block afeter taking this.

                meta_t * tmp;
                uint16_t next_idx = jump_to_next( idx );

                cur->used = 1;
                cur->size = size;

                /*
                    Jump to next when created and clear it up just for the
                    case it was previously used also set appropriate size so
                    we can jump to next later.
                */

                tmp = CAST( jump_to_next( idx ) );
                tmp->used = 0;
                tmp->size = next_idx - idx - sizeof( meta_t );
                return &buf[idx + sizeof( meta_t )];
            }
            else
            if ( cur->size <= size )
            {
                /*
                    There is enough space for new block but no for creating
                    another block afeter taking this so take the rest of the
                    space between blocks.
                */

                cur->used = 1;
                cur->size = jump_to_next( idx ) - idx - sizeof( meta_t );
                return &buf[idx + sizeof( meta_t )];
            }
            else
            {
                //  No enough space in this block go to next.

                idx = jump_to_next( idx );
            }
        }
        else
        {
            //  Used block go to next.

            idx = jump_to_next( idx );
        }
    }
}

void
mem_free( void * ptr )
{
    uint16_t idx;
    meta_t * cur;
    meta_t * prev;
    meta_t * next;

    /*
        This loop search for allocated block according to provided address and
        free it. Then it check surrounding space if they can be merged to create
        greater block of available space - kind of defragmentation.
    */

    for ( idx = 0; ptr < &buf[MEM_ALLOC_BUFFER_SIZE];  )
    {
        cur = CAST( idx );

        if ( ( !cur->used ) && ( !cur->size ) )
        {
            //  This was the last node in array.
            PRINTF( "Free Last" );
            return;
        }

        if ( &buf[idx + sizeof( meta_t )] == ptr )
        {
            /*
                Node matched - now check the surrounding nodes.
            */

            if ( idx != 0 )
            {
                /*
                    Check previous only in case not 0 index (begining of the
                    space) and if free then merge them.
                */

                if ( !prev->used )
                {
                    PRINTF( "Previous not used" );
                    prev->size = cur->size + sizeof( meta_t );
                }
            }

            next = CAST( jump_to_next( idx ) );

            if ( ( !next->used ) && ( !next->size ) )
            {
                //  Next node is the last one so just set current to be last.

                PRINTF( "Next is the last - set current to be last" );
                cur->used = 0;
                cur->size = 0;
                ptr = NULL;
                return;
            }
            else
            if ( !next->used )
            {
                //  Merge with next node if free.

                PRINTF( "Next not used" );
                cur->used = 0;
                cur->size += next->size + sizeof( meta_t );
                ptr = NULL;
                return;
            }
            else
            {
                //  No option to do defragmentation.

                PRINTF( "Cant de fragment" );
                cur->used = 0;
                ptr = NULL;
                return;
            }
        }
        else
        {
            idx = jump_to_next( idx );
        }

        prev = cur;
    }
}

#if ( MA_DEBUG == 1 )

void
mem_print( void )
{
    uint16_t node;
    uint16_t idx;
    meta_t * cur;

    PRINTF( "\r\n Node \t | Used \t | Size \t | Address ");

    for ( node = 0, idx = 0; idx < MEM_ALLOC_BUFFER_SIZE; ++node )
    {
        cur = CAST( idx );

        PRINTF( "\r\n %d \t | %d \t | %d \t | %x ", node, cur->used, cur->size,
                &buf[idx + sizeof( meta_t )] );

        if ( ( !cur->used ) && ( !cur->size ) )
        {
            return;
        }

        idx = jump_to_next( idx );
    }

    PRINTF( "\r\n" );
}


#endif

// ---------------------------------------------------------------- END OF FILE