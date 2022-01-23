#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <limits.h>


/*
管理可用内存区域的数据结构：双向链表  用来存储空闲内存

malloc:
1. 在链表上寻找是否有满足需求的空闲内存，如果有，使用该内存
2. 如果没有，call sbrk() 申请内存

free:
1. 将内存加入LinkedList
2. 如果前后相连，合并Node
*/

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

#define SPLIT_BLOCK_THRESHOLD 16

Node* head = NULL;
Node* tail = NULL;

//First Fit malloc
void * ff_malloc(size_t size);

/**
 * @brief Use First Fit strategy to search for the free block in LinkedList
 * @param need_size the data size we need
 * @return if successfully find proper block in LinkedList, return the pointer pointed to that block
 *         else, return NULL (which means we need to call sbrk()) 
*/
Node * ff_search_free_block(size_t need_size);

Node* apply_extra_space_in_heap(size_t data_size);

void * my_sbrk(size_t sz);

//Best Fit malloc
void * bf_malloc(size_t size);

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




/****************************************util**************************/
void printError(const char * msg);
/****************************************util**************************/

