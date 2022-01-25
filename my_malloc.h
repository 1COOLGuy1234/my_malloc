#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <limits.h>


/****************************************************************************************************
Data structures that maintain free blocks: Doubly Linked List

malloc:
1. Search for allocable free block that is big enough in linked list. If do have, use the block. 
2. If do not have, call `sbrk()` to extend heap

free:
1. add Node into linked list
2. If node is adjacent to the next node or prev node in memory, merge them
/****************************************************************************************************

/*
Node is used to store the meta data for one clock
*/
struct Node {
    struct Node* prev;
    struct Node* next;
    size_t data_size;
};
typedef struct Node Node;

#define NODE_SIZE sizeof(Node)

#define SPLIT_BLOCK_THRESHOLD 32

Node* head = NULL;
Node* tail = NULL;


/****************************************malloc**************************************/
//First Fit malloc
void * ff_malloc(size_t size);

//Best Fit malloc
void * bf_malloc(size_t size);

Node* apply_extra_space_in_heap(size_t data_size);

void * my_sbrk(size_t sz);

void split_block(Node* ptr, size_t need_size);

/*********************************************************************************/

/**************************************free****************************************/
//First Fit free
void ff_free(void * ptr);

//Best Fit free
void bf_free(void * ptr);

void my_free(Node* ptr);

void insert_free_block(Node* ptr);

void merge_adjacent_block(Node* ptr);

void delete_node(Node* ptr);

void merge_with_low_block(Node* ptr);

void merge_with_high_block(Node* ptr);

void merge_with_both_nodes(Node* ptr);

void split_block(Node* ptr, size_t need_size);

/*********************************************************************************/


/****************************************util**************************/
void printError(const char * msg);
unsigned long get_total_free_size();
unsigned long get_largest_free_data_segment_size();
/****************************************util**************************/

