/**
 * @file mem_alloc.h
 * @author mv (mvidojevic82@gmail.com)
 * @brief Memory Allocator
 * @version 0.1
 * @date 2020-09-05
 *
 * @copyright Copyright (c) 2020 - GPLv3
 *
 */

#ifndef _MEM_ALLOC_H_
#define _MEM_ALLOC_H_

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------------------------------------------- INCLUDES

#include <stdint.h>

// ------------------------------------------------------- MODULE CONFIGURATION

#ifndef MEM_ALLOC_BUFFER_SIZE
#define MEM_ALLOC_BUFFER_SIZE ( 1024 )
#endif

#ifndef MEM_ALLOC_UNITS_MAX
#define MEM_ALLOC_UNITS_MAX ( 64 )
#endif

// --------------------------------------------------------- EXPORTED FUNCTIONS

/**
 * @brief
 *
 * @param size
 * @return void*
 */
void *
mem_alloc( uint32_t size );

/**
 * @brief
 *
 * @param ptr
 */
void
mem_free( void * ptr );

#ifdef __cplusplus
}
#endif
#endif

// ---------------------------------------------------------------- END OF FILE