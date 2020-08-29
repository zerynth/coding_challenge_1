I created a circular buffer with an associated mutex to avoid conflicts
when accessing on the data stored on the buffer. To control the circular
buffer, put (on tail), get (from head) and avaiable funcitons have been
implemented (circular_buf.c/circular_buf.h). The buffer functionalities
are tested with a combination of a uart ISR that put new data received
on the buffer and a thread that keeps getting the data if there are any
new data avaiable. Since it is not possbile to use mutex in the isr, I
gave to the isr an higher priority than the thread, but made it so that
the uart interrupt is disabled if the buffer becomes full. In this way
the tail of the buffer will not move foward the head while it might get
accessed by the read bytes thread.
If the data are succesfully retrieved by the thread, it will segment the
new string following '\n' char and use the segmented string to do some task.
