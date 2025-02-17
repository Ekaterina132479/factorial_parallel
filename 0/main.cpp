#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

mutex mtx; // Мьютекс для синхронизации вывода

// Функция для вычисления минора матрицы
vector<vector<int>> getMinor(const vector<vector<int>>& matrix, int row, int col) {
    int n = matrix.size();
    vector<vector<int>> minor;
    for (int i = 0; i < n; i++) {
        if (i == row) continue; // Пропускаем заданную строку
        vector<int> newRow;
        for (int j = 0; j < n; j++) {
            if (j == col) continue; // Пропускаем заданный столбец
            newRow.push_back(matrix[i][j]);
        }
        minor.push_back(newRow);
    }
    return minor;
}

// Функция для рекурсивного вычисления определителя матрицы
int determinant(const vector<vector<int>>& matrix) {
    int n = matrix.size();
    if (n == 1) return matrix[0][0]; // Базовый случай для 1x1 матрицы
    if (n == 2) return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]; // Базовый случай для 2x2 матрицы
    
    int det = 0;
    vector<thread> threads;
    vector<int> results(n);

    // Лямбда-функция для вычисления членов разложения
    auto computeCofactor = [&](int col) {
        vector<vector<int>> minor = getMinor(matrix, 0, col);
        int cofactor = (col % 2 == 0 ? 1 : -1) * matrix[0][col] * determinant(minor);
        results[col] = cofactor;
    };

    // Запускаем потоки для вычисления каждого минорного определителя
    for (int col = 0; col < n; col++) {
        threads.emplace_back(computeCofactor, col);
    }

    // Ждём завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }

    // Складываем результаты
    for (int col = 0; col < n; col++) {
        det += results[col];
    }
    
    return det;
}

int main() {
    // Пример матрицы 3x3
    vector<vector<int>> matrix = {
        {1, 2, 3, 100},
        {4, 5, 6, -5},
        {7, 8, 9, -1},
	{3, -512, 4, 0}
    };
    
    cout << "Исходная матрица:" << endl;
    for (const auto& row : matrix) {
        for (int val : row) {
            cout << val << " ";
        }
        cout << endl;
    }
    
    int det = determinant(matrix);
    cout << "Определитель матрицы: " << det << endl;
    
    return 0;
}
