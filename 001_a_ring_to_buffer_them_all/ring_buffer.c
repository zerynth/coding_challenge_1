#define RING_BUFFER_CAPACITY 100

typedef struct{
    unsigned char *buffer;    // data buffer
    size_t capacity;          // maximum number of items in the buffer
    size_t count;             // number of items in the buffer
    unsigned char *head;      // pointer to head
    unsigned char *tail;      // pointer to tail
}ring_buffer;

ring_buffer *rb=NULL;
uint8_t stopCondition=0;

uint8_t rb_init(size_t cp)
{
    rb->buffer = (unsigned char *)malloc(cp);
    if(rb->buffer == NULL)
        return 1; //error condition
    rb->capacity = cp;
    rb->count = 0;
    rb->head = rb->buffer;
    rb->tail = rb->buffer;
    return 0; //success condition
}

void rb_free()
{
    free(rb->buffer);
    rb->capacity = 0;
    rb->count = 0;
    rb->head = rb->buffer;
    rb->tail = rb->buffer;
}

uint8_t rb_put(unsigned char *item)
{
    if(rb->count == rb->capacity){ //buffer is full!
        return 1; //error condition
    }
    rtos_mutex_lock(); //hypothetical RTOS MUTEX LOCK FUNCTION
    memcpy(rb->head, item, 1);
    rb->head = rb->head + 1;
    if(rb->head == rb->buffer + rb->capacity)
        rb->head = rb->buffer;
    rb->count++;
    rtos_mutex_unlock(); //hypothetical RTOS MUTEX UNLOCK FUNCTION
    return 0; //success condition
}

uint8_t rb_get(unsigned char *item)
{
    if(rb->count == 0){ //buffer is empty!
        return 1; //error condition
    }
    rtos_mutex_lock(); //hypothetical RTOS MUTEX LOCK FUNCTION
    memcpy(item, rb->tail, 1);
    rb->tail = rb->tail + 1;
    if(rb->tail == rb->buffer + rb->capacity)
        rb->tail = rb->buffer;
    rb->count--;
    rtos_mutex_unlock(); //hypothetical RTOS MUTEX UNLOCK FUNCTION
    return 0; //success condition
}

size_t rb_available()
{
    return rb->count;
}


////////////////////////////////////////////////////////
///////// UART ISR AUTOMATICALLY CALLED ////////////////
////////////////////////////////////////////////////////
_interrupt_ void rx_isr(void)
{
    unsigned char c;
    c = UARTREG; //hypothetical UART register containing one of the received bytes
    if(rb_put(&c)){
        //do somenthing in case of buffer overrun
    }
}


////////////////////////////////////////////////////////
///////// CONTINUOSLY READING FUNCTION /////////////////
////////////////////////////////////////////////////////
void *reading_function() {
    unsigned char c;
    while(1){
        if(rb_available()){
            rb_get(&c);
            printf("%u\n", ch);
        }
    }
}


////////////////////////////////////////////////////////
//////////////////////// MAIN //////////////////////////
////////////////////////////////////////////////////////
int main()
{
    rb=(ring_buffer *)malloc(sizeof(ring_buffer));
    rb_init(RING_BUFFER_CAPACITY);

    start_thread(reading_function); //hypothetical function to start thread
    enable_uart_interrupt(); //hypothetical function to enable uart interrupts

    while(!stopCondition){ // run until stop condition occurs
        //do other (e.g. intercept the stop condition)
    }

    rb_free();
    return 0;
}
