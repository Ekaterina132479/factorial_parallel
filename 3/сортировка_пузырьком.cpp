#include <iostream>
#include <vector>
#include <omp.h>

using namespace std;

// Функция для параллельной сортировки пузырьком (odd-even sort)
void parallelBubbleSort(vector<int>& arr) {
    int n = arr.size();
    bool sorted = false;

    while (!sorted) {
        sorted = true;

        // Четный проход
#pragma omp parallel for
        for (int i = 0; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                sorted = false;
            }
        }

        // Синхронизация потоков
#pragma omp barrier

// Нечетный проход
#pragma omp parallel for
        for (int i = 1; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                sorted = false;
            }
        }

        // Синхронизация потоков
#pragma omp barrier
    }
}

// Функция для вывода массива
void printArray(const vector<int>& arr) {
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;
}

int main() {
    vector<int> arr = { 5, 3, 8, 6, 2, 7, 4, 1 };

    cout << "source array: ";
    printArray(arr);

    parallelBubbleSort(arr);

    cout << "sorted array: ";
    printArray(arr);

    return 0;
}
