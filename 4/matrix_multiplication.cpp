#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace chrono;

const int SIZE = 10000;
const int NUM_THREADS = 8;

// матрица
vector<vector<int>> create_matrix() {
    return vector<vector<int>>(SIZE, vector<int>(SIZE, 1));
}

// вектор
vector<int> create_vector() {
    return vector<int>(SIZE, 1);
}

// однопоточное
vector<int> multiply_single_threaded(const vector<vector<int>>& A, const vector<int>& B) {
    vector<int> C(SIZE, 0);
    for (int i = 0; i < SIZE; i++) {
        for (int k = 0; k < SIZE; k++) {
            C[i] += A[i][k] * B[k];
        }
    }
    return C;
}

// многопоточное
vector<int> multiply_parallel(const vector<vector<int>>& A, const vector<int>& B) {
    vector<int> C(SIZE, 0);
    omp_set_num_threads(NUM_THREADS); 

#pragma omp parallel for schedule(dynamic) num_threads(NUM_THREADS)
    for (int i = 0; i < SIZE; i++) {
        int sum = 0;
        for (int k = 0; k < SIZE; k++) {
            sum += A[i][k] * B[k];
        }
        C[i] = sum;
    }
    return C;
}

int main() {
    vector<vector<int>> A = create_matrix();
    vector<int> B = create_vector();

    // однопоточное
    auto start = high_resolution_clock::now();
    auto C1 = multiply_single_threaded(A, B);
    auto end = high_resolution_clock::now();
    cout << "Single-threaded time: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;

    // многопоточное
    start = high_resolution_clock::now();
    auto C2 = multiply_parallel(A, B);
    end = high_resolution_clock::now();
    cout << "Parallel time: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;

    return 0;
}
