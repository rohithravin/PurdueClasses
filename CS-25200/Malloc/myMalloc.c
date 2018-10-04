#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "myMalloc.h"
#include "printing.h"

/* Due to the way assert() prints error messges we use out own assert function
 * for deteminism when testing assertions
 */
#ifdef TEST_ASSERT
  inline static void assert(int e) {
    if (!e) {
      const char * msg = "Assertion Failed!\n";
      write(2, msg, strlen(msg));
      exit(1);
    }
  }
#else
  #include <assert.h>
#endif
//hi
/*
 * Mutex to ensure thread safety for the freelist
 */
static pthread_mutex_t mutex;

/*
 * Array of sentinel nodes for the freelists
 */
header freelistSentinels[N_LISTS];

/*
 * Pointer to the second fencepost in the most recently allocated chunk from
 * the OS. Used for coalescing chunks
 */
header * lastFencePost;

/*
 * Pointer to maintian the base of the heap to allow printing based on the
 * distance from the base of the heap
 */ 
void * base;

/*
 * List of chunks allocated by  the OS for printing boundary tags
 */
header * osChunkList [MAX_OS_CHUNKS];
size_t numOsChunks = 0;

/*
 * direct the compiler to run the init function before running main
 * this allows initialization of required globals
 */
static void init (void) __attribute__ ((constructor));

// Helper functions for manipulating pointers to headers
static inline header * get_header_from_offset(void * ptr, ptrdiff_t off);
static inline header * get_left_header(header * h);
static inline header * ptr_to_header(void * p);

// Helper functions for allocating more memory from the OS
static inline void initialize_fencepost(header * fp, size_t left_size);
static inline void insert_os_chunk(header * hdr);
static inline void insert_fenceposts(void * raw_mem, size_t size);
static header * allocate_chunk(size_t size);

// Helper functions for freeing a block
static inline void deallocate_object(void * p);

// Helper functions for allocating a block
static inline header * allocate_object(size_t raw_size);

// Helper functions for verifying that the data structures are structurally 
// valid
static inline header * detect_cycles();
static inline header * verify_pointers();
static inline bool verify_freelist();
static inline header * verify_chunk(header * chunk);
static inline bool verify_tags();

static void init();

static bool isMallocInitialized;

/**
 * @brief Helper function to retrieve a header pointer from a pointer and an 
 *        offset
 *
 * @param ptr base pointer
 * @param off number of bytes from base pointer where header is located
 *
 * @return a pointer to a header offset bytes from pointer
 */
static inline header * get_header_from_offset(void * ptr, ptrdiff_t off) {
	return (header *)((char *) ptr + off);
}

/**
 * @brief Helper function to get the header to the right of a given header
 *
 * @param h original header
 *
 * @return header to the right of h
 */
header * get_right_header(header * h) {
	return get_header_from_offset(h, get_size(h));
}

/**
 * @brief Helper function to get the header to the left of a given header
 *
 * @param h original header
 *
 * @return header to the right of h
 */
inline static header * get_left_header(header * h) {
  return get_header_from_offset(h, -h->left_size);
}

/**
 * @brief Fenceposts are marked as always allocated and may need to have
 * a left object size to ensure coalescing happens properly
 *
 * @param fp a pointer to the header being used as a fencepost
 * @param left_size the size of the object to the left of the fencepost
 */
inline static void initialize_fencepost(header * fp, size_t left_size) {
	set_state(fp,FENCEPOST);
	set_size(fp, ALLOC_HEADER_SIZE);
	fp->left_size = left_size;
}

/**
 * @brief Helper function to maintain list of chunks from the OS for debugging
 *
 * @param hdr the first fencepost in the chunk allocated by the OS
 */
inline static void insert_os_chunk(header * hdr) {
  if (numOsChunks < MAX_OS_CHUNKS) {
    osChunkList[numOsChunks++] = hdr;
  }
}

/**
 * @brief given a chunk of memory insert fenceposts at the left and 
 * right boundaries of the block to prevent coalescing outside of the
 * block
 *
 * @param raw_mem a void pointer to the memory chunk to initialize
 * @param size the size of the allocated chunk
 */
inline static void insert_fenceposts(void * raw_mem, size_t size) {
  // Convert to char * before performing operations
  char * mem = (char *) raw_mem;

  // Insert a fencepost at the left edge of the block
  header * leftFencePost = (header *) mem;
  initialize_fencepost(leftFencePost, ALLOC_HEADER_SIZE);

  // Insert a fencepost at the right edge of the block
  header * rightFencePost = get_header_from_offset(mem, size - ALLOC_HEADER_SIZE);
  initialize_fencepost(rightFencePost, size - 2 * ALLOC_HEADER_SIZE);
}

/**
 * @brief Allocate another chunk from the OS and prepare to insert it
 * into the free list
 *
 * @param size The size to allocate from the OS
 *
 * @return A pointer to the allocable block in the chunk (just after the 
 * first fencpost)
 */
static header * allocate_chunk(size_t size) {
  void * mem = sbrk(size);
  
  insert_fenceposts(mem, size);
  header * hdr = (header *) ((char *)mem + ALLOC_HEADER_SIZE);
  set_state(hdr, UNALLOCATED);
  set_size(hdr, size - 2 * ALLOC_HEADER_SIZE);
  hdr->left_size = ALLOC_HEADER_SIZE;
  return hdr;
}

/**
 * @brief Finds the correct index for the freeSentinelList with the given size
 *
 * @param requested_size The size that needs to convert to the corresponding index
 *
 * @return A number that is an index to the freeSentinelList based on the size given
 */
static inline int getIndex(size_t requested_size){
  int size = ((requested_size - ALLOC_HEADER_SIZE)+7) & ~7;
  int starting_index = (size / 8) -1;
  if(starting_index > (N_LISTS-1))
        starting_index = N_LISTS -1;
  return starting_index;
}


static inline void * allocate(size_t requested_size){
  int starting_index = getIndex(requested_size);
  header * freelist;
  header * allocate_node = NULL;
  void * answer;
  bool block_allocated = false;
  //FIND THE NODE WE WANT TO ALLOCATE FROM THE AVAIALBE FREELIST
  for(int x = starting_index; x < N_LISTS; x++){
    freelist = &freelistSentinels[x];
    if(freelist->next != freelist){
      header * node = freelist->next;
      while(node != freelist){
        if(get_size(node) >= requested_size){
          allocate_node = node;
            starting_index = x;
          break;
        }
        node = node->next;
      }
      if(allocate_node != NULL)
        break;
    }
  }
  if(allocate_node == NULL)
    return NULL;
  //CASE 1: SIZE IS TOO BIG SO WE ARE SPLITTING IT
  if(get_size(allocate_node) - requested_size > ALLOC_HEADER_SIZE  ) {
      header * other_node = get_header_from_offset((void *)(allocate_node), get_size(allocate_node) - requested_size);
      set_size_and_state(other_node,requested_size,ALLOCATED);
      set_size_and_state(allocate_node,get_size(allocate_node) - requested_size,UNALLOCATED);
      other_node->left_size = get_size(allocate_node);
      header * right_block =get_header_from_offset((void *)(other_node), get_size(other_node));
      right_block->left_size = get_size(other_node);
      if((((get_size(allocate_node)-ALLOC_HEADER_SIZE) / 8) -1 )  < starting_index) {
          allocate_node->next->prev = allocate_node->prev;
          allocate_node->prev->next = allocate_node->next;
          starting_index = ( ( (get_size(allocate_node)-ALLOC_HEADER_SIZE) / 8) -1 ) ;
          freelist = &freelistSentinels[starting_index];
          allocate_node->prev = freelist;
          allocate_node->next = freelist->next;
          freelist->next->prev = allocate_node;
          freelist->next = allocate_node;
      }
      return (void *)((char *)(other_node) + ALLOC_HEADER_SIZE);
  }
  //CASE 2: DON'T NEED TO SPLIT CAUSE IT'S EXACT SIZE
  //CASE 3: THE REMAINDER OF THE SIZE IS LESS THAN THE HEADER_SIZE SO NEED TO ALLOCATE ALL OF IT
  else{
    //remove header from the free list
    allocate_node->next->prev = allocate_node->prev;
    allocate_node->prev->next = allocate_node->next;
    allocate_node->next = NULL;
    allocate_node->prev = NULL;
    set_state(allocate_node, ALLOCATED);
    //return pointer to the start of the allocated block
    char * temp = (char *)(allocate_node) + ALLOC_HEADER_SIZE;
    answer = (void *)(temp);
    return answer;
  }
  return NULL;;
}
 
static inline void allocateos(header * hdr){
  header * left_fence = get_left_header(hdr);
  header * right_fence = get_left_header(left_fence);
  //CHECK IF CHUNK IS NEXT TO PREVIOUS CHUNKS
  if(lastFencePost == right_fence) {
    //COMBINE BOTH FENCEPOSTS
    header * first_fence = get_left_header(hdr);
    set_size_and_state(first_fence,get_size(hdr) + get_size(first_fence), UNALLOCATED);
    header * second_fence = get_left_header(first_fence);
    set_size_and_state(second_fence,get_size(first_fence) + get_size(second_fence), UNALLOCATED);
    header * block = get_left_header(second_fence);
    int block_starting_index = getIndex(get_size(block));
    //CHECK IS BLOCK NEXT TO BOTH FENCEPOSTS CAN BE COMBINED
    if(get_state(block) == UNALLOCATED){
      set_size_and_state(block,get_size(second_fence) + get_size(block), UNALLOCATED);
      int starting_index = getIndex(get_size(block));
      if(block_starting_index != starting_index){
        header * freelist = &freelistSentinels[starting_index];
        block->prev->next = block->next;
        block->next->prev  = block->prev;
        block->next = freelist->next;
        block->prev = freelist;
        freelist->next->prev = block;
        freelist->next = block; 
        lastFencePost = get_header_from_offset(block, get_size(block));
      }
    }  
    else{
      int starting_index = getIndex(get_size(second_fence));
      header * freelist = &freelistSentinels[starting_index];
      second_fence->next = freelist->next;
      second_fence->prev = freelist;
      freelist->next->prev = second_fence;
      freelist->next = second_fence;
      lastFencePost = get_header_from_offset(second_fence, get_size(second_fence));
    }
  }
  //WHEN CHUNK IS NOT NEXT TO PREVIOUS CHUNKS
  else{
    insert_os_chunk(left_fence);
    int starting_index = getIndex(get_size(hdr));
    header * freelist = &freelistSentinels[starting_index];
    hdr->next = freelist->next;
    hdr->prev = freelist;
    freelist->next->prev = hdr;
    freelist->next = hdr;
  }
}


/**
 * @brief Helper allocate an object given a raw request size from the user
 *
 * @param raw_size number of bytes the user needs
 *
 * @return A block satisfying the user's request
 */
static inline header * allocate_object(size_t raw_size) {
	//check to make sure malloc allocator is initilzed 
	if(isMallocInitialized == false){
		isMallocInitialized = true;
		init();
	}
	//if the requested size is 0 return null
	if(raw_size == 0)
		return NULL;
	//get requested size with the header and rounded up to the nearest 8-byte
	size_t requested_size =  (raw_size + ALLOC_HEADER_SIZE + 7) & ~7;
	if(requested_size < sizeof(header)){
		requested_size = sizeof(header);
	}
  void * answer;
	answer = allocate(requested_size);
	if(answer != NULL){
		pthread_mutex_unlock(&mutex);
		return answer;
	}
  //call sbrk
  while(1){
    header * hdr = allocate_chunk(ARENA_SIZE);
    allocateos(hdr);
    answer = allocate(requested_size);
    if(answer != NULL){
      pthread_mutex_unlock(&mutex);
      return answer;
    }
  }	
}



/**
 * @brief Helper to get the header from a pointer allocated with malloc
 *
 * @param p pointer to the data region of the block
 *
 * @return A pointer to the header of the block
 */
static inline header * ptr_to_header(void * p) {
  return (header *)((char *) p - ALLOC_HEADER_SIZE); //sizeof(header));
}

/**
 * @brief Helper to manage deallocation of a pointer returned by the user
 *
 * @param p The pointer returned to the user by a call to malloc
 */
static inline void deallocate_object(void * p) {
  if(p == NULL)
    return;
  //IF BLOCK IS NOT ALLOCATED, THEN RETURN ERROR
	header * block = ptr_to_header(p);
	if(get_state(block) != ALLOCATED){
		printf("Double Free Detected\n");
		assert(0);
		return;
	}
	header * right_block = get_right_header(block);
	header * left_block = get_left_header(block);
  set_state(block, UNALLOCATED);
	if((get_state(right_block) != UNALLOCATED && get_state(left_block) != UNALLOCATED) ){
    int index = getIndex(get_size(block));
		header * freelist = &freelistSentinels[index];
		block->next = freelist->next;
		block->prev = freelist;
		freelist->next->prev = block;
		freelist->next = block;
		return;
	}
  //CHECK IF RIGHT BLOCK IS UNALLOCATE, IF IT IS COMBINE THEM
	if(get_state(right_block) == UNALLOCATED){
		set_size(block,get_size(block) + get_size(right_block));
    int block_index = getIndex(get_size(block));
		int right_index = getIndex(get_size(right_block));
		if(right_index == block_index){
			block->next = right_block->next;
			block->prev = right_block->prev;
			right_block->next->prev = block;
			right_block->prev->next = block;
			right_block->next = NULL;
			right_block->prev = NULL;
		}
		else{
			right_block->next->prev = right_block->prev;
      right_block->prev->next = right_block->next;
      right_block->next = NULL;
			right_block->prev = NULL;
			header * freelist = &freelistSentinels[block_index];
			block->next = freelist->next;
			block->prev = freelist;
			freelist->next->prev = block;
			freelist->next = block;
		}
		header * fence = get_right_header(block);
    	fence->left_size = get_size(block);
	}
  //CHECK IF LEFT BLOCK IS UNALLOCATE, IF IT IS COMBINE THEM
	if(get_state(left_block) == UNALLOCATED){
    int left_original_index = getIndex(get_size(left_block));
		set_size(left_block, get_size(left_block) + get_size(block));
    int block_index = getIndex(get_size(block));
    int left_index = getIndex(get_size(left_block));
		if(left_original_index == left_index){
			if(block->next != NULL && block->prev != NULL){
    	    	block->next->prev = block->prev;
       			block->prev->next = block->next;
      		}
			block->next = NULL;
			block->prev = NULL;
		}
		else{
      left_block->next->prev = left_block->prev;
      left_block->prev->next = left_block->next;
      header * freelist = &freelistSentinels[left_index];
			left_block->next = freelist->next;
			left_block->prev = freelist;
			freelist->next->prev = left_block;
			freelist->next = left_block;		
			if(block->next != NULL && block->prev != NULL){
        block->next->prev = block->prev;
       	block->prev->next = block->next;
      }
			block->next = NULL;
			block->prev = NULL;
		}
    	header * fence = get_right_header(left_block);
    	fence->left_size = get_size(left_block);
	}
  return;
}

/**
 * @brief Helper to detect cycles in the free list
 * https://en.wikipedia.org/wiki/Cycle_detection#Floyd's_Tortoise_and_Hare
 *
 * @return One of the nodes in the cycle or NULL if no cycle is present
 */
static inline header * detect_cycles() {
  for (int i = 0; i < N_LISTS; i++) {
    header * freelist = &freelistSentinels[i];
    for (header * slow = freelist->next, * fast = freelist->next->next; 
         fast != freelist; 
         slow = slow->next, fast = fast->next->next) {
      if (slow == fast) {
        return slow;
      }
    }
  }
  return NULL;
}

/**
 * @brief Helper to verify that there are no unlinked previous or next pointers
 *        in the free list
 *
 * @return A node whose previous and next pointers are incorrect or NULL if no
 *         such node exists
 */
static inline header * verify_pointers() {
  for (int i = 0; i < N_LISTS; i++) {
    header * freelist = &freelistSentinels[i];
    for (header * cur = freelist->next; cur != freelist; cur = cur->next) {
      if (cur->next->prev != cur || cur->prev->next != cur) {
        return cur;
      }
    }
  }
  return NULL;
}

/**
 * @brief Verify the structure of the free list is correct by checkin for 
 *        cycles and misdirected pointers
 *
 * @return true if the list is valid
 */
static inline bool verify_freelist() {
  header * cycle = detect_cycles();
  if (cycle != NULL) {
    fprintf(stderr, "Cycle Detected\n");
    print_sublist(print_object, cycle->next, cycle);
    return false;
  }

  header * invalid = verify_pointers();
  if (invalid != NULL) {
    fprintf(stderr, "Invalid pointers\n");
    print_object(invalid);
    return false;
  }

  return true;
}

/**
 * @brief Helper to verify that the sizes in a chunk from the OS are correct
 *        and that allocated node's canary values are correct
 *
 * @param chunk AREA_SIZE chunk allocated from the OS
 *
 * @return a pointer to an invalid header or NULL if all header's are valid
 */
static inline header * verify_chunk(header * chunk) {
	if (get_state(chunk) != FENCEPOST) {
		fprintf(stderr, "Invalid fencepost\n");
		print_object(chunk);
		return chunk;
	}
	
	for (; get_state(chunk) != FENCEPOST; chunk = get_right_header(chunk)) {
		if (get_size(chunk)  != get_right_header(chunk)->left_size) {
			fprintf(stderr, "Invalid sizes\n");
			print_object(chunk);
			return chunk;
		}
	}
	
	return NULL;
}

/**
 * @brief For each chunk allocated by the OS verify that the boundary tags
 *        are consistent
 *
 * @return true if the boundary tags are valid
 */
static inline bool verify_tags() {
  for (size_t i = 0; i < numOsChunks; i++) {
    header * invalid = verify_chunk(osChunkList[i]);
    if (invalid != NULL) {
      return invalid;
    }
  }

  return NULL;
}

/**
 * @brief Initialize mutex lock and prepare an initial chunk of memory for allocation
 */
static void init() {
  // Initialize mutex for thread safety
  pthread_mutex_init(&mutex, NULL);

#ifdef DEBUG
  // Manually set printf buffer so it won't call malloc when debugging the allocator
  setvbuf(stdout, NULL, _IONBF, 0);
#endif // DEBUG

  // Allocate the first chunk from the OS
  header * block = allocate_chunk(ARENA_SIZE);

  header * prevFencePost = get_header_from_offset(block, -ALLOC_HEADER_SIZE);
  insert_os_chunk(prevFencePost);

  lastFencePost = get_header_from_offset(block, get_size(block));

  // Set the base pointer to the beginning of the first fencepost in the first
  // chunk from the OS
  base = ((char *) block) - ALLOC_HEADER_SIZE; //sizeof(header);

  // Initialize freelist sentinels
  for (int i = 0; i < N_LISTS; i++) {
    header * freelist = &freelistSentinels[i];
    freelist->next = freelist;
    freelist->prev = freelist;
  }

  // Insert first chunk into the free list
  header * freelist = &freelistSentinels[N_LISTS - 1];
  freelist->next = block;
  freelist->prev = block;
  block->next = freelist;
  block->prev = freelist;

	isMallocInitialized = true;
}

/* 
 * External interface
 */
void * my_malloc(size_t size) {
  pthread_mutex_lock(&mutex);
  header * hdr = allocate_object(size); 
  pthread_mutex_unlock(&mutex);
  return hdr;
}

void * my_calloc(size_t nmemb, size_t size) {
  return memset(my_malloc(size * nmemb), 0, size * nmemb);
}

void * my_realloc(void * ptr, size_t size) {
  void * mem = my_malloc(size);
  memcpy(mem, ptr, size);
  my_free(ptr);
  return mem; 
}

void my_free(void * p) {
  pthread_mutex_lock(&mutex);
  deallocate_object(p);
  pthread_mutex_unlock(&mutex);
}

bool verify() {
  return verify_freelist() && verify_tags();
}
