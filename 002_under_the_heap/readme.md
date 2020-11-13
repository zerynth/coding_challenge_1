# Under the heap

Write a memory allocator in C trying to minimize memory fragmentation.

# my approach

I created a memory allocator that works with a preallocated block of memory to
ensure that all the allocated data will be contiguous.
To handle the allocation I used a linked list of mem_blocks (headers) which
cointains informations on size of the memory block, if the block is free and a pointer to
the next block on the list.
To reduce memory fragmentation I implemented an algorithm to choose the best block to
use as storage, checking between all the free blocks the one with the smallest size that
is still enough to contain the desired string.
The custom malloc choose the right block with the algorithm and, if the size of it
is not perfect, it splits it to occupy just as much as needed. To ensure that memory
fragmentation is avoided on the long run, a merge function that merges all the contiguous
free block of memory has been implemented.

Everything is on mem_alloc.c with some test of memory allocation in main() function.