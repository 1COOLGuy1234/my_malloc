# Report for Homework 1 – ECE650 

## Section 1: Requirements and summary of development. 

### Requirement

The objective is to develop the malloc library. We need to develop two versions of malloc. Specifically, malloc with First Fit and Best Fit strategy. Then we should develop the corresponding `free()` function. The key to realize malloc is to utilize `sbrk()` system call to control heap. Moreover, we need to choose suitable data structure to maintain the memory block. Some function are also be required like merge adjacent free memory block when call `free()`. 

### Development environment

I developed this project in OS Linux Ubuntu 20.04, and I used VSCode as IDE and used gdb to debug my code.

### What this report includes?

In this report, I will summarize the main goals of this project. I will also declare my design thought and how I implemented it. Finally, I will study the performance of my malloc, and make some analysis.

## Section 2: Design, implementation and test 

### Data structure

The data structure I choose to manage the memory block is doubly linked list. I use one doubly linked list to manage all free memory blocks. That is to say, each time I free a memory block, I will add the block into the linked list. The node will record the previous node, the next node, and the data size of this block. Actually, each memory block is divided into two sections: the meta section and data section. In my project, the meta section is consist of two pointers and a size_t variable, so it takes 24 bytes. In other words, each block is started with 24 bytes meta section, then the data section is followed.

### Two strategies: First Fit & Best Fit

For FF malloc, the parameter is *data* *size.* The function will go through the linked list to find the first block that can accommodate *data* *size*. Once you find the suitable memory block, delete it from linked list because it will be reused. If the function cannot find the suitable block, it will call sbrk() to extend the heap to *data* *size + meta size* byte. Then the space will be allocated.

For BF malloc, the only difference is that it will go through the whole linked list to find one block that not only can accommodate the block but also the smallest, which can contribute the memory saving. Apparently, it will cost more time than FF strategy. 

### The split strategy

I also developed the function of split block. I note that  once the chosen memory block is much larger than the requested memory,  it will waste a lot of memory. I define the variable `SPLIT_THRESHOLD`, once the block data size is `NODE_SIZE + SPLIT_THRESHOLD` bytes larger than requested memory, split the block into 2 blocks, the first will be malloced, and the second will still in linked list and is free. 

### How to test my code

After I finished one specific function, like merge adjacent free block, or delete node in linked list, I would write an easy main function to test my code. I would draw memory block in drafts and used gdb to trace the pointer address. Then I would see if the function goes as I expected. If not, I would step in the function to see what went wrong.

## Section 3: Performance Results & Analysis

The performance sheet（in default iterate times and numbers of items）

|       benchmarks        |      | First Fit            | Best Fit             |
| :---------------------: | ---- | -------------------- | -------------------- |
|    equal_size_allocs    |      | time: 11.89s         | time: 11.56s         |
|                         |      | Fragmentation: 0.999 | Fragmentation: 0.999 |
| small_range_rand_allocs |      | time: 5.16s          | time: 1.58s          |
|                         |      | Fragmentation: 0.545 | Fragmentation: 0.854 |
| large_range_rand_allocs |      | time: 36.66s         | time: 44.78s         |
|                         |      | Fragmentation: 0.825 | Fragmentation: 0.978 |

In this situation, my `SPLIT_THRESHOLD` is 32 bytes.

1. In equal_size_allocs benchmark, the time and fragmentation are all very similar. Actually, in original version my bf_malloc is very  slow. Then I applied the trick that once it find the block that hold the same data size it need, choose this block and stop the search process. In this benchmark, each malloc size is the same, so the bf_malloc almost does the same thing as ff_malloc.
2. In small_range_rand_allocs benchmark, best fit is quicker than first fit! I think it is because in small_range allocs, 32 bytes split threshold is a good choice. The best fit strategy will make the block split less, generating less small free blocks in linked list, then reduced the searching time.
3. In large_range_rand_allocs benchmark, FF outperforms BF. It is the normal case, because ff search less blocks than bf. However, if I switch the `SPLIT_THRESHOLD` to a specific number that meaningful to large_rand, the BF can also be quicker than FF.

