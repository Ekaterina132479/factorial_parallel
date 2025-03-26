#include <mpi.h>
#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

// Функция для умножения двух матриц
void multiply_matrices(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int start_row, int end_row) {
    int N = A.size();  // Размер матриц
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    int rank, size;
    int N = 1000;  // Размер матрицы
    vector<vector<int>> A(N, vector<int>(N));
    vector<vector<int>> B(N, vector<int>(N));
    vector<vector<int>> C(N, vector<int>(N));

    // Инициализация MPI
    MPI_Init(&argc, &argv); // Инициализация MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Получает идентификатор процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Получает количество процессов

    // Процесс 0 генерирует матрицы A и B
    if (rank == 0) {
        srand(time(0));
        // Генерация первой матрицы A (её строки будут разделяться между процессами)
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = rand() % 10;
            }
        }
        // Генерация второй матрицы B (её строки доступны всем процессам для умножения)
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                B[i][j] = rand() % 10;
            }
        }
    }

    // Распределение строк матрицы A между процессами
    int rows_per_process = N / size;
    vector<vector<int>> local_A(rows_per_process, vector<int>(N));

    // Используем MPI_Scatter чтобы распределить строки матрицы A
    MPI_Scatter(&A[0][0], rows_per_process * N, MPI_INT, &local_A[0][0], rows_per_process * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Используем MPI_Bcast для отправки матрицы B всем процессам
    MPI_Bcast(&B[0][0], N * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Локальные вычисления: умножаем локальный блок строк матрицы A на всю матрицу B
    vector<vector<int>> local_C(rows_per_process, vector<int>(N));
    multiply_matrices(local_A, B, local_C, 0, rows_per_process);

    // Сбор результатов на процессе 0 с помощью MPI_Gather
    MPI_Gather(&local_C[0][0], rows_per_process * N, MPI_INT, &C[0][0], rows_per_process * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Процесс 0 выводит несколько элементов результирующей матрицы для проверки
    if (rank == 0) {
        cout << "Результат умножения матриц (несколько элементов):" << endl;
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                cout << C[i][j] << " ";
            }
            cout << endl;
        }
    }

    // Измерение времени выполнения
    double start_time = MPI_Wtime();

    // Вычисление локальных блоков и сбор результатов
    if (rank == 0) {
        // Измеряем время для всех процессов
        double end_time = MPI_Wtime();
        cout << "Время выполнения (в секундах): " << end_time - start_time << endl;
    }

    // Завершаем работу с MPI
    MPI_Finalize();

    return 0;
}
