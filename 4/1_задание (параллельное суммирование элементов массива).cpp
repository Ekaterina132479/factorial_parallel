#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

#define SIZE 10000000 

int main() {
    std::vector<int> arr(SIZE);

 
    std::srand(std::time(nullptr));

    
    for (int i = 0; i < SIZE; i++) {
        arr[i] = std::rand() % 100 + 1;
    }

    int num_threads = 4;  
    omp_set_num_threads(num_threads);
    std::cout << "num_threads: " << num_threads << std::endl;

    // однопоточное
    long long sequential_sum = 0;
    double start_time = omp_get_wtime();
    for (int i = 0; i < SIZE; i++) {
        sequential_sum += arr[i];
    }
    double sequential_time = omp_get_wtime() - start_time;

    // многопоточное
    long long parallel_sum = 0;
    start_time = omp_get_wtime();

#pragma omp parallel for reduction(+:parallel_sum)
    for (int i = 0; i < SIZE; i++) {
        parallel_sum += arr[i];
    }

    double parallel_time = omp_get_wtime() - start_time;

    
    std::cout << "sequential_sum: " << sequential_sum << "\n";
    std::cout << "parallel_sum: " << parallel_sum << "\n";
    std::cout << "sequential_time: " << sequential_time << "\n";
    std::cout << "parallel_time: " << parallel_time << "\n";

    
    if (sequential_sum == parallel_sum) {
        std::cout << "Ok.\n";
    }
    else {
        std::cout << "Not ok.\n";
    }

    return 0;
}
