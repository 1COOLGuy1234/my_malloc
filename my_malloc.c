#include "my_malloc.h"


/********************util***************************/
void printError(const char * msg) {
    fprintf(stderr, "Wrong: %s\n", msg);
    exit(EXIT_FAILURE);
}

unsigned long get_largest_free_data_segment_size() {
    Node* p = head;
    size_t max_data_size = 0;
    while (p != NULL) {
        if (p->data_size > max_data_size) {
            max_data_size = p->data_size;
        }
        p = p->next;
    }
    return max_data_size;
}

unsigned long get_total_free_size() {
    Node* p = head;
    size_t sum_data_size = 0;
    while (p != NULL) {
        sum_data_size += p->data_size;
        p = p->next;
    }
    return sum_data_size;
}
/********************util***************************/



/**
 * @brief encapsulate `sbrk()`, auto check if sbrk failed
 * @param sz the data size I want to apply for
 * @return If failed, return NULL; Else, return new heap top address
*/
void * my_sbrk(size_t sz) {
    if (sbrk(NODE_SIZE + sz) == (void *)(-1)) {   // if sbrk failed
        return NULL;
    }   
    return sbrk(0);
}

/**
 * @brief if we cannot find enough space in LinkedList, call this function to extend extra space in heap
 * @param data_size the data size(bytes) that I malloced, the function will apply data_size + NODE_SIZE bytes
 * @return If failed, return NULL; If succeeded, return the start address of the applied space
*/
Node* apply_extra_space_in_heap(size_t data_size) {
    Node* start_address = sbrk(0); // get the current heap top
    if (!my_sbrk(data_size)) {
        return NULL;
    }
    // now we get an space (NODE_SIZE + data_size)
    // DATA section start at `meta_info + NODE_SIZE`
    start_address->next = NULL;
    start_address->prev = NULL;
    start_address->data_size = data_size;
    return start_address;
}


/**
 * @brief malloc with First Fit strategy
 * @param size the space size to apply
 * @return the start address of the applied memory; If malloc failed, return NULL
*/
void * ff_malloc(size_t size) {
    // search suitable block in Free LinkedList
    Node* p = head;
    while (p != NULL) {  // FIRST FIT
        if (p->data_size >= size) {  // if p is suitable
            // Case1 : can split a block which holds more than 16 bytes data, split it
            if (p->data_size - size >= NODE_SIZE + SPLIT_BLOCK_THRESHOLD) {
                // split the block
                split_block(p, size);
                return (void*)p + NODE_SIZE; 
            }
            // Case2 : If split, the remaining block is too small, DO NOT split it.
            else {
                delete_node(p);
                return (void*)p + NODE_SIZE;    // NOTE: malloc should return the start address of data
            }   
        }
        p = p->next;
    }
    // No suitable block in Free LinkedList, extend heap
    void* new_space = apply_extra_space_in_heap(size);
    if (!new_space) {   // extend failed
        return NULL;
    }
    return new_space + NODE_SIZE;
}

/**
 * @brief encapsulate `my_free(Node* ptr)`
 * @param ptr the start address of the block needed to be freed
 * @return void
 */
void ff_free(void* ptr) {
    my_free(ptr - NODE_SIZE);
}

/**
 * @brief encapsulate `my_free(Node* ptr)`
 * @param ptr the start address of the block needed to be freed
 * @return void
 */
void bf_free(void * ptr) {
    my_free(ptr - NODE_SIZE);
}


/**
 * @brief free block. Specifically, add the block to LinkedList, if there are adjacent block, merge them.
 * @param ptr the start address of the block needed to be freed
 * @return void
 */
void my_free(Node* ptr) {
    // If head == NULL(LinkedList is empty), head = ptr tail = ptr
    if (head == NULL) {
        assert(tail == NULL);
        head = ptr;
        tail = ptr;
        ptr->next = NULL;
        ptr->prev = NULL;
        return;
    }
    // add the block into LinkedList
    insert_free_block(ptr);
}


/**
 * @brief insert node into linked list when free memory block
 * @param ptr the node pointer which point to the block we will free
 * @return void
 */ 
void insert_free_block(Node* ptr) {
    if (ptr < head) {
        Node* old_head = head;
        head = ptr;
        head->next = old_head;
        head->prev = NULL;
        old_head->prev = head;
        merge_adjacent_block(ptr);
        return;
    }
    else if (ptr > tail) {
        Node* old_tail = tail;
        tail = ptr;
        tail->next = NULL;
        tail->prev = old_tail;
        old_tail->next = tail;
        merge_adjacent_block(ptr);
        return;
    }
    Node* p = head;
    while (p->next != NULL) {
        if (p == ptr) {  // double free
            printError("free(): double free detected");
        }
        if (ptr > p && ptr < p->next) {
            ptr->next = p->next;
            ptr->prev = p;
            p->next = ptr;
            ptr->next->prev = ptr;
            merge_adjacent_block(ptr);
            return;
        }
        p = p->next;
    }
    // If everthing goes correctly, the function has returned and the following code will not be implemented
    // The remaining possibility: ptr == tail
    if (ptr == tail) {
        printError("free(): double free detected");
    }
    printError("The function insert_free_block has bug!");
}


/**
 * @brief merge adjancent free memory block
 * @param ptr point to the block that we just freed
 * @return void
 */ 
void merge_adjacent_block(Node* ptr) {
    Node* low_node = ptr->prev;
    Node* high_node = ptr->next;
    // check the lower block
    int is_close_to_low_node = (low_node != NULL && ((void*)low_node + NODE_SIZE + low_node->data_size == ptr)) ? 1 : 0;
    // check the higher block
    int is_close_to_high_node = (high_node != NULL && ((void*)ptr + NODE_SIZE + ptr->data_size == high_node)) ? 1 : 0;
    // merge
    if (is_close_to_low_node && is_close_to_high_node) {  // close to both
        merge_with_both_nodes(ptr);
        return;
    }
    else if (is_close_to_low_node && !is_close_to_high_node) {
        merge_with_low_block(ptr);
        return;
    }
    else if (!is_close_to_low_node && is_close_to_high_node) {
        merge_with_high_block(ptr);
        return;
    }
    else {
        return;
    }
}

/**
 * @brief maintain linked list. delete the block from list that has been reused
 * @param ptr point to the block that will be reused
 * @return void
 */
void delete_node(Node* ptr) {
    if (ptr == head && head == tail) {   // Just one node
        head->prev = NULL;
        head->next = NULL;
        head = NULL;
        tail = NULL;
        return;
    }
    else if (ptr == head) {  // if delete head node
        head = head->next;
        head->prev = NULL;
        return; 
    }
    else if (ptr == tail) {
        tail = tail->prev;
        tail->next = NULL;
        return;
    }
    else {
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
        return;
    }
}

/**
 * @brief helper function to realize the function `merge_adjacent_block(Node* ptr)`
 * @param ptr the same with `merge_adjacent_block(Node* ptr)`
 * @return void
 */ 
void merge_with_low_block(Node* ptr) {
    Node* new_node = ptr->prev;
    new_node->data_size = ptr->data_size + ptr->prev->data_size + NODE_SIZE;
    delete_node(ptr);
}

/**
 * @brief helper function to realize the function `merge_adjacent_block(Node* ptr)`
 * @param ptr the same with `merge_adjacent_block(Node* ptr)`
 * @return void
 */
void merge_with_high_block(Node* ptr) {
    Node* new_node = ptr;
    new_node->data_size = ptr->data_size + ptr->next->data_size + NODE_SIZE;
    delete_node(ptr->next);
}

/**
 * @brief helper function to realize the function `merge_adjacent_block(Node* ptr)`
 * @param ptr the same with `merge_adjacent_block(Node* ptr)`
 * @return void
 */
void merge_with_both_nodes(Node* ptr) {
    Node* new_node = ptr->prev;
    new_node->data_size = ptr->prev->data_size + ptr->data_size + ptr->next->data_size + NODE_SIZE + NODE_SIZE;
    delete_node(new_node->next);
    delete_node(new_node->next);
}

/**
 * @brief split the free block @arg `ptr` point to into 2 blocks. The first block will be malloced, and the data size is @arg `need_size`. The second block is still free, and the data_size is original data_size - need_size - NODE_SIZE
 * @param ptr the block is to be splitted
 * @param need_size the first block's data size
 * @return void
*/
void split_block(Node* ptr, size_t need_size) {
    Node* new_meta = (void*)ptr + NODE_SIZE + need_size;
    new_meta->data_size = ptr->data_size - need_size - NODE_SIZE;
    ptr->data_size = need_size;
    if (ptr == head && head == tail) {
        head = new_meta;
        tail = new_meta;
        new_meta->prev = NULL;
        new_meta->next = NULL;
        return;
    }
    else if (ptr == head) {
        Node* old_head = ptr;
        head = new_meta;
        head->next = old_head->next;
        head->next->prev = head;
        head->prev = NULL;
        return;
    }
    else if (ptr == tail) {
        Node* old_tail = ptr;
        tail = new_meta;
        tail->next = NULL;
        tail->prev = old_tail->prev;
        tail->prev->next = tail;
        return;
    }
    else {
        Node* left = ptr->prev;
        Node* right = ptr->next;
        left->next = new_meta;
        new_meta->prev = left;
        right->prev = new_meta;
        new_meta->next = right;
        return;
    }
}


/**
 * @brief malloc with First Fit strategy
 * @param size the space size to apply
 * @return the start address of the applied memory; If malloc failed, return NULL
*/
void * bf_malloc(size_t size) {
    // search suitable block in Free LinkedList
    if (head != NULL) {
        Node* p = head;
        // size_t min_data_size = head->data_size;
        // Node* matched_node = head;
        size_t min_data_size = LONG_MAX;
        Node* matched_node = NULL;
        while (p != NULL) {  // search the BEST FIT node
            if (p->data_size == size) {
                matched_node = p;
                min_data_size = p->data_size;
                break;
            }
            else if (p->data_size > size && p->data_size < min_data_size) {
                min_data_size = p->data_size;
                matched_node = p;
            }
            p = p->next;
        }
        // while loop finished
        // if (matched_node != head || (matched_node == head && matched_node->data_size >= size) ) { // if we find the BEST FIT node
        if (matched_node) { // if we find the BEST FIT node
            // CASE1 : split
            if (matched_node->data_size - size >= NODE_SIZE + SPLIT_BLOCK_THRESHOLD) {
                split_block(matched_node, size);
                return (void*)matched_node + NODE_SIZE;
            }
            // CASE2 : DO NOT split
            else {
                delete_node(matched_node);
                return (void*)matched_node + NODE_SIZE;
            }
        }
        // If we cannot find the matched_node, go to next section
    }
    // No suitable block in Free LinkedList, extend heap
    void* new_space = apply_extra_space_in_heap(size);
    if (!new_space) {   // extend failed
        return NULL;
    }
    return new_space + NODE_SIZE;
}

