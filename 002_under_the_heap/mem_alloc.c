#define _CRT_SECURE_NO_WARNINGS  /* to avoid some C11 warnings on my machine. Might be removed on other machines.*/
/**
 * @file custom heap allocator example
 * @author Elia Guglielmin
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/** @brief reserved mamory to be used to allocate all memory in a contiguous chunk of memory */
#define RESERVED_MEMORY 10000

/** @breif block of memory, element of the linked list used to handle memory alloc */
typedef struct mem_block {
	/** @brief number of bytes of the block */
	size_t size;
	/** @brief tells if the memory block is free */
	bool is_free;
	/** @brief pointer to the next memory block */
	mem_block* next_block;
} mem_block;

/**
 * @brief initialize a new memory block
 *
 * @param block pointer to block to init
 * @param size number of bytes to assign to the block
 * @next pointer to the next block of the list
 */
void init_mem_block(mem_block* block, size_t size, mem_block* next)
{
	block->size = size;
	block->is_free = true;
	block->next_block = next;
}

/**
 * @brief set fields of an occupied block of memory
 *
 * @param block pointer to the block
 * @param size bytes to occupy
 * @param pointer to next block
 */
void occupy_mem_block(mem_block* block, size_t size, mem_block* next)
{
	block->size = size;
	block->is_free = false;
	block->next_block = next;
}

/**
 * @brief initialize the chunk of memory that will be used for allocation
 *
 * @param first_block pointer for the block that will occpuy the first position of the list
 * @param tot_size total ammount of memory to block for future allocation
 */
void init_memory(mem_block* first_block, size_t tot_size)
{
	init_mem_block(first_block, tot_size - sizeof(mem_block), NULL);
}

/**
 * @brief split a block of memory in two sub-blocks
 *
 * the original block is split in a block that will allocate
 * the next datas and a block that will remain empty.
 *
 * @param fit_block pointer to the original block that can fit the data incoming.
 *					this is the block that will be splitted
 * @param size number of bytes that needs to be allocated
 */
void split(mem_block* fit_block, size_t size)
{
	//ammount of memory of fit_block that will be free after the split
	size_t remaning_free_size = fit_block->size - size - sizeof(mem_block);

	// new block originated from the split that will remain empty
	mem_block *new_block = (mem_block*)((size_t*)fit_block + size + sizeof(mem_block));
	init_mem_block(new_block, remaning_free_size, fit_block->next_block);
	// we allocate the remaning memory of the original block
	occupy_mem_block(fit_block, size, new_block);
}

/**
 * @brief check if a block of memory is big enough to allocate the required ammount of bytes
 *
 * @param first_block pointer to the first block of our list
 * @param block pointer to the block that has to allocate data
 * @param size number of bytes to allocate
 *
 * @retval 1 if there is enough space
 * @retval 0 if there is not
 */
bool can_fit_alloc_mem(mem_block* first_block, mem_block* block, size_t size)
{
	// address value of the end of the block that would like to allocate
	size_t block_end_addr = (size_t)block + size + sizeof(mem_block);
	// last byte of our pre-allocated memory
	size_t end_of_mem_addr = (size_t)first_block + RESERVED_MEMORY;

	return (block_end_addr < end_of_mem_addr);
}

/**
 * @brief check if the a block and its next are free
 *
 * @param block pointer to block
 *
 * @retval bool true if both are free
 */
bool contiguous_are_free(mem_block* block)
{
	return (block->is_free && block->next_block->is_free);
}

/**
 * @brief merge all contiguous free block in all the memory pre-allocated
 *
 * cycle through all the pre-allocated memory and, when it finds multiple
 * free block contiguous it merges them together
 *
 * @param first_block pointer to the first block of our list
 *
 * @retval number of merged blocks
 */
size_t my_merge(mem_block* first_block)
{
	mem_block* iter_block = first_block;
	size_t n_merged = 0;

	// cycle all the linked list
	while (iter_block->next_block != NULL) {
		// this with the while will merge together all the contiguous free blocks
		if (contiguous_are_free(iter_block) == true) {
			// adjust size to the end of the next free block
			iter_block->size += iter_block->next_block->size + sizeof(mem_block);
			// update pointer to jump to the next->next
			iter_block->next_block = iter_block->next_block->next_block;
			n_merged++;
		}
		else {
			// pass through occupied blocks
			iter_block = iter_block->next_block;
		}
	}

	return n_merged;
}

/**
 * @brief return the smallest block of free memory that can allocate a new block
 *
 * with this function it is possible to always use the best allocation
 * site to store our data instead of using, for example the first
 * site that can store it. 
 *
 * @param first_block pointer to the first block of our list
 * @param size number of bytes to allocate
 *
 * @retval best_fit pointer to the smallest fittable memory block.
 * @retval NULL if none is found
 */
mem_block* find_best_fit(mem_block* first_block, size_t size)
{
	mem_block *best_fit = NULL;
	mem_block *iter_block = first_block;

	// keeps track of the smalles difference of number of bytes
	// of a memory block and number of bytes that needs to be stored
	size_t smaller_diff = RESERVED_MEMORY;

	// merge fragmentate memory to ensure to not miss possible allocation site
	my_merge(first_block);

	// cycle through all the preallocated memory
	while (can_fit_alloc_mem(first_block, iter_block, size)){
		// if the block is big enough and free
		if ((iter_block->size >= size) && (iter_block->is_free == true)) {
			// if the block is the smallest that can fit found
			//up until now, we update both it and the best_fit pointer
			if ((iter_block->size - size) < smaller_diff) {
				smaller_diff = iter_block->size - size;
				best_fit = iter_block;
			}
		}
		// we reached the end of the list, quit the loop
		if (iter_block->next_block == NULL)
			break;

		// update iter_block to continue the cycle
		iter_block = iter_block->next_block;
	}

	// return the best block fot our data
	return best_fit;
}

/**
 * @brief custom memory allocation function
 *
 * this function allocates data on the preallocated memory,
 * while trying to find the best fittable block of memory that
 * can store our data.
 *
 * @param first_block pointer to the first block of our list
 * @param size number of bytes to allocate
 */
void* my_malloc(mem_block *first_block, size_t size)
{
	// block where data will be allocated
	mem_block *target_block = NULL;

	// assign to the target block the smallest fittable block of memory
	target_block = find_best_fit(first_block, size);

	if (target_block->size == size) {
		// perfect size found set the block to occupied
		target_block->is_free = false;
		// return a pointer to the memory 
		return (void*)(++target_block);
	} else if (target_block->size > (size + sizeof(mem_block))) {
		// size of the target block is bigger the what our data need, we split it
		split(target_block, size);
		// return a pointer to the memory 
		return (void*)(++target_block);
	} else
		return NULL; // no good memory found, might be good to rise an error here
}

/**
 * @brief simple function to set a memory block to free
 *
 * @param block block to free
 */
void my_free(mem_block* block)
{
	mem_block* block_to_free = block;

	block_to_free->is_free = true;
}

/** @brief main for tests */
int main()
{
	// preallocate some memory
	char* memory_pointer = (char*)malloc(RESERVED_MEMORY * sizeof(char));
	// create the first block of the linked list that points to the first byte of memory
	mem_block* free_block = (mem_block*)memory_pointer;
	// init the first block
	init_memory(free_block, RESERVED_MEMORY);

	// lets allocate three strings and check if they are allocated
	// on differents locations concatenating and printing them.
	char* my_str1 = (char*)my_malloc(free_block, 7);
	strncpy(my_str1, "Hello ", 10);
	char *my_str2 = (char*)my_malloc(free_block, 6);
	strncpy(my_str2, "World", 6);
	char* my_str3 = (char*)my_malloc(free_block, 4);
	strncpy(my_str3, "!!!", 4);
	printf("%s%s%s", my_str1, my_str2, my_str3);
	
	// free the memory, we ned --ptr to point to the header
	my_free((mem_block*)--my_str1);
	my_free((mem_block*)--my_str2);
	my_free((mem_block*)--my_str2);
}

