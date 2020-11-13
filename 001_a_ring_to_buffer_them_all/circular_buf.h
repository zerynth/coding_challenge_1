/**
 * @file circular buffer API functions
 * @author Elia Guglielmin
 */

#ifndef __CIRCULAR_BUF_H__
#define __CIRCULAR_BUF_H__

#include <stdio.h>
#include <stdbool.h>
#include "mutex_and_isr.h"

/** @brief circular buffer structure */
typedef struct cbuffer {
    /** @brief pointer to buffer to use */
    uint8_t *buffer;
    /** @brief size of the buffer */
    size_t size;
    /** @brief data head */
    size_t head;
    /** @brief data tail */
    size_t tail;
    /** @brief 1 if the buffer is full */
    bool full;
    /** @brief mutex associated to circular buffer */
    some_mutex_t *mutex;
} cbuffer;

/**
 * @brief initialize a circular buffer structure
 *
 * @param cbuf_st pointer to the circular buffer struct to init
 * @param buf pointer to the buffer to initialize the circular buffer with
 * @param buf_size size of the passed buffer
 */
void cbuf_init(cbuffer *cbuf_st, uint8_t *buf, size_t buf_size, some_mutex_t *mutex);

/** @brief retrun bool to check if the circular buffer is full (true if full) */
bool cbuf_is_full(cbuffer *cbuf_st);

/**
 * @brief retrun bool to check if the circular buffer is empty (true if empty)
 *
 * check made using both the cb full boolean and the head == tail check.
 */
bool cbuf_is_empty(cbuffer *cbuf_st);

/**
 * @brief put u8 on buffer for isr use.
 *
 * isr cannot use mutex, so a function without the mutex will be
 * used for the uart. To avoid concurrency, when writing o the
 * buffer from the isr, the interrupt of uart isr will be
 * disabled if the buffer ever get full and enabled when some bytes
 * have been read.
 *
 * @param cbuf_st pointer to the circular buffer struct
 * @param data u8 data to put on tail
 */
void cbuf_put_u8_isr(cbuffer *cbuf_st, uint8_t data);

/**
 * @brief get an unsigned char from the buffer head
 *
 * get an unsigned char to the head of the circular buffer and
 * move foward the head. Using the mutex we assure that the buffer
 * will be accessed once at the time.
 * 
 * @param cbuf_st pointer to the circular buffer struct
 * @param data u8 data pointer that will receive the head's data
 *
 * @retval -1 if the buffer is empty and no data can be retrieved
 * @retval mutex state ( 0 free 1 taken)
 */
int cbuf_get_u8(cbuffer *cbuf_st, uint8_t *data);

/**
 * @brief return number of unread avaiable bytes
 *
 * @param cbuf_st pointer to the circular buffer struct
 *
 * @retval n number of bytes of new datas
 */
size_t cbuf_avaiable_data(cbuffer *cbuf_st);
#endif