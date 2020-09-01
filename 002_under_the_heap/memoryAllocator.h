#include <string.h>
#include <stdio.h>
#include <unistd.h>

/**
 * @brief The header for each block
 * @note The blocks are maintained as a linked list, The blocks are ordered in the increasing order of addresses
 *       ie, last two bits are always zero - can be used to store other information
 *       LSB = 0 => free block
 *       LSB = 1 => allocated/busy block 
 *       So for a free block, the value stored in size_status will be the same as the block size
 *       And for an allocated block, the value stored in size_status will be one more than the block size
 *       The value stored here does not include the space required to store the header 
 */
typedef struct block_hd{
	struct block_hd* next;
	int sizeP; /**< The size_status field is the size of the payload + padding and is always a multiple of 4*/
} header_t;

/**
 *  Global variable - This will always point to the first block 
 *  ie, the block with the lowest address 
 * */
header_t* list_head = NULL;


 /**
 * @brief Initialize the memory allocator
 * @param[in] size : Specifies the size of the chunk which needs to be allocated
 * @pre \a Not intended to be called more than once by a program
 * @return  0 on success and -1 on failure
 */
int init_memory_allocator(int size);

 /**
 * @brief allocate 'size' bytes.
 *        Check for sanity of size - Return NULL when appropriate
 *        Round up size to a multiple of 4
 *        Traverse the list of blocks and allocate the best free block which can accommodate the requested size
 * @param[in] size : Specifies the size of the chunk which needs to be allocated
 * @return  Returns NULL on failure
 */
void* alloc_memory(int size);

 /**
 * @brief free up a previously allocated block
 * @param[in] ptr: Address of the block to be freed up
 * @return  0 on success and -1 on failure
 */
int free_memory(void *ptr);
