/*
 * =====================================================================================
 *
 *	Filename:  		sma.c
 *
 *  Description:	Base code for Assignment 3 for ECSE-427 / COMP-310
 *
 *  Version:  		1.0
 *  Created:  		6/11/2020 9:30:00 AM
 *  Revised:  		-
 *  Compiler:  		gcc
 *
 *  Author:  		Mohammad Mushfiqur Rahman
 *      
 *  Instructions:   Please address all the "TODO"s in the code below and modify 
 * 					them accordingly. Feel free to modify the "PRIVATE" functions.
 * 					Don't modify the "PUBLIC" functions (except the TODO part), unless
 * 					you find a bug! Refer to the Assignment Handout for further info.
 * =====================================================================================
 */

/* Includes */
#include "sma.h" // Please add any libraries you plan to use inside this file
/* Definitions*/
#define MAX_TOP_FREE (128 * 1024) // Max top free block size = 128 Kbytes
//	TODO: Change the Header size if required

//	TODO: Add constants here

struct block_header {
	
	int block_used; // 2 if unused, 1 if used
	int block_size;

	struct block_header* next_free;
	struct block_header* prev_free;	
};
#define FREE_BLOCK_HEADER_SIZE sizeof(struct block_header) // Size of the Header in a free memory block

struct block_tail { //mirrors the corresponding block_header, which is distance FREE_BLOCK_HEADER_SIZE + block_header->block_size away

	int block_size;
	int block_used;
};
#define FREE_BLOCK_TAIL_SIZE sizeof(struct block_tail)

typedef enum //	Policy type definition
{
	WORST,
	NEXT
} Policy;

char *sma_malloc_error;
char str[1024];
struct block_header *freeListHead = NULL;			  //	The pointer to the HEAD of the doubly linked free memory list
struct block_header *freeListTail = NULL;
struct block_header *currentFitBlock = NULL;			  //	The pointer to the TAIL of the doubly linked free memory list
int currentFitValid; //1 means valid, 2 means invalid
unsigned long totalAllocatedSize = 0; //	Total Allocated memory in Bytes
unsigned long totalFreeSize = 0;	  //	Total Free memory in Bytes in the free memory list
Policy currentPolicy = WORST;		  //	Current Policy
//	TODO: Add any global variables here

/*
 * =====================================================================================
 *	Public Functions for SMA
 * =====================================================================================
 */

/*
 *	Funcation Name: sma_malloc
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates a memory block of input size from the heap, and returns a 
 * 					pointer pointing to it. Returns NULL if failed and sets a global error.
 */
void *sma_malloc(int size)
{

	void *pMemory = NULL;

	// Checks if the free list is empty
	if (freeListHead == NULL)
	{
		// Allocate memory by increasing the Program Break
		pMemory = allocate_pBrk(size);
	}
	// If free list is not empty
	else
	{
		// Allocate memory from the free memory list
		pMemory = allocate_freeList(size);

		// If a valid memory could NOT be allocated from the free memory list
		if (pMemory == (void *)-2)
		{
			// Allocate memory by increasing the Program Break
			pMemory = allocate_pBrk(size);
		}
	}

	// Validates memory allocation
	if (pMemory < 0 || pMemory == NULL)
	{
		sma_malloc_error = "Error: Memory allocation failed!";
		return NULL;
	}

	// Updates SMA Info
	totalAllocatedSize += size;

	return pMemory;
}

/*
 *	Funcation Name: sma_free
 *	Input type:		void*
 * 	Output type:	void
 * 	Description:	Deallocates the memory block pointed by the input pointer
 */
void sma_free(void *ptr)
{
	//	Checks if the ptr is NULL
	if (ptr == NULL)
	{
		puts("Error: Attempting to free NULL!");
	}
	//	Checks if the ptr is beyond Program Break
	else if (ptr > sbrk(0))
	{
		puts("Error: Attempting to free unallocated space!");
	}
	else
	{
		//	Adds the block to the free memory list
		add_block_freeList(ptr);
	}
}

/*
 *	Funcation Name: sma_mallopt
 *	Input type:		int
 * 	Output type:	void
 * 	Description:	Specifies the memory allocation policy
 */
void sma_mallopt(int policy)
{
	// Assigns the appropriate Policy
	if (policy == 1)
	{
		currentPolicy = WORST;
	}
	else if (policy == 2)
	{
		currentPolicy = NEXT;
	}
}

/*
 *	Funcation Name: sma_mallinfo
 *	Input type:		void
 * 	Output type:	void
 * 	Description:	Prints statistics about current memory allocation by SMA.
 */
void sma_mallinfo()
{
	//	Finds the largest Contiguous Free Space (should be the largest free block)
	int largestFreeBlock = get_largest_freeBlock();
	char str[60];

	//	Prints the SMA Stats
	sprintf(str, "Total number of bytes allocated: %lu", totalAllocatedSize);
	puts(str);
	sprintf(str, "Total free space: %lu", totalFreeSize);
	puts(str);
	sprintf(str, "Size of largest contigious free space (in bytes): %d", largestFreeBlock);
	puts(str);
}

/*
 *	Funcation Name: sma_realloc
 *	Input type:		void*, int
 * 	Output type:	void*
 * 	Description:	Reallocates memory pointed to by the input pointer by resizing the
 * 					memory block according to the input size.
 */
void *sma_realloc(void *ptr, int size)
{
	// TODO: 	Should be similar to sma_malloc, except you need to check if the pointer address
	//			had been previously allocated.
	// Hint:	Check if you need to expand or contract the memory. If new size is smaller, then
	//			chop off the current allocated memory and add to the free list. If new size is bigger
	//			then check if there is sufficient adjacent free space to expand, otherwise find a new block
	//			like sma_malloc.
	//			Should not accept a NULL pointer, and the size should be greater than 0.
	if(ptr ==NULL || size<=0){
		puts("sma_realloc was unsuccessful: invalid inputs");
		return (void*) -2;
	}
	char *ptrHead;
	ptrHead = ptr - FREE_BLOCK_HEADER_SIZE;
	void *pMemory = NULL;
	int pSize = get_blockSize(ptr);

	if (size > pSize){ //check if there is sufficient adjacent free space to expand, otherwise find a new block

		pMemory = sma_malloc(size);
		memcpy(pMemory ,ptr, pSize);
		sma_free(ptr);
		return pMemory;

	} else if (size < pSize){ //chop off the current allocated memory and add to the free list.

		if( ((struct block_header*)ptrHead)->block_used == 2){ //from free list
			allocate_block(pMemory, size , pSize-size, 0);
		} else if(((struct block_header*)ptrHead)->block_used == 1){ //not free
			allocate_block(pMemory, size , pSize-size, 1);
		}

		return pMemory;

	} else if (size = pSize) {//nothing to be done

		return ptr;

	}
}

/*
 * =====================================================================================
 *	Private Functions for SMA
 * =====================================================================================
 */

/*
 *	Funcation Name: allocate_pBrk
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory by increasing the Program Break
 */
void *allocate_pBrk(int size)
{
	void *newBlock;
	int excessSize;

	//	DONE TODO: 	Allocate memory by incrementing the Program Break by calling sbrk() or brk()
	//	Hint:	Getting an exact "size" of memory might not be the best idea. Why?
	//			Also, if you are getting a larger memory, you need to put the excess in the free list

	//	Allocates the Memory Block

	newBlock = sbrk(size + 64*1024); //64kb overhead to avoid repeated syscalls

	excessSize = (64*1024) - (FREE_BLOCK_TAIL_SIZE+FREE_BLOCK_HEADER_SIZE);

	allocate_block(newBlock+FREE_BLOCK_HEADER_SIZE, size , excessSize, 0);

	return newBlock+FREE_BLOCK_HEADER_SIZE; //returns a pointer that points to the break of the heap
}

/*
 *	Funcation Name: allocate_freeList
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory from the free memory list
 */
void *allocate_freeList(int size)
{
	void *pMemory = NULL;

	if (currentPolicy == WORST)
	{
		// Allocates memory using Worst Fit Policy
		pMemory = allocate_worst_fit(size);
	}
	else if (currentPolicy == NEXT)
	{
		// Allocates memory using Next Fit Policy
		pMemory = allocate_next_fit(size);
	}
	else
	{
		pMemory = NULL;
	}

	return pMemory;
}

/*
 *	Funcation Name: allocate_worst_fit
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory using Worst Fit from the free memory list
 */
void *allocate_worst_fit(int size)
{
	void *worstBlock;
	worstBlock = (struct block_header *)freeListHead;
	int excessSize;
	int blockFound = 0;
	void *temp;
	temp = (struct block_header *)freeListHead;
	//	TODO: 	Allocate memory by using Worst Fit Policy
	//	Hint:	Start off with the freeListHead and iterate through the entire list to get the largest block
	while (temp != NULL){
		if ( get_blockSize(temp+FREE_BLOCK_HEADER_SIZE) >= size ){

			if(get_blockSize(temp+FREE_BLOCK_HEADER_SIZE) >= get_blockSize(worstBlock+FREE_BLOCK_HEADER_SIZE)){
				worstBlock = temp;
				blockFound = 1;
			}
		}
		temp = ((struct block_header *)temp)->next_free;
	}
	//	Checks if appropriate block is found.
	if (blockFound)
	{
		excessSize = (((struct block_header *)worstBlock)->block_size) - size;

		//	Allocates the Memory Block
		allocate_block(worstBlock+FREE_BLOCK_HEADER_SIZE, size, excessSize, 1);
		return worstBlock+FREE_BLOCK_HEADER_SIZE;
	}
	else
	{
		//	Assigns invalid address if appropriate block not found in free list
		return (void *)-2;
	}

	//return worstBlock+FREE_BLOCK_HEADER_SIZE;
}

/*
 *	Funcation Name: allocate_next_fit
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory using Next Fit from the free memory list
 */
void *allocate_next_fit(int size)
{
	void *nextBlock = NULL;
	int excessSize;
	int blockFound = 0;
	void *temp;
	//	TODO: 	Allocate memory by using Next Fit Policy
	//	Hint:	Start off with the freeListHead, and keep track of the current position in the free memory list.
	//			The next time you allocate, it should start from the current position
//first go in increasing address size only

	if (currentFitValid == 1 ){ //currentFitValid and currentFitBlock both help to determine if we have an appropriate "resume point" for nextFit

		temp = currentFitBlock;
		while (temp != NULL){
			if ( get_blockSize(temp+FREE_BLOCK_HEADER_SIZE) >= size ){

				if((temp < nextBlock && temp >= (void *)currentFitBlock) || (blockFound == 0 && temp >= (void *)currentFitBlock)) {
					nextBlock = temp;
					blockFound = 1;
				}
			}
		temp = ((struct block_header *)temp)->next_free;
		}
	}

	if(blockFound == 1) goto FOUNDBLOCK; //if the first search (only on resume) yielded result, go straight to allocating this block
	blockFound = 0;

//then iterate through the whole list (general case)

	temp = freeListHead;
	//int iGeneral = 0;
	while (temp != NULL){
		if ( get_blockSize(temp+FREE_BLOCK_HEADER_SIZE) >= size ){

			if(temp < nextBlock || blockFound == 0){
				nextBlock = temp;
				blockFound = 1;
			}
		}
		temp = ((struct block_header *)temp)->next_free;
	}

FOUNDBLOCK:
	//	Checks if appropriate found is found.
	if (blockFound)
	{
		excessSize = (((struct block_header *)nextBlock)->block_size) - size;

		if (excessSize > FREE_BLOCK_TAIL_SIZE+FREE_BLOCK_HEADER_SIZE){//determine if we should consider the excess size leftover from worstfit as a candidate for nextFit
			currentFitValid = 1;
			currentFitBlock = nextBlock + FREE_BLOCK_HEADER_SIZE + FREE_BLOCK_TAIL_SIZE + size; 
			//pointer for nextFit calls, knowing where to resume

			((struct block_header *)currentFitBlock)->block_size = excessSize - FREE_BLOCK_HEADER_SIZE - FREE_BLOCK_TAIL_SIZE;
		} else {
			currentFitBlock = NULL;
			currentFitValid = 2;
		}
		//	Allocates the Memory Block
		allocate_block(nextBlock+FREE_BLOCK_HEADER_SIZE, size, excessSize, 1);
		return nextBlock+FREE_BLOCK_HEADER_SIZE;
	}
	else
	{
		//	Assigns invalid address if appropriate block not found in free list
		return (void *)-2;
	}
}

/*
 *	Funcation Name: allocate_block
 *	Input type:		void*, int, int, int
 * 	Output type:	void
 * 	Description:	Performs routine operations for allocating a memory block
 */
void allocate_block(void *newinputBlock, int size, int excessSize, int fromFreeList)
{
	char *newBlock;
	newBlock = (char *)newinputBlock - FREE_BLOCK_HEADER_SIZE;
	char *excessFreeBlock; //	pointer for any excess free block
	int addFreeBlock;

	// 	Checks if excess free size is big enough to be added to the free memory list
	//	Helps to reduce external fragmentation

	//	TODO: Adjust the condition based on your Head and Tail size (depends on your TAG system)
	//	Hint: Might want to have a minimum size greater than the Head/Tail sizes
	addFreeBlock = excessSize > FREE_BLOCK_HEADER_SIZE + FREE_BLOCK_TAIL_SIZE;

	if (addFreeBlock) //append to free list tail
	{
		//	TODO: Create a free block using the excess memory size, then assign it to the Excess Free Block	 
		//newBlock about to be USED
		((struct block_header *)newBlock)->block_size = size;
		((struct block_header *)newBlock)->block_used = 1;

		char *t;
		t = (newBlock + size + FREE_BLOCK_HEADER_SIZE);
		((struct block_tail *)t)->block_size = size;
		((struct block_tail *)t)->block_used = 1;

//excessBlock assigned to FREEBLOCKLIST		

		excessFreeBlock = t + FREE_BLOCK_TAIL_SIZE;

		excessSize = excessSize - (FREE_BLOCK_HEADER_SIZE+FREE_BLOCK_TAIL_SIZE);
		((struct block_header *)excessFreeBlock)->block_size = excessSize;
		((struct block_header *)excessFreeBlock)->block_used = 2;
		((struct block_header *)excessFreeBlock)->next_free = NULL;
		((struct block_header *)excessFreeBlock)->prev_free = NULL;
		
		char *t2;
		t2 = (excessFreeBlock + excessSize + FREE_BLOCK_HEADER_SIZE);
		((struct block_tail *)t2)->block_size = excessSize;
		((struct block_tail *)t2)->block_used = 2;

		//	Checks if the new block was allocated from the free memory list
		if (fromFreeList)
		{
			//	Removes new block and adds the excess free block to the free list
			replace_block_freeList(newBlock + FREE_BLOCK_HEADER_SIZE, excessFreeBlock + FREE_BLOCK_HEADER_SIZE);
		}
		else
		{
			//	Adds excess free block to the free list
			add_block_freeList(excessFreeBlock+FREE_BLOCK_HEADER_SIZE);
		}
	}
	//	Otherwise add the excess memory to the new block
	else //excessSize is too small
	{
		//	TODO: Add excessSize to size and assign it to the new Block
		((struct block_header *)newBlock)->block_size = excessSize + size;
		((struct block_header *)newBlock)->block_used = 1;
		((struct block_header *)newBlock)->next_free = NULL;
		((struct block_header *)newBlock)->prev_free = NULL;
		char *t;

		t = (newBlock + excessSize + FREE_BLOCK_HEADER_SIZE);
		((struct block_tail *)t)->block_size = excessSize+size;
		((struct block_tail *)t)->block_used = 1;

		//	Checks if the new block was allocated from the free memory list
		if (fromFreeList)
		{
			//	Removes the new block from the free list
			remove_block_freeList(newBlock+ FREE_BLOCK_HEADER_SIZE);
		}
	}
}

/*
 *	Funcation Name: replace_block_freeList
 *	Input type:		void*, void*
 * 	Output type:	void
 * 	Description:	Replaces old block with the new block in the free list
 */
void replace_block_freeList(void *oldinputBlock, void *newinputBlock)
{
	char *oldBlock;
	char *newBlock;
	oldBlock = oldinputBlock - FREE_BLOCK_HEADER_SIZE;
	newBlock = newinputBlock - FREE_BLOCK_HEADER_SIZE;
	//	TODO: Replace the old block with the new block
	((struct block_header *)newBlock)->next_free = ((struct block_header *)oldBlock)->next_free;
	((struct block_header *)newBlock)->prev_free = ((struct block_header *)oldBlock)->prev_free;
//assign pointers TO the old block TO the newBlock
	if ((((struct block_header *)oldBlock)->next_free) != NULL )
		((struct block_header *)((struct block_header *)oldBlock)->next_free)->prev_free = (struct block_header *)newBlock;

	if ((((struct block_header *)oldBlock)->prev_free) != NULL )
		((struct block_header *)((struct block_header *)oldBlock)->prev_free)->next_free = (struct block_header *)newBlock;

	//	Updates SMA info
	if (freeListHead == (struct block_header *)oldBlock){
		freeListHead = (struct block_header *)newBlock;
	}
	if (freeListTail == (struct block_header *)oldBlock){
		freeListTail = (struct block_header *)newBlock;
	}
	totalFreeSize -= get_blockSize(oldinputBlock);
}

/*
 *	Funcation Name: add_block_freeList
 *	Input type:		void*
 * 	Output type:	void
 * 	Description:	Adds a memory block to the the free memory list
 */
void add_block_freeList(void *inputBlock) //assume we are pointing at the allocated space // NOT the header
{
	char* block;
	block = inputBlock - FREE_BLOCK_HEADER_SIZE;
	//	TODO: 	Add the block to the free list
	//	Hint: 	You could add the free block at the end of the list, but need to check if there
	//			exists a list. You need to add the TAG to the list.
	//			Also, you would need to check if merging with the "adjacent" blocks is possible or not.
	//			Merging would be tideous. Check adjacent blocks, then also check if the merged
	//			block is at the top and is bigger than the largest free block allowed (128kB).

	//	Updates SMA info
		char *t;
		t = (block + FREE_BLOCK_HEADER_SIZE + (((struct block_header *)block)->block_size));
		((struct block_tail *)t)->block_size = (((struct block_header *)block)->block_size);

		if( ((struct block_header *)block)->block_used == 1){ //check if block was previously allocated
			((struct block_header *)block)->block_used = 2;
			((struct block_tail *)t)->block_used = 2;
			totalAllocatedSize -= (unsigned long)get_blockSize(block+FREE_BLOCK_HEADER_SIZE);
		}
		
		totalFreeSize += (unsigned long)get_blockSize(block+FREE_BLOCK_HEADER_SIZE);
		int mergeOccured = 0;//check if new freeblock is "nomad" ; if it did not merge, we then have to append it to the tail of the list
		//TODO: merge!!
		int debugMerge = 2;//quick switch to not run merge IGNORE

		if(freeListHead != NULL ){ //set up prev and next blocks in memory, so our merging process is facilitated
			char *nextHead;
			nextHead = t + FREE_BLOCK_TAIL_SIZE;

			char *nextTail;
			nextTail = nextHead + ((struct block_header *)nextHead)->block_size + FREE_BLOCK_HEADER_SIZE;

			char *previousTail;
			previousTail = block - FREE_BLOCK_TAIL_SIZE;

			char *previousHead;
			previousHead = previousTail - ((struct block_tail *)previousTail)->block_size - FREE_BLOCK_HEADER_SIZE;

 //case where next memory block is free
 			if (((struct block_header *)nextHead) != NULL && debugMerge == 2){ // keep new block head, next block tail
 				
 				if (((struct block_header *)nextHead)->block_used == 2){
					mergeOccured =1;

					t = nextTail;

					((struct block_header *)block)->block_size += (((struct block_header *)nextHead)->block_size + FREE_BLOCK_HEADER_SIZE +FREE_BLOCK_TAIL_SIZE);
					((struct block_header *)block)->block_used = 2;

					
					((struct block_header *)block)->next_free = ((struct block_header *)nextHead)->next_free;
 					((struct block_header *)block)->prev_free = ((struct block_header *)nextHead)->prev_free;

					if (((struct block_header *)((struct block_header *)nextHead)->next_free) != NULL )
						((struct block_header *)((struct block_header *)nextHead)->next_free)->prev_free = (struct block_header *)block;

					if (((struct block_header *)((struct block_header *)nextHead)->prev_free) != NULL )
						((struct block_header *)((struct block_header *)nextHead)->prev_free)->next_free = (struct block_header *)block;
					
 					if (freeListHead == (struct block_header *)nextHead){
 						freeListHead = (struct block_header *)block;
 					}
 					if (freeListTail == (struct block_header *)nextHead){
 						freeListTail = (struct block_header *)block;
 					}

 					((struct block_tail *)t)->block_size = ((struct block_header *)block)->block_size;
 					((struct block_tail *)t)->block_used = 2;
					totalFreeSize +=(FREE_BLOCK_HEADER_SIZE+FREE_BLOCK_TAIL_SIZE);
 				}	
 			}
 //case where previous memory block is free 
 			if (((struct block_tail *)previousTail) != NULL){ //extend previous block! keep previous block head, use current block tail; next
 				
 				if (((struct block_tail *)previousTail)->block_used == 2){
					mergeOccured=1;
 					((struct block_header *)previousHead)->block_used = 2;
 					((struct block_header *)previousHead)->block_size += (((struct block_header *)block)->block_size + FREE_BLOCK_HEADER_SIZE +FREE_BLOCK_TAIL_SIZE);
 					((struct block_tail *)t)->block_size = ((struct block_header *)previousHead)->block_size;
 					((struct block_tail *)t)->block_used = 2;

					totalFreeSize +=(FREE_BLOCK_HEADER_SIZE+FREE_BLOCK_TAIL_SIZE);

					block = previousHead;
 				}
 			}
 		}
 		if (mergeOccured == 0){//append to free list tail! IF we didnt merge (case of nomad block)
 			if (freeListHead == NULL){//is free list empty?
				freeListHead = (struct block_header *)block;
				freeListTail = (struct block_header *)block;
				((struct block_header *)block)->prev_free = NULL;
			}
			else{
				((struct block_header *)freeListTail)->next_free = (struct block_header *)block;
				((struct block_header *)block)->prev_free = freeListTail;
			}

			((struct block_header *)block)->next_free = NULL;
			freeListTail = (struct block_header *)block;
 		}
		//check if i have to lower break
		if(sbrk(0) == (t+FREE_BLOCK_TAIL_SIZE) && ((struct block_header *)block)->block_size > 128*1024){

			int sizeDiff = ((struct block_header *)block)->block_size - (64*1024);

			((struct block_header *)block)->block_size = 64*1024;
			t = block + FREE_BLOCK_HEADER_SIZE+ ((struct block_header *)block)->block_size;
 			((struct block_tail *)t)->block_size = ((struct block_header *)block)->block_size;
 			((struct block_tail *)t)->block_used = 2;
			sbrk(-sizeDiff);
			totalFreeSize -= sizeDiff;
		}
}

/*
 *	Funcation Name: remove_block_freeList
 *	Input type:		void*
 * 	Output type:	void
 * 	Description:	Removes a memory block from the the free memory list
 */
void remove_block_freeList(void *inputblock)
{
	char *block;
	block = inputblock - FREE_BLOCK_HEADER_SIZE;
	//	TODO: 	Remove the block from the free list
	//	Hint: 	You need to update the pointers in the free blocks before and after this block.
	//			You also need to remove any TAG in the free block.

	//regular case
	if((freeListHead != (struct block_header *)block) && (freeListTail != (struct block_header *)block)) {
		(((struct block_header *)block)->next_free)->prev_free = (((struct block_header *)block)->prev_free);
		(((struct block_header *)block)->prev_free)->next_free = (((struct block_header *)block)->next_free);

	}
	//block is the only block in freeList
	else if((freeListHead == (struct block_header *)block) && (freeListTail == (struct block_header *)block)) {
		//no need to dereference anything
		freeListHead = NULL;
		freeListTail =NULL;

	}//AT LEAST 1 other element in free list
	else if (freeListHead == (struct block_header *)block){
		freeListHead = ((struct block_header *)block)->next_free;
		(((struct block_header *)block)->next_free)->prev_free = NULL;
	}
	else if (freeListTail == (struct block_header *)block){
		freeListTail = ((struct block_header *)block)->prev_free;
		(((struct block_header *)block)->prev_free)->next_free = NULL;
	}


	char *t;
	t = block + FREE_BLOCK_HEADER_SIZE + (((struct block_header *)block)->block_size);
	((struct block_tail *)t)->block_used = 1; 

	totalAllocatedSize += get_blockSize(block + FREE_BLOCK_HEADER_SIZE);

	totalFreeSize -= get_blockSize(block + FREE_BLOCK_HEADER_SIZE);
}

/*
 *	Funcation Name: get_blockSize
 *	Input type:		void*
 * 	Output type:	int
 * 	Description:	Extracts the Block Size
 */
int get_blockSize(void *ptr)
{

	int pSize;
	char *headerholder;
	headerholder = ptr - FREE_BLOCK_HEADER_SIZE;
	pSize = ((struct block_header *)headerholder)->block_size;

	return pSize;
}

/*
 *	Funcation Name: get_largest_freeBlock
 *	Input type:		void
 * 	Output type:	int
 * 	Description:	Extracts the largest Block Size
 */
int get_largest_freeBlock()
{
	void* cur;
	int largestBlockSize = 0;
	if (freeListHead == NULL){
		puts("No free blocks!");
	} else {
		cur = freeListHead;
		largestBlockSize = get_blockSize((char *)cur+FREE_BLOCK_HEADER_SIZE);
		while (cur != NULL){
			if (get_blockSize(cur+FREE_BLOCK_HEADER_SIZE)>= largestBlockSize){

				largestBlockSize = get_blockSize((char *)cur+FREE_BLOCK_HEADER_SIZE);
			}	
			cur = ((struct block_header *)cur)->next_free;
		}
	}

	//	DONE TODO: Iterate through the Free Block List to find the largest free block and return its size

	return largestBlockSize;
}

void printFreeList() { //sequentially print out freeList element sizes and addresses for debugging.

	void* cur;

	if (freeListHead == NULL){
		puts("No free blocks!");
	} else {
		puts("========================================");
		puts("here are the following blocks:");
		cur = freeListHead;
		while (cur != NULL){
			sprintf(str, "block address: %p \nblock size: %d", cur, get_blockSize(cur+FREE_BLOCK_HEADER_SIZE));
			puts(str);
			cur = ((struct block_header *)cur)->next_free;
		}
		puts("========================================");
	}
}