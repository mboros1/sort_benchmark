
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <stddef.h>  // For size_t

#define CACHELINE_SIZE 64
#define BLOCK_SIZE 64

// Swap function to exchange two elements
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Align pointer to cacheline
void* align_cacheline(void* p) {
    uintptr_t ip = (uintptr_t)p;
    ip = (ip + CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1);
    return (void*)ip;
}

// Swap elements according to offsets
void swap_offsets(int* first, int* last, unsigned char* offsets_l, unsigned char* offsets_r, size_t num, bool use_swaps) {
    if (use_swaps) {
        for (size_t i = 0; i < num; ++i) {
            swap(first + offsets_l[i], last - offsets_r[i]);
        }
    } else if (num > 0) {
        int* l = first + offsets_l[0];
        int* r = last - offsets_r[0];
        int tmp = *l;
        *l = *r;
        for (size_t i = 1; i < num; ++i) {
            l = first + offsets_l[i];
            *r = *l;
            r = last - offsets_r[i];
            *l = *r;
        }
        *r = tmp;
    }
}

// Simplified partition_left function
int partition_left(int *arr, int low, int high) {
    int pivot = arr[low];
    int *first = arr + low;
    int *last = arr + high + 1;

    // Find the first element greater than the pivot
    while (--last > first && *last >= pivot);

    // Find the first element less than or equal to the pivot
    while (first < last && *first <= pivot) first++;

    while (first < last) {
        swap(first, last);
        while (--last > first && *last >= pivot);
        while (first < last && *first <= pivot) first++;
    }

    swap(arr + low, last);

    return last - arr;
}

// Function to perform the branchless partition
int partition_right_branchless(int *arr, int low, int high) {
    int pivot = arr[low];
    int *first = arr + low;
    int *last = arr + high + 1;

    // Find the first element greater than or equal to the pivot
    while (++first <= arr + high && *first < pivot);

    // Find the first element strictly smaller than the pivot
    if (first - arr == low + 1) {
        while (first < last && !(*(last - 1) < pivot)) last--;
    } else {
        while (!(*(last - 1) < pivot)) last--;
    }

    bool already_partitioned = first >= last;
    if (!already_partitioned) {
        swap(first, last - 1);
        first++;

        unsigned char offsets_l_storage[BLOCK_SIZE + CACHELINE_SIZE];
        unsigned char offsets_r_storage[BLOCK_SIZE + CACHELINE_SIZE];
        unsigned char* offsets_l = align_cacheline(offsets_l_storage);
        unsigned char* offsets_r = align_cacheline(offsets_r_storage);

        int *offsets_l_base = first;
        int *offsets_r_base = last;
        size_t num_l = 0, num_r = 0, start_l = 0, start_r = 0;

        while (first < last) {
            size_t num_unknown = last - first;
            size_t left_split = num_l == 0 ? (num_r == 0 ? num_unknown / 2 : num_unknown) : 0;
            size_t right_split = num_r == 0 ? (num_unknown - left_split) : 0;

            // Fill the offset blocks
            if (left_split >= BLOCK_SIZE) {
                for (size_t i = 0; i < BLOCK_SIZE;) {
                    offsets_l[num_l] = i++; num_l += !(*(first++) < pivot);
                    offsets_l[num_l] = i++; num_l += !(*(first++) < pivot);
                    offsets_l[num_l] = i++; num_l += !(*(first++) < pivot);
                    offsets_l[num_l] = i++; num_l += !(*(first++) < pivot);
                    offsets_l[num_l] = i++; num_l += !(*(first++) < pivot);
                    offsets_l[num_l] = i++; num_l += !(*(first++) < pivot);
                    offsets_l[num_l] = i++; num_l += !(*(first++) < pivot);
                    offsets_l[num_l] = i++; num_l += !(*(first++) < pivot);
                }
            } else {
                for (size_t i = 0; i < left_split;) {
                    offsets_l[num_l] = i++; num_l += !(*(first++) < pivot);
                }
            }

            if (right_split >= BLOCK_SIZE) {
                for (size_t i = 0; i < BLOCK_SIZE;) {
                    offsets_r[num_r] = ++i; num_r += (*(last -= 1) < pivot);
                    offsets_r[num_r] = ++i; num_r += (*(last -= 1) < pivot);
                    offsets_r[num_r] = ++i; num_r += (*(last -= 1) < pivot);
                    offsets_r[num_r] = ++i; num_r += (*(last -= 1) < pivot);
                    offsets_r[num_r] = ++i; num_r += (*(last -= 1) < pivot);
                    offsets_r[num_r] = ++i; num_r += (*(last -= 1) < pivot);
                    offsets_r[num_r] = ++i; num_r += (*(last -= 1) < pivot);
                    offsets_r[num_r] = ++i; num_r += (*(last -= 1) < pivot);
                }
            } else {
                for (size_t i = 0; i < right_split;) {
                    offsets_r[num_r] = ++i; num_r += (*(last -= 1) < pivot);
                }
            }

            // Swap elements and update block sizes and first/last boundaries
            size_t num = num_l < num_r ? num_l : num_r;
            swap_offsets(offsets_l_base, offsets_r_base, offsets_l + start_l, offsets_r + start_r, num, num_l == num_r);
            num_l -= num; num_r -= num;
            start_l += num; start_r += num;

            if (num_l == 0) {
                start_l = 0;
                offsets_l_base = first;
            }

            if (num_r == 0) {
                start_r = 0;
                offsets_r_base = last;
            }
        }

        // We have now fully identified [first, last)'s proper position. Swap the last elements.
        if (num_l) {
            for (size_t i = 0; i < num_l; ++i) {
                swap(offsets_l_base + offsets_l[i + start_l], --last);
            }
            first = last;
        }
        if (num_r) {
            for (size_t i = 0; i < num_r; ++i) {
                swap(offsets_r_base - offsets_r[i + start_r], first);
                first++;
            }
            last = first;
        }
    }

    // Put the pivot in the right place.
    int pivot_pos = first - arr - 1;
    arr[low] = arr[pivot_pos];
    arr[pivot_pos] = pivot;

    return pivot_pos;
}

// Sorts the elements *a, *b and *c and places the median at *b
void sort3(int *a, int *b, int *c) {
    if (*b < *a) swap(a, b);
    if (*c < *b) swap(b, c);
    if (*b < *a) swap(a, b);
}

// Optimized quicksort function with median of three pivot selection
void quicksort(int *arr, int low, int high, bool leftmost) {
    while (low < high) {
        // Use a while loop for tail recursion elimination
        int size = high - low + 1;

        // Choose pivot as median of 3
        int mid = low + (high - low) / 2;
        sort3(&arr[low], &arr[mid], &arr[high]);
        int pivot = arr[mid];
        swap(&arr[low], &arr[mid]);

        // If the leftmost condition is met, use partition_left
        if (!leftmost && arr[low - 1] == pivot) {
            low = partition_left(arr, low, high) + 1;
            continue;
        }

        // Partition and get results
        int pi = partition_right_branchless(arr, low, high);

        // Recursively sort the smaller partition first
        if (pi - low < high - pi) {
            quicksort(arr, low, pi - 1, leftmost);
            low = pi + 1;
            leftmost = false;
        } else {
            quicksort(arr, pi + 1, high, false);
            high = pi - 1;
        }
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

// Function to print an array
void print_array(int *arr, int size) {
    for (int i = 0; i < size; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    const int array_size = 100;
    int array[array_size];

    // Seed for random number generation
    srand((unsigned int)time(NULL));

    // Generate a small random array of integers
    for (int i = 0; i < array_size; ++i) {
        array[i] = rand() % 100;
    }

    printf("Array before sorting:\n");
    print_array(array, array_size);

    // Sort the array
    quicksort(array, 0, array_size - 1, true);

    printf("Array after sorting:\n");
    print_array(array, array_size);

    // Verify that the array is sorted
    if (is_sorted(array, array_size)) {
        printf("Array is sorted.\n");
    } else {
        printf("Array is not sorted.\n");
    }

    return 0;
}
