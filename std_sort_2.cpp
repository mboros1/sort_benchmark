#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>

int main() {
    const int array_size = 1000000;
    const int iterations = 10;

    int **arrays = new int*[iterations];
    for (int i = 0; i < iterations; ++i) {
        arrays[i] = new int[array_size];
    }

    double elapsed_time = 0;
    srand((unsigned int)time(NULL));

    for (int i = 0; i < 100; ++i) {
        for (int i = 0; i < iterations; ++i) {
            for (int j = 0; j < array_size; ++j) {
                arrays[i][j] = (std::rand() % 100) + 1;
            }
        }

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < iterations; ++i) {
            std::sort(arrays[i], arrays[i] + array_size);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        elapsed_time += elapsed.count();
    }

    std::cout << "std::sort execution time: " << (elapsed_time / 100) << " seconds for 10 iterations" << std::endl;

    for (int i = 0; i < iterations; ++i) {
        delete[] arrays[i];
    }
    delete[] arrays;
    return 0;
}
