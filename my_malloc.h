#include <stdlib.h>

//First Fit malloc
void * ff_malloc(size_t size);
//Best Fit malloc
void * bf_malloc(size_t size);

//First Fit free
void ff_free(void * ptr);
//Best Fit free
void bf_free(void * ptr);
