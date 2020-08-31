// The following memory allocator is based on block memory allocation. The
// available memory space is divided in blocks of the same size (with size
// equals to the maximum possible size for single allocation) in order to reduce
// the memory fragmentation that is typical effect with allocation of different size.
// The dynamic memory allocation and deallocation are performed once respectively
// when the program starts and when the program ends in order to have a deterministic
// and fast real time response

#define MEMORY_SIZE 4096 // bytes available for dynamic allocation
#define BLOCK_SIZE 32 // maximum number of bytes for a single allocation
#define N_BLOCKS (MEMORY_SIZE/BLOCK_SIZE)  // number of 32B blocks to fill the available 4KB

void *block_ptr[N_BLOCKS]; //pointers to the N_BLOCKS 32B blocks
uint8_t block_used[N_BLOCKS]; //flags to mark if a block has been allocated
uint8_t next_candidate_block=0; //index of the block candidates for the next allocation
uint8_t last_deallocated_block=N_BLOCKS; //index of the last deallocated block
uint8_t n_used_blocks=0; //counter of the allocated blocks

void create_allocator(){
    uint8_t i;
    for(i=0; i<N_BLOCKS; i++){
        blocks_ptr[i]=malloc(BLOCK_SIZE);
        memset(blocks_ptr[i], 0, BLOCK_SIZE);
        block_used[i]=0;
    }
}

void *allocate(size_t sz){
    uint8_t i;
    uint8_t idx;
    uint8_t start_idx;

    if(sz>BLOCK_SIZE)
        return NULL;

    if(n_used_blocks==N_BLOCKS)
        return NULL;

    //The following logic is used to accelerate the identification of the next usable block.
    //When a block has been deallocate, it will be the next block to be allocated again
    //otherwise, the next block never allocated will be chosen

    if(last_deallocated_block<N_BLOCKS){ //if a block has been deallocated...
        start_idx=last_deallocated_block;//...it will be used for the next allocation
        last_deallocated_block=N_BLOCKS; // reset the last_deallocated_block index
    }
    else{
        start_idx=next_candidate_block;
        next_candidate_block=(next_candidate_block+1)%N_BLOCKS;
    }

    for(i=0; i<N_BLOCKS; i++){
        idx=(start_idx+i)%N_BLOCKS;
        if(blocks_ptr[idx] && block_used[idx]==0){
            memset(blocks_ptr[idx], 0, BLOCK_SIZE);
            block_used[idx]=1;
            n_used_blocks++;
            return blocks_ptr[idx];
        }
    }

    return NULL;
}

void deallocate(void *ptr){
    uint8_t idx;

    if(ptr == NULL)
        return;

    //fast identification of the block to deallocate
    idx=(ptr-blocks_ptr[0])/BLOCK_SIZE;

    memset(blocks_ptr[idx], 0, BLOCK_SIZE);
    block_used[idx]=0;
    last_deallocated_block=idx;
    n_used_blocks--;
}

void destroy_allocator(){
    uint8_t i;
    for(i=0; i<N_BLOCKS; i++){
        free(blocks_ptr[i]);
        blocks_ptr[i]=NULL;
        block_used[i]=0;
    }
}
