# A ring to buffer the all

Write a thread safe ring buffer implementation in C (at least ```get()```, ```put()``` and ```available()```
functions); assume some RTOS with basic functionalities (semaphores, mutexes,
disable/enable interrupts, etc...).

Demonstrate the usage of the ring buffer with:
- a UART ISR as the producer that fills the ring buffer in the background (assume that when ISR is called, some UART register holds the incoming byte)
- a thread continuously reading bytes from the ring buffer and segmenting them in lines terminated by '\n'

---
## Instructions

1. Edit `CMakeLists.txt` to adjust the pre-compile macros. More details about
each macro can be found in `ring_buffer.h`.
2. Compile demo - `make compile`
3. Run demo - `make run`
4. Clean - `make clean`

**Note:** Cmake is necessary.