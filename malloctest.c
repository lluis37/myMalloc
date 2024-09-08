#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mymalloc.h"

int main(int argc, char **argv) {
    int test = atoi(argv[1]);

    switch(test) {
        case 1:
            // Check for error message in case ptr is not on heap
            int n = 0;
            free(&n);
            break;

        case 2:
            // Check for error message in case ptr points to middle of an object
            int *p = malloc(24);
            free(p + 1);
            break;

        case 3:
            // Check for error message in case ptr points to a header
            p = malloc(24);
            free((char *) p - 24);
            break;

        case 4:
            // Check for error message in case memory is already free
            // This should also check to make sure that free actually free's
            // memory
            p = malloc(24);
            int *q = p;
            free(p);
            free(q);
            break;

        case 5:
            // Make sure that free() coalesces memory
            p = malloc(1000);
            q = malloc(1000);
            int *r = malloc(1000);
            int *s = malloc(1000);

            // free(q), then free(s), free(r)
            // --> 3000 + 24*2 bytes should be available to be allocated
            free(q);
            free(s);
            free(r);

            q = malloc(3000);
            break;
            
        case 6:
            // Make sure free() deallocates objects

            // Allocate all of memory then deallocate it
            p = malloc(4072);
            free(p);

            p = malloc(500);
            break;

        case 7:
            // Check for error message in case of trying to malloc() too many
            // bytes
            p = malloc(5000);
            break;

        case 8:
            // Check for error message in case of trying to malloc()
            // when all memory is allocated
            p = malloc(4072);
            q = malloc(100);
            break;

        case 9:
            // Make sure that memory from malloc() has 8 byte alignment
            p = (int *) malloc(1);
            if ( ((int) p) % 8) {
                printf("Address returned by malloc() is not aligned\n");
            }
            break;
        
    }
}