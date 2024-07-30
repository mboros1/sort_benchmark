#include <algorithm>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>

int main() {
    const int array_size = 1000000;
    const int iterations = 10;
    std::vector<std::vector<int>> arrays(iterations, std::vector<int>(array_size));

    std::srand(std::time(nullptr));

    for (int i = 0; i < iterations; ++i) {
        for (int j = 0; j < array_size; ++j) {
            arrays[i][j] = (std::rand() % 100) + 1;
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        std::sort(arrays[i].begin(), arrays[i].end());
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "std::sort execution time: " << elapsed.count() << " seconds for 10 iterations" << std::endl;

    return 0;
}

