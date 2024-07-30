
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

        unsigned char offsets_l[256];
        unsigned char offsets_r[256];
        int *offsets_l_base = first;
        int *offsets_r_base = last;
        size_t num_l = 0, num_r = 0, start_l = 0, start_r = 0;
        size_t block_size = 8;

        while (first < last) {
            size_t num_unknown = last - first;
            size_t left_split = num_l == 0 ? (num_r == 0 ? num_unknown / 2 : num_unknown) : 0;
            size_t right_split = num_r == 0 ? (num_unknown - left_split) : 0;

            // Fill the offset blocks
            if (left_split >= block_size) {
                for (size_t i = 0; i < block_size;) {
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

            if (right_split >= block_size) {
                for (size_t i = 0; i < block_size;) {
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
            for (size_t i = 0; i < num; ++i) {
                swap(offsets_l_base + offsets_l[i], offsets_r_base - offsets_r[i]);
            }
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

// Optimized quicksort function
void quicksort(int *arr, int low, int high) {
    if (low < high) {
        int pi = partition_right_branchless(arr, low, high);
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
    quicksort(array, 0, array_size - 1);

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
