/**
 * @file mem_alloc.h
 * @author mv (mvidojevic82@gmail.com)
 * @brief Memory Allocator
 * @version 0.1
 * @date 2020-09-05
 *
 * @copyright Copyright (c) 2020 - GPLv3
 *
 * ---
 *
 * POC - Simple and light weight memory allocator with configurable static
 * allocated memory. Main features :
 *  - No limited number af allocated blocks
 *  - Self defragmentation each time memory is freed
 *
 * Based on idea from - https://github.com/thi-ng/tinyalloc
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

#ifndef MA_DEBUG
#define MA_DEBUG ( 1 )
#endif

#ifndef MEM_ALLOC_BUFFER_SIZE
#define MEM_ALLOC_BUFFER_SIZE ( 1024 )
#endif

// --------------------------------------------------------- EXPORTED FUNCTIONS

/**
 * @brief Initialize allocator
 * @return void
 * @note In most cases (depending on arch) must be called prior to other
 * functions from the library.
 */
void
mem_initialize( void );

/**
 * @brief Allocate space
 *
 * @param size size of the space to allocate
 * @return void* pointer to the memory location.
 */
void *
mem_alloc( uint16_t size );

/**
 * @brief Free space
 *
 * @param ptr pointer to the begging of the previously allocated space.
 * @warning You must provides start address of the allocated memory, if some
 * of the address from the middle is provied memory will not be freed.
 */
void
mem_free( void * ptr );

#if ( MA_DEBUG == 1 )

void
mem_print( void );

#endif

#ifdef __cplusplus
}
#endif
#endif

// ---------------------------------------------------------------- END OF FILE