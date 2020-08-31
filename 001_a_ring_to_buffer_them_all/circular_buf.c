#include <string.h>
#include "circular_buf.h"

/**
 * @brief reset the circular buffer structure
 *
 * clear the buffer used by the structure and set
 * head and tail to 0. The buffer size and mutex unchanged.
 *
 * @param cbuf_st pointer to the circular buffer struct to reset
 */
static void cbuf_reset(cbuffer *cbuf_st)
{
	//take mutex
    if (mutex_take(cbuf_st->mutex) == 0) {
        memset(cbuf_st->buffer, 0, cbuf_st->size);
        cbuf_st->head = 0;
        cbuf_st->tail = 0;
        cbuf_st->full = false;
        // free mutex
        mutex_give(cbuf_st->mutex);
    }
}

void cbuf_init(cbuffer *cbuf_st, uint8_t *buf, size_t buf_size, some_mutex_t *mutex)
{
    cbuf_st->buffer = buf;
    cbuf_st->size = buf_size;
    cbuf_st->mutex = mutex;
    // we ensure that the buffer is cleared
    cbuf_reset(cbuf_st);
}

bool cbuf_is_full(cbuffer *cbuf_st)
{
    return cbuf_st->full;
}

bool cbuf_is_empty(cbuffer *cbuf_st)
{
    return (cbuf_st->full && (cbuf_st->head == cbuf_st->tail));
}

/**
 * @brief advance tail of the buffer by 1
 *
 * if the buffer is full, the head will also advance by one
 * to avoid the inversion of the position of the two.
 *
 * @param cbuf_st pointer to the circular buffer struct
 */
static void advance_tail(cbuffer *cbuf_st)
{
	// if the buf is full I move the head with the tail to add data
    if (cbuf_is_full(cbuf_st)) 
        cbuf_st->head = (cbuf_st->head + 1) % cbuf_st->size;
	
	// advance the tail
    cbuf_st->tail = (cbuf_st->tail + 1) % cbuf_st->size;
    // if tail reach the head then the buff is full
    if (cbuf_st->tail == cbuf_st->head)
        cbuf_st->full = true;
    else
        cbuf_st->full = false;
}

/**
 * @brief advence head of the buffer by 1
 *
 * @param cbuf_st pointer to the circular buffer struct
 */
static void advance_head(cbuffer *cbuf_st)
{
    cbuf_st->head = (cbuf_st->head + 1) % cbuf_st->size;
    cbuf_st->full = false;
}

/**
 * @brief put an unsigned char on buffer's tail
 *
 * Add an unsigned char to the tail of the circular buffer and
 * move foward the tail. Using the mutex we assure that the buffer
 * will be accessed once at the time.
 *
 * @param cbuf_st pointer to the circular buffer struct
 * @param data u8 data to put on tail
 *
 * @retval mutex state ( 0 free 1 taken)
 */
int cbuf_put_u8(cbuffer *cbuf_st, uint8_t data)
{
	//take mutex
    if (mutex_take(cbuf_st->mutex) == 0) {
    	// put data in the buffer
        cbuf_st->buffer[cbuf_st->tail] = data;
        advance_tail(cbuf_st);
        // free mutex
        mutex_give(cbuf_st->mutex);
    }

    return cbuf_st->mutex->is_taken; 
}

void cbuf_put_u8_isr(cbuffer *cbuf_st, uint8_t data)
{
    cbuf_st->buffer[cbuf_st->tail] = data;
    advance_tail(cbuf_st);
}

int cbuf_get_u8(cbuffer *cbuf_st, uint8_t *data)
{
	// if buf is empty, return err
    if (cbuf_is_empty(cbuf_st))
        return -1;
	
	// take mutex
    if (mutex_take(cbuf_st->mutex) == 0) {
    	// assign data of the buffer to the data pointer
        *data = cbuf_st->buffer[cbuf_st->head];
        advance_head(cbuf_st);
        // free mutex
        mutex_give(cbuf_st->mutex);
    }

	// return the state of the mutex
    return cbuf_st->mutex->is_taken;
}

size_t cbuf_avaiable_data(cbuffer *cbuf_st)
{
    int ret_n = 0;
	// take the mutex if it is free
    if (mutex_take(cbuf_st->mutex) == 0) {
        size_t head = cbuf_st->head;
        size_t tail = cbuf_st->tail;
        size_t max = cbuf_st->size;

        if (cbuf_is_full(cbuf_st)) // full buffer, we return the max number of bytes
            ret_n = max;
        else if (head > tail)
            ret_n = (max - head + tail); //count from head to max and from 0 to tail
        else
            ret_n = (tail - head); //this one covers tail >= head, also empty case is covered

		// free the mutex
        mutex_give(cbuf_st->mutex);
    }

    return ret_n; //if I do not have access to data 0 is returned since no data can be read
}
