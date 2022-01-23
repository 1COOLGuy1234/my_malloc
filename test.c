#include <stdio.h>

int main() {
    size_t N = 30;
    int * array = create_array(N);

    for (int i = 0; i < 10; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    ff_free(array);
    printf("The head is %p\n", head);
    printf("The tail is %p\n", tail);
    printf("The head size is %lu\n", head->data_size);
    printf("The tail size is %lu\n", tail->data_size);


    int * t1 = bf_malloc(5 * sizeof(*t1));
    printf("The head is %p\n", head);
    printf("The tail is %p\n", tail);
    printf("The head size is %lu\n", head->data_size);
    printf("The tail size is %lu\n", tail->data_size);
    // printf("new_array[4] == %d\n", new_array[4]);

    int * t2 = bf_malloc(sizeof(*t2));
    printf("The head is %p\n", head);
    printf("The tail is %p\n", tail);
    printf("The head size is %lu\n", head->data_size);
    printf("The tail size is %lu\n", tail->data_size);

    int * t3 = bf_malloc(48);
    printf("The head is %p\n", head);
    printf("The tail is %p\n", tail);
    // printf("The head size is %lu\n", head->data_size);
    // printf("The tail size is %lu\n", tail->data_size);

    ff_free(t1);
    ff_free(t3);
    ff_free(t2);
    printf("The head is %p\n", head);
    printf("The tail is %p\n", tail);
    printf("The head size is %lu\n", head->data_size);
    printf("The tail size is %lu\n", tail->data_size);


    return EXIT_SUCCESS;
}