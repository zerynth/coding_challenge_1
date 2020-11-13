# A ring to buffer the all

Write a thread safe ring buffer implementation in C (at least ```get()```, ```put()``` and ```available()```
functions); assume some RTOS with basic functionalities (semaphores, mutexes,
disable/enable interrupts, etc...).

Demonstrate the usage of the ring buffer with:
- a UART ISR as the producer that fills the ring buffer in the background (assume that when ISR is called, some UART register holds the incoming byte)
- a thread continuously reading bytes from the ring buffer and segmenting them in lines terminated by '\n'

# my approach:

I created a circular buffer with an associated mutex to avoid conflicts when accessing on the data
stored on the buffer. To easily use the circular buffer, put (on tail), get (from head) and avaiable
funcitons have been implemented in circular_buf.c/.h.
The buffer functionalities are demonstrated with a combination of a uart ISR that put new data received
on the buffer and a thread that keeps getting the data, if there are any new data avaiable.

Since it is not possbile to use mutex in the isr, I gave to the isr an higher priority than the thread,
but made it so that the uart interrupt is disabled if the buffer becomes full. In this way, the tail
of the buffer will not move forward the head while it might get accessed by the read bytes thread.
The thread will re-enable the interrupt once that it has emptied the buffer.
If the data are succesfully retrieved by the thread, it will segment the new string following '\n' char
and use the segmented string to do some tasks.

map to files:

	circular_buf.c/.h: API and definitions related to the circular buffer
	
	mutex_and_isr.c/.h: some example and sample definitions to be used to demonstrate how the code works.

	main.c: ISR and thread definition and example of the code at work.
