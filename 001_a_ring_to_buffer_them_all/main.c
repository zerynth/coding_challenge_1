/**
 * @file main file
 * @author Elia Guglielmin
 *
 * I created a circular buffer with an associated mutex to avoid conflicts
 * when accessing on the data stored on the buffer. To control the circular
 * buffer, put (on tail), get (from head) and avaiable funcitons have been
 * implemented (circular_buf.c/circular_buf.h). The buffer functionalities
 * are tested with a combination of a uart ISR that put new data received
 * on the buffer and a thread that keeps getting the data if there are any
 * new data avaiable. Since it is not possbile to use mutex in the isr, I
 * gave to the isr an higher priority than the thread, but made it so that
 * the uart interrupt is disabled if the buffer becomes full. In this way
 * the tail of the buffer will not move foward the head while it might get
 * accessed by the read bytes thread.
 * If the data are succesfully retrieved by the thread, it will segment the
 * new string following '\n' char and use the segmented string to do some task.
 */

#include "circular_buf.h"

/** @brief size of the circular buffer */
#define BUF_SIZE 200

/** @brief application main circular buffer strucrt */
cbuffer my_circular_buffer;

/** @brief buffer used for the circular buffer struct */
uint8_t my_buffer[BUF_SIZE] = { 0 };

/** @brief mutex used with circular buffer */
some_mutex_t cbuf_mutex;

/**
 * @brief uart data register.
 *
 * we are assuming that its value will change when the uart gets
 * data from some source.
 */ 
uint8_t uart_data_reg = 0;

//declaration of the thread
void read_bytes_thread(void);

/** @brief initialize hardware peripherals */
void init_hardware(void)
{
    //init peripherals
    SET_ISR_PRIORITY(2);
    ENABLE_UART_ISR();
}

/** @brief UART ISR sample */
void UART_ISR(void)
{
    // puts data from uart register in the circular buffer tail
    cbuf_put_u8_isr(&my_circular_buffer, uart_data_reg);
    // if the buffer get full we might start change data accesses by other threads
    // disabling the isr will avoid that.
    if (cbuf_is_full(&my_circular_buffer))
        DISABLE_UART_ISR();
}

/**
 * @brief specific rtos kernel's thread sleep function
 *
 * @param ms milleseconds to sleep
 */
void thread_sleep_ms(int ms)
{
    //sample for sleep funcion
}

/**
 * @brief do required operations with the '\n' terminatig string
 *
 * @param buf pointer to buffer with the string
 * @param str_len number of bytes before '\n' char.
 */
void do_something_with_string(uint8_t *buf, size_t str_len)
{
    // do something with the string terminating with '\n'
    // note that the nuber of bytes to be used is known in str_len
}

/**
 * @brief sample to spawn a new thread with a chosen priority
 *
 * higher value on priority argument assumes higher priority
 * of the thread.
 *
 * @param task thread to spwan
 * @param priority priority of the task. higher value = higher priority
 */
void spawn_thread(void *task(void), size_t priority)
{
    //spawn task thread
}

/**
 * @brief thread that continusly read bytes from the circular buffer
 *
 * the buffer is filled by the uart isr when some data are received.
 * this thread get the data from the circular buffer, segment them
 * searching for the '\n' bytes and send the segmented string to 
 * a function that will use it.
 *
 * This thread has a lower priority than the ISR, so it will be blocked
 * when some new data are received.
 */
void read_bytes_thread(void)
{
    size_t tmp_buf_len = 200;
    uint8_t tmp_buffer[200] = { 0 };
    size_t str_index = 0;
    size_t n_new_bytes = 0;
    
    while (1) {
    	// check if new data are avaiable on the circular buffer
        n_new_bytes = cbuf_avaiable_data(&my_circular_buffer);

        // if we have new data we store them on the temp buffer
        if (n_new_bytes != 0) {
            // we get all the new bytes received without sleeping.
            for (int i = 0; i < n_new_bytes; i++) {
                // get data from circular buffer head and store it on tmp_buffer.
                cbuf_get_u8(&my_circular_buffer, &tmp_buffer[str_index]);
                //if we read a '\n' we do something with that string and reset the string index.
                if (tmp_buffer[str_index] == '\n') {
                    do_something_with_string(tmp_buffer, str_index+1); //str_index + 1 to get the number of bytes.
                    str_index = 0;
                } else {
                    // % to avoid overflow of the buffer, we could create and error handler for this.
                    str_index = (str_index + 1) % tmp_buf_len;
                }
            }

            // since the uart will disable its interrupt if the circular buffer get full
            // we enable the interrupt when we empty the buffer to be sure that the 
            // uart continue receiving after we read the data.
            if (cbuf_is_empty(&my_circular_buffer))
                ENABLE_UART_ISR(); 
        }

        // sleep for 100 ms to avoid the thread to consume to much resources
        thread_sleep_ms(100);
    }
}

/**
 * @brief main function
 */
int main() {
	// initialize uart and start receiving
    init_hardware();
	// initialize mutex on circular buffer
    mutex_init(&cbuf_mutex, 0);
    // initialize circular buffer
    cbuf_init(&my_circular_buffer, my_buffer, BUF_SIZE, &cbuf_mutex);

	// spawn the read thread
    spawn_thread((void*)read_bytes_thread, 1);

    while (1);
}

