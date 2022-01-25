#include <stdio.h>
#include <stdlib.h>

int main() {
    int * t = malloc(100);
    free(t);
    free(t);
    return 0;
}