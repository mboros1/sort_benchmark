#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define RANGE 100

int getMax(int arr[], int n) {
    int max = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > max)
            max = arr[i];
    return max;
}

void countingSort(int arr[], int n, int exp) {
    int output[n];
    int count[RANGE + 1] = {0};

    for (int i = 0; i < n; i++)
        count[(arr[i] / exp) % 10]++;

    for (int i = 1; i <= RANGE; i++)
        count[i] += count[i - 1];

    for (int i = n - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
    }

    for (int i = 0; i < n; i++)
        arr[i] = output[i];
}

void radixSort(int arr[], int n) {
    int m = getMax(arr, n);

    for (int exp = 1; m / exp > 0; exp *= 10)
        countingSort(arr, n, exp);
}

void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

bool isSorted(int *arr, int size) {
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

    for (int i = 0; i < iterations; ++i) {
        arrays[i] = malloc(array_size * sizeof(int));
    }

    double elapsed_time = 0;
    srand((unsigned int)time(NULL));

    for (int i = 0; i < 100; ++i) {
        for (int i = 0; i < iterations; ++i) {
            for (int j = 0; j < array_size; ++j) {
                arrays[i][j] = (rand() % 100) + 1;
            }
        }

        struct timeval start, end;
        gettimeofday(&start, NULL);

        for (int i = 0; i < iterations; ++i) {
            radixSort(arrays[i], array_size);
        }

        gettimeofday(&end, NULL);

        elapsed_time += (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    }

    printf("radix_sort execution time: %.6f seconds for 10 iterations\n", elapsed_time / 100);
    for (int i = 0; i < iterations; ++i) {
        if (!isSorted(arrays[i], array_size)) {
            printf("Array %d is not sorted!\n", i);
        } else {
            printf("Array %d is sorted.\n", i);
        }
    }
    for (int i = 0; i < iterations; ++i) {
        free(arrays[i]);
    }
    free(arrays);

    return 0;
}


