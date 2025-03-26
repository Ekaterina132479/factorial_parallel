#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

const int N = 1000000; // Размер массива

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); // Запускаем систему MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение общего количества процессов (mpirun -np 4 ./program)

    int local_size = N / size; // Размер подмассива для каждого процесса
    std::vector<int> local_array(local_size);
    std::vector<int> global_array;

    if (rank == 0) { // Создание массива на главном процессе (rank == 0)
        global_array.resize(N);
        std::srand(std::time(nullptr));
        for (int i = 0; i < N; ++i) {
            global_array[i] = std::rand() % 100; // Заполняем случайными числами от 0 до 99
        }
    }

    double start_time = MPI_Wtime(); // Засекаем время выполнения
    // Разделение массива между процессами (MPI_Scatter)
    MPI_Scatter(global_array.data(), local_size, MPI_INT, local_array.data(), local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Вычисление локальной суммы (в отдельном куске массива) 
    int local_sum = 0; 
    for (int num : local_array) {
        local_sum += num;
    }

    // Суммирование локальных сумм
    int global_sum = 0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    // Вывод результата
    if (rank == 0) {
        std::cout << "MPI Global Sum: " << global_sum << std::endl;
        std::cout << "MPI Execution Time: " << (end_time - start_time) << " seconds" << std::endl;

        // Последовательное суммирование для сравнения
        start_time = MPI_Wtime();
        int sequential_sum = 0;
        for (int num : global_array) {
            sequential_sum += num;
        }
        end_time = MPI_Wtime();

        std::cout << "Sequential Sum: " << sequential_sum << std::endl;
        std::cout << "Sequential Execution Time: " << (end_time - start_time) << " seconds" << std::endl;
    }

    MPI_Finalize();  // Завершение работы MPI
    return 0;
}
