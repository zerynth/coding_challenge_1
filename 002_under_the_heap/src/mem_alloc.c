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

#include "mem_alloc.h"

// ------------------------------------------------------------------ VARIABLES

static volatile uint8_t mem_buf[MEM_ALLOC_BUFFER_SIZE];

// --------------------------------------------------------- EXPORTED FUNCTIONS

void *
mem_alloc( uint32_t size )
{

}

void
mem_free( void * ptr )
{

}

// ---------------------------------------------------------------- END OF FILE