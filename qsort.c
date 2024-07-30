#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

// Swap function to exchange two elements
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Simplified partition function
int partition(int *arr, int low, int high) {
    int pivot = arr[high]; // use the last element as pivot
    int i = low - 1; // index of the smaller element

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// Simplified quicksort function
void quicksort(int *arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}
// Function to check if an array is sorted
bool is_sorted(int *arr, int size) {
    for (int i = 1; i < size; ++i) {
        if (arr[i - 1] > arr[i]) {
            return false;
        }
    }
    return true;
}

int main() {
    const int array_size = 1000000;
    const int iterations = 10;
    int **arrays = malloc(iterations * sizeof(int *));

    // Seed for random number generation
    srand((unsigned int)time(NULL));

    // Generate large random arrays of integers
    for (int i = 0; i < iterations; ++i) {
        arrays[i] = malloc(array_size * sizeof(int));
        for (int j = 0; j < array_size; ++j) {
            arrays[i][j] = rand() % 1000000;
        }
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Benchmark optimized quicksort
    for (int i = 0; i < iterations; ++i) {
        quicksort(arrays[i], 0, array_size - 1);
    }

    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("naive quicksort execution time: %.6f seconds for 10 iterations\n", elapsed);
    // Verify that each array is sorted
    for (int i = 0; i < iterations; ++i) {
        if (!is_sorted(arrays[i], array_size)) {
            printf("Array %d is not sorted!\n", i);
        } else {
            printf("Array %d is sorted.\n", i);
        }
    }
    // Free allocated memory
    for (int i = 0; i < iterations; ++i) {
        free(arrays[i]);
    }
    free(arrays);

    return 0;
}
