#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace chrono;

const int SIZE = 1000;
const int BLOCK_SIZE = 32; // Размер блока для блочного умножения
const int NUM_THREADS = 8;

// Функция создания матрицы, заполненной единицами
vector<vector<int>> create_matrix() {
    return vector<vector<int>>(SIZE, vector<int>(SIZE, 1));
}

// Блочное умножение матриц без распараллеливания
vector<vector<int>> multiply_single_threaded(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    vector<vector<int>> C(SIZE, vector<int>(SIZE, 0));
    for (int i = 0; i < SIZE; i += BLOCK_SIZE) {
        for (int j = 0; j < SIZE; j += BLOCK_SIZE) {
            for (int ii = i; ii < min(i + BLOCK_SIZE, SIZE); ++ii) {
                for (int jj = j; jj < min(j + BLOCK_SIZE, SIZE); ++jj) {
                    int sum = 0;
                    for (int k = 0; k < SIZE; ++k) {
                        sum += A[ii][k] * B[k][jj];
                    }
                    C[ii][jj] = sum;
                }
            }
        }
    }
    return C;
}

// Блочное умножение матриц с использованием OpenMP
vector<vector<int>> multiply_parallel(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    vector<vector<int>> C(SIZE, vector<int>(SIZE, 0));
    omp_set_num_threads(NUM_THREADS); // Явно задаем количество потоков

#pragma omp parallel for schedule(dynamic) num_threads(NUM_THREADS)
    for (int i = 0; i < SIZE; i += BLOCK_SIZE) {
        for (int j = 0; j < SIZE; j += BLOCK_SIZE) {
            for (int ii = i; ii < min(i + BLOCK_SIZE, SIZE); ++ii) {
                for (int jj = j; jj < min(j + BLOCK_SIZE, SIZE); ++jj) {
                    int sum = 0;
                    for (int k = 0; k < SIZE; ++k) {
                        sum += A[ii][k] * B[k][jj];
                    }
                    C[ii][jj] = sum;
                }
            }
        }
    }
    return C;
}

int main() {
    vector<vector<int>> A = create_matrix();
    vector<vector<int>> B = create_matrix();

    auto start = high_resolution_clock::now();
    auto C1 = multiply_single_threaded(A, B);
    auto end = high_resolution_clock::now();
    cout << "Single-threaded time: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;

    start = high_resolution_clock::now();
    auto C2 = multiply_parallel(A, B);
    end = high_resolution_clock::now();
    cout << "Parallel time: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;

    return 0;
}
