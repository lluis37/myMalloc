#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "mymalloc.h" 

#define NUM_OPERATIONS 120
#define NUM_ITERATIONS 50

// Helper function to get current time in microseconds
long long current_time() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec * 1000000 + time.tv_usec;
}

void test_single_allocation_and_free() {
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        void *ptr = mymalloc(1, __FILE__, __LINE__);
        myfree(ptr, __FILE__, __LINE__);
    }
}

void test_multiple_allocations_and_frees() {
    void *pointers[NUM_OPERATIONS];
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        pointers[i] = mymalloc(1, __FILE__, __LINE__);
    }
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        myfree(pointers[i], __FILE__, __LINE__);
    }
}

void test_random_choice_allocation_and_free() {
    void *pointers[NUM_OPERATIONS] = {NULL};
    int count = 0, index = 0;
    while (count < NUM_OPERATIONS) {
        if ((rand() % 2 == 0 || index == 0) && count < NUM_OPERATIONS) {
            pointers[index++] = mymalloc(1, __FILE__, __LINE__);
            count++;
        } else if (index > 0) {
            myfree(pointers[--index], __FILE__, __LINE__);
            pointers[index] = NULL;
        }
    }
    while (index > 0) {
        myfree(pointers[--index], __FILE__, __LINE__);
    }
}


void test_increasing_and_decreasing_allocations() {
    //printf("\n--- Test 4: Increasing and Decreasing Allocations ---\n");

    int increase_step = 8; // Define the step size for increasing allocations
    int max_allocations = 10; // Limit the number of allocations to prevent overflow
    void* pointers[max_allocations]; // Store pointers to allocated memory
    int i;
    /*
    // Increasing allocations
    for (i = 0; i < max_allocations; i++) {
        int size = (i + 1) * increase_step;
        pointers[i] = malloc(size); // Attempt to allocate memory
        if (pointers[i] == NULL) {
            printf("Allocation failed for size %d\n", size);
            break; // Exit the loop if allocation fails
        } else {
            printf("Allocated size %d at %p\n", size, pointers[i]);
        }
        //print_memory_list(); // Debug: Print the state of memory after each allocation
    }

    int successful_allocations = i; // Keep track of how many allocations succeeded

    // Decreasing deallocations
    for (i = successful_allocations - 1; i >= 0; i--) {
        free(pointers[i]); // Attempt to free allocated memory
        printf("Freed memory at %p\n", pointers[i]);
        //print_memory_list(); // Debug: Print the state of memory after each deallocation
    }
    */
}


void test_random_allocations_and_frees() {
    //printf("\n--- Test 5: Random Allocation Sizes and Order ---\n");
    const int MAX_OPERATIONS = 120; // Total number of operations
    void *ptrs[MAX_OPERATIONS];
    int allocCount = 0;

    for (int i = 0; i < MAX_OPERATIONS; i++) {
        if (rand() % 2 == 0 || allocCount == 0) { // Allocate
            size_t size = (rand() % 16) + 1; // Random size between 1 and 16 bytes
            void *ptr = mymalloc(size, __FILE__, __LINE__);
            if (ptr != NULL) {
                ptrs[allocCount++] = ptr;
            }
        } else { // Free
            int indexToFree = rand() % allocCount;
            myfree(ptrs[indexToFree], __FILE__, __LINE__);
            ptrs[indexToFree] = ptrs[--allocCount]; // Move the last valid pointer to the freed spot
        }
    }

    // Free any remaining allocations
    for (int i = 0; i < allocCount; i++) {
        myfree(ptrs[i], __FILE__, __LINE__);
    }

    //print_memory_list(); // Optional: Print final memory state
}


void run_test(void (*test_func)(), const char *test_name) {
    long long total_time = 0;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        long long start_time = current_time();
        test_func();
        long long end_time = current_time();
        total_time += (end_time - start_time);
    }
    printf("%s: Average Time = %lld microseconds\n", test_name, total_time / NUM_ITERATIONS);
}

int main() {
    struct timeval start, end;
    long seconds, useconds;
    double totalTime[5] = {0};

    

    for (int i = 0; i < 50; i++) {
        gettimeofday(&start, NULL);
        test_single_allocation_and_free();
        gettimeofday(&end, NULL);
        seconds = end.tv_sec - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        totalTime[0] += ((seconds * 1000000) + useconds);

        gettimeofday(&start, NULL);
        test_multiple_allocations_and_frees();
        gettimeofday(&end, NULL);
        seconds = end.tv_sec - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        totalTime[1] += ((seconds * 1000000) + useconds);

        gettimeofday(&start, NULL);
        test_random_choice_allocation_and_free();
        gettimeofday(&end, NULL);
        seconds = end.tv_sec - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        totalTime[2] += ((seconds * 1000000) + useconds);

        // Test 4
        gettimeofday(&start, NULL);
        test_increasing_and_decreasing_allocations();
        gettimeofday(&end, NULL);
        seconds = end.tv_sec - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        totalTime[3] += ((seconds * 1000000) + useconds);

        // Test 5
        gettimeofday(&start, NULL);
        test_random_allocations_and_frees();
        gettimeofday(&end, NULL);
        seconds = end.tv_sec - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        totalTime[4] += ((seconds * 1000000) + useconds);
    }
    

    printf("Test 1: Average Time = %f microseconds\n", totalTime[0] / 50);
    printf("Test 2: Average Time = %f microseconds\n", totalTime[1] / 50);
    printf("Test 3: Average Time = %f microseconds\n", totalTime[2] / 50);
    printf("Test 4: Average Time = %f microseconds\n", totalTime[3] / 50);
    printf("Test 5: Average Time = %f microseconds\n", totalTime[4] / 50);
    

    return 0;
}




