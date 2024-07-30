import numpy as np
import timeit

array_size = 1000000
arrays = [np.random.randint(0, 1000000, array_size) for _ in range(10)]

def benchmark_sort():
    for array in arrays:
        np.sort(array)

time = timeit.timeit('benchmark_sort()', globals=globals(), number=1)

print(f'numpy.sort execution time: {time:.6f} seconds for 10 iterations')
