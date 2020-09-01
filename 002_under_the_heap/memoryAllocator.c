#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "memoryAllocator.h"

static int isFree(header_t* head)
{
	if(head->sizeP % 4 == 0){
		return 1;
	}else if(head->sizeP % 4 == 1){
		return 0;
	}
  return 0;
}

static void  setAlloc(header_t* head)
{	
	if(head->sizeP % 4 == 1){
		printf("its already allocated");
	}else if(head->sizeP % 4 == 0){
		head->sizeP += 1;
	}	
 }

static void setFree(header_t* head)
{
	if(head->sizeP % 4 == 0){
		printf("its already freed");
	}else if(head->sizeP % 4 == 1){
		head->sizeP -= 1;
	}
}

int initMemoryAllocator(int size)
{
	int page_size;
	int pading_size;
	int fd;
	int alloc_size;
	void* space_ptr;
	static int allocated_once = 0;

	if(!allocated_once)
	{
		printf("initMemoryAllocator allocated a space during a previous call \n");
		return -1;
	}
	if(size <= 0)
	{
		printf("Requested block size is negative! \n");
		return -1;
	}

	/* Get the page_size */
	page_size = getpagesize();

	/* Calculate pading_size as the padding required to round up size to a multiple of page_size */
	pading_size = size % page_size;
	pading_size = (page_size - pading_size) % page_size;

	alloc_size = size + pading_size;

	/* Using mmap to allocate memory */
	fd = open("/dev/zero", O_RDWR);
	if(fd == -1)
	{
		printf("Cannot open /dev/zero\n");
		return -1;
	}
	space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (MAP_FAILED == space_ptr)
	{
		printf("mmap cannot allocate space\n");
		allocated_once = 0;
		return -1;
	}

	allocated_once = 1;

	/* To begin with, there is only one big, free block */
	list_head = (header_t*)space_ptr;
	list_head->next = NULL;
	/* Remember that the 'size' stored in block size excludes the space for the header */
	list_head->sizeP = alloc_size - (int)sizeof(header_t);

	return 0;
}

void* alloc_memory(int size)
{
	if(size <= 0){
		return NULL;
	}
	/* Check size and round it up to a multiple of 4 */
	int pading_size = 0;
	/* keep track of pading_size to round size to multiple of 4*/
	if((size % 4) != 0){
		pading_size = size % 4;
		pading_size = (4 - pading_size) % 4;
		size = size + pading_size;
	}	
	// Search for the best fit block in the free list
	header_t *blockToCheck = list_head;
	header_t *blockToKeep = NULL;
	/* to check whether the best block to find */
	int found = 0; 
	while(blockToCheck != NULL){
		/* if block size status is exactly the size wanted to alloc, alloc and return it */	
		if(blockToCheck->sizeP == size){
			blockToKeep = blockToCheck;
			setAlloc(blockToKeep);
			found = 1;
			return (void*) (blockToKeep + sizeof(header_t));
		/*if block is freed and is more than size keep the block*/
		}else if(isFree(blockToCheck)){
			if(blockToCheck->sizeP > size){
				if(blockToKeep == NULL){
				/*keep the first block the meets the requirements */
				blockToKeep = blockToCheck;
				found = 1;
			/* if the keep block size is bigger than current check block and > size,
			   reset keepblock */
			}else if(blockToCheck->sizeP < blockToKeep->sizeP){
				blockToKeep = blockToCheck;
				found = 1;
				}
			}
		}	
		/* Check block to next block, repeat while loop */
		blockToCheck = blockToCheck->next;
	}
	
	/* if can't find any suitable block return NULL */
	if(found == 0){
		return NULL;
	}
	/* If a block is found, check to see if we can split it,
	  i.e it has space leftover for a new block(header + payload)*/
	int freeSpace = blockToKeep->sizeP - size;
	int minBlock = 4 + sizeof(header_t);	
	int pading_size = 0;
	/* compare free space and min block, if free space is big enug as min block split into another
	   free block */
	if(freeSpace >= minBlock){
		/* assign new block address to freespace */
		header_t* newBlock = (header_t*) ((void*) blockToKeep + sizeof(header_t) + size);
		/* If split, update the size of the resulting blocks*/
		newBlock->next = blockToKeep->next; 
		blockToKeep->next = newBlock;
		newBlock->sizeP = freeSpace - sizeof(header_t);
	/* Mark the allocated block and return it*/
	}else if(freeSpace < minBlock){
		/* otherwise if freespace is not big enough to be split, keep it as pading_size */
		pading_size = freeSpace;
	}
	/* remember to add pading_size of freeSpace */
	blockToKeep->sizeP = size + pading_size;
	setAlloc(blockToKeep);
	/* return address to pay load not headre */
	return (void*)(blockToKeep + sizeof(header_t));
}

int free_memory(void *ptr)
{
	/* Check if the pointer is pointing to the start of the payload of an allocated block*/
	if(ptr == NULL){
		return -1;
	}
	header_t *curr = list_head;
	header_t *prev = NULL;
	int found = 0;

	/*check whether ptr is pointing to the first node*/
	/*assign a block to payload of a block */
	void *listhead = (void*)(curr + sizeof(header_t));
	/* if the address is the same as ptr */
	if(listhead == ptr){
		/* confirm found to 1 */
		found = 1;
		/* set block to free and check whether next block is free */
		if(isFree(curr)){
			return -1;
		}else if(!isFree(curr)){
			setFree(curr);
			if(isFree(curr->next)){
				/* remember to add size of block header as it is hiddern */
				curr->sizeP = curr->sizeP + sizeof(header_t) + (curr->next)->sizeP;
				curr->next = (curr->next)->next;
			}
			/* return done but what if ptr is not referring to first block
			   continue next the method */
			return 0;
		}
	}

	/*search for ptr in list*/
	while(curr->next!= NULL && found == 0){
		/* always keep track of the block prev and after curr */
		prev = curr;
		curr = curr->next;
		void *listCheck = (void*) (curr + sizeof(header_t));
		if(listCheck == ptr){
			/* if its already freed, just return failed */
			if(isFree(curr)){
				return -1;
			}
			found = 1;
		}
	}
	/* If so, free it.*/
	if(found == 0){
		return -1;
	}else if(found == 1){
		if(!isFree(curr)){
			setFree(curr);
		}
	}
	/* Check the blocks to the left and right */
	if(isFree(prev)){
		if(isFree(curr->next)){
			prev->sizeP = prev->sizeP + sizeof(header_t) + curr->sizeP + sizeof(header_t) + (curr->next)->sizeP;
			/* set the new free block next to whatever next*/
			prev->next = (curr->next)->next;
		}else{
			prev->sizeP = prev->sizeP + sizeof(header_t) + curr->sizeP;
			/*update next*/
			prev->next = curr->next;
		} 
	}else if(isFree(curr->next)){
		curr->sizeP = curr->sizeP + sizeof(header_t) + (curr->next)->sizeP;
		/*update next*/
		curr->next = (curr->next)->next;
	}
	/* with either or both of them*/
	return 0;
}
